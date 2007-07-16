//
// Automated Testing Framework (atf)
//
// Copyright (c) 2007 The NetBSD Foundation, Inc.
// All rights reserved.
//
// This code is derived from software contributed to The NetBSD Foundation
// by Julio M. Merino Vidal, developed as part of Google's Summer of Code
// 2007 program.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this
//    software must display the following acknowledgement:
//        This product includes software developed by the NetBSD
//        Foundation, Inc. and its contributors.
// 4. Neither the name of The NetBSD Foundation nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
// CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "config.h"

extern "C" {
#include <sys/param.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>
}

#include <cassert>
#include <cerrno>
#include <cstdlib>

#include "atfprivate/exceptions.hpp"
#include "atfprivate/filesystem.hpp"

atf::directory::directory(const std::string& path)
{
    DIR* dp = ::opendir(path.c_str());
    if (dp == NULL)
        throw system_error("atf::main::directory::directory",
                           "opendir(3) failed", errno);

    struct dirent* dep;
    while ((dep = ::readdir(dp)) != NULL)
        insert(dep->d_name);

    if (::closedir(dp) == -1)
        throw system_error("atf::main::directory::directory",
                           "closedir(3) failed", errno);
}

std::string
atf::get_branch_path(const std::string& path)
{
    std::string branch;

    if (path.empty())
        branch = ".";
    else {
        std::string::size_type endpos = path.length() - 1;
        while (path[endpos] == '/')
            endpos--;

        std::string::size_type endpos2 = path.rfind('/', endpos);
        if (endpos2 == std::string::npos)
            branch = ".";
        else if (endpos2 == 0)
            branch = "/";
        else
            branch = path.substr(0, endpos2);
    }

#if defined(HAVE_CONST_DIRNAME)
    assert(branch == ::dirname(path.c_str()));
#endif // defined(HAVE_CONST_DIRNAME)

    return branch;
}

std::string
atf::get_leaf_name(const std::string& path)
{
    std::string leaf;

    if (path.empty())
        leaf = ".";
    else {
        std::string::size_type endpos = path.length() - 1;
        while (path[endpos] == '/')
            endpos--;

        std::string::size_type begpos = path.rfind('/', endpos);
        if (begpos == std::string::npos)
            begpos = 0;
        else
            begpos++;

        leaf = path.substr(begpos, endpos - begpos + 1);
    }

#if defined(HAVE_CONST_BASENAME)
    assert(leaf == ::basename(path.c_str()));
#endif // defined(HAVE_CONST_BASENAME)

    return leaf;
}

std::string
atf::get_temp_dir(void)
{
    const char* tmpdir = std::getenv("TMPDIR");
    return tmpdir == NULL ? "/tmp" : tmpdir;
}

std::string
atf::get_work_dir(void)
{
#if defined(MAXPATHLEN)
    char buf[MAXPATHLEN];

    if (::getcwd(buf, sizeof(buf)) == NULL)
        throw system_error("atf::main::get_work_dir",
                           "getcwd(3) failed", errno);

    return std::string(buf);
#else // !defined(MAXPATHLEN)
#   error "Not implemented."
#endif // defined(MAXPATHLEN)
}

bool
atf::exists(const std::string& path)
{
    bool ok;

    int res = ::access(path.c_str(), F_OK);
    if (res == 0)
        ok = true;
    else if (res == -1 && errno == ENOENT)
        ok = false;
    else
        throw system_error("atf::exists", "access(2) failed", errno);

    return ok;
}

void
atf::change_directory(const std::string& dir)
{
    if (::chdir(dir.c_str()) == -1)
        throw system_error("atf::chdir", "chdir(2) failed", errno);
}

std::string
atf::create_temp_dir(const std::string& pattern)
{
    char* buf = new char[pattern.length() + 1];
    std::string res;
    try {
        std::strcpy(buf, pattern.c_str());
        if (::mkdtemp(buf) == NULL)
            throw system_error("atf::create_temp_dir", "mkdtemp(3) failed",
                               errno);
        res = buf;
        delete [] buf;
    } catch (...) {
        delete [] buf;
        throw;
    }
    assert(!res.empty());
    return res;
}

// XXX: Temporary hack.  The directory class should be refactored to also
// store the type of each of its entries.  Then use that information
// instead of this function.
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}
static
bool
is_directory(const std::string& path)
{
    struct stat sb;
    if (::stat(path.c_str(), &sb) == -1)
        throw atf::system_error("is_directory(" + path + ")",
                                "stat(2) failed", errno);
    return ((sb.st_mode & S_IFMT) == S_IFDIR);
}

void
atf::rm_rf(const std::string& path)
{
    assert(!path.empty());
    assert(path[0] == '/');
    assert(path != std::string(path.length(), '/'));

    directory dir(path);
    dir.erase(".");
    dir.erase("..");

    for (directory::iterator iter = dir.begin(); iter != dir.end();
         iter++) {
        const std::string& entry = path + "/" + (*iter);

        if (is_directory(entry))
            rm_rf(entry);
        else {
            if (::unlink(entry.c_str()) == -1)
                throw system_error("atf::rm_rf(" + path + ")",
                                   "unlink(" + entry + ") failed", errno);
        }
    }

    if (::rmdir(path.c_str()) == -1)
        throw system_error("atf::rm_rf(" + path + ")",
                           "rmdir(" + path + ") failed", errno);
}
