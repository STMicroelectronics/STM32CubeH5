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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "qoraal/qoraal.h"
#include "qoraal-http/config.h"
#include "qoraal-http/qoraal.h"
#include "qoraal-http/httpparse.h"


typedef struct HTTP_METHOD_S {
    uint16_t        type ;
    uint16_t        len ;
    char*           name ;
} HTTP_METHOD_T ;

static const HTTP_METHOD_T _httpparse_methods[] = { HTTP_PARSE_REQUEST_METHODS } ;
/*
#define isdigit(c)  ('0' <= (c) && (c) <= '9')
#define isxdigit(c) \
    (isdigit(c) || ('A' <= (c) && (c) <= 'F') || ('a' <= (c) && (c) <= 'f'))
*/
void 
urldecode(char *dst, const char *src, uint32_t dstlen)
{
    if (!dstlen) dstlen = (uint32_t)-1 ;
        char a, b;
        while (*src && dstlen--) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit((unsigned char)a) && isxdigit((unsigned char)b))) {
                        if (a >= 'a')
                                a -= 'a'-'A';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'a'-'A';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;

                } else if (*src == '+') {
                        *dst++ = ' ';
                        src++ ;
                } else {
                        *dst++ = *src++;
                }
        }
        *dst++ = '\0';
}

static char hex[] = "0123456789abcdef";

char 
i2a(char code) {
    return hex[code & 15];
}

int 
urlencode( char *pstr, char * buf, int len )
{
    char
         *pbuf = buf;

     while(*pstr && (pbuf < buf + len)){
        if( isalnum((int)*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~' ){
            *pbuf++ = *pstr;
        }
        else if( *pstr == ' ' ){
            *pbuf++ = '+';
        }
        else{
            *pbuf++ = '%',
            *pbuf++ = i2a(*pstr >> 4),
            *pbuf++ = i2a(*pstr & 15);
        }
        pstr++;
    }
    *pbuf = '\0';

    return pbuf - buf ;
}

int 
htmlencode( char *pstr, char * buf, int len )
{
    char
         *pbuf = buf;

     while(*pstr && (pbuf < buf + len)){
        if(*pstr == '"' ){ // &quot;
            *pbuf++ = '&' ;
            *pbuf++ = 'q' ;
            *pbuf++ = 'u' ;
            *pbuf++ = 'o' ;
            *pbuf++ = 't' ;
            *pbuf++ = ';' ;
        }
        else {
            *pbuf++ = *pstr ;
        }

        pstr++;
    }
    *pbuf = '\0';

    return pbuf - buf ;
}



static const char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/', '\0'};
static const unsigned int mod_table[] = {0, 2, 1};

int 
isbase64(char c)
{
   return c && strchr(encoding_table, c) != NULL;
}

char 
_value(char c)
{
   const char *p = strchr(encoding_table, c);
   if(p) {
      return p-encoding_table;
   } else {
      return 0;
   }
}

int 
base64_decode(unsigned char *dest, const unsigned char *src, int srclen)
{
   *dest = 0;
   if(*src == 0)
   {
      return 0;
   }
   unsigned char *p = dest;
   do
   {
       char a,b,c,d ;

      a = _value(src[0]);
      b = _value(src[1]);
      c = _value(src[2]);
      d = _value(src[3]);
      *p++ = (a << 2) | (b >> 4);
      *p++ = (b << 4) | (c >> 2);
      *p++ = (c << 6) | d;
      if(!isbase64(src[1]))
      {
         p -= 2;
         break;
      }
      else if(!isbase64(src[2]))
      {
         p -= 2;
         break;
      }
      else if(!isbase64(src[3]))
      {
         p--;
         break;
      }
      src += 4;
      while(*src && (*src == 13 || *src == 10)) src++;
   }
   while(srclen-= 4);
   *p = 0;
   return p-dest;
}

/*
 * Used to encode username:password for basic authentication.
 */
char*
base64_encode(const char *data,
                    size_t input_length,
                    char* encoded_data,
                    size_t output_length)
{
    unsigned int i, j ;
    if (output_length < BASE64_OUTPUT_LENGTH(input_length)) {
        return 0 ;
    }

    for (i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[output_length - 1 - i] = '=';

    return encoded_data;
}

char *
strnchr(const char *p, char c, size_t n)
{
    if (!p)
        return (0);

    while (n-- > 0) {
        if (*p == c)
            return ((char *)p);
        p++;
    }
    return (0);
}

char *
strntrim(const char *p, size_t n)
{
    if (!p)
        return (0);

    while (n-- > 0) {
        if (*p != ' ')
            return ((char *)p);
        p++;
    }
    return (0);
}

/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
char *
strnstr(const char * s, const char *find, size_t slen)

{
    char c, sc;
    size_t len;

    if ((c = *find++) != '\0') {
        len = strlen(find);
        do {
            do {
                if ((sc = *s++) == '\0' || slen-- < 1)
                    return (NULL);
            } while (sc != c);
            if (len > slen)
                return (NULL);
        } while (strncmp(s, find, len) != 0);
        s--;
    }
    return ((char *)s);
}

/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
char *
memstr(char *haystack, char *needle, int size)
{
    char *p;
    char needlesize = strlen(needle);

    for (p = haystack; p <= (haystack-needlesize+size); p++)
    {
        if (memcmp(p, needle, needlesize) == 0)
            return p; /* found */
    }
    return NULL;
}

int 
strnicmp(const char *pStr1, const char *pStr2, size_t Count)
{
    unsigned char c1, c2;
    int v;

    if (Count == 0)
        return 0;

    do {
        c1 = (unsigned char)*pStr1++;
        c2 = (unsigned char)*pStr2++;
        /* the casts are necessary when pStr1 is shorter & char is signed */
        v = ( int) tolower(c1) - ( int) tolower(c2);
    } while ((v == 0) && (c1 != '\0') && (--Count > 0));

    return v;
}

char*
httpparse_headers(char* data, int len, HTTP_HEADER_T* headers, int count)
{
    //int result  ;
    int i ;
    char *last, *next, *content = 0;

    DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_DEBUG, "-->> httpparse_headers") ;

#define header_isspace(c) ((c) == ' ' || (c) == '\t')

    if (data) {

        next = data ;

        while (len>0) {
            last = next ;
            next = strnchr (last, '\r', len)  ;
            if (!next) { break ; }
            *next++ = '\0' ;
            if (*next++ != '\n') break ;
            if ((next - last)<=2) {
                content = next ;
                DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_INFO,
                        "httpparse_response : content at offset %d\r\n",
                        next - data);
                break ;
            }
            for (i = 0; i<count; i++) {
                if (headers[i].key && !headers[i].value && strnicmp(headers[i].key, last, strlen(headers[i].key)) == 0) {
                    headers[i].value = strnchr (last, ':', next - last)  ;
                    if (headers[i].value) {
                        headers[i].value++ ;
                        while (header_isspace(*headers[i].value)) { headers[i].value++ ; }
                        DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_INFO,
                            "httpparse_response : header %s = %s\r\n",
                            headers[i].key, headers[i].value);
                    }

                }
            }
            len -= next - last ;

        }

    }

    return content ;
}


/**
 * @brief   Parse a HTTP response header and return a pointer to the start of the content.
 * @details The function expects the complete header to be present in the 'data' of length 'len'.
 *          The header is expected to start with 'HTTP/1.1 xxx'.
 *          xxx will be the result returned regardless if the header was mal-formatted.
 *          in case of a mal-formatted header:
 *              - parsing will stop.
 *              - content pointer will be zero.
 *
 * @param[in] data          Pointer to http (header) data.
 * @param[in] len           Length of data.
 * @param[in/out] headers   Pointer to an array of headers to parse.
 * @param[in] count         Elements in array headers.
 * @param[out] content      Pointer to start of content.
 *
 * @return http result code, like 200 for OK.
 *
 * @http
 */
int32_t
httpparse_response(char* data, int len, HTTP_HEADER_T* headers, int count, char** content)
{
    int result  ;
    //int i ;
    char *next ;

    DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_DEBUG, ("-->> httpparse_response")) ;

    if (content) *content = 0 ;
    if ((sscanf(data, "HTTP/1.1 %d", &result) == 0) &&
            (sscanf(data, "HTTP/1.0 %d", &result) == 0)) {
        DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_ERROR,
                        "httpparse_response :E: connect invalid header!");
        return -1 ;
    }

    DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_INFO,
                        "httpparse_response : http result %d", result);

    next = strnchr (data, '\n', len) ;
    if (next++) {

        len -= (next - data) ;

        next = httpparse_headers(next, len, headers, count) ;
        if (content) *content = next ;
    }

    return result ;
}

/**
 * @brief   Parse a HTTP request header and return a pointer to the start of the content.
 * @details The function expects the complete header to be present in the 'data' of length 'len'.
 *          The header is expected to start with 'GET /xxx HTTP/1.1'.
 *          xxx will be the result returned regardless if the header was mal-formatted.
 *          in case of a mal-formatted header:
 *              - parsing will stop.
 *              - content pointer will be zero.
 *
 * @param[in] data          Pointer to http (header) data.
 * @param[in] len           Length of data.
 * @param[in/out] headers   Pointer to an array of headers to parse.
 * @param[in] count         Elements in array headers.
 * @param[out] content      Pointer to start of content.
 *
 * @return http result code, like 200 for OK.
 *
 * @http
 */
int32_t
httpparse_request(char* data, int len, HTTP_HEADER_T* headers, int count, char** endpoint, char** content)
{
    int result = HTTP_HEADER_METHOD_UNDEFINED ;
    unsigned int i ;
    char *last, *next ;

    DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_DEBUG, "-->> httpparse_request") ;

    if (content) *content = 0 ;
    if (endpoint) *endpoint = 0 ;

    for (i=0; i<sizeof(_httpparse_methods)/sizeof(_httpparse_methods[0]); i++) {
        if (strncmp(data, _httpparse_methods[i].name, _httpparse_methods[i].len) == 0) {
            next = strnchr(data, '/', len) ;
            if (!next) break ;
            last = strnchr(next, ' ', len) ;
            if (!last) break ;
            *last = '\0' ;
            last = strntrim (last+1, len) ;
            if (strncmp(last, "HTTP/1.1", 8) != 0) break ;
            *last = '\0' ;
            result = _httpparse_methods[i].type ;
            if (endpoint) *endpoint = next + 1;
            break ;
        }
    }

    if (result == HTTP_HEADER_METHOD_UNDEFINED) {
        DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_ERROR,
                        "httpparse_request : unknown method %d", result);
        return -1 ;

    }
    DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_INFO,
                        "httpparse_response : http result %d", result);


    next = strnchr (data, '\n', len)  ;
    if (next++) {

        len -= (next - data) ;

        next = httpparse_headers(next, len, headers, count) ;
        if (content) *content = next ;
    }

    return result ;
}


/**
 * @brief   This function will inspect the headers and if the content type is chunked it will
 *          put the content pointer to the start of the first chunk.
 *
 * @param[in] data          Pointer to start http content (past the headers).
 * @param[in] len           Length of 'data'.
 * @param[in] headers       Pointer to an array of headers that was parsed.
 * @param[in] count         Elements in array headers.
 * @param[out] content      Pointer to start of content.
 *
 * @return http result code, like 200 for OK.
 *
 * @http
 */
uint32_t
httpparse_content (char* data, int len, HTTP_HEADER_T* headers, int count, char** content, int32_t *content_length)
{
    int32_t i ;
    uint32_t chunk_length = 0 ;
    uint32_t length = 0 ;

    DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_DEBUG, "-->>httpparse_content") ;

    if (content_length) *content_length = 0 ;
    *content = 0 ;
    for (i=0; i<count; i++) {
        if (headers[i].value && (strcmp(headers[i].key, HTTP_HEADER_KEY_TRANSFER_ENCODING) == 0)) {
            if (strncmp(headers[i].value, "chunked", strlen("chunked")) == 0) {
                if (data && (len > 0)) {
                    if (sscanf(data, "%x\r\n", (unsigned int*)&chunk_length)) {
                        *content = strnchr (data,'\n', len)  ;
                        if (*content) (*content)++ ;
                    }
                }
                DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_INFO,
                    "httpparse_content : for chunked %d", chunk_length) ;
                break ;
            }
        }
    }
    for (i=0; i<count; i++) {
       if (headers[i].value && (strcmp(headers[i].key, HTTP_HEADER_KEY_CONTENT_LENGTH) == 0)) {
            if (sscanf(headers[i].value, "%u", (unsigned int*)&length)) {
                if (*content == 0) {
                *content = data ;
                }
            }
            DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_INFO,
                "httpparse_content : content length %d", *content_length) ;
                break ;
            break ;
        }
    }

    if (!chunk_length) {
        if (content_length) *content_length = length ;
        return length ;
    }

    return chunk_length ;
}


int32_t
httpparse_append_headers (char* data, int len, const HTTP_HEADER_T* headers, int headers_count, int last)
{
    int i ;
    int offset = strlen(data) ;
    int start = offset ;

    for (i=0; i<headers_count; i++) {
        int key_len = strlen(headers[i].key) ;
        int value_len = strlen(headers[i].value) ;
        if (offset + key_len + value_len + 4 < len) {
            memcpy (&data[offset], headers[i].key, key_len) ;
            offset += key_len ;
            memcpy (&data[offset], ": ", 2) ;
            offset += 2 ;
            memcpy (&data[offset], headers[i].value, value_len) ;
            offset += value_len ;
            memcpy (&data[offset], "\r\n", 3) ;
            offset += 2 ;
        } else {
            break ;
        }
    }
    if (last) {
        memcpy (&data[offset], "\r\n", 3) ;
        offset += 2 ;

    }

    return offset  - start;
}

char*
httpparse_get_multipart_separator( const HTTP_HEADER_T* headers,int headers_count)
{
    char * separator = 0 ;
    int i ;

    // multipart/form-data; boundary=-------------------------7df19e1f9006a2

    for (i=0; i<headers_count; i++) {
        if (strcmp(headers[i].key, HTTP_HEADER_KEY_CONTENT_TYPE) == 0) {
            separator = strstr (headers[i].value, "boundary=") ;
            if (separator) {
                separator = strchr (separator, '=') ;
                if (separator) separator++ ;

            }
            DBG_MESSAGE_HTTP_PARSE (DBG_MESSAGE_SEVERITY_REPORT,
                "httpparse_content : content-type %s : %s",
                        headers[i].value, separator ? separator : "(separator not found)") ;
             break ;

        }
    }

    return separator ;

}

static inline int myspace(int c)
{
    // Step 1: force to unsigned, since signed chars can be negative and mess with checks
    unsigned char uc = (unsigned char)c;

    // Step 2: compare explicitly to the classic whitespace characters
    return (uc == ' '  ||
            uc == '\t' ||
            uc == '\n' ||
            uc == '\v' ||
            uc == '\f' ||
            uc == '\r');
}


/**
 * @brief   httpparse_url_parse
 * @details Parse a url in the form "http://[credentials@]host[:port]/name".

 *
 * @param[in] url
 * @param[out] https
 * @param[out] port
 * @param[out] host
 * @param[out] name
 * @param[out] credentials
 *
 * @return EOK
 * @retval E_PARM
 *
 * @http
 */
int32_t 
httpparse_url_parse (char* url, int *https, int *port, char** host,
        char** endpoint, char** credentials)
{
    //scriptget  "http://[USR:PSWD@]192.168.7.1:80/test.cfg"
// #define _isurlspace(x) (isspace(x) || ((x) == '/'))

    char* pport ;
    char* pcred ;

    // *endpoint = 0 ;
    // *host = "" ;
    if (!url || !host || !endpoint || !https || !port) {
        return E_PARM ;
    }
    *port = 0 ;
    *https = 0 ;
    *endpoint = 0 ;
    if (credentials) *credentials = 0 ;
    (*host) = url ;

    while (*(*host) != '\0' && myspace((unsigned char)*(*host))) {
        (*host)++;
    }
    if (strncmp ((*host), "http:", 5) == 0) {
        *https = 0 ;
        (*host) += 5 ;
        //*port =  _last_port ? _last_port : 80 ;
    } else if (strncmp ((*host), "https:", 6) == 0) {
        *https = 1 ;
        (*host) += 6 ;
        //*port =  _last_port ? _last_port : 443 ;
    } else {
        return E_PARM ;

    }
    while (*(*host) != '\0' && myspace((unsigned char)*(*host))) {
        (*host)++;
    }
    
    if (((*host)[0] == '/') && ((*host)[1] == '/'))  (*host) += 2 ;


    if (credentials) *credentials = 0 ;
    pcred = strstr((*host), "@") ;
    if (pcred) {
        *pcred = 0 ;
        pcred++ ;
        if (credentials)  *credentials = *host ;
        *host = pcred ;

    }

    pport = strstr((*host), ":") ;
    if (pport) {
        *pport = 0 ;
        pport++ ;
        while (*pport != '\0' && myspace((unsigned char)*pport)) {
            pport++;
        }
        (*endpoint) = strstr(pport, "/") ;
    }
    else {
        (*endpoint) = strstr((*host), "/") ;
    }

    if ((*endpoint)) {
        *(*endpoint) = 0 ;
        (*endpoint)++ ;
    }
    //else {
    //  res = E_PARM ;
    //  break ;
    //}



    while ((*endpoint) && isspace ((int)*(*endpoint))) (*endpoint)++ ;

    if (strlen(*host) > 0) {

        if (pport == 0) {
            *port = *https ? 443 : 80 ;
        }
    }
    if (pport) {
        *port = atoi(pport) ;
    }


    return EOK ;
}

