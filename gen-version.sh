#!/usr/bin/env bash
# Generate Version.h for Clover

# Usage: ./gen-version.sh <build info string>
#
# <build info string>    Optional string (used to define compiler/build options)
#                        This isn't required; Version.h will be created without it
#                        if a <build info string> isn't specified
#

set -u

# Manually fetch the svn branch if it's missing (shallow/single branch clone)
if ! git show-ref --verify --quiet refs/heads/svn &> /dev/null; then
    git remote set-branches --add origin svn
    git fetch origin svn:svn
fi

# Undo shallow clones
if [[ $(git rev-parse --is-shallow-repository) == "true" ]]; then
    git fetch --unshallow
fi

# Fix git-svn history if it doesn't exist
if ! git show-ref | grep -q refs/remotes/git-svn; then
    git checkout svn
    git svn init svn://svn.code.sf.net/p/cloverefiboot/code/
    git update-ref refs/remotes/git-svn "$(git rev-parse HEAD)"
    git svn rebase
    git checkout development
fi

# Use value from RehabMan's _svnver.txt if present
if [[ -e _svnver.txt ]]; then
    SVN_REVISION=$(< _svnver.txt)
else
    SVN_REVISION=$(git svn info | awk '/Revision/ {print $2}')
fi

SUFFIX="RM-$(git symbolic-ref --short HEAD)@$(git rev-parse --short HEAD)"
REVISION="$SVN_REVISION-$SUFFIX"
BUILD_DATE=$(date '+%Y-%m-%d %H:%M:%S')

rm -f Version.h
cat << EOF > Version.h
#define FIRMWARE_BUILDDATE "$BUILD_DATE"
#define FIRMWARE_REVISION L"$REVISION"
#define REVISION_STR "Clover revision: $REVISION"
EOF

if [[ ${1+x} && $1 ]]; then
    echo "#define BUILDINFOS_STR \"$1\"" >> Version.h
fi
