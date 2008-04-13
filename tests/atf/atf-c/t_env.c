/*
 * Automated Testing Framework (atf)
 *
 * Copyright (c) 2007, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>

#include <atf-c.h>

#include "atf-c/env.h"
#include "atf-c/text.h"

/* ---------------------------------------------------------------------
 * Auxiliary functions.
 * --------------------------------------------------------------------- */

#define CE(stm) ATF_CHECK(!atf_is_error(stm))

/* ---------------------------------------------------------------------
 * Test cases for the free functions.
 * --------------------------------------------------------------------- */

ATF_TC(has);
ATF_TC_HEAD(has, tc)
{
    atf_tc_set_md_var(tc, "descr", "Tests the atf_env_has function");
}
ATF_TC_BODY(has, tc)
{
    ATF_CHECK(atf_env_has("PATH"));
    ATF_CHECK(!atf_env_has("_UNDEFINED_VARIABLE_"));
}

ATF_TC(get);
ATF_TC_HEAD(get, tc)
{
    atf_tc_set_md_var(tc, "descr", "Tests the atf_env_get function");
}
ATF_TC_BODY(get, tc)
{
    const char *val;

    ATF_CHECK(atf_env_has("PATH"));

    val = atf_env_get("PATH");
    ATF_CHECK(strlen(val) > 0);
    ATF_CHECK(strchr(val, ':') != NULL);
}

ATF_TC(set);
ATF_TC_HEAD(set, tc)
{
    atf_tc_set_md_var(tc, "descr", "Tests the atf_env_set function");
}
ATF_TC_BODY(set, tc)
{
    char *oldval;

    ATF_CHECK(atf_env_has("PATH"));
    CE(atf_text_format(&oldval, "%s", atf_env_get("PATH")));
    CE(atf_env_set("PATH", "foo-bar"));
    ATF_CHECK(strcmp(atf_env_get("PATH"), oldval) != 0);
    ATF_CHECK(strcmp(atf_env_get("PATH"), "foo-bar") == 0);
    free(oldval);

    ATF_CHECK(!atf_env_has("_UNDEFINED_VARIABLE_"));
    CE(atf_env_set("_UNDEFINED_VARIABLE_", "foo2-bar2"));
    ATF_CHECK(strcmp(atf_env_get("_UNDEFINED_VARIABLE_"),
                     "foo2-bar2") == 0);
}

ATF_TC(unset);
ATF_TC_HEAD(unset, tc)
{
    atf_tc_set_md_var(tc, "descr", "Tests the atf_env_unset function");
}
ATF_TC_BODY(unset, tc)
{
    ATF_CHECK(atf_env_has("PATH"));
    CE(atf_env_unset("PATH"));
    ATF_CHECK(!atf_env_has("PATH"));
}

/* ---------------------------------------------------------------------
 * Main.
 * --------------------------------------------------------------------- */

ATF_TP_ADD_TCS(tp)
{
    ATF_TP_ADD_TC(tp, has);
    ATF_TP_ADD_TC(tp, get);
    ATF_TP_ADD_TC(tp, set);
    ATF_TP_ADD_TC(tp, unset);

    return atf_no_error();
}