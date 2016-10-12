#ifndef KUMA_LEXER_H
#define KUMA_LEXER_H

#include "stdlib.h"
#include "stdio.h"

typedef struct klist_s klist;
typedef struct klist_node_s klist_node;

struct klist_node_s
{
    void * data;
    klist_node *next;
    klist_node *prev;
};

struct klist_s
{
    size_t size;
    klist_node *head;
    klist_node *tail;
};

klist * klist_new();
void klist_destroy(klist *list);

klist_node * klist_add_node_at(klist *list, size_t index);
klist_node * klist_add_node_first(klist *list);
klist_node * klist_add_node_last(klist *list);

#define klist_add(list, v) \
    { klist_node *node = klist_add_node_last(list); \
    node->data = (void*)v; }

#define klist_add_at(list, v, index) \
    { klist_node *node = klist_add_node_at(list, index); \
    node->data = (void*)v; }

#define klist_add_first(list, v) \
    { klist_node *node = klist_add_node_first(list); \
    node->data = (void*)v; }

#define klist_add_last(list, v) \
    { klist_node *node = klist_add_node_last(list); \
    node->data = (void*)v; }

void * klist_remove_node(klist *list, klist_node *node);
void * klist_remove_node_at(klist *list, size_t index);
void * klist_remove_node_first(klist *list);
void * klist_remove_node_last(klist *list);

#define klist_remove(list, v) \
    { klist_node *node = klist_find_node(list, (void*)v); \
    klist_remove_node(list, node);}

#define klist_remove_at(list, index) \
    klist_remove_node_at(list, index);

#define klist_remove_first(list) \
    klist_remove_node_first(list);

#define klist_remove_last(list) \
    klist_remove_node_last(list);

void klist_remove_all(klist *list);
void klist_remove_all_free(klist *list);

#define klist_get_at(t, list, index) \
    (t)(klist_get_node_at(list, index)->data)

#define klist_get_first(t, list) \
    (t)(klist_get_node_first(list)->data)

#define klist_get_last(t, list) \
    (t)(klist_get_node_last(list)->data)

size_t klist_size(klist *list);

klist_node * klist_get_node_at(klist *list, size_t index);
klist_node * klist_get_node_first(klist *list);
klist_node * klist_get_node_last(klist *list);
klist_node * klist_get_node_next(klist_node *node);
klist_node * klist_get_node_prev(klist_node *node);
klist_node * klist_find_node(klist *list, void *element);

#endif
