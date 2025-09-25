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



#ifndef __COLLECTION_H__
#define __COLLECTION_H__

#include <stdint.h>

struct collection ;
struct clist ;

typedef uint32_t collectionheap ;

struct clist { /* table entry: */
    struct clist *          next; /* next entry in chain */
    uintptr_t               keyval[]; /* handle to key nd value */
};

struct collection_it {
    struct clist *          p; /* this entry */
    unsigned int            idx ;
};

#ifdef __cplusplus
extern "C" {
#endif

    struct collection *     collection_init (collectionheap heap, unsigned int hashsize) ;
    struct clist*           collection_install_size (struct collection * col, const char *key, unsigned int keysize, unsigned int valuesize) ;
    struct clist*           collection_replace (struct collection * col, const char *key, unsigned int keysize, const char *value, unsigned int valuesize) ;
    struct clist*           collection_lookup (struct collection * col, const char *key, unsigned int keysize, const char *value, unsigned int valuesize) ;
    struct clist*           collection_get (struct collection * col, const char *key, unsigned int keysize) ;
    const char*             collection_get_key (struct collection * col, struct clist* np) ;
    char*                   collection_get_value (struct collection * col, struct clist* np) ;
    unsigned int            collection_remove (struct collection * col, const char *key, unsigned int keysize) ;
    void                    collection_remove_all (struct collection * col, void (*cb)(struct collection *, struct clist*, uint32_t), uint32_t parm) ;
    void                    collection_destroy (struct collection * col) ;
    unsigned int            collection_count (struct collection * col) ;

    struct clist*           collection_it_first (struct collection * col, struct collection_it* it) ;
    struct clist*           collection_it_next (struct collection * col, struct collection_it* it) ;
    struct clist*           collection_it_at (struct collection * col, const char *key, unsigned int len, struct collection_it* it) ;
    struct clist*           collection_it_get (struct collection * col, struct collection_it* it) ;

    unsigned int            collection_hashtab_size (struct collection * col) ;
    unsigned int            collection_hashtab_cnt (struct collection * col, unsigned int idx) ;


#ifdef __cplusplus
}
#endif

#endif /* __COLLECTION_H__ */


