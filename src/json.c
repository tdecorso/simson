#include "json.h"
#include <stdlib.h>
#include <string.h>

typedef uint8_t u8;

static json_text_t* text_create(const char* text, size_t len) {
    json_text_t* t = malloc(sizeof(json_text_t));
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

static void text_destroy(json_text_t* t) {
    if (!t) return;
    free(t->data);
    free(t);
}

static json_member_t* member_create(const char* text, size_t len) {
    json_member_t* m = malloc(sizeof(json_member_t));
    if (!m) return NULL;
    m->key = text_create(text, len);
    if (!m->key) {
        free(m);
        return NULL;
    }
    m->value = NULL;
    return m;
}

static void member_destroy(json_member_t* m) {
    if (!m) return;
    text_destroy(m->key);
    json_value_destroy(m->value);
    free(m);
}

static json_array_t* array_create(size_t capacity) {
    json_array_t* a = malloc(sizeof(json_array_t));
    if (!a) return NULL;
    a->data = malloc(sizeof(json_member_t*) * capacity);
    if (!a->data) {
        free(a);
        return NULL;
    }
    a->len = 0;
    a->cap = capacity;
    return a;
}

static void array_destroy(json_array_t* arr) {
    if (!arr) return;
    for (size_t i = 0; i < arr->len; i++) {
        member_destroy(arr->data[i]);
    }
    free(arr);
}

static json_object_t* object_create(size_t capacity) {
    json_object_t* o = malloc(sizeof(json_object_t));
    if (!o) return NULL;
    o->data = malloc(sizeof(json_value_t*) * capacity);
    if (!o->data) {
        free(o);
        return NULL;
    }
    o->len = 0;
    o->cap = capacity;
    return o;
}

static void object_destroy(json_object_t* obj) {
    if (!obj) return;
    for (size_t i = 0; i < obj->len; i++) {
        json_value_destroy(obj->data[i]);
    }
    free(obj);
}

json_value_t* json_value_create(json_type_t type) {
    json_value_t* v = malloc(sizeof(json_value_t));
    if (!v) return NULL;
    v->type = type;
    switch(v->type) {
    case JSON_BOOL: {
        v->as.boolean = false;
        break;
    }
    case JSON_NUMBER: {
        v->as.number = 0;
        break;
    }
    case JSON_STRING: {
        v->as.string = NULL;
        break;
    }
    case JSON_ARRAY: {
        v->as.array = NULL;
        break;
    }
    case JSON_OBJECT: {
        v->as.object = NULL;
        break;
    }
    default: break;
    }
    return v;
}

void json_value_destroy(json_value_t* v) {
    if (!v) return;
    switch (v->type) {
    case JSON_STRING: {
        text_destroy(v->as.string);
        break;
    }
    case JSON_ARRAY: {
        array_destroy(v->as.array);
        break;
    }
    case JSON_OBJECT: {
        object_destroy(v->as.object);
        break;
    }
    default:
        break;
    }
    free(v);
}

typedef struct parser {
    const uint8_t* cur;
    const uint8_t* start;
    const uint8_t* end;
    size_t start_line;
    size_t start_col;
    size_t line;
    size_t col;
} parser_t;

void parser_init(parser_t* p, const u8* buffer, size_t len) {
    p->cur = buffer;
    p->start = buffer;
    p->end = buffer + len;
    p->start_line = 1;
    p->start_col = 0;
    p->line = 1;
    p->col = 0;
}

bool at_end(parser_t* p) {
    return p->cur >= p->end;
}

u8 peek(parser_t* p) {
    return at_end(p) ? '\0' : *p->cur;
}

void advance(parser_t* p) {
    u8 c = peek(p);
    if (c == '\0') return;
    if (c == '\n') {
        p->line++;
        p->col = 0;
    }
    else p->col++;
    p->cur++;
}

void skip_ws(parser_t* p) {
    while (1) {
        u8 c = peek(p);
        if (c != '\n' && c != '\r' && c != '\t' && c != ' ') break;
        advance(p);
    }
}

json_value_t* parse_object(parser_t* p) {
    return NULL;
}

json_value_t* parse_array(parser_t* p) {
    return NULL;
}

json_value_t* parse_string(parser_t* p) {
    return NULL;
}

json_value_t* parse_literal(parser_t* p, const char* pattern, size_t len) {
    for (size_t i = 0; i < len; i++) {
        u8 c = peek(p);
        if (c != pattern[i]) return NULL;
        advance(p);
    }
    json_value_t* v = json_value_create(JSON_BOOL);
    if (!v) return NULL;
    if (pattern[0] == 'n') return v;
    v->as.boolean = len == 4;
    return v;
}

json_value_t* parse_number(parser_t* p) {
    return NULL;
}

bool is_digit(u8 c) {
    return c >= '0' && c <= '9';
}

json_value_t* parse_value(parser_t* p) {
    skip_ws(p);
    u8 c = peek(p);
    switch (c) {
    case '{': return parse_object(p);
    case '[': return parse_array(p);
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

json_value_t* json_value_parse(const u8* buffer, size_t len) {
    if (!buffer || len == 0) return NULL;
    parser_t parser; parser_init(&parser, buffer, len);
    return parse_value(&parser);
}
