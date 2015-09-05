//
// Created by Christopher Redden on 09/08/15.
//

#ifndef KUMA_KUMA_STRING_H
#define KUMA_KUMA_STRING_H

#include <i386/types.h>

typedef struct
{
    u_int8_t *buf;
    u_int32_t buffer_size;
    u_int32_t string_len;
} __ks__string__type;

#define KUMA_STRING_GROW_FACTOR 16

extern "C"
{

u_int8_t *kuma_string_create();
void kuma_string_destroy(u_int8_t *kstring);
void kuma_string_resize(u_int8_t *kstring, u_int32_t size);
void kuma_string_add_char(u_int8_t *kstring, u_int8_t char_value);
void kuma_string_append(u_int8_t *kstring_dest, u_int8_t *kstring_src);
void kuma_string_copy(u_int8_t *kstring_dest, u_int8_t *kstring_src);
void kuma_string_set(u_int8_t *kstring_dest, const char *src);

};

#endif //KUMA_KUMA_STRING_H
