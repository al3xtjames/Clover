#!/usr/bin/env bash
# Generate Version.h for Clover

# Usage: ./gen-version.sh <build info string>
#
# <build info string>    Optional string (used to define compiler/build options)
#                        This isn't required; Version.h will be created without it
#                        if a <build info string> isn't specified
#

# Fix git-svn history if it doesn't exist
if [ -z "$(git show-ref | grep refs/remotes/git-svn)" ]; then
	git config remote.origin.fetch "+refs/heads/*:refs/remotes/origin/*"
	git fetch origin --unshallow
	git checkout svn
	git svn init svn://svn.code.sf.net/p/cloverefiboot/code/
	git update-ref refs/remotes/git-svn $(git rev-parse HEAD)
	git svn rebase
	git checkout development
	git svn rebase
fi

# Use value from RehabMan's _svnver.txt if present
if [ -e _svnver.txt ]; then
	SVN_REVISION=$(cat _svnver.txt)
else
	SVN_REVISION=$(git svn info | grep Revision | awk '{print $2}')
fi

SUFFIX="RM-$(git symbolic-ref --short HEAD)@$(git rev-parse --short HEAD)"
REVISION="$SVN_REVISION-$SUFFIX"
BUILD_DATE=$(date '+%Y-%m-%d %H:%M:%S')

if [ -e Version.h ]; then
	rm -f Version.h
fi

echo -e \#define FIRMWARE_VERSION \"2.31\"\ > Version.h
echo -e \#define FIRMWARE_BUILDDATE \"$BUILD_DATE\"\ >> Version.h
echo -e \#define FIRMWARE_REVISION L\"$REVISION\"\ >> Version.h
echo -e \#define REVISION_STR \"Clover revision: $REVISION\" >> Version.h

if [ ! -z "$1" ]; then
	echo -e \#define BUILDINFOS_STR \"$1\" >> Version.h
fi
