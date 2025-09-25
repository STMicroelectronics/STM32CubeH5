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

/** 
* @mainpage \ref navaro
* @defgroup Navaro Embedded Software
*
* @authors Natie van Rooyen
* 
*/

#ifndef __LISTS_H__
#define __LISTS_H__

#include <stdint.h>
#include <string.h>
#include "../debug.h"

#if !defined LISTS_USE_INLINE
#define LISTS_USE_INLINE            0
#endif

#define LISTS_ASSERT  DBG_ASSERT_T

#ifdef WIN32

#define INLINE          /*static*/ __inline
#if LISTS_USE_INLINE
#define USE_INLINE      static __inline
#else 
#define USE_INLINE
#endif

#else

#undef INLINE
#define INLINE          /*static*/ inline
#if LISTS_USE_INLINE
#define USE_INLINE      static inline
#else 
#define USE_INLINE
#endif


#endif

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

#ifndef OFFSETOF
#define OFFSETOF(TYPE, MEMBER) ((intptr_t) &((TYPE *)0)->MEMBER)
#endif

#define NULL_LLO                       0

#define LISTS_NEXT_REF(pllo, offsetof_next) \
           ((plists_t*)((uintptr_t)(pllo) + offsetof_next)) 
#define LISTS_NEXT_VOLATILE_REF(pllo, offsetof_next) \
           ((plists_t*volatile)((uintptr_t)(pllo) + offsetof_next)) 

typedef void*           plists_t ;          // placeholder for the linked-list object

typedef struct stack_s {
    plists_t            head ;
} stack_t ;

typedef struct fifo_s {
    plists_t            head ;
     plists_t           tail ;
} fifo_t ;

typedef struct linked_s {
    plists_t            head ;
    plists_t            tail ;
} linked_t ;


#define LISTS_LINKED_DECL(name) linked_t name = {0, 0/*,0,0*/}
#define LISTS_FIFO_DECL(name) fifo_t name = {0, 0}
#define LISTS_STACK_DECL(name) stack_t name = {0}

#define LISTS_FIFO_DATA()       {0,0}

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

    static INLINE void          stack_init (stack_t* ll) ;
    static INLINE plists_t      stack_head (stack_t* ll) ;
    static INLINE plists_t      stack_next (plists_t obj, unsigned int offsetof_next) ;
    static INLINE void          stack_add_head (stack_t* ll, plists_t obj, unsigned int offsetof_next) ;
    static INLINE void          stack_add_next (plists_t obj, plists_t add, unsigned int offsetof_next) ;
    USE_INLINE void             stack_add_tail (stack_t* ll, plists_t add, unsigned int offsetof_next) ;
    static INLINE void          stack_remove_head (stack_t* ll, unsigned int offsetof_next) ;
    static INLINE void          stack_remove_next (plists_t obj, unsigned int offsetof_next) ;
    USE_INLINE plists_t         stack_remove (stack_t* ll, plists_t obj, unsigned int offsetof_next) ;

    static INLINE void          fifo_init (fifo_t* ll) ;
    static INLINE int           fifo_is_empty (fifo_t* ll) ;
    static INLINE void          fifo_queue (fifo_t* ll, plists_t add, unsigned int offsetof_next) ;
    static INLINE plists_t      fifo_dequeue (fifo_t* ll, unsigned int offsetof_next) ;
    static INLINE plists_t      fifo_last (fifo_t* ll) ;
    static INLINE plists_t      fifo_first (fifo_t* ll) ;

    static INLINE void          linked_init (linked_t* ll) ;
    static INLINE plists_t      linked_head (linked_t* ll) ;
    static INLINE plists_t      linked_tail (linked_t* ll) ;
    static INLINE plists_t      linked_next (plists_t obj, unsigned int offsetof_next) ;
    USE_INLINE void             linked_add_head (linked_t* ll, plists_t add, unsigned int offsetof_next) ;
    USE_INLINE void             linked_add_tail (linked_t* ll, plists_t add, unsigned int offsetof_next) ;
    USE_INLINE void             linked_add_head_list (linked_t* ll, linked_t* add, unsigned int offsetof_next) ;
    USE_INLINE void             linked_add_next (linked_t* ll, plists_t obj, plists_t add, unsigned int offsetof_next) ;
    USE_INLINE void             linked_remove_head (linked_t* ll, unsigned int offsetof_next) ;
    USE_INLINE void             linked_remove_next (linked_t* ll, plists_t obj, unsigned int offsetof_next) ;
    USE_INLINE plists_t         linked_remove (linked_t* ll, plists_t obj, unsigned int offsetof_next) ;


#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Module inline functions.                                                  */
/*===========================================================================*/

static INLINE void 
stack_init (stack_t* ll) 
{
    ll->head = NULL_LLO ;
}

static INLINE plists_t 
stack_head (stack_t* ll) 
{
    return ll->head ;
}

static INLINE plists_t 
stack_next (plists_t obj, unsigned int offsetof_next) 
{
    LISTS_ASSERT(obj, ("stack_next")) ;
    return *LISTS_NEXT_REF(obj, offsetof_next) ;
}

static INLINE void 
stack_add_head (stack_t* ll, plists_t obj, unsigned int offsetof_next) 
{
    LISTS_ASSERT(obj, ("stack_add_head")) ;
    if (ll->head != NULL_LLO) {
        *LISTS_NEXT_REF(obj, offsetof_next) = ll->head ; 
    } else {
        *LISTS_NEXT_REF(obj, offsetof_next) = NULL_LLO ;
    }
    ll->head = obj ;
}

static INLINE void 
stack_add_next (plists_t obj, plists_t add, unsigned int offsetof_next) 
{
    plists_t last  ;
    LISTS_ASSERT(obj && add, ("stack_add_next")) ;
    last = *LISTS_NEXT_REF(obj, offsetof_next) ;
    *LISTS_NEXT_REF(add, offsetof_next) = last ;
    *LISTS_NEXT_REF(obj, offsetof_next) = add ;
}


static INLINE void
stack_remove_head (stack_t* ll, unsigned int offsetof_next) 
{
    if (ll->head != NULL_LLO) {
        ll->head = *LISTS_NEXT_REF(ll->head, offsetof_next) ;
    }
}

static INLINE void 
stack_remove_next (plists_t obj, unsigned int offsetof_next) 
{
    plists_t next ;
    plists_t last ;
    LISTS_ASSERT(obj, ("stack_remove_next")) ;
    next = *LISTS_NEXT_REF(obj, offsetof_next) ;
    if (next != NULL_LLO) {
        last = *LISTS_NEXT_REF(next, offsetof_next) ;
    } else { 
        last = NULL_LLO ;
    }

    *LISTS_NEXT_REF(obj, offsetof_next) = last ;
}


/*==================================================================================================
                                 QUEUE
==================================================================================================*/

static INLINE void 
fifo_init (fifo_t* ll)
{
    memset ((void*)ll, 0xaa, sizeof (fifo_t)) ;
    ll->head = ll->tail = NULL_LLO ;
}

static INLINE int 
fifo_is_empty (fifo_t* ll)
{
    return ll->head == NULL_LLO ;
}

static INLINE plists_t 
fifo_first (fifo_t* ll)
{
    return ll->head  ;
}

static INLINE plists_t 
fifo_last (fifo_t* ll)
{
    return ll->tail  ;
}

static INLINE void 
fifo_queue (fifo_t* ll, plists_t add, unsigned int offsetof_next)
{
    LISTS_ASSERT(add, ("fifo_queue")) ;
    *LISTS_NEXT_VOLATILE_REF(add, offsetof_next) = NULL_LLO ;
    if (ll->head == NULL_LLO) {
        ll->head = ll->tail = add ;
    } else { 
        *LISTS_NEXT_VOLATILE_REF(ll->tail, offsetof_next) = add; 
        ll->tail = add ; 
    }
    LISTS_ASSERT(*LISTS_NEXT_VOLATILE_REF(ll->tail, offsetof_next) == NULL_LLO, ("fifo_queue 2")) ;
}

static INLINE plists_t 
fifo_dequeue (fifo_t* ll, unsigned int offsetof_next)
{
    plists_t head = ll->head ;
    if (head != NULL_LLO) {
        ll->head = *LISTS_NEXT_VOLATILE_REF(ll->head, offsetof_next) ;
        if (ll->head == NULL_LLO) {
            ll->tail = NULL_LLO ;
        }
    }
    LISTS_ASSERT(ll->tail ? (*LISTS_NEXT_VOLATILE_REF(ll->tail, offsetof_next) == NULL_LLO) : 1, ("fifo_dequeue")) ;
    return head ;
}

static INLINE void 
linked_init (linked_t* ll) 
{
    ll->head = NULL_LLO ;
    ll->tail = NULL_LLO ;
}

static INLINE plists_t 
linked_head (linked_t* ll) 
{
    return ll->head ;
}

static INLINE plists_t 
linked_tail (linked_t* ll) 
{
    return ll->tail ;
}

static INLINE plists_t 
linked_next (plists_t obj, unsigned int offsetof_next) 
{
    LISTS_ASSERT(obj, ("linked_next")) ;
    return *LISTS_NEXT_REF(obj, offsetof_next) ;
}

#if LISTS_USE_INLINE
#include "lists_inline.h"
#endif /* LISTS_USE_INLINE */

#endif /* __LISTS_H__ */
