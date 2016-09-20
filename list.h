#ifndef KUMA_LEXER_H
#define KUMA_LEXER_H

#include "stdlib.h"
#include "stdio.h"

typedef struct kuma_list_s kuma_list_t;
typedef struct kuma_node_s kuma_list_node_t;

kuma_list_t * kuma_list_new();
void kuma_list_destroy(kuma_list_t *list);

void kuma_list_add(kuma_list_t *list, void *element);
void kuma_list_add_at(kuma_list_t *list, void *element, size_t index);
void kuma_list_add_first(kuma_list_t *list, void *element);
void kuma_list_add_last(kuma_list_t *list, void *element);

void * kuma_list_remove(kuma_list_t *list, void *element);
void * kuma_list_remove_at(kuma_list_t *list, void *element, size_t index);
void * kuma_list_remove_first(kuma_list_t *list);
void * kuma_list_remove_last(kuma_list_t *list);

void kuma_list_remove_all(kuma_list_t *list);
void kuma_list_remove_all_free(kuma_list_t *list);

void * kuma_list_get_at(kuma_list_t *list, size_t index);
void * kuma_list_get_first(kuma_list_t *list);
void * kuma_list_get_last(kuma_list_t *list);
void * kuma_list_get_node(kuma_list_t *list, kuma_list_node_t *node);

size_t kuma_list_size(kuma_list_t *list);

kuma_list_node_t * kuma_list_node_at(kuma_list_t *list, size_t index);
kuma_list_node_t * kuma_list_node_first(kuma_list_t *list);
kuma_list_node_t * kuma_list_node_last(kuma_list_t *list);
kuma_list_node_t * kuma_list_node_next(kuma_list_node_t *node);
kuma_list_node_t * kuma_list_node_prev(kuma_list_node_t *node);

#define INLINE static inline

#define KUMA_LIST_INSTANCE(Name, T, FName) \
    typedef kuma_list_t Name; \
    INLINE Name* FName##_create() { return NULL; } \
    INLINE Name* FName##_destroy(Name *a) { kuma_list_free(a); return NULL; } \
    INLINE Name* FName##_append(Name* a, T b) { return kuma_list_append(a, (void *)b); } \
    INLINE Name* FName##_prepend(Name* a, T b) { return kuma_list_prepend(a, (void *)b); } \
    INLINE Name* FName##_first(Name* a) { return kuma_list_first(a); } \
    INLINE Name* FName##_last(Name* a) { return kuma_list_last(a); } \
    INLINE Name* FName##_next(Name* a) { return kuma_list_next(a); } \
    INLINE Name* FName##_prev(Name* a) { return kuma_list_previous(a); } \
    INLINE int   FName##_length(Name* a) { return kuma_list_length(a); } \
    INLINE T     FName##_get(Name* a, int b) { return (T)kuma_list_nth_data(a, b); } \
    INLINE T     FName##_data(Name* a) { return (T)(a->data); }

#endif
