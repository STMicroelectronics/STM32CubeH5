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



#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <stdint.h>

struct dictionary ;
struct dlist ;

typedef int  (*DLIST_COMPARE_T)(struct dictionary * /* dict */, uintptr_t parm, struct dlist * /* first */, struct dlist * /* second */) ;

struct dlist { /* table entry: */
    struct dlist *          next; /* next entry in chain */
    uintptr_t               keyval[]; /* handle to key */
};

struct dictionary_it {
    struct dlist *          np; /* this entry */
    struct dlist *          prev; /* previous entry */
    int                     idx ;
    DLIST_COMPARE_T         cmp ;
    uintptr_t               parm ;
    uint32_t                unique ; /* items to sort is a uniqie collection */
};


#define DICTIONARY_MALLOC(heap, size)           qoraal_malloc (heap, size)
#define DICTIONARY_FREE(heap, mem)              qoraal_free (heap, mem)

#define DICTIONARY_KEYTYPE_STRING               0
#define DICTIONARY_KEYTYPE_CONST_STRING         1
#define DICTIONARY_KEYTYPE_USHORT               3
#define DICTIONARY_KEYTYPE_BINARY               4

#define DICTIONARY_MKKEY(keytype,spec)          (((uint32_t)(keytype)<<16) | ((uint32_t)(spec)))
#define DICTIONARY_KEYSPEC_STRING               DICTIONARY_MKKEY(DICTIONARY_KEYTYPE_STRING, 0)
#define DICTIONARY_KEYSPEC_CONST_STRING         DICTIONARY_MKKEY(DICTIONARY_KEYTYPE_CONST_STRING, 0)
#define DICTIONARY_KEYSPEC_USHORT               DICTIONARY_MKKEY(DICTIONARY_KEYTYPE_USHORT, 0)
#define DICTIONARY_KEYSPEC_UINT                 DICTIONARY_MKKEY(DICTIONARY_KEYTYPE_BINARY, 1)
#define DICTIONARY_KEYSPEC_BINARY_2             DICTIONARY_MKKEY(DICTIONARY_KEYTYPE_BINARY, 2)
#define DICTIONARY_KEYSPEC_BINARY_3             DICTIONARY_MKKEY(DICTIONARY_KEYTYPE_BINARY, 3)
#define DICTIONARY_KEYSPEC_BINARY_4             DICTIONARY_MKKEY(DICTIONARY_KEYTYPE_BINARY, 4)
#define DICTIONARY_KEYSPEC_BINARY(n)            DICTIONARY_MKKEY(DICTIONARY_KEYTYPE_BINARY, n)

#ifdef __cplusplus
extern "C" {
#endif

    struct dictionary *     dictionary_init(uintptr_t heap, unsigned int keyspec, unsigned int hashsize) ;
    struct dlist*           dictionary_install_size(struct dictionary * dict, const char *key, unsigned int valuesize) ;
    struct dlist*           dictionary_replace(struct dictionary * dict, const char *key, const char *value, unsigned int valuesize) ;
    struct dlist*           dictionary_lookup(struct dictionary * dict, const char *key, const char *value, unsigned int valuesize) ;
    struct dlist*           dictionary_get(struct dictionary * dict, const char *key) ;
    const char*             dictionary_get_key (struct dictionary * dict, struct dlist* np) ;
    unsigned int            dictionary_get_key_size (struct dictionary * dict, struct dlist* np) ;
    char*                   dictionary_get_value (struct dictionary * dict, struct dlist* np) ;
    unsigned int            dictionary_remove(struct dictionary * dict, const char *key) ;
    void                    dictionary_remove_all(struct dictionary * dict, void (*cb)(struct dictionary *, struct dlist*, uintptr_t), uintptr_t parm, uint8_t destroy) ;
    void                    dictionary_destroy(struct dictionary * dict) ;
    unsigned int            dictionary_count (struct dictionary * dict) ;

    struct dlist*           dictionary_it_first (struct dictionary * dict, struct dictionary_it* it, DLIST_COMPARE_T cmp, uintptr_t parm, uint32_t unique) ;
    struct dlist*           dictionary_it_next (struct dictionary * dict, struct dictionary_it* it) ;
    struct dlist*           dictionary_it_at (struct dictionary * dict, const char *key, struct dictionary_it* it) ;
    struct dlist*           dictionary_it_get (struct dictionary * dict, struct dictionary_it* it) ;
    void                    dictionary_it_remove (struct dictionary * dict, struct dictionary_it* it) ;
    struct dlist*           dictionary_it_move (struct dictionary * dict, struct dictionary_it* it, struct dictionary * dest) ;

    unsigned int            dictionary_hashtab_size (struct dictionary * dict) ;
    unsigned int            dictionary_hashtab_cnt (struct dictionary * dict, unsigned int idx) ;

#ifdef __cplusplus
}
#endif

#endif /* __DICTIONARY_H__ */


