#!/bin/bash
# Generate Version.h for Clover

if [ -z "$(git show-ref | grep refs/remotes/git-svn)" ]; then
	git svn init svn://svn.code.sf.net/p/cloverefiboot/code/
	git update-ref refs/remotes/git-svn $(git rev-parse HEAD)
	git svn rebase
fi

SVN_REVISION=$(git svn info | grep Revision | awk '{print $2}')
SUFFIX="RM-$(git rev-parse --short HEAD)"
REVISION="$SVN_REVISION-$SUFFIX"
BUILD_DATE=$(date '+%Y-%m-%d %H:%M:%S')

if [ -e Version.h ]; then
	rm -f Version.h
fi

echo -e \#define FIRMWARE_VERSION \"2.31\"\ > Version.h
echo -e \#define FIRMWARE_BUILDDATE \"$BUILD_DATE\"\ >> Version.h
echo -e \#define FIRMWARE_REVISION L\"$REVISION\"\ >> Version.h
echo -e \#define REVISION_STR \"Clover revision: $REVISION\" >> Version.h
