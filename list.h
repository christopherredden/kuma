#ifndef KUMA_LEXER_H
#define KUMA_LEXER_H

#include "stdlib.h"
#include "stdio.h"

typedef struct klist_s klist;
typedef struct klist_node_s klist_node;

klist * klist_new();
void klist_destroy(klist *list);

void klist_add(klist *list, void *element);
void klist_add_at(klist *list, void *element, size_t index);
void klist_add_first(klist *list, void *element);
void klist_add_last(klist *list, void *element);

void * klist_remove(klist *list, void *element);
void * klist_remove_at(klist *list, void *element, size_t index);
void * klist_remove_first(klist *list);
void * klist_remove_last(klist *list);

void klist_remove_all(klist *list);
void klist_remove_all_free(klist *list);

void * klist_get_at(klist *list, size_t index);
void * klist_get_first(klist *list);
void * klist_get_last(klist *list);
void * klist_get_node(klist *list, klist_node *node);

size_t klist_size(klist *list);

klist_node * klist_node_at(klist *list, size_t index);
klist_node * klist_node_first(klist *list);
klist_node * klist_node_last(klist *list);
klist_node * klist_node_next(klist_node *node);
klist_node * klist_node_prev(klist_node *node);

#define INLINE static inline

#define KUMA_LIST_INSTANCE(Name, T, FName) \
    typedef klist Name; \
    INLINE Name* FName##_create() { return NULL; } \
    INLINE Name* FName##_destroy(Name *a) { klist_free(a); return NULL; } \
    INLINE Name* FName##_append(Name* a, T b) { return klist_append(a, (void *)b); } \
    INLINE Name* FName##_prepend(Name* a, T b) { return klist_prepend(a, (void *)b); } \
    INLINE Name* FName##_first(Name* a) { return klist_first(a); } \
    INLINE Name* FName##_last(Name* a) { return klist_last(a); } \
    INLINE Name* FName##_next(Name* a) { return klist_next(a); } \
    INLINE Name* FName##_prev(Name* a) { return klist_previous(a); } \
    INLINE int   FName##_length(Name* a) { return klist_length(a); } \
    INLINE T     FName##_get(Name* a, int b) { return (T)klist_nth_data(a, b); } \
    INLINE T     FName##_data(Name* a) { return (T)(a->data); }

#endif
