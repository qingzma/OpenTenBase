/*
 * Tencent is pleased to support the open source community by making OpenTenBase available.  
 * 
 * Copyright (C) 2019 THL A29 Limited, a Tencent company.  All rights reserved.
 * 
 * OpenTenBase is licensed under the BSD 3-Clause License, except for the third-party component listed below. 
 * 
 * A copy of the BSD 3-Clause License is included in this file.
 * 
 * Other dependencies and licenses:
 * 
 * Open Source Software Licensed Under the PostgreSQL License: 
 * --------------------------------------------------------------------
 * 1. Postgres-XL XL9_5_STABLE
 * Portions Copyright (c) 2015-2016, 2ndQuadrant Ltd
 * Portions Copyright (c) 2012-2015, TransLattice, Inc.
 * Portions Copyright (c) 2010-2017, Postgres-XC Development Group
 * Portions Copyright (c) 1996-2015, The PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, The Regents of the University of California
 * 
 * Terms of the PostgreSQL License: 
 * --------------------------------------------------------------------
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without a written agreement
 * is hereby granted, provided that the above copyright notice and this
 * paragraph and the following two paragraphs appear in all copies.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
 * DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 * 
 * 
 * Terms of the BSD 3-Clause License:
 * --------------------------------------------------------------------
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of THL A29 Limited nor the names of its contributors may be used to endorse or promote products derived from this software without 
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 * DAMAGE.
 * 
 */
/*
  Name:        imath.h
  Purpose:    Arbitrary precision integer arithmetic routines.
  Author:    M. J. Fromberger <http://spinning-yarns.org/michael/sw/>
  Info:        Id: imath.h 21 2006-04-02 18:58:36Z sting

  Copyright (C) 2002 Michael J. Fromberger, All Rights Reserved.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 */
/* contrib/pgcrypto/imath.h */

#ifndef IMATH_H_
#define IMATH_H_

/* use always 32bit digits - should some arch use 16bit digits? */
#define USE_LONG_LONG

#include <limits.h>

typedef unsigned char mp_sign;
typedef unsigned int mp_size;
typedef int mp_result;

#ifdef USE_LONG_LONG
typedef uint32 mp_digit;
typedef uint64 mp_word;

#define MP_DIGIT_MAX       0xFFFFFFFFULL
#define MP_WORD_MAX           0xFFFFFFFFFFFFFFFFULL
#else
typedef uint16 mp_digit;
typedef uint32 mp_word;

#define MP_DIGIT_MAX       0xFFFFUL
#define MP_WORD_MAX           0xFFFFFFFFUL
#endif

typedef struct mpz
{
    mp_digit   *digits;
    mp_size        alloc;
    mp_size        used;
    mp_sign        sign;
} mpz_t    ,
           *mp_int;

#define MP_DIGITS(Z) ((Z)->digits)
#define MP_ALLOC(Z)  ((Z)->alloc)
#define MP_USED(Z)     ((Z)->used)
#define MP_SIGN(Z)     ((Z)->sign)

extern const mp_result MP_OK;
extern const mp_result MP_FALSE;
extern const mp_result MP_TRUE;
extern const mp_result MP_MEMORY;
extern const mp_result MP_RANGE;
extern const mp_result MP_UNDEF;
extern const mp_result MP_TRUNC;
extern const mp_result MP_BADARG;

#define MP_DIGIT_BIT    (sizeof(mp_digit) * CHAR_BIT)
#define MP_WORD_BIT        (sizeof(mp_word) * CHAR_BIT)

#define MP_MIN_RADIX    2
#define MP_MAX_RADIX    36

extern const mp_sign MP_NEG;
extern const mp_sign MP_ZPOS;

#define mp_int_is_odd(Z)  ((Z)->digits[0] & 1)
#define mp_int_is_even(Z) !((Z)->digits[0] & 1)

mp_size        mp_get_default_precision(void);
void        mp_set_default_precision(mp_size s);
mp_size        mp_get_multiply_threshold(void);
void        mp_set_multiply_threshold(mp_size s);

mp_result    mp_int_init(mp_int z);
mp_int        mp_int_alloc(void);
mp_result    mp_int_init_size(mp_int z, mp_size prec);
mp_result    mp_int_init_copy(mp_int z, mp_int old);
mp_result    mp_int_init_value(mp_int z, int value);
mp_result    mp_int_set_value(mp_int z, int value);
void        mp_int_clear(mp_int z);
void        mp_int_free(mp_int z);

mp_result    mp_int_copy(mp_int a, mp_int c);    /* c = a     */
void        mp_int_swap(mp_int a, mp_int c);    /* swap a, c */
void        mp_int_zero(mp_int z);        /* z = 0     */
mp_result    mp_int_abs(mp_int a, mp_int c);        /* c = |a|     */
mp_result    mp_int_neg(mp_int a, mp_int c);        /* c = -a     */
mp_result    mp_int_add(mp_int a, mp_int b, mp_int c);    /* c = a + b */
mp_result    mp_int_add_value(mp_int a, int value, mp_int c);
mp_result    mp_int_sub(mp_int a, mp_int b, mp_int c);    /* c = a - b */
mp_result    mp_int_sub_value(mp_int a, int value, mp_int c);
mp_result    mp_int_mul(mp_int a, mp_int b, mp_int c);    /* c = a * b */
mp_result    mp_int_mul_value(mp_int a, int value, mp_int c);
mp_result    mp_int_mul_pow2(mp_int a, int p2, mp_int c);
mp_result    mp_int_sqr(mp_int a, mp_int c);        /* c = a * a */

mp_result mp_int_div(mp_int a, mp_int b,    /* q = a / b */
           mp_int q, mp_int r); /* r = a % b */
mp_result mp_int_div_value(mp_int a, int value,    /* q = a / value */
                 mp_int q, int *r);        /* r = a % value */
mp_result mp_int_div_pow2(mp_int a, int p2,        /* q = a / 2^p2  */
                mp_int q, mp_int r);    /* r = q % 2^p2  */
mp_result    mp_int_mod(mp_int a, mp_int m, mp_int c);    /* c = a % m */

#define   mp_int_mod_value(A, V, R) mp_int_div_value((A), (V), 0, (R))
mp_result    mp_int_expt(mp_int a, int b, mp_int c);        /* c = a^b     */
mp_result    mp_int_expt_value(int a, int b, mp_int c);    /* c = a^b     */

int            mp_int_compare(mp_int a, mp_int b); /* a <=> b       */
int            mp_int_compare_unsigned(mp_int a, mp_int b);        /* |a| <=> |b| */
int            mp_int_compare_zero(mp_int z);        /* a <=> 0       */
int            mp_int_compare_value(mp_int z, int value);    /* a <=> v       */

/* Returns true if v|a, false otherwise (including errors) */
int            mp_int_divisible_value(mp_int a, int v);

/* Returns k >= 0 such that z = 2^k, if one exists; otherwise < 0 */
int            mp_int_is_pow2(mp_int z);

mp_result mp_int_exptmod(mp_int a, mp_int b, mp_int m,
               mp_int c);        /* c = a^b (mod m) */
mp_result mp_int_exptmod_evalue(mp_int a, int value,
                      mp_int m, mp_int c);        /* c = a^v (mod m) */
mp_result mp_int_exptmod_bvalue(int value, mp_int b,
                      mp_int m, mp_int c);        /* c = v^b (mod m) */
mp_result mp_int_exptmod_known(mp_int a, mp_int b,
                     mp_int m, mp_int mu,
                     mp_int c); /* c = a^b (mod m) */
mp_result    mp_int_redux_const(mp_int m, mp_int c);

mp_result    mp_int_invmod(mp_int a, mp_int m, mp_int c);        /* c = 1/a (mod m) */

mp_result    mp_int_gcd(mp_int a, mp_int b, mp_int c);    /* c = gcd(a, b)   */

mp_result mp_int_egcd(mp_int a, mp_int b, mp_int c,        /* c = gcd(a, b)   */
            mp_int x, mp_int y);    /* c = ax + by       */

mp_result    mp_int_sqrt(mp_int a, mp_int c);    /* c = floor(sqrt(q)) */

/* Convert to an int, if representable (returns MP_RANGE if not). */
mp_result    mp_int_to_int(mp_int z, int *out);

/* Convert to nul-terminated string with the specified radix, writing at
   most limit characters including the nul terminator  */
mp_result mp_int_to_string(mp_int z, mp_size radix,
                 char *str, int limit);

/* Return the number of characters required to represent
   z in the given radix.  May over-estimate. */
mp_result    mp_int_string_len(mp_int z, mp_size radix);

/* Read zero-terminated string into z */
mp_result    mp_int_read_string(mp_int z, mp_size radix, const char *str);
mp_result mp_int_read_cstring(mp_int z, mp_size radix, const char *str,
                    char **end);

/* Return the number of significant bits in z */
mp_result    mp_int_count_bits(mp_int z);

/* Convert z to two's complement binary, writing at most limit bytes */
mp_result    mp_int_to_binary(mp_int z, unsigned char *buf, int limit);

/* Read a two's complement binary value into z from the given buffer */
mp_result    mp_int_read_binary(mp_int z, unsigned char *buf, int len);

/* Return the number of bytes required to represent z in binary. */
mp_result    mp_int_binary_len(mp_int z);

/* Convert z to unsigned binary, writing at most limit bytes */
mp_result    mp_int_to_unsigned(mp_int z, unsigned char *buf, int limit);

/* Read an unsigned binary value into z from the given buffer */
mp_result    mp_int_read_unsigned(mp_int z, unsigned char *buf, int len);

/* Return the number of bytes required to represent z as unsigned output */
mp_result    mp_int_unsigned_len(mp_int z);

/* Return a statically allocated string describing error code res */
const char *mp_error_string(mp_result res);

#if 0
void        s_print(char *tag, mp_int z);
void        s_print_buf(char *tag, mp_digit *buf, mp_size num);
#endif

#endif   /* end IMATH_H_ */
