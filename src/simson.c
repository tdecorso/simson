#include "simson.h"
#include <stdlib.h>
#include <string.h>

#define MAX_DEPTH 500
#define DEFAULT_ARRAY_CAPACITY 4
#define DEFAULT_OBJECT_CAPACITY 4

typedef uint8_t u8;
typedef uint32_t u32;

typedef struct parser {
    const u8* cur;
    const u8* end;
    size_t line;
    size_t col;
    size_t depth;
} parser_t;

static simson_text_t* text_create(const char* text, size_t len) {
    simson_text_t* t = malloc(sizeof(simson_text_t));
    if (!t) return NULL;
    t->data = malloc(len);
    if (!t->data) {
        free(t);
        return NULL;
    }
    memcpy(t->data, text, len);
    t->len = len;
    return t;
}

static void text_destroy(simson_text_t* t) {
    if (!t) return;
    free(t->data);
    free(t);
}

static simson_member_t* member_create(simson_text_t* text, simson_value_t* v) {
    simson_member_t* m = malloc(sizeof(simson_member_t));
    if (!m) return NULL;
    m->key = text;
    m->value = v;
    return m;
}

static void member_destroy(simson_member_t* m) {
    if (!m) return;
    text_destroy(m->key);
    simson_value_destroy(m->value);
    free(m);
}

static simson_array_t* array_create(size_t capacity) {
    simson_array_t* a = malloc(sizeof(simson_array_t));
    if (!a) return NULL;
    a->data = malloc(sizeof(simson_value_t*) * capacity);
    if (!a->data) {
        free(a);
        return NULL;
    }
    a->len = 0;
    a->cap = capacity;
    return a;
}

static int array_append(simson_array_t* a, simson_value_t* v) {
    if (a->len == a->cap) {
        size_t ncap = a->cap * 2;
        simson_value_t** nvalues = realloc(a->data, ncap * sizeof(simson_value_t*));
        if (!nvalues) return 1;
        a->data = nvalues;
        a->cap = ncap;
    }
    a->data[a->len] = v;
    a->len++;
    return 0;
}

static void array_destroy(simson_array_t* arr) {
    if (!arr) return;
    for (size_t i = 0; i < arr->len; i++) {
        simson_value_destroy(arr->data[i]);
    }
    free(arr->data);
    free(arr);
}

static simson_object_t* object_create(size_t capacity) {
    simson_object_t* o = malloc(sizeof(simson_object_t));
    if (!o) return NULL;
    o->data = malloc(sizeof(simson_member_t*) * capacity);
    if (!o->data) {
        free(o);
        return NULL;
    }
    o->len = 0;
    o->cap = capacity;
    return o;
}

static int object_append(simson_object_t* o, simson_member_t* m) {
    if (o->len == o->cap) {
        size_t ncap = o->cap * 2;
        simson_member_t** nmembers = realloc(o->data, ncap * sizeof(simson_member_t*));
        if (!nmembers) return 1;
        o->data = nmembers;
        o->cap = ncap;
    }
    o->data[o->len] = m;
    o->len++;
    return 0;
}

static void object_destroy(simson_object_t* obj) {
    if (!obj) return;
    for (size_t i = 0; i < obj->len; i++) {
        member_destroy(obj->data[i]);
    }
    free(obj->data);
    free(obj);
}

simson_value_t* simson_value_create(simson_type_t type) {
    simson_value_t* v = malloc(sizeof(simson_value_t));
    if (!v) return NULL;
    v->type = type;
    switch(v->type) {
    case SIMSON_BOOL: {
        v->as.boolean = false;
        break;
    }
    case SIMSON_NUMBER: {
        v->as.number = 0;
        break;
    }
    case SIMSON_STRING: {
        v->as.string = NULL;
        break;
    }
    case SIMSON_ARRAY: {
        v->as.array = NULL;
        break;
    }
    case SIMSON_OBJECT: {
        v->as.object = NULL;
        break;
    }
    default: break;
    }
    return v;
}

void simson_value_destroy(simson_value_t* v) {
    if (!v) return;
    switch (v->type) {
    case SIMSON_STRING: {
        text_destroy(v->as.string);
        break;
    }
    case SIMSON_ARRAY: {
        array_destroy(v->as.array);
        break;
    }
    case SIMSON_OBJECT: {
        object_destroy(v->as.object);
        break;
    }
    default:
        break;
    }
    free(v);
}



static void parser_init(parser_t* p, const u8* buffer, size_t len) {
    p->cur = buffer;
    p->end = buffer + len;
    p->line = 1;
    p->col = 0;
    p->depth = 0;
}

static bool at_end(parser_t* p) {
    return p->cur >= p->end;
}

static u8 peek(parser_t* p) {
    return at_end(p) ? '\0' : *p->cur;
}

static void advance(parser_t* p) {
    u8 c = peek(p);
    if (c == '\0') return;
    if (c == '\n') {
        p->line++;
        p->col = 0;
    }
    else p->col++;
    p->cur++;
}

static void skip_ws(parser_t* p) {
    while (1) {
        u8 c = peek(p);
        if (c != '\n' && c != '\r' && c != '\t' && c != ' ') break;
        advance(p);
    }
}

static int utf8_decode(const uint8_t* buffer, size_t offset, size_t len, uint32_t* cp_out, size_t* cp_len_out) {
    if (!buffer || len == 0) return 1;
    if (!cp_out) return 1;
    if (offset >= len) return 1;
    u8 b0 = buffer[offset];
    if (b0 <= 0x7F) {
        *cp_out = b0;
        if (cp_len_out) *cp_len_out = 1;
        return 0;
    }
    if (b0 >= 0xC2 && b0 <= 0xDF) {
        if (len < 2) return 1;
        if (offset + 1 >= len) return 1;
        u8 b1 = buffer[offset + 1];
        if (b1 < 0x80 || b1 > 0xBF) return 1;
        *cp_out = ((b0 & 0x1F) << 6) | (b1 & 0x3F);
        if (cp_len_out) *cp_len_out = 2;
        return 0;
    }
    if (b0 >= 0xE0 && b0 <= 0xEF) {
        if (len < 3) return 1;
        if (offset + 2 >= len) return 1;
        u8 b1 = buffer[offset + 1];
        u8 b2 = buffer[offset + 2];
        if (b1 < 0x80 || b1 > 0xBF ||b2 < 0x80 || b2 > 0xBF) return 1;
        if ((b0 == 0xE0 && b1 < 0xA0) ||(b0 == 0xED && b1 > 0x9F)) return 1;
        *cp_out = ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
        if (*cp_out >= 0xD800 && *cp_out <= 0xDFFF) return 1;
        if (cp_len_out) *cp_len_out = 3;
        return 0;
    }

    if (b0 >= 0xF0 && b0 <= 0xF4) {
        if (len < 4) return 1;
        if (offset + 3 >= len) return 1;
        u8 b1 = buffer[offset + 1];
        u8 b2 = buffer[offset + 2];
        u8 b3 = buffer[offset + 3];
        if (b1 < 0x80 || b1 > 0xBF ||
            b2 < 0x80 || b2 > 0xBF ||
            b3 < 0x80 || b3 > 0xBF) return 1;

        if ((b0 == 0xF0 && b1 < 0x90) ||(b0 == 0xF4 && b1 > 0x8F)) return 1;
        *cp_out = ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
        if (*cp_out >= 0xD800 && *cp_out <= 0xDFFF) return 1;
        if (cp_len_out) *cp_len_out = 4;
        return 0;
    }

    return 1;
}

static bool is_unescaped(u32 cp) {
    return cp == 0x20 || cp == 0x21 || (cp >= 0x23 && cp <= 0x5B) || (cp >= 0x5D && cp <= 0x10FFFF);
}

static bool is_hex(u8 c) {
    return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9');
}

static u8 hex_value(u8 c) {
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    else return c - '0';
}

static u32 parse_hex(parser_t* p) {
    u32 cp = 0;
    for (size_t i = 0; i < 4; i++) {
        u8 b = peek(p);
        if (!is_hex(b)) return 0x10000;
        advance(p);
        cp = (cp << 4) | hex_value(b);
    }
    return cp;
}

static bool is_alpha(u8 c) {
    return (c >= 'a' && c <= 'z') || 
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
            c == '_';
}

static simson_text_t* parse_text(parser_t* p) {
    const u8* start = p->cur;
    advance(p);

    while (1) {
        u32 cp = 0; size_t cp_len = 0;
        if (utf8_decode(p->cur, 0, (size_t) (p->end - p->cur), &cp, &cp_len)) return NULL;
        if (cp == '\0') return NULL;
        if (cp == '"') {
            advance(p);
            break;
        }
        if (is_unescaped(cp)) {
            p->cur += cp_len;
            continue;
        }
        if (cp == '\\') {
            advance(p);
            u8 c = peek(p);
            if (c == '"' || c == '\\' || c == '/' || c == 'b' || c == 'f' || c == 'n' || c == 'r'  || c == 't') {
                advance(p);
                continue;
            }
            if (c == 'u') {
                advance(p);
                u32 w1 = parse_hex(p);
                if (w1 > 0xFFFF) return NULL;
                if (w1 >= 0xD800 && w1 <= 0xDFFF) {
                    c = peek(p);
                    if (c != '\\') return NULL;
                    advance(p);
                    c = peek(p);
                    if (c != 'u') return NULL;
                    advance(p);
                    u32 w2 = parse_hex(p);
                    if (w2 < 0xDC00 || w2 > 0xDFFF) return NULL;
                    if (w2 > 0xFFFF) return NULL;
                } else if (w1 >= 0xDC00 && w1 <= 0xDFFF) return NULL;
                continue;
            }
        }
        return NULL;
    }
    u8 c = peek(p);
    if (is_alpha(c)) return NULL;
    size_t required = (size_t) (p->cur - start);
    return text_create((const char*)start, required);
}

static simson_value_t* parse_string(parser_t* p) {
    simson_text_t* text = parse_text(p);
    if (!text) return NULL;
    simson_value_t* v = simson_value_create(SIMSON_STRING);
    if (!v) {
        text_destroy(text);
        return NULL;
    }
    v->as.string = text;
    return v;
}

static simson_value_t* parse_literal(parser_t* p, const char* pattern, size_t len) {
    for (size_t i = 0; i < len; i++) {
        u8 c = peek(p);
        if (c != pattern[i]) return NULL;
        advance(p);
    }
    simson_value_t* v = simson_value_create(SIMSON_BOOL);
    if (!v) return NULL;
    if (pattern[0] == 'n') return v;
    v->as.boolean = len == 4;
    return v;
}

static bool is_digit(u8 c) {
    return c >= '0' && c <= '9';
}

static bool is_digit1_9(u8 c) {
    return c >= '1' && c <= '9';
}

static simson_value_t* parse_number(parser_t* p) {
    const u8* start = p->cur;
    u8 c = peek(p);
    if (c == '-') {
        advance(p);
        c = peek(p);
    }
    if (c == '0') {
        advance(p);
        c = peek(p);
        if (is_digit1_9(c)) return NULL;
    }
    else if (is_digit1_9(c)) {
        advance(p);
        while (1) {
            c = peek(p);
            if (c == '\0' && at_end(p)) break;
            if (!is_digit(c)) break;
            advance(p);
        }
    } else return NULL;
    if (c == '.') {
        advance(p);
        c = peek(p);
        if (!is_digit(c)) return NULL;
        advance(p);
        while (1) {
            c = peek(p);
            if (!is_digit(c)) break;
            advance(p);
        }
    }
    if (c == 'e' || c == 'E') {
        advance(p);
        c = peek(p);
        if (c == '+' || c == '-') {
            advance(p);
            c = peek(p);
        }
        if (!is_digit(c)) return NULL;
        advance(p);
        while (1) {
            c = peek(p);
            if (!is_digit(c)) break;
            advance(p);
        }
    }
    size_t len = (size_t) (p->cur - start);
    char* buffer = malloc(len + 1);
    if (!buffer) return NULL;
    memcpy(buffer, start, len);
    buffer[len] = '\0';
    simson_value_t* v = simson_value_create(SIMSON_NUMBER);
    if (!v) {
        free(buffer);
        return NULL;
    }
    v->as.number = strtod(buffer, NULL);
    free(buffer);
    return v;
}

static simson_value_t* parse_value(parser_t* p);

static simson_value_t* parse_array(parser_t* p) {
    if (p->depth > MAX_DEPTH) return NULL;
    simson_value_t* v = simson_value_create(SIMSON_ARRAY);
    if (!v) return NULL;
    advance(p);
    u8 c = peek(p);
    if (c == ']') {
        advance(p);
        p->depth--;
        return v;
    }
    v->as.array = array_create(DEFAULT_ARRAY_CAPACITY);
    if (!v->as.array) {
        simson_value_destroy(v);
        return NULL;
    }
    simson_value_t* iv = parse_value(p);
    if (!iv) {
        simson_value_destroy(v);
        return NULL;
    }
    if (array_append(v->as.array, iv)) {
        simson_value_destroy(iv);
        simson_value_destroy(v);
        return NULL;
    }
    p->depth++;
    while (1) {
        skip_ws(p);
        c = peek(p);
        if (c != ',') break;
        advance(p);
        iv = parse_value(p);
        if (!iv) {
            simson_value_destroy(v);
            return NULL;
        }
        if (array_append(v->as.array, iv)) {
            simson_value_destroy(iv);
            simson_value_destroy(v);
            return NULL;
        }
    }
    c = peek(p);
    if (c == ']') {
        advance(p);
        p->depth--;
        return v;
    }
    simson_value_destroy(v);
    return NULL;
}

static simson_member_t* parse_member(parser_t* p) {
    skip_ws(p);
    simson_text_t* t = parse_text(p);
    if (!t) return NULL;
    u8 c = peek(p);
    if (c != ':') {
        text_destroy(t);
        return NULL;
    }
    advance(p);
    simson_value_t* v = parse_value(p);
    if (!v) {
        text_destroy(t);
        return NULL;
    }
    simson_member_t* m = member_create(t, v);
    if (!m) {
        text_destroy(t);
        simson_value_destroy(v);
        return NULL;
    }
    return m;
}

static simson_value_t* parse_object(parser_t* p) {
    if (p->depth > MAX_DEPTH) return NULL;
    simson_value_t* v = simson_value_create(SIMSON_OBJECT);
    if (!v) return NULL;
    advance(p);
    u8 c = peek(p);
    if (c == '}') {
        advance(p);
        p->depth--;
        return v;
    }
    v->as.object = object_create(DEFAULT_OBJECT_CAPACITY);
    if (!v->as.object) {
        simson_value_destroy(v);
        return NULL;
    }
    simson_member_t* m = parse_member(p);
    if (!m) {
        simson_value_destroy(v);
        return NULL;
    }
    if (object_append(v->as.object, m)) {
        member_destroy(m);
        simson_value_destroy(v);
        return NULL;
    }
    while (1) {
        skip_ws(p);
        c = peek(p);
        if (c != ',') break;
        advance(p);
        m = parse_member(p);
        if (!m) {
            simson_value_destroy(v);
            return NULL;
        }
        if (object_append(v->as.object, m)) {
            member_destroy(m);
            simson_value_destroy(v);
            return NULL;
        }
    }
    c = peek(p);
    if (c == '}') {
        advance(p);
        p->depth--;
        return v;
    }
    simson_value_destroy(v);
    return NULL;
}

static simson_value_t* parse_value(parser_t* p) {
    skip_ws(p);
    u8 c = peek(p);
    switch (c) {
    case '{': {
        p->depth++;
        return parse_object(p);
    }
    case '[': {
        p->depth++;
        return parse_array(p);
    }
    case '"': return parse_string(p);
    case 't': return parse_literal(p, "true", 4);
    case 'f': return parse_literal(p, "false", 5);
    case 'n': return parse_literal(p, "null", 4);
    case '-': return parse_number(p);
    default: break;
    }

    if (is_digit(c)) return parse_number(p);

    return NULL;
}

simson_value_t* simson_value_parse(const u8* buffer, size_t len) {
    if (!buffer || len == 0) return NULL;
    parser_t parser; parser_init(&parser, buffer, len);
    simson_value_t* result = parse_value(&parser);
    if (!result) return NULL;
    skip_ws(&parser);
    if (!at_end(&parser)) {
        simson_value_destroy(result);
        return NULL;
    }
    return result;
}
