/*
    Copyright (C) 2015-2023, Navaro, All Rights Reserved
    SPDX-License-Identifier: MIT

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

#include "collection.h"

#include <string.h>
#include "../port/port.h"

#define COLLECTION_MALLOC(heap, size)           engine_port_malloc (heap, size)
#define COLLECTION_FREE(heap, mem)              engine_port_free (heap, mem)


typedef struct clist *  (*COLLECTION_KEYVAL_ALLOC_T)(struct collection * /* coll */, const char * /* s */, unsigned int /* keysize */, unsigned int /* valuesize */) ;
typedef void            (*COLLECTION_KEYVAL_FREE_T)(struct collection * /* coll */, struct clist * /* np */) ;
typedef unsigned int    (*COLLECTION_KEY_HASH_T)(struct collection * /* coll */, const char * /* s */, unsigned int /* keysize */) ;
typedef unsigned int    (*COLLECTION_KEY_CMP_T)(struct collection * /* coll */, struct clist * /* np */, const char * /* s */, unsigned int /* keysize */) ;
typedef const char*     (*COLLECTION_KEY_T)(struct collection * /* coll */, struct clist * /* np */) ;
typedef char*           (*COLLECTION_VALUE_T)(struct collection * /* coll */, struct clist * /* np */) ;


struct collection_keyval {
    COLLECTION_KEYVAL_ALLOC_T       alloc ;
    COLLECTION_KEYVAL_FREE_T        free ;
    COLLECTION_KEY_HASH_T           hash ;
    COLLECTION_KEY_CMP_T            cmp ;
    COLLECTION_KEY_T                key ;
    COLLECTION_VALUE_T              value ;
} ;

struct collection {
    const struct collection_keyval * key ;
    unsigned int                    hashsize ;
    unsigned int                    count ;
    collectionheap                  heap ;
    struct clist *                  hashtab[]; /* pointer table */
} ;


struct clist *
collection_str_keyval_alloc (struct collection * coll, const char *s,
                unsigned int keysize, unsigned int valuesize)
{
    char *p;
    struct clist * np ;

    if (s == 0) return 0 ;
    np = (struct clist *) COLLECTION_MALLOC(coll->heap,
            sizeof(struct clist) +
            sizeof(uintptr_t) +
            valuesize);
    if (!np) return 0 ;
    if (keysize == 0) keysize = strlen(s) ;
    p = (char *) COLLECTION_MALLOC(coll->heap, keysize+1); /* +1 for ?\0? */
    if (!p ) {
        COLLECTION_FREE(coll->heap, np);
        return 0;
    }
   strncpy(p, s, keysize);
   p[keysize] = '\0' ;
   np->keyval[0] = (uintptr_t)p ;
   return np ;
}

void
collection_str_keyval_free(struct collection * coll, struct clist *np)
{
    COLLECTION_FREE (coll->heap, (char *)np->keyval[0]) ;
    COLLECTION_FREE (coll->heap, np) ;
}


unsigned int
collection_str_key_hash(struct collection * coll, const char *s, unsigned int keysize)
{
    unsigned hashval;
    if (s == 0) return 0 ;
    if (keysize == 0) keysize = strlen(s) ;
    for (hashval = 0; (*s != '\0') && keysize; s++, keysize--) {
      hashval = *s + 31 * hashval;
    }
    return hashval % coll->hashsize;
}

unsigned int
collection_str_key_cmp(struct collection * coll, struct clist *np,
                const char *s, unsigned int keysize)
{
    char* p = (char*)np->keyval[0] ;
    if (s == p) return 1 ;// NULL key
    if (keysize == 0) keysize = strlen(s) ;
    if ((strncmp(s, p, keysize) == 0) && (p[keysize] == 0)) {
        return 1 ;
    }

    return 0 ;
}

const char*
collection_str_key(struct collection * coll, struct clist *np)
{
    return (const char*)np->keyval[0] ;
}

char*
collection_str_value(struct collection * coll, struct clist *np)
{
    return (char*)&np->keyval[1] ;
}

static struct clist *
dict_remove (struct collection * coll, const char *s, unsigned int keysize) {
    struct clist *np;
    struct clist *prev = 0 ;
    unsigned hashval = coll->key->hash (coll, s, keysize) ;
    for (np = coll->hashtab[hashval]; np != 0; np = np->next) {
        if (coll->key->cmp(coll, np, s, keysize)) {
          break ; /* found */
        }
        prev = np ;
    }
    if (np) {
        coll->count-- ;
        if (prev) {
            prev->next = np->next ;
        }
        else {
            coll->hashtab[hashval] = np->next ;
        }
    }
    return np;

}

static struct clist *
dict_lookup (struct collection * coll, const char *key, unsigned int keysize)
{
    struct clist *np;
    for (np = coll->hashtab[coll->key->hash(coll, key, keysize)];
                np != 0; np = np->next) {
        if (coll->key->cmp(coll, np, key, keysize)) {
          return np; /* found */
        }
    }
    return 0; /* not found */
}

struct collection *
collection_init (collectionheap heap, unsigned int hashsize)
{
    static const struct collection_keyval str_key = {
            &collection_str_keyval_alloc,
            &collection_str_keyval_free,
            &collection_str_key_hash,
            &collection_str_key_cmp,
            &collection_str_key,
            &collection_str_value

    };

    struct collection * coll ;
#define DICTSIZE(hashsize)      (sizeof(struct collection) + \
                                sizeof(struct clist *) * hashsize)

     coll =   (struct collection *) COLLECTION_MALLOC(heap, DICTSIZE(hashsize)) ;
     if (coll) {
        memset (coll,0,DICTSIZE(hashsize)) ;

        coll->key = &str_key ;
        coll->hashsize = hashsize ;
        coll->heap = heap ;
     }
    return coll ;
}

struct clist*
collection_install_size(struct collection * coll, const char *key,
                unsigned int keysize, unsigned int valuesize)
{
    struct clist *np;
    unsigned hashval;
     if ((np = dict_lookup(coll, key, keysize)) == 0) { /* not found */
        np = coll->key->alloc(coll, key, keysize, valuesize) ;
        if (np == 0) return 0;
        hashval = coll->key->hash(coll, key, keysize);
        np->next = coll->hashtab[hashval];
        coll->hashtab[hashval] = np;
        coll->count++ ;
    }

    return np ;
}

struct clist*
collection_replace(struct collection * coll, const char *key,
                unsigned int keysize, const char *value, unsigned int valuesize)
{
    struct clist*  np = collection_install_size(coll, key,  keysize,  valuesize) ;
    if (!np) return 0 ;
    char* p = coll->key->value(coll, np);
    memcpy (p, value, valuesize) ;
    return np ;
}

struct clist*
collection_lookup(struct collection * coll, const char *key,
                unsigned int keysize, const char *value, unsigned int valuesize)
{
    struct clist *np;
    unsigned hashval;
     if ((np = dict_lookup(coll, key, keysize)) == 0) { /* not found */
        np = coll->key->alloc(coll, key, keysize, valuesize) ;
        if (np == 0) return 0;
        hashval = coll->key->hash(coll, key, keysize);
        np->next = coll->hashtab[hashval];
        coll->hashtab[hashval] = np;
        coll->count++ ;
        char* p = coll->key->value(coll, np);
        memcpy (p, value, valuesize) ;

    }

    return np ;
}


struct clist*
collection_get(struct collection * coll, const char *key, unsigned int keysize)
{
    return dict_lookup(coll, key, keysize) ;
}

unsigned int
collection_remove(struct collection * coll, const char *key, unsigned int keysize)
{
    struct clist *np;

    if ((np = dict_remove(coll, key, keysize)) == 0) { /* not found */
        return 0 ;
    } 

    coll->key->free (coll, np) ;

    return 1;
}

void
collection_remove_all(struct collection * coll,
            void (*cb)(struct collection *, struct clist*, uint32_t), uint32_t parm)
{
    struct clist *np;
    unsigned  i ;
    for (i=0; i<coll->hashsize; i++) {
     for (np = coll->hashtab[i]; np != 0; np = coll->hashtab[i]) {
         if (cb) {
             cb (coll, np, parm) ;
         }
         coll->hashtab[i] = np->next ;
         coll->key->free (coll, np) ;
         coll->count-- ;
     }

    }
}

void
collection_destroy(struct collection * coll)
{
    collection_remove_all (coll, 0, 0) ;
    COLLECTION_FREE (coll->heap, coll) ;
}

struct clist*
collection_it_first (struct collection * coll, struct collection_it* it)
{
    it->p = coll->hashtab[0];
    it->idx = 0 ;
    if (it->p) return it->p ;

    return collection_it_next (coll, it) ;
}

struct clist*
collection_it_next (struct collection * coll, struct collection_it* it)
{
   struct clist *np;
    unsigned  i ;

    if (it->p && it->p->next) {
        it->p = it->p->next ;
        return it->p ;

    }
     for (i=it->idx+1; i<coll->hashsize; i++) {
         for (np = coll->hashtab[i]; np != 0; ) {
                 it->p = np ;
                 it->idx = i ;
                 return np ;
 
         }

     }

     return 0 ;

}

struct clist*
collection_it_at (struct collection * coll, const char *key,
                unsigned int len, struct collection_it* it)
{
    it->idx = coll->key->hash(coll, key, len) ;
    it->p = dict_lookup(coll, key, len) ;
    return it->p ;
}

struct clist*
collection_it_get (struct collection * coll, struct collection_it* it)
{
    return it->p ;
}

const char*
collection_get_key (struct collection * coll, struct clist* np)
{
    return coll->key->key (coll, np) ;
}

char*
collection_get_value (struct collection * coll, struct clist* np)
{
    return coll->key->value (coll, np) ;
}

unsigned int
collection_count (struct collection * coll)
{
    return coll->count ;
}

unsigned int
collection_hashtab_size (struct collection * coll)
{
    return coll->hashsize ;
}

unsigned int
collection_hashtab_cnt (struct collection * coll, unsigned int idx)
{
    struct clist *np;
    unsigned int cnt = 0 ;
    for (np = coll->hashtab[idx]; np != 0; np = np->next) {
        cnt++;

    }
    return cnt ;
}

