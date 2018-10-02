#!/bin/sh

# Get the current patch version
MAJOR_VERSION=$(egrep -o 'breakhack_MAJOR_VERSION [0-9]+' CMakeLists.txt | awk '{print $2}')
MINOR_VERSION=$(egrep -o 'breakhack_MINOR_VERSION [0-9]+' CMakeLists.txt | awk '{print $2}')
LAST_PATCH_VERSION=$(egrep -o 'breakhack_PATCH_VERSION [0-9]+' CMakeLists.txt | awk '{print $2}')
NEXT_PATCH_VERSION=$((LAST_PATCH_VERSION + 1))

LAST_TAG=v$MAJOR_VERSION.$MINOR_VERSION.$LAST_PATCH_VERSION
NEXT_TAG=v$MAJOR_VERSION.$MINOR_VERSION.$NEXT_PATCH_VERSION

# Update the version and create release notes
sed -i -e "s/breakhack_PATCH_VERSION [0-9]\+/breakhack_PATCH_VERSION $NEXT_PATCH_VERSION/" CMakeLists.txt
git log --oneline $LAST_TAG..HEAD > build/releasenotes/$NEXT_TAG.txt
git add build/releasenotes/$NEXT_TAG.txt
git commit -a -m"Patch version raised to $NEXT_VERSION"

# Create the tag
git tag $NEXT_TAG

# Push to repo
git push
git push origin $NEXT_TAG

