#include "simson.h"
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEPTH 500
#define DEFAULT_ARRAY_CAPACITY 4
#define DEFAULT_OBJECT_CAPACITY 4
#define DEFAULT_TB_CAPACITY 32

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
    t->data = malloc(len > 0 ? len : 1);
    if (!t->data) {
        free(t);
        return NULL;
    }
    if (len > 0) memcpy(t->data, text, len);
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

static int array_reserve(simson_array_t* a, size_t needed) {
    if (needed <= a->cap) return 0;
    if (needed > SIZE_MAX / sizeof(simson_value_t*)) return 1;
    size_t ncap = a->cap ? a->cap : DEFAULT_ARRAY_CAPACITY;
    while (ncap < needed) {
        if (ncap > SIZE_MAX / 2) {
            ncap = needed;
            break;
        }
        ncap *= 2;
    }
    simson_value_t** nvalues = realloc(a->data, ncap * sizeof(simson_value_t*));
    if (!nvalues) return 1;
    a->data = nvalues;
    a->cap = ncap;
    return 0;
}

static int array_append(simson_array_t* a, simson_value_t* v) {
    if (array_reserve(a, a->len + 1)) return 1;
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

static int object_reserve(simson_object_t* o, size_t needed) {
    if (needed <= o->cap) return 0;
    if (needed > SIZE_MAX / sizeof(simson_member_t*)) return 1;
    size_t ncap = o->cap ? o->cap : DEFAULT_OBJECT_CAPACITY;
    while (ncap < needed) {
        if (ncap > SIZE_MAX / 2) {
            ncap = needed;
            break;
        }
        ncap *= 2;
    }
    simson_member_t** nmembers = realloc(o->data, ncap * sizeof(simson_member_t*));
    if (!nmembers) return 1;
    o->data = nmembers;
    o->cap = ncap;
    return 0;
}

static int object_append(simson_object_t* o, simson_member_t* m) {
    if (object_reserve(o, o->len + 1)) return 1;
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

simson_value_t* simson_value_null(void) {
    return simson_value_create(SIMSON_NULL);
}

simson_value_t* simson_value_bool(bool value) {
    simson_value_t* v = simson_value_create(SIMSON_BOOL);
    if (v) v->as.boolean = value;
    return v;
}

simson_value_t* simson_value_number(double value) {
    simson_value_t* v = simson_value_create(SIMSON_NUMBER);
    if (v) v->as.number = value;
    return v;
}

simson_value_t* simson_value_string(const char* text, size_t len) {
    if (!text && len > 0) return NULL;
    simson_value_t* v = simson_value_create(SIMSON_STRING);
    if (!v) return NULL;
    v->as.string = text_create(text ? text : "", len);
    if (!v->as.string) {
        simson_value_destroy(v);
        return NULL;
    }
    return v;
}

simson_value_t* simson_value_array(void) {
    return simson_value_create(SIMSON_ARRAY);
}

simson_value_t* simson_value_object(void) {
    return simson_value_create(SIMSON_OBJECT);
}

int simson_value_type(const simson_value_t* v, simson_type_t* out) {
    if (!v || !out) return 1;
    *out = v->type;
    return 0;
}

int simson_value_get_bool(const simson_value_t* v, bool* out) {
    if (!v || !out || v->type != SIMSON_BOOL) return 1;
    *out = v->as.boolean;
    return 0;
}

int simson_value_get_number(const simson_value_t* v, double* out) {
    if (!v || !out || v->type != SIMSON_NUMBER) return 1;
    *out = v->as.number;
    return 0;
}

int simson_value_get_string(const simson_value_t* v, const char** data, size_t* len) {
    if (!v || !data || !len || v->type != SIMSON_STRING) return 1;
    if (v->as.string) {
        *data = v->as.string->data;
        *len = v->as.string->len;
    } else {
        *data = NULL;
        *len = 0;
    }
    return 0;
}

static int ensure_array(simson_value_t* v) {
    if (!v || v->type != SIMSON_ARRAY) return 1;
    if (!v->as.array) {
        v->as.array = array_create(DEFAULT_ARRAY_CAPACITY);
        if (!v->as.array) return 1;
    }
    return 0;
}

static int ensure_object(simson_value_t* v) {
    if (!v || v->type != SIMSON_OBJECT) return 1;
    if (!v->as.object) {
        v->as.object = object_create(DEFAULT_OBJECT_CAPACITY);
        if (!v->as.object) return 1;
    }
    return 0;
}

int simson_array_len(const simson_value_t* v, size_t* len) {
    if (!v || !len || v->type != SIMSON_ARRAY) return 1;
    *len = v->as.array ? v->as.array->len : 0;
    return 0;
}

int simson_array_get(const simson_value_t* v, size_t index, simson_value_t** out) {
    if (out) *out = NULL;
    if (!v || !out || v->type != SIMSON_ARRAY || !v->as.array || index >= v->as.array->len) return 1;
    *out = v->as.array->data[index];
    return 0;
}

int simson_array_append(simson_value_t* v, simson_value_t* value) {
    if (!value || ensure_array(v)) return 1;
    return array_append(v->as.array, value);
}

int simson_array_insert(simson_value_t* v, size_t index, simson_value_t* value) {
    if (!value || ensure_array(v)) return 1;
    simson_array_t* a = v->as.array;
    if (index > a->len) return 1;
    if (array_reserve(a, a->len + 1)) return 1;
    memmove(&a->data[index + 1], &a->data[index], (a->len - index) * sizeof(simson_value_t*));
    a->data[index] = value;
    a->len++;
    return 0;
}

int simson_array_set(simson_value_t* v, size_t index, simson_value_t* value) {
    if (!value || !v || v->type != SIMSON_ARRAY || !v->as.array || index >= v->as.array->len) return 1;
    simson_value_t** slot = &v->as.array->data[index];
    if (*slot == value) return 0;
    simson_value_destroy(*slot);
    *slot = value;
    return 0;
}

simson_value_t* simson_array_remove(simson_value_t* v, size_t index) {
    if (!v || v->type != SIMSON_ARRAY || !v->as.array || index >= v->as.array->len) return NULL;
    simson_array_t* a = v->as.array;
    simson_value_t* removed = a->data[index];
    memmove(&a->data[index], &a->data[index + 1], (a->len - index - 1) * sizeof(simson_value_t*));
    a->len--;
    return removed;
}

static int object_find_index(const simson_object_t* o, const char* key, size_t key_len, size_t* index) {
    for (size_t i = 0; i < o->len; i++) {
        simson_member_t* m = o->data[i];
        if (m && m->key && m->key->len == key_len &&
            (key_len == 0 || memcmp(m->key->data, key, key_len) == 0)) {
            *index = i;
            return 0;
        }
    }
    return 1;
}

int simson_object_len(const simson_value_t* v, size_t* len) {
    if (!v || !len || v->type != SIMSON_OBJECT) return 1;
    *len = v->as.object ? v->as.object->len : 0;
    return 0;
}

int simson_object_get(const simson_value_t* v, const char* key, size_t key_len, simson_value_t** out) {
    if (out) *out = NULL;
    if (!v || !key || !out || v->type != SIMSON_OBJECT || !v->as.object) return 1;
    size_t index;
    if (object_find_index(v->as.object, key, key_len, &index)) return 1;
    *out = v->as.object->data[index]->value;
    return 0;
}

int simson_object_key(const simson_value_t* v, size_t index, const char** key, size_t* key_len) {
    if (key) *key = NULL;
    if (key_len) *key_len = 0;
    if (!v || !key || !key_len || v->type != SIMSON_OBJECT || !v->as.object || index >= v->as.object->len) return 1;
    simson_member_t* m = v->as.object->data[index];
    if (!m || !m->key) return 1;
    *key = m->key->data;
    *key_len = m->key->len;
    return 0;
}

int simson_object_set(simson_value_t* v, const char* key, size_t key_len, simson_value_t* value) {
    if (!key || !value || ensure_object(v)) return 1;
    simson_object_t* o = v->as.object;
    size_t index;
    if (object_find_index(o, key, key_len, &index) == 0) {
        simson_member_t* m = o->data[index];
        if (m->value == value) return 0;
        simson_value_destroy(m->value);
        m->value = value;
        return 0;
    }
    simson_text_t* t = text_create(key, key_len);
    if (!t) return 1;
    simson_member_t* m = member_create(t, value);
    if (!m) {
        text_destroy(t);
        return 1;
    }
    if (object_append(o, m)) {
        free(m);
        text_destroy(t);
        return 1;
    }
    return 0;
}

simson_value_t* simson_object_remove(simson_value_t* v, const char* key, size_t key_len) {
    if (!v || !key || v->type != SIMSON_OBJECT || !v->as.object) return NULL;
    size_t index;
    if (object_find_index(v->as.object, key, key_len, &index)) return NULL;
    simson_object_t* o = v->as.object;
    simson_member_t* m = o->data[index];
    simson_value_t* removed = m->value;
    memmove(&o->data[index], &o->data[index + 1], (o->len - index - 1) * sizeof(simson_member_t*));
    o->len--;
    text_destroy(m->key);
    free(m);
    return removed;
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
    advance(p);
    const u8* start = p->cur;

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
    size_t required = (size_t) (p->cur - start) - 1;
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
    simson_type_t type = pattern[0] == 'n' ? SIMSON_NULL : SIMSON_BOOL;
    simson_value_t* v = simson_value_create(type);
    if (!v) return NULL;
    if (type == SIMSON_BOOL) v->as.boolean = len == 4;
    return v;
}

static bool is_digit(u8 c) {
    return c >= '0' && c <= '9';
}

static bool is_digit1_9(u8 c) {
    return c >= '1' && c <= '9';
}

static double parse_double(const char* text) {
    const char* decimal = localeconv()->decimal_point;
    if (!decimal || strcmp(decimal, ".") == 0) return strtod(text, NULL);
    size_t decimal_len = strlen(decimal);
    const char* dot = strchr(text, '.');
    if (!dot || decimal_len == 0) return strtod(text, NULL);
    size_t len = strlen(text);
    size_t extra = decimal_len > 1 ? decimal_len - 1 : 0;
    if (extra > SIZE_MAX - len - 1) return strtod(text, NULL);
    char* normalized = malloc(len + extra + 1);
    if (!normalized) return strtod(text, NULL);
    size_t prefix = (size_t) (dot - text);
    memcpy(normalized, text, prefix);
    memcpy(normalized + prefix, decimal, decimal_len);
    size_t suffix_len = len - prefix - 1;
    memcpy(normalized + prefix + decimal_len, dot + 1, suffix_len + 1);
    double result = strtod(normalized, NULL);
    free(normalized);
    return result;
}

static int format_double(char* buffer, size_t cap, int precision, double number) {
    int len = snprintf(buffer, cap, "%.*g", precision, number);
    if (len < 0 || (size_t) len >= cap) return -1;
    const char* decimal = localeconv()->decimal_point;
    if (!decimal || decimal[0] == '\0' || strcmp(decimal, ".") == 0) return len;
    char* dot = strstr(buffer, decimal);
    if (!dot) return len;
    size_t decimal_len = strlen(decimal);
    size_t suffix_len = strlen(dot + decimal_len);
    if (decimal_len > 1) memmove(dot + 1, dot + decimal_len, suffix_len + 1);
    *dot = '.';
    return len - (int) (decimal_len - 1);
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
    double number = parse_double(buffer);
    free(buffer);
    if (!isfinite(number)) return NULL;
    simson_value_t* v = simson_value_create(SIMSON_NUMBER);
    if (!v) return NULL;
    v->as.number = number;
    return v;
}

static simson_value_t* parse_value(parser_t* p);

static simson_value_t* parse_array(parser_t* p) {
    if (p->depth > MAX_DEPTH) return NULL;
    simson_value_t* v = simson_value_create(SIMSON_ARRAY);
    if (!v) return NULL;
    advance(p);
    skip_ws(p);
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
    skip_ws(p);
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

simson_value_t* simson_value_parse(const char* buffer, size_t len) {
    if (!buffer || len == 0) return NULL;
    parser_t parser; parser_init(&parser, (const u8*) buffer, len);
    simson_value_t* result = parse_value(&parser);
    if (!result) return NULL;
    skip_ws(&parser);
    if (!at_end(&parser)) {
        simson_value_destroy(result);
        return NULL;
    }
    return result;
}

typedef struct text_buffer {
    char* data;
    size_t len;
    size_t cap;
} text_buffer_t;

static text_buffer_t* text_buffer_create(size_t cap) {
    text_buffer_t* tb = malloc(sizeof(text_buffer_t));
    if (!tb) return NULL;
    tb->data = malloc(cap);
    if (!tb->data) {
        free(tb);
        return NULL;
    }
    tb->data[0] = '\0';
    tb->len = 0;
    tb->cap = cap;
    return tb;
}

static int text_buffer_reserve(text_buffer_t* tb, size_t extra) {
    if (extra > SIZE_MAX - tb->len - 1) return 1;
    size_t needed = tb->len + extra + 1;
    if (needed <= tb->cap) return 0;
    size_t ncap = tb->cap;
    while (ncap < needed) {
        if (ncap > SIZE_MAX / 2) {
            ncap = needed;
            break;
        }
        ncap *= 2;
    }
    char* data = realloc(tb->data, ncap);
    if (!data) return 1;
    tb->data = data;
    tb->cap = ncap;
    return 0;
}

static int text_buffer_append(text_buffer_t* tb, const char* text, size_t len) {
    if (text_buffer_reserve(tb, len)) return 1;
    if (len > 0) memcpy(tb->data + tb->len, text, len);
    tb->len += len;
    tb->data[tb->len] = '\0';
    return 0;
}

static void text_buffer_destroy(text_buffer_t* tb) {
    if (!tb) return;
    free(tb->data);
    free(tb);
}

static int text_buffer_indent(text_buffer_t* tb, size_t depth) {
    static const char spaces[] = "                                ";
    size_t count = depth * 2;
    while (count > 0) {
        size_t chunk = count < sizeof(spaces) - 1 ? count : sizeof(spaces) - 1;
        if (text_buffer_append(tb, spaces, chunk)) return 1;
        count -= chunk;
    }
    return 0;
}

static int serialize_number(double number, text_buffer_t* tb) {
    if (!isfinite(number)) return 1;
    char candidate[64];
    char best[64];
    size_t best_len = 0;
    for (int precision = 1; precision <= 17; precision++) {
        int n = format_double(candidate, sizeof(candidate), precision, number);
        if (n < 0) return 1;
        double parsed = parse_double(candidate);
        if (parsed == number && signbit(parsed) == signbit(number)) {
            if (best_len == 0 || (size_t) n < best_len) {
                memcpy(best, candidate, (size_t) n);
                best_len = (size_t) n;
            }
        }
    }
    if (best_len == 0) {
        int n = format_double(best, sizeof(best), 17, number);
        if (n < 0) return 1;
        best_len = (size_t) n;
    }
    return text_buffer_append(tb, best, best_len);
}

static int serialize_text(const simson_text_t* text, text_buffer_t* tb) {
    if (text_buffer_append(tb, "\"", 1)) return 1;
    if (text && text->data && text->len > 0) {
        if (text_buffer_append(tb, text->data, text->len)) return 1;
    }
    return text_buffer_append(tb, "\"", 1);
}

static int serialize_value(const simson_value_t* v, bool pretty, size_t depth, text_buffer_t* tb);

static int serialize_array(const simson_array_t* arr, bool pretty, size_t depth, text_buffer_t* tb) {
    if (text_buffer_append(tb, "[", 1)) return 1;
    if (!arr || arr->len == 0) return text_buffer_append(tb, "]", 1);
    for (size_t i = 0; i < arr->len; i++) {
        if (i > 0 && text_buffer_append(tb, ",", 1)) return 1;
        if (pretty) {
            if (text_buffer_append(tb, "\n", 1)) return 1;
            if (text_buffer_indent(tb, depth + 1)) return 1;
        }
        if (serialize_value(arr->data[i], pretty, depth + 1, tb)) return 1;
    }
    if (pretty) {
        if (text_buffer_append(tb, "\n", 1)) return 1;
        if (text_buffer_indent(tb, depth)) return 1;
    }
    return text_buffer_append(tb, "]", 1);
}

static int serialize_object(const simson_object_t* obj, bool pretty, size_t depth, text_buffer_t* tb) {
    if (text_buffer_append(tb, "{", 1)) return 1;
    if (!obj || obj->len == 0) return text_buffer_append(tb, "}", 1);
    for (size_t i = 0; i < obj->len; i++) {
        if (i > 0 && text_buffer_append(tb, ",", 1)) return 1;
        if (pretty) {
            if (text_buffer_append(tb, "\n", 1)) return 1;
            if (text_buffer_indent(tb, depth + 1)) return 1;
        }
        const simson_member_t* m = obj->data[i];
        if (!m) return 1;
        if (serialize_text(m->key, tb)) return 1;
        if (pretty) {
            if (text_buffer_append(tb, ": ", 2)) return 1;
        } else if (text_buffer_append(tb, ":", 1)) {
            return 1;
        }
        if (serialize_value(m->value, pretty, depth + 1, tb)) return 1;
    }
    if (pretty) {
        if (text_buffer_append(tb, "\n", 1)) return 1;
        if (text_buffer_indent(tb, depth)) return 1;
    }
    return text_buffer_append(tb, "}", 1);
}

static int serialize_value(const simson_value_t* v, bool pretty, size_t depth, text_buffer_t* tb) {
    if (!v || depth > MAX_DEPTH) return 1;
    switch (v->type) {
    case SIMSON_NULL:
        return text_buffer_append(tb, "null", 4);
    case SIMSON_BOOL:
        return text_buffer_append(tb, v->as.boolean ? "true" : "false", v->as.boolean ? 4 : 5);
    case SIMSON_NUMBER:
        return serialize_number(v->as.number, tb);
    case SIMSON_STRING:
        return serialize_text(v->as.string, tb);
    case SIMSON_ARRAY:
        return serialize_array(v->as.array, pretty, depth, tb);
    case SIMSON_OBJECT:
        return serialize_object(v->as.object, pretty, depth, tb);
    default:
        return 1;
    }
}

int simson_value_serialize(simson_value_t* v, bool pretty, char** buffer, size_t* len) {
    if (!v || !buffer || !len) return 1;
    *buffer = NULL;
    *len = 0;
    text_buffer_t* tb = text_buffer_create(DEFAULT_TB_CAPACITY);
    if (!tb) return 1;
    if (serialize_value(v, pretty, 0, tb)) {
        text_buffer_destroy(tb);
        return 1;
    }
    *buffer = tb->data;
    *len = tb->len;
    free(tb);
    return 0;
}
