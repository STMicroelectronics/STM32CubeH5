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


USE_INLINE void 
stack_add_tail (stack_t* ll, plists_t add, unsigned int offsetof_next)
{
    plists_t previous ;
    plists_t start ;
    LISTS_ASSERT(add, ("stack_remove")) ;

    previous = NULL_LLO ;
    for ( start = stack_head (ll) ;
        (start!=NULL_LLO)
            ; ) {

        previous = start ;
        start = stack_next (start, offsetof_next);

    }
    if (previous) {
        stack_add_next (previous, add, offsetof_next) ;
    } else {
        stack_add_head (ll, add, offsetof_next) ;
    }

}

USE_INLINE plists_t
stack_remove (stack_t* ll, plists_t obj, unsigned int offsetof_next)
{
    plists_t previous ;
    plists_t start ;
    LISTS_ASSERT(obj, ("stack_remove")) ;

    previous = NULL_LLO ;
    for ( start = stack_head (ll) ;
        (start!=NULL_LLO) &&
        (start!=obj)
            ; ) {

        previous = start ;
        start = stack_next (start, offsetof_next);

    }

    if (start == obj) {

        if (previous == NULL_LLO) {
            stack_remove_head (ll, offsetof_next);
        } else {
            stack_remove_next (previous, offsetof_next) ;
        }

        return obj ;
    }

    return NULL_LLO ;
}

USE_INLINE void 
linked_add_head (linked_t* ll, plists_t add, unsigned int offsetof_next)
{
    LISTS_ASSERT(add, ("linked_next")) ;
    if (ll->head == NULL_LLO) {
        ll->tail = add ; 
        *LISTS_NEXT_REF(add, offsetof_next) = NULL_LLO ;
    } else {
        *LISTS_NEXT_REF(add, offsetof_next) = ll->head ;
    }
    ll->head = add ;
}

USE_INLINE void 
linked_add_head_list (linked_t* ll, linked_t* add, unsigned int offsetof_next)
{
    LISTS_ASSERT(add->tail, ("linked_next")) ;
    LISTS_ASSERT(add->head, ("linked_next")) ;
    
    if (ll->head == NULL_LLO) {
        ll->tail = add->tail ; 
    } else {
        LISTS_ASSERT(ll->tail, ("linked_next")) ;
        *LISTS_NEXT_REF(add->tail, offsetof_next) = ll->head ;
    }
    ll->head = add->head ; ;
    add->head = NULL_LLO ;
    add->tail = NULL_LLO ;
}

USE_INLINE void 
linked_add_tail (linked_t* ll, plists_t add, unsigned int offsetof_next) 
{
    LISTS_ASSERT(add, ("linked_add_tail")) ;
    *LISTS_NEXT_REF(add, offsetof_next) = NULL_LLO ;

    if (ll->head == NULL_LLO) {
        ll->head = ll->tail = add ; 
    } else { 
        if (ll->tail == NULL_LLO) {
            plists_t t  ;
            for (t = ll->head; t != NULL_LLO; t = *LISTS_NEXT_REF(t, offsetof_next)) {
                ll->tail = t ; 
            }
        }
        *LISTS_NEXT_REF(ll->tail, offsetof_next) = add; 
        ll->tail = add ; 
    }
}

USE_INLINE void 
linked_add_next (linked_t* ll, plists_t obj, plists_t add, unsigned int offsetof_next) 
{
    plists_t last  ;
    LISTS_ASSERT(obj && add, ("linked_add_next")) ;
    last = *LISTS_NEXT_REF(obj, offsetof_next) ;
    *LISTS_NEXT_REF(add, offsetof_next) = last ;
    *LISTS_NEXT_REF(obj, offsetof_next) = add ;
    if (obj == ll->tail) {
        ll->tail = add ;
        *LISTS_NEXT_REF(add, offsetof_next) = NULL_LLO ;
    }
}

USE_INLINE void 
linked_remove_head (linked_t* ll, unsigned int offsetof_next) 
{
    if (ll->head != NULL_LLO) {
        ll->head = *LISTS_NEXT_REF(ll->head, offsetof_next) ; 
    }
    if (ll->head == NULL_LLO) {
        ll->tail = NULL_LLO ;
    }
}

USE_INLINE void 
linked_remove_next (linked_t* ll, plists_t obj, unsigned int offsetof_next) 
{
    plists_t next ;
    LISTS_ASSERT(obj, ("linked_remove_next")) ;

    next = *LISTS_NEXT_REF(obj, offsetof_next) ;
    if (next != NULL_LLO) {
        *LISTS_NEXT_REF(obj, offsetof_next) = *LISTS_NEXT_REF(next, offsetof_next) ; 
        if (*LISTS_NEXT_REF(obj, offsetof_next) == NULL_LLO) {
            ll->tail = obj ; 
        }
    } 
}

USE_INLINE plists_t 
linked_remove (linked_t* ll, plists_t obj, unsigned int offsetof_next) 
{
    plists_t previous ;
    plists_t start ;
    LISTS_ASSERT(obj, ("linked_remove")) ;

    previous = NULL_LLO ;
    for ( start = linked_head (ll) ;
        (start!=NULL_LLO) &&
        (start != obj)
            ; ) {

        previous = start ;
        start = linked_next (start, offsetof_next);

    }

    if (start == obj) {

        if (previous == NULL_LLO) {
            linked_remove_head (ll, offsetof_next);
        } else {
            linked_remove_next (ll, previous, offsetof_next) ;
        }

        return obj ;
    }

    return NULL_LLO ;
}