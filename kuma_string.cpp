//
// Created by Christopher Redden on 09/08/15.
//

#include "kuma_string.h"
#include <stdlib.h>
#include <string.h>

u_int8_t * kuma_string_create()
{
    __ks__string__type *kstring = (__ks__string__type*)malloc(sizeof(__ks__string__type));
    kstring->buf = 0;
    kstring->string_len = 0;
    kstring->buffer_size = 0;

    return (u_int8_t*)kstring;
}

void kuma_string_destroy(u_int8_t *kstring)
{
    __ks__string__type *kstring_ptr = (__ks__string__type*)kstring;

    if(kstring_ptr->buf != 0)
    {
        free(kstring_ptr->buf);
    }

    free(kstring);
}

void kuma_string_resize(u_int8_t *kstring, u_int32_t size)
{
    __ks__string__type *kstring_ptr = (__ks__string__type*)kstring;
    if(kstring_ptr->string_len == size)
    {
        return;
    }

    u_int32_t buf_size = (KUMA_STRING_GROW_FACTOR - (size % KUMA_STRING_GROW_FACTOR)) % KUMA_STRING_GROW_FACTOR;

    u_int8_t *output = (u_int8_t*)malloc(buf_size);
    if(kstring_ptr->buf != 0)
    {
        memcpy(output, kstring_ptr->buf, kstring_ptr->buffer_size);
        free(kstring_ptr->buf);
    }

    kstring_ptr->buf = output;
    kstring_ptr->buffer_size = buf_size;
}

void kuma_string_add_char(u_int8_t *kstring, u_int8_t char_value)
{
    __ks__string__type *kstring_ptr = (__ks__string__type*)kstring;

    if(kstring_ptr->string_len >= kstring_ptr->buffer_size)
    {
        kuma_string_resize(kstring, kstring_ptr->string_len+1);
    }

    kstring_ptr->string_len++;
    kstring_ptr->buf[kstring_ptr->string_len] = char_value;
}

void kuma_string_append(u_int8_t *kstring_dest, u_int8_t *kstring_src)
{
    __ks__string__type *kstring_dest_ptr = (__ks__string__type*)kstring_dest;
    __ks__string__type *kstring_src_ptr = (__ks__string__type*)kstring_src;

    u_int32_t new_size = (kstring_dest_ptr->string_len + kstring_src_ptr->string_len);
    if(kstring_dest_ptr->buffer_size < new_size)
    {

        kuma_string_resize(kstring_dest, new_size);
    }

    for(u_int32_t i = 0; i < kstring_src_ptr->string_len; i++)
    {
        kuma_string_add_char(kstring_dest, kstring_src_ptr->buf[i]);
    }
}

void kuma_string_copy(u_int8_t *kstring_dest, u_int8_t *kstring_src)
{
    __ks__string__type *kstring_dest_ptr = (__ks__string__type *) kstring_dest;
    __ks__string__type *kstring_src_ptr = (__ks__string__type *) kstring_src;

    if (kstring_dest_ptr->buf != 0)
    {
        free(kstring_dest_ptr->buf);
    }

    kstring_dest_ptr->string_len = kstring_src_ptr->string_len;
    kstring_dest_ptr->buffer_size = kstring_src_ptr->buffer_size;
    kstring_dest_ptr->buf = (u_int8_t*) malloc(kstring_dest_ptr->buffer_size);

    memcpy(kstring_dest_ptr->buf, kstring_src_ptr->buf, kstring_dest_ptr->buffer_size);
}

void kuma_string_set(u_int8_t *kstring_dest, const char *src)
{
    __ks__string__type *kstring_dest_ptr = (__ks__string__type *) kstring_dest;

    if (kstring_dest_ptr->buf != 0)
    {
        free(kstring_dest_ptr->buf);
    }

    kstring_dest_ptr->string_len = strlen(src);
    kstring_dest_ptr->buffer_size = strlen(src) + 1;
    kstring_dest_ptr->buf = (u_int8_t*) malloc(kstring_dest_ptr->buffer_size);

    memcpy(kstring_dest_ptr->buf, src, kstring_dest_ptr->buffer_size);
}