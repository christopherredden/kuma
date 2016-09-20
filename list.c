#include "list.h"

struct kuma_node_s
{
    void * data;
    kuma_list_node_t *next;
    kuma_list_node_t *prev;
};

struct kuma_list_s
{
    size_t size;
    kuma_list_node_t *head;
    kuma_list_node_t *tail;
};

static void link_behind(kuma_list_node_t *const base, kuma_list_node_t *ins)
{
    if (ins->next != NULL)
        ins->next->prev = ins->prev;

    if (ins->prev != NULL)
        ins->prev->next = ins->next;

    if (base->prev == NULL) 
    {
        ins->prev       = NULL;
        ins->next       = base;
        base->prev      = ins;
    } 
    else 
    {
        ins->prev       = base->prev;
        ins->prev->next = ins;
        ins->next       = base;
        base->prev      = ins;
    }
}

static void link_after(kuma_list_node_t *base, kuma_list_node_t *ins)
{
    if (ins->next)
        ins->next->prev = ins->prev;

    if (ins->prev)
        ins->prev->next = ins->next;

    if (!base->next) 
    {
        ins->prev       = base;
        base->next      = ins;
        ins->next       = NULL;
    } 
    else 
    {
        ins->next       = base->next;
        ins->next->prev = ins;
        ins->prev       = base;
        base->next      = ins;
    }
}

static void swap_adjacent(kuma_list_node_t *n1, kuma_list_node_t *n2)
{
    if (n1->next == n2) {
        if (n2->next)
            n2->next->prev = n1;

        n1->next = n2->next;

        if (n1->prev)
            n1->prev->next = n2;

        n2->prev = n1->prev;

        n1->prev = n2;
        n2->next = n1;
        return;
    }

    if (n2->next == n1) {
        if (n1->next)
            n1->next->prev = n2;

        n2->next = n1->next;

        if (n2->prev)
            n2->prev->next = n1;

        n1->prev = n2->prev;

        n2->prev = n1;
        n1->next = n2;
        return;
    }
}

static void swap(kuma_list_node_t *n1, kuma_list_node_t *n2)
{
    if (n1->next == n2 || n2->next == n1) 
    {
        swap_adjacent(n1, n2);
        return;
    }

    kuma_list_node_t *n1_left  = n1->prev;
    kuma_list_node_t *n1_right = n1->next;
    kuma_list_node_t *n2_left  = n2->prev;
    kuma_list_node_t *n2_right = n2->next;

    if (n1_left)
        n1_left->next = n2;

    n2->prev = n1_left;

    if (n1_right)
        n1_right->prev = n2;

    n2->next = n1_right;

    if (n2_left)
        n2_left->next = n1;

    n1->prev = n2_left;

    if (n2_right)
        n2_right->prev = n1;

    n1->next = n2_right;
}

static void *unlink(kuma_list_t *list, kuma_list_node_t *node)
{
    void *data = node->data;

    if (node->prev != NULL)
        node->prev->next = node->next;

    if (node->prev == NULL)
        list->head = node->next;

    if (node->next == NULL)
        list->tail = node->prev;

    if (node->next != NULL)
        node->next->prev = node->prev;

    free(node);
    list->size--;

    return data;
}

static int unlink_all(kuma_list_t *list, int freed)
{
    if (list->size == 0)
        return 0;

    kuma_list_node_t *node = list->head;

    while (node) 
    {
        kuma_list_node_t *tmp = node->next;

        if (freed)
        {
            free(node->data);
        }

        unlink(list, node);
        node = tmp;
    }

    return 1;
}

static kuma_list_node_t * get_node(kuma_list_t *list, void *element)
{
    kuma_list_node_t *node = list->head;
    while (node) 
    {
        if (node->data == element)
        {
            return node;
        }

        node = node->next;
    }
    return NULL;
}

static kuma_list_node_t * get_node_at(kuma_list_t *list, size_t index)
{
    size_t i;
    kuma_list_node_t *node = NULL;

    if (index < list->size / 2) 
    {
        node = list->head;
        for (i = 0; i < index; i++)
        {
            node = node->next;
        }
    }
    else 
    {
        node = list->tail;
        for (i = list->size - 1; i > index; i--)
        {
            node = node->prev;
        }
    }

    return node;
}

kuma_list_t * kuma_list_new()
{
    kuma_list_t *list = calloc(1, sizeof(kuma_list_t));

    return list;
}

void kuma_list_destroy(kuma_list_t *list)
{
    if(list->size > 0)
    {
        kuma_list_remove_all(list);
    }

    free(list);
}

void kuma_list_add(kuma_list_t *list, void *element)
{
    kuma_list_add_last(list, element);
}

void kuma_list_add_at(kuma_list_t *list, void *element, size_t index)
{
    kuma_list_node_t *base = kuma_list_node_at(list, index);

    kuma_list_node_t *node = calloc(1, sizeof(kuma_list_node_t));

    node->data = element;

    link_behind(base, node); 

    if(index == 0)
    {
        list->head = node;
    }

    list->size++;
}

void kuma_list_add_first(kuma_list_t *list, void *element)
{
    kuma_list_node_t *node = calloc(1, sizeof(kuma_list_node_t));

    if(list->size == 0)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }

    list->size++;
}

void kuma_list_add_last(kuma_list_t *list, void *element)
{
    kuma_list_node_t *node = calloc(1, sizeof(kuma_list_node_t));

    node->data = element;

    if(list->size == 0)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;
}

void * kuma_list_remove(kuma_list_t *list, void *element)
{
    kuma_list_node_t *node = get_node(list, element);

    void *data = node->data;
    unlink(list, node);

    return data;
}

void * kuma_list_remove_at(kuma_list_t *list, void *element, size_t index)
{
    kuma_list_node_t *node = get_node_at(list, index);

    void *data = node->data;
    unlink(list, node);

    return data;
}

void * kuma_list_remove_first(kuma_list_t *list)
{
    void *element = unlink(list, list->head);

    return element;
}

void * kuma_list_remove_last(kuma_list_t *list)
{
    void *element = unlink(list, list->tail);

    return element;
}

void kuma_list_remove_all(kuma_list_t *list)
{
    int unlinked = unlink_all(list, 0);

    if(unlinked)
    {
        list->head = NULL;
        list->tail = NULL;
    }
}

void kuma_list_remove_all_free(kuma_list_t *list)
{
    int unlinked = unlink_all(list, 1);

    if(unlinked)
    {
        list->head = NULL;
        list->tail = NULL;
    }
}

void * kuma_list_get_at(kuma_list_t *list, size_t index)
{
    kuma_list_node_t *node = get_node_at(list, index);

    return node->data;
}

void * kuma_list_get_first(kuma_list_t *list)
{
    if(list->size == 0)
    {
        return NULL;
    }

    return list->head->data;
}

void * kuma_list_get_last(kuma_list_t *list)
{
    if(list->size == 0)
    {
        return NULL;
    }

    return list->tail->data;
}

void * kuma_list_get_node(kuma_list_t *list, kuma_list_node_t *node)
{
    return node->data;
}

size_t kuma_list_size(kuma_list_t *list)
{
    return list->size;
}

kuma_list_node_t * kuma_list_node_at(kuma_list_t *list, size_t index)
{
    return get_node_at(list, index);
}

kuma_list_node_t * kuma_list_node_first(kuma_list_t *list)
{
    return list->head;
}

kuma_list_node_t * kuma_list_node_last(kuma_list_t *list)
{
    return list->tail;
}

kuma_list_node_t * kuma_list_node_next(kuma_list_node_t *node)
{
    return node->next;
}

kuma_list_node_t * kuma_list_node_prev(kuma_list_node_t *node)
{
    return node->prev;
}
