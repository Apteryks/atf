#! /bin/sh
#
# Automated Testing Framework (atf)
#
# Copyright (c) 2007 The NetBSD Foundation, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this
#    software must display the following acknowledgement:
#        This product includes software developed by the NetBSD
#        Foundation, Inc. and its contributors.
# 4. Neither the name of The NetBSD Foundation nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
# CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

Prog_Name=${0##*/}

if [ ! -f ./libs/atf.hpp ]; then
    echo "${Prog_Name}: must be run from atf source's top directory" 1>&2
    exit 1
fi

if [ ! -f configure ]; then
    echo "${Prog_Name}: nothing to clean" 1>&2
    exit 1
fi

[ -f Makefile ] || ./configure
make distclean

# Top-level directory.
rm -f .gdb_history
rm -f INSTALL
rm -f Makefile.in
rm -f aclocal.m4
rm -rf autom4te.cache
rm -f config.h.in
rm -f configure
rm -f mkinstalldirs

# `admin' directory.
rm -f admin/compile
rm -f admin/config.guess
rm -f admin/config.sub
rm -f admin/depcomp
rm -f admin/install-sh
rm -f admin/ltmain.sh
rm -f admin/missing

# `tests/bootstrap' directory.
rm -f tests/bootstrap/package.m4
rm -f tests/bootstrap/testsuite

# Files and directories spread all around the tree.
find . -name '#*' | xargs rm -rf
find . -name '*~' | xargs rm -rf
find . -name .deps | xargs rm -rf
find . -name .gdb_history | xargs rm -rf
find . -name .libs | xargs rm -rf

# vim: syntax=sh:expandtab:shiftwidth=4:softtabstop=4
