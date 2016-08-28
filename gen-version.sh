#!/bin/bash
# Generate Version.h for Clover

if [ -z $(git show-ref | grep refs/remotes/git-svn) ]; then
	git svn init svn://svn.code.sf.net/p/cloverefiboot/code/
	git update-ref refs/remotes/git-svn refs/remotes/origin/master
	git svn rebase
fi

REVISION=$(git svn info | grep Revision | awk '{print $2}')
BUILD_DATE=$(date '+%Y-%m-%d %H:%M:%S')

if [ -e Version.h ]; then
	rm -f Version.h
fi

echo -e \#define FIRMWARE_VERSION \"2.31\"\ > Version.h
echo -e \#define FIRMWARE_BUILDDATE \"$BUILD_DATE\"\ >> Version.h
echo -e \#define FIRMWARE_REVISION L\"$REVISION\"\ >> Version.h
echo -e \#define REVISION_STR \"Clover revision: $REVISION\" >> Version.h
