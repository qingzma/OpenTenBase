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
 * px-crypt.c
 *        Wrapper for various crypt algorithms.
 *
 * Copyright (c) 2001 Marko Kreen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * contrib/pgcrypto/px-crypt.c
 */

#include "postgres.h"

#include "contrib/pgcrypto/px.h"
#include "contrib/pgcrypto/px-crypt.h"


static char *
run_crypt_des(const char *psw, const char *salt,
              char *buf, unsigned len)
{
    char       *res;

    res = px_crypt_des(psw, salt);
    if (res == NULL || strlen(res) > len - 1)
        return NULL;
    strcpy(buf, res);
    return buf;
}

static char *
run_crypt_md5(const char *psw, const char *salt,
              char *buf, unsigned len)
{
    char       *res;

    res = px_crypt_md5(psw, salt, buf, len);
    return res;
}

static char *
run_crypt_bf(const char *psw, const char *salt,
             char *buf, unsigned len)
{
    char       *res;

    res = _crypt_blowfish_rn(psw, salt, buf, len);
    return res;
}

struct px_crypt_algo
{
    char       *id;
    unsigned    id_len;
    char       *(*crypt) (const char *psw, const char *salt,
                                      char *buf, unsigned len);
};

static const struct px_crypt_algo
            px_crypt_list[] = {
    {"$2a$", 4, run_crypt_bf},
    {"$2x$", 4, run_crypt_bf},
    {"$2$", 3, NULL},            /* N/A */
    {"$1$", 3, run_crypt_md5},
    {"_", 1, run_crypt_des},
    {"", 0, run_crypt_des},
    {NULL, 0, NULL}
};

char *
px_crypt(const char *psw, const char *salt, char *buf, unsigned len)
{
    const struct px_crypt_algo *c;

    for (c = px_crypt_list; c->id; c++)
    {
        if (!c->id_len)
            break;
        if (strncmp(salt, c->id, c->id_len) == 0)
            break;
    }

    if (c->crypt == NULL)
        return NULL;

    return c->crypt(psw, salt, buf, len);
}

/*
 * salt generators
 */

struct generator
{
    char       *name;
    char       *(*gen) (unsigned long count, const char *input, int size,
                                    char *output, int output_size);
    int            input_len;
    int            def_rounds;
    int            min_rounds;
    int            max_rounds;
};

static struct generator gen_list[] = {
    {"des", _crypt_gensalt_traditional_rn, 2, 0, 0, 0},
    {"md5", _crypt_gensalt_md5_rn, 6, 0, 0, 0},
    {"xdes", _crypt_gensalt_extended_rn, 3, PX_XDES_ROUNDS, 1, 0xFFFFFF},
    {"bf", _crypt_gensalt_blowfish_rn, 16, PX_BF_ROUNDS, 4, 31},
    {NULL, NULL, 0, 0, 0, 0}
};

int
px_gen_salt(const char *salt_type, char *buf, int rounds)
{
    int            res;
    struct generator *g;
    char       *p;
    char        rbuf[16];

    for (g = gen_list; g->name; g++)
        if (pg_strcasecmp(g->name, salt_type) == 0)
            break;

    if (g->name == NULL)
        return PXE_UNKNOWN_SALT_ALGO;

    if (g->def_rounds)
    {
        if (rounds == 0)
            rounds = g->def_rounds;

        if (rounds < g->min_rounds || rounds > g->max_rounds)
            return PXE_BAD_SALT_ROUNDS;
    }

    res = px_get_random_bytes((uint8 *) rbuf, g->input_len);
    if (res < 0)
        return res;

    p = g->gen(rounds, rbuf, g->input_len, buf, PX_MAX_SALT_LEN);
    crypt_memset(rbuf, 0, sizeof(rbuf));

    if (p == NULL)
        return PXE_BAD_SALT_ROUNDS;

    return strlen(p);
}
