// Copyright (c) 2012-2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "clientversion.h"

#include "tinyformat.h"

#include <string>

/**
 * Name of client reported in the 'version' message. Report the same name
 * for both bmtpd and bmtp-qt, to make it harder for attackers to
 * target servers or GUI users specifically.
 */
const std::string CLIENT_NAME("BMTP Core");

/**
 * Client version number
 */
#define CLIENT_VERSION_SUFFIX ""


/**
 * The following part of the code determines the CLIENT_BUILD variable.
 * Several mechanisms are used for this:
 * * first, if HAVE_BUILD_INFO is defined, include build.h, a file that is
 *   generated by the build environment, possibly containing the output
 *   of git-describe in a macro called BUILD_DESC
 * * secondly, if this is an exported version of the code, GIT_ARCHIVE will
 *   be defined (automatically using the export-subst git attribute), and
 *   GIT_COMMIT will contain the commit id.
 * * then, three options exist for determining CLIENT_BUILD:
 *   * if BUILD_DESC is defined, use that literally (output of git-describe)
 *   * if not, but GIT_COMMIT is defined, use v[maj].[min].[rev].[build]-g[commit]
 *   * otherwise, use v[maj].[min].[rev].[build]-unk
 * finally CLIENT_VERSION_SUFFIX is added
 */

//! First, include build.h if requested
#ifdef HAVE_BUILD_INFO
#include "build.h"
#endif

//! git will put "#define GIT_ARCHIVE 1" on the next line inside archives.
#define GIT_ARCHIVE 1
#ifdef GIT_ARCHIVE
#define GIT_COMMIT_ID "cbcb549"
#define GIT_COMMIT_DATE "Tue, 9 Feb 2016 16:54:57 -0500"
#endif

#define BUILD_DESC_WITH_SUFFIX(maj, min, rev, build, suffix) \
    "v" DO_STRINGIZE(maj) "." DO_STRINGIZE(min) "." DO_STRINGIZE(rev) "." DO_STRINGIZE(build) "-" DO_STRINGIZE(suffix)

#define BUILD_DESC_FROM_COMMIT(maj, min, rev, build, commit) \
    "v" DO_STRINGIZE(maj) "." DO_STRINGIZE(min) "." DO_STRINGIZE(rev) "." DO_STRINGIZE(build) "-g" commit

#define BUILD_DESC_FROM_UNKNOWN(maj, min, rev, build) \
    "v" DO_STRINGIZE(maj) "." DO_STRINGIZE(min) "." DO_STRINGIZE(rev) "." DO_STRINGIZE(build) "-unk"

#ifndef BUILD_DESC
#ifdef BUILD_SUFFIX
#define BUILD_DESC BUILD_DESC_WITH_SUFFIX(CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR, CLIENT_VERSION_REVISION, CLIENT_VERSION_BUILD, BUILD_SUFFIX)
#elif defined(GIT_COMMIT_ID)
#define BUILD_DESC BUILD_DESC_FROM_COMMIT(CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR, CLIENT_VERSION_REVISION, CLIENT_VERSION_BUILD, GIT_COMMIT_ID)
#else
#define BUILD_DESC BUILD_DESC_FROM_UNKNOWN(CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR, CLIENT_VERSION_REVISION, CLIENT_VERSION_BUILD)
#endif
#endif

#ifndef BUILD_DATE
#ifdef GIT_COMMIT_DATE
#define BUILD_DATE GIT_COMMIT_DATE
#else
#define BUILD_DATE __DATE__ ", " __TIME__
#endif
#endif

const std::string CLIENT_BUILD(BUILD_DESC CLIENT_VERSION_SUFFIX);
const std::string CLIENT_DATE(BUILD_DATE);

static std::string FormatVersion(int nVersion)
{
    if (nVersion % 100 == 0)
        return strprintf("%d.%d.%d", nVersion / 1000000, (nVersion / 10000) % 100, (nVersion / 100) % 100);
    else
        return strprintf("%d.%d.%d.%d", nVersion / 1000000, (nVersion / 10000) % 100, (nVersion / 100) % 100, nVersion % 100);
}

std::string FormatFullVersion()
{
    return CLIENT_BUILD;
}

/** 
 * Format the subversion field according to BIP 14 spec (https://github.com/bitcoin/bips/blob/master/bip-0014.mediawiki) 
 */
std::string FormatSubVersion(const std::string& name, int nClientVersion, const std::vector<std::string>& comments)
{
    std::ostringstream ss;
    ss << "/";
    ss << name << ":" << FormatVersion(nClientVersion);
    if (!comments.empty()) {
        std::vector<std::string>::const_iterator it(comments.begin());
        ss << "(" << *it;
        for (++it; it != comments.end(); ++it)
            ss << "; " << *it;
        ss << ")";
    }
    ss << "/";
    return ss.str();
}
