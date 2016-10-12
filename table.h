#ifndef KUMA_TABLE_H
#define KUMA_TABLE_H

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "limits.h"

#define INITIAL_SIZE 1024

typedef struct ktable_s ktable;
typedef struct ktable_pair_s ktable_pair;
typedef struct ktable_bucket_s ktable_bucket;

struct ktable_pair_s
{
    char *key;
    size_t keylen;
    void *data;

    ktable_pair *next;
    ktable_pair *prev;
};

struct ktable_bucket_s
{
    size_t size;
    ktable_pair *head;
    ktable_pair *tail;
};

struct ktable_s
{
    size_t size;
    ktable_bucket *buckets;
};

uint32_t ktable_hash(char *key)
{
    uint32_t hash, i;
    for(hash = i = 0; i < strlen(key); ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

ktable * ktable_new_size(size_t initial_size)
{
    if(initial_size < 1) return NULL;

    ktable *table = calloc(1, sizeof(ktable));

    table->buckets = calloc(initial_size, sizeof(ktable_bucket));

    for(int i = 0; i < initial_size; i++)
    {
        table->buckets[i].size = 0;
        table->buckets[i].head = NULL;
        table->buckets[i].tail = NULL;
    }

    table->size = initial_size;

    return table;
}

ktable * ktable_new()
{
    return ktable_new_size(INITIAL_SIZE);
}

void ktable_destroy(ktable *table)
{
    if(table->size > 0)
    {
        for(size_t i = 0; i < table->size; i++)
        {
            ktable_bucket *bucket = &(table->buckets[i]);

            if(bucket->size == 0)
                continue;

            ktable_pair *pair = bucket->head;

            while (pair != NULL)
            {
                ktable_pair *tmp = pair;
                free(pair->key);
                pair = pair->next;
                free(tmp);
            }
        }

        free(table->buckets);
    }

    free(table);
}

ktable_pair * ktable_new_pair(char *key, void *element)
{
    ktable_pair *pair = calloc(1, sizeof(ktable_pair));

    pair->keylen = strlen(key);
    pair->key = memcpy(malloc(pair->keylen), key, pair->keylen);
    pair->data = element;
    pair->next = NULL;
    pair->prev = NULL;

    return pair;
}

ktable_pair * ktable_get_pair(ktable_bucket *bucket, char *key)
{
    ktable_pair *pair = bucket->head;
    size_t keylen = strlen(key);

    while (pair)
    {
        if (pair->keylen == keylen)
        {
            if(strcmp(pair->key, key) == 0)
            {
                return pair;
            }
        }

        pair = pair->next;
    }

    return NULL;
}

void ktable_set(ktable *table, char *key, void *element)
{
    size_t index = ktable_hash(key) % table->size;
    ktable_bucket *bucket = &(table->buckets[index]);

    if(bucket->size == 0)
    {
        ktable_pair *pair = ktable_new_pair(key, element);
        bucket->head = pair;
        bucket->tail = pair;
        bucket->size++;
    }
    else
    {
        ktable_pair *pair = ktable_get_pair(bucket, key);

        if(pair == NULL)
        {
            ktable_pair *pair = ktable_new_pair(key, element);
            pair->prev = bucket->tail;
            bucket->tail->next = pair;
            bucket->tail = pair;
            bucket->size++;
        }
        else
        {
            pair->data = element;
        }
    }
}

void * ktable_get(ktable *table, char *key)
{
    size_t index = ktable_hash(key) % table->size;
    ktable_bucket *bucket = &(table->buckets[index]);
    ktable_pair *pair = ktable_get_pair(bucket, key);

    if(pair != NULL)
    {
        return pair->data;
    }

    return NULL;
}

void ktable_remove(ktable *table, char *key)
{
    size_t index = ktable_hash(key) % table->size;
    ktable_bucket *bucket = &(table->buckets[index]);
    ktable_pair *pair = ktable_get_pair(bucket, key);

    if(pair != NULL)
    {
        if (pair->prev != NULL)
            pair->prev->next = pair->next;

        if (pair->prev == NULL)
            bucket->head = pair->next;

        if (pair->next == NULL)
            bucket->tail = pair->prev;

        if (pair->next != NULL)
            pair->next->prev = pair->prev;

        free(pair->key);
        free(pair);
        bucket->size--;
    }
}

int ktable_exists(ktable *table, char *key)
{
    if(table == NULL)
    {
        return 0;
    }

    if(key == NULL)
    {
        return 0;
    }

    size_t index = ktable_hash(key) % table->size;
    ktable_bucket *bucket = &(table->buckets[index]);
    ktable_pair *pair = ktable_get_pair(bucket, key);

    if(pair == NULL)
    {
        return 0;
    }

    return 1;
}

#endif
