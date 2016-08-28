#!/bin/bash
# Generate Version.h for Clover

REVISION=$(git svn info | grep Revision | awk '{print $2}')
BUILD_DATE=$(date '+%Y-%m-%d %H:%M:%S')

if [ -e Version.h ]; then
	rm -f Version.h
fi

echo -e \#define FIRMWARE_VERSION \"2.31\"\ > Version.h
echo -e \#define FIRMWARE_BUILDDATE \"$BUILD_DATE\"\ >> Version.h
echo -e \#define FIRMWARE_REVISION L\"$REVISION\"\ >> Version.h
echo -e \#define REVISION_STR \"Clover revision: $REVISION\" >> Version.h
