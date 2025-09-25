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

#define JSMN_STRICT
#define JSMN_PARENT_LINKS
#define JSMN_PARENT_CNT
#include "qoraal-http/json/jsmn_tools.h"
#undef JSMN_HEADER
#include "qoraal-http/json/jsmn.h"
#include <string.h>
#include <stdio.h>


#define _PRINTF(x,...)

int
jsmn_eq(const char *json, jsmntok_t *tok, const char *s)
{
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
          strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

jsmntok_t *
_next(jsmntok_t *t)
{
  return t + 1 + t->size ;
}

jsmntok_t *
jsmn_get_next(jsmntok_t *first, jsmntok_t *t)
{
  if (t == 0) {
    return first + 1 ;
  }
  if (t + t->size < first + first->size) {
    return _next(t) ;
  }
  return 0 ;
}

int
jsmn_object_enum (const char *js, jsmntok_t *t, size_t count,
                  jsmn_key_val_t * out, size_t out_count)
{
  jsmntok_t *start = t;
  jsmntok_t *key;
  jsmntok_t *val;

  if (t->type != JSMN_OBJECT) {
      _PRINTF("OBJECT EXPECTED!\r\n");
      return -1;
  }

  t = t + 1 ;

  while (t < start + count) {
    if (t->type == JSMN_STRING) {
      key = t ;
      val = t + 1 ;
      t += 1 ;

    } else if (t->type == JSMN_OBJECT) {
      _PRINTF("UNEXPECTED: OBJECT\r\n");
      return -1; ;
    } else if (t->type == JSMN_PRIMITIVE) {
      _PRINTF("UNEXPECTED: '%.*s'\r\n", t->end - t->start, js + t->start);
      return -1;
    } else {
      _PRINTF("UNEXPECTED:\r\n");
      return -1;
    }

#ifdef DEBUG
    _PRINTF("KEY: '%.*s': ", key->end - key->start, js + key->start);

    if ((val->type == JSMN_STRING) || (val->type == JSMN_PRIMITIVE)) {
        _PRINTF("VAL: '%.*s'\r\n", val->end - val->start, js + val->start);
    } else {
        _PRINTF("VAL: '%s\r\n", (val->type == JSMN_OBJECT) ? "OBJECT" : "ARRAY");
    }
#endif

    int i ;
    for (i=0; i<out_count; i++) {
      if (!out[i].value && (jsmn_eq(js, key, out[i].key) == 0)) {
         out[i].value = val ;
      }
    }

    t = _next (t) ;

  }

  return 0 ;
}

int jsmn_copy_str (const char *js, jsmntok_t *t, char * sz, size_t size)
{
  return snprintf (sz, size, "%.*s",  t->end - t->start, js + t->start) ;
}

unsigned int jsmn_get_hex (const char *js, jsmntok_t *t)
{
    char sz[12] ;
    unsigned int userial ;
    snprintf (sz, 12, "%.*s",  t->end - t->start, js + t->start) ;
    sscanf(sz, "%x", &userial) ;
    return userial;

}

unsigned int jsmn_object_is_null(const char *js, jsmntok_t* t)
{
	if ((t->type == JSMN_PRIMITIVE) && (js[t->start] == 'n')) {
		// This is a primitive, starting with the character 'n', so it is null
		// See https://github.com/zserge/jsmn?tab=readme-ov-file#api
		return 1;
	}
	return 0;
}

