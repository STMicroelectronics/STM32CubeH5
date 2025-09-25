/*
    Copyright (C) 2015-2025, Navaro, All Rights Reserved
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

#include <string.h>
#include <stdint.h>
#include "qoraal/qoraal.h"
#include "qoraal/common/dictionary.h"


#define DICTIONARY_KEY_SIZE(keyspec)            (keyspec & 0xFFFF)

typedef struct dlist *  (*DICTIONARY_KEYVAL_ALLOC_T)(struct dictionary * /* dict */, const char * /* s */, unsigned int /* valuesize */) ;
typedef void            (*DICTIONARY_KEYVAL_FREE_T)(struct dictionary * /* dict */, struct dlist * /* np */) ;
typedef unsigned int    (*DICTIONARY_KEY_HASH_T)(struct dictionary * /* dict */, const char * /* s */) ;
typedef unsigned int    (*DICTIONARY_KEY_CMP_T)(struct dictionary * /* dict */, struct dlist * /* np */, const char * /* s */) ;
typedef const char*     (*DICTIONARY_KEY_T)(struct dictionary * /* dict */, struct dlist * /* np */) ;
typedef char*           (*DICTIONARY_VALUE_T)(struct dictionary * /* dict */, struct dlist * /* np */) ;


struct dictionary_keyval {
    DICTIONARY_KEYVAL_ALLOC_T       alloc ;
    DICTIONARY_KEYVAL_FREE_T        free ;
    DICTIONARY_KEY_HASH_T           hash ;
    DICTIONARY_KEY_CMP_T            cmp ;
    DICTIONARY_KEY_T                key ;
    DICTIONARY_VALUE_T              value ;
} ;

struct dictionary {
    const struct dictionary_keyval * key ;
    unsigned int                    hashsize ;
    unsigned int                    keyspec ;
    unsigned int                    count ;
    uintptr_t                       heap ;
    struct dlist *                  hashtab[] ;
} ;


static struct dlist *
dictionary_str_keyval_alloc(struct dictionary * dict, const char *s,
                    unsigned int valuesize)
{
    char *p;
    struct dlist * np ;
    if (s == 0) return 0 ;
    np = (struct dlist *) DICTIONARY_MALLOC(dict->heap,
            		sizeof(struct dlist) + sizeof(uintptr_t) + valuesize);
    if (!np) return 0 ;
    p = (char *) DICTIONARY_MALLOC(dict->heap, strlen(s)+1); /* + 1 for \0 */
    if (!p ) {
        DICTIONARY_FREE(dict->heap, np);
        return 0;
    }
   strcpy(p, s);
   np->keyval[0] = (uintptr_t)p ;
   return np ;
}

static void
dictionary_str_keyval_free(struct dictionary * dict, struct dlist *np)
{
    DICTIONARY_FREE (dict->heap, (char *)np->keyval[0]) ;
    DICTIONARY_FREE (dict->heap, np) ;
}

static struct dlist *
dictionary_const_str_keyval_alloc(struct dictionary * dict, const char *s,
                    unsigned int valuesize)
{
   struct dlist * np ;
    if (s == 0) return 0 ;
    np = (struct dlist *) DICTIONARY_MALLOC(dict->heap,
            		sizeof(struct dlist) + sizeof(uintptr_t) + valuesize);
    if (!np) return 0 ;
    np->keyval[0] = (uintptr_t)s ;
    return np ;
}

static void
dictionary_const_str_keyval_free(struct dictionary * dict, struct dlist *np)
{
    DICTIONARY_FREE(dict->heap, np);
}

static unsigned int
dictionary_str_key_hash(struct dictionary * dict, const char *s)
{
    unsigned int hashval = 5381; // Start with a prime number
    while (*s != '\0') {
        hashval = ((hashval << 5) + hashval) + *s; // hashval * 33 + *s
        s++;
    }
    return hashval % dict->hashsize;
}

static unsigned int
dictionary_str_key_cmp(struct dictionary * dict, struct dlist *np,
                    const char *s)
{
    char* p = (char*)np->keyval[0] ;
    if (s == p) return 1 ;// NULL key
    if (strcmp(s, p) == 0) {
        return 1 ;
    }
    return 0 ;
}

static const char*
dictionary_str_key(struct dictionary * dict, struct dlist *np)
{
    return (const char*)np->keyval[0] ;
}


static char*
dictionary_str_value(struct dictionary * dict, struct dlist *np)
{
    return (char*)&np->keyval[1] ;
}

static struct dlist *
dictionary_ushort_keyval_alloc(struct dictionary * dict, const char *s,
                    unsigned int valuesize)
{
    struct dlist * np ;
    np = (struct dlist *) DICTIONARY_MALLOC(dict->heap,
                    sizeof(struct dlist) + sizeof(uint32_t) + valuesize);
    if (!np) return 0 ;
    ((uintptr_t*)np->keyval)[0] =  *((uint16_t*)s)  ; /* for alignment of value */
    return np ;
}

static void
dictionary_ushort_keyval_free(struct dictionary * dict, struct dlist *np)
{
    DICTIONARY_FREE (dict->heap, np) ;
    return ;
}

static unsigned
dictionary_ushort_key_hash(struct dictionary * dict, const char *s)
{
    return *((uint16_t*)s) % dict->hashsize ;
}


static unsigned int
dictionary_ushort_key_cmp(struct dictionary * dict, struct dlist *np,
                    const char *s)
{
    if (((uintptr_t*)np->keyval)[0] == *((uint16_t*)s)) {
        return 1 ;
    }
    return 0 ;
}

const char*
dictionary_ushort_key(struct dictionary * dict, struct dlist *np)
{
    return (const char*)&np->keyval[0] ;
}

char*
dictionary_ushort_value(struct dictionary * dict, struct dlist *np)
{
    uint32_t  * pkeyval = (uint32_t*)np->keyval ;
    return (char*)&pkeyval[1] ;
}

static struct dlist *
dictionary_binary_keyval_alloc(struct dictionary * dict, const char *s,
                    unsigned int valuesize)
{
    struct dlist * np ;
    unsigned int keylen = dict->keyspec & 0xFFFF ;
    np = (struct dlist *) DICTIONARY_MALLOC(dict->heap,
            	sizeof(struct dlist) + keylen * sizeof(uint32_t) + valuesize);
    if (!np) return 0 ;
    memcpy(np->keyval, s, keylen*sizeof(uint32_t)) ;
    return np ;
}

static void
dictionary_binary_keyval_free(struct dictionary * dict, struct dlist *np)
{
    DICTIONARY_FREE (dict->heap, np) ;
    return ;
}

static unsigned
dictionary_binary_key_hash(struct dictionary * dict, const char *s)
{
	uint32_t  * pkey = (uint32_t*)s ;
    uint16_t len = dict->keyspec & 0xFFFF ;
    unsigned int i ;
    uint32_t hash = 0 ;
    for (i=0; i<len; i++) {
        hash += pkey[i] ;
    }
    return hash % dict->hashsize ;
}


static unsigned int
dictionary_binary_key_cmp(struct dictionary * dict, struct dlist *np,
                    const char *s)
{
	uint32_t  * pkey = (uint32_t*)s ;
	uint32_t  * pkeyval = (uint32_t*)np->keyval ;
    uint16_t len = dict->keyspec & 0xFFFF ;
    unsigned int i ;
    uint32_t cmp = 1 ;
    for (i=0; i<len; i++) {
        if (pkey[i] != pkeyval[i]) {
            cmp = 0 ;
            break ;
        }
    }
    return cmp ;
}

const char*
dictionary_key(struct dictionary * dict, struct dlist *np)
{
    return (const char*)&np->keyval[0] ;
}

char*
dictionary_value(struct dictionary * dict, struct dlist *np)
{
    uint32_t  * pkeyval = (uint32_t*)np->keyval ;
    return (char*)&pkeyval[dict->keyspec & 0xFFFF] ;
}

static struct dlist *
dict_remove (struct dictionary * dict, const char *s) {
    struct dlist *np;
    struct dlist *prev = 0 ;
    unsigned hashval = dict->key->hash (dict, s) ;
    for (np = dict->hashtab[hashval]; np != 0; np = np->next) {
        if (dict->key->cmp(dict, np, s)) {
          break ; /* found */
        }
        prev = np ;
    }
    if (np) {
        dict->count-- ;
        if (prev) {
            prev->next = np->next ;
        }
        else {
            dict->hashtab[hashval] = np->next ;
        }
    }
    return np;
}

/* look forkeys in hashtab */
static struct dlist *
dict_lookup (struct dictionary * dict, const char *key)
{
    struct dlist *np;
    for (np = dict->hashtab[dict->key->hash(dict, key)]; np != 0;
                        np = np->next) {
        if (dict->key->cmp(dict, np, key)) {
          return np; /* found */
        }
    }
    return 0; /* not found */
}

struct dictionary *
dictionary_init (uintptr_t heap, unsigned int keyspec, unsigned int hashsize)
{
    static const struct dictionary_keyval str_key = {
            &dictionary_str_keyval_alloc,
            &dictionary_str_keyval_free,
            &dictionary_str_key_hash,
            &dictionary_str_key_cmp,
            &dictionary_str_key,
            &dictionary_str_value

    };
    static const struct dictionary_keyval const_str_key = {
            &dictionary_const_str_keyval_alloc,
            &dictionary_const_str_keyval_free,
            &dictionary_str_key_hash,
            &dictionary_str_key_cmp,
            &dictionary_str_key,
            &dictionary_str_value

    };
    static const struct dictionary_keyval ushort_key = {
            &dictionary_ushort_keyval_alloc,
            &dictionary_ushort_keyval_free,
            &dictionary_ushort_key_hash,
            &dictionary_ushort_key_cmp,
            &dictionary_ushort_key,
            &dictionary_ushort_value

    };

    static const struct dictionary_keyval binary_key = {
            &dictionary_binary_keyval_alloc,
            &dictionary_binary_keyval_free,
            &dictionary_binary_key_hash,
            &dictionary_binary_key_cmp,
            &dictionary_key,
            &dictionary_value

    };


    struct dictionary * dict ;
#define DICTSIZE(hashsize) \
            (sizeof(struct dictionary) + sizeof(struct dlist *) * hashsize)

     dict =   (struct dictionary *) DICTIONARY_MALLOC(heap, DICTSIZE(hashsize)) ;
     if (dict) {
        memset (dict,0,DICTSIZE(hashsize)) ;

        dict->keyspec = keyspec ;

        if ((keyspec >> 16) == DICTIONARY_KEYTYPE_BINARY) {
            dict->key = &binary_key ;

        } else if (keyspec == DICTIONARY_KEYSPEC_USHORT) {
            dict->key = &ushort_key ;

       } else if (keyspec == DICTIONARY_KEYSPEC_CONST_STRING) {
            dict->key = &const_str_key ;

        } else {
            dict->key = &str_key ;

        }

        dict->hashsize = hashsize ;
        dict->heap = heap ;

     }
    return dict ;
}

/* put key in hashtab, alloc if not found */
struct dlist*
dictionary_install_size(struct dictionary * dict, const char *key,
                    unsigned int valuesize)
{
    struct dlist *np;
    unsigned hashval;
     if ((np = dict_lookup(dict, key)) == 0) { /* not found */
        np = dict->key->alloc(dict, key, valuesize) ;
        if (np == 0) return 0;
        hashval = dict->key->hash(dict, key);
        np->next = dict->hashtab[hashval];
        dict->hashtab[hashval] = np;
        dict->count++ ;
    }

    return np ;
}

struct dlist*
dictionary_replace(struct dictionary * dict, const char *key, const char *value,
                    unsigned int valuesize)
{
    struct dlist*  np = dictionary_install_size(dict, key,  valuesize) ;
    if (!np) return 0 ;
    char* p = dict->key->value(dict, np);
    memcpy (p, value, valuesize) ;
    return np ;
}

/* find key in hashtab, alloc and set value if not found */
struct dlist*
dictionary_lookup(struct dictionary * dict, const char *key, const char *value,
                    unsigned int valuesize)
{
    struct dlist *np;
    unsigned hashval;
     if ((np = dict_lookup(dict, key)) == 0) { /* not found */
        np = dict->key->alloc(dict, key, valuesize) ;
        if (np == 0) return 0;
        hashval = dict->key->hash(dict, key);
        np->next = dict->hashtab[hashval];
        dict->hashtab[hashval] = np;
        dict->count++ ;
        char* p = dict->key->value(dict, np);
        memcpy (p, value, valuesize) ;

    }

    return np ;
}


struct dlist*
dictionary_get(struct dictionary * dict, const char *key)
{
    return dict_lookup(dict, key) ;
}

unsigned int
dictionary_remove(struct dictionary * dict, const char *key)
{
    struct dlist *np;

    if ((np = dict_remove(dict, key)) == 0) { /* not found */
        return 0 ;
    }

    dict->key->free (dict, np) ;

    return 1;
}

void
dictionary_remove_all(struct dictionary * dict, void (*cb)(struct dictionary *,
                    struct dlist*, uintptr_t), uintptr_t parm, uint8_t destroy)
{
    struct dlist *np;
    unsigned  i ;
    for (i=0; i<dict->hashsize; i++) {
     for (np = dict->hashtab[i]; np != 0; np = dict->hashtab[i]) {
         if (cb) {
             cb (dict, np, parm) ;
         }
         dict->hashtab[i] = np->next ;
         dict->key->free (dict, np) ;
         dict->count-- ;

     }
    }

    // TODO: For this macro there are two include files above that pollute this
    // unit a lot.
    // DBG_CHECKV_T(dict->count == 0, "UTIL  :A: dictionary_remove_all") ;

    if (destroy) {
        DICTIONARY_FREE (dict->heap, dict) ;
    }

}

void
dictionary_destroy(struct dictionary * dict)
{
    dictionary_remove_all (dict, 0, 0, 1) ;
}

static struct dlist*
_it_next (struct dictionary * dict, struct dictionary_it* it)
{
   struct dlist *np;
    unsigned  i ;

    if (it->np && it->np->next) {
        it->prev = it->np ;
        it->np = it->np->next ;
        return it->np ;
    }
    it->prev = 0 ;
     for (i=it->idx+1; i<dict->hashsize; i++) {
         for (np = dict->hashtab[i]; np != 0; ) {
                 it->np = np ;
                 it->idx = i ;
                 return np ;

         }
     }
    it->np = 0 ;
    return 0 ;

}

struct dlist*
dictionary_it_first (struct dictionary * dict, struct dictionary_it* it,
                    DLIST_COMPARE_T cmp, uintptr_t parm, uint32_t unique)
{
    struct dlist *np ;
    struct dlist *nextnp = 0 ;
    int idx = -1 ;
    it->idx = -1 ;
    it->cmp = cmp ;
    it->parm = parm ;
    it->np = 0 ;
    it->unique = unique ;

    nextnp = _it_next (dict, it) ;
    if (!it->cmp || !nextnp) return nextnp ;
    idx = it->idx ;

    while ((np = _it_next (dict, it))) {
        if (it->cmp(dict, it->parm, nextnp, np)>0) {
            nextnp = np ;
            idx = it->idx ;
        }
    }

    it->np = nextnp ;
    it->idx = idx ;
    return nextnp ;
}


struct dlist*
dictionary_it_next (struct dictionary * dict, struct dictionary_it* it)
{
    if (!it->cmp) return _it_next (dict, it) ;

    struct dlist *nextnp  = 0 ;
    struct dlist *np;
    struct dictionary_it _it_equal = {it->np, it->prev, it->idx, 0, 0} ;
    struct dictionary_it _it = {0, 0, -1, 0, 0, 0} ;
    int idx = -1 ;

    if (!(it->unique)) {
        while ((np = _it_next (dict, &_it_equal))) {
            if (it->cmp(dict, it->parm, it->np, np)==0) {
                it->np = np ;
                it->idx = _it_equal.idx ;
                return np ;
            }

         }

    }

    while ((np = _it_next (dict, &_it))) {
        if (it->np == np) {
           continue ;
        }
        if (it->cmp(dict, it->parm, it->np, np)>=0) {
            continue ;
        }
        if (!nextnp) {
            nextnp = np ;
            idx = _it.idx ;
        }
        else if (it->cmp(dict, it->parm, nextnp, np)>0) {
            nextnp = np ;
            idx = _it.idx ;
         }
     }

    it->np = nextnp ;
    it->idx = idx ;
    return nextnp ;
}

struct dlist*
dictionary_it_at (struct dictionary * dict, const char *key,
                    struct dictionary_it* it)
{
    memset (it, 0, sizeof(struct dictionary_it)) ;
    it->idx = dict->key->hash(dict, key) ;
    it->prev = 0 ;

    for (it->np = dict->hashtab[dict->key->hash(dict, key)]; it->np != 0;
            it->np = it->np->next) {
        if (dict->key->cmp(dict, it->np, key)) {
          return it->np ; /* found */
        }
        it->prev = it->np ;
    }

    it->np = 0 ;
    it->idx = -1 ;

    return 0 ;
}

struct dlist*
dictionary_it_get (struct dictionary * dict, struct dictionary_it* it)
{
    return it->np ;

}

const char*
dictionary_get_key (struct dictionary * dict, struct dlist* np)
{
    return dict->key->key (dict, np) ;

}

unsigned int
dictionary_get_key_size (struct dictionary * dict, struct dlist* np)
{
	if (!(dict->keyspec & 0xFFFF)) {
		return strlen (dictionary_get_key (dict, np)) + 1 ;
	}
	if (dict->keyspec == DICTIONARY_KEYTYPE_USHORT) {
		return sizeof (uint16_t) ;
	}

	return (dict->keyspec & 0xFFFF) * sizeof (uint32_t) ;
}

char*
dictionary_get_value (struct dictionary * dict, struct dlist* np)
{
    return dict->key->value (dict, np) ;
}

unsigned int
dictionary_count (struct dictionary * dict)
{
    return dict->count ;
}

unsigned int
dictionary_hashtab_size (struct dictionary * dict)
{
    return dict->hashsize ;
}

unsigned int
dictionary_hashtab_cnt (struct dictionary * dict, unsigned int idx)
{
    struct dlist *np;
    unsigned int cnt = 0 ;
    for (np = dict->hashtab[idx]; np != 0; np = np->next) {
        cnt++;
    }
    return cnt ;
}

void
dictionary_it_remove (struct dictionary * dict, struct dictionary_it* it)
{
    struct dlist *np = it->np ;
    struct dlist *prev = it->prev ;
    if (np) {
        dict->count-- ;
        if (prev) {
            prev->next = np->next ;
        }
        else {
            dict->hashtab[it->idx] = np->next ;
        }
        dict->key->free (dict, np) ;

    }

}

struct dlist*
dictionary_it_move (struct dictionary * dict, struct dictionary_it* it,
                    struct dictionary * dest)
{
    struct dlist *np = it->np ;
    struct dlist *prev = it->prev ;
    unsigned int hashval = it->idx ;
    struct dlist* res = dictionary_it_next (dict, it) ;

    if (!np || (dict->keyspec != dest->keyspec)) return res ;

    const char * key = dict->key->key (dict, np) ;

    if (dict_lookup(dest, key)) return res ;

    dict->count-- ;
    it->prev = prev ;
    if (prev) {
        prev->next = np->next ;
    }
    else {
        dict->hashtab[hashval] = np->next ;
    }

    hashval = dest->key->hash(dest, key);
    np->next = dest->hashtab[hashval];
    dest->hashtab[hashval] = np;
    dest->count++ ;

    return res ;
}

