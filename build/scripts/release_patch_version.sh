#!/bin/sh

# Get the current patch version
LAST_VERSION=$(egrep -o 'breakhack_PATCH_VERSION [0-9]+' CMakeLists.txt | awk '{print $2}')
NEXT_VERSION=$((LAST_VERSION + 1))

LAST_TAG=early-access-v$LAST_VERSION
NEXT_TAG=early-access-v$NEXT_VERSION

# Update the version and create release notes
sed -i -e "s/breakhack_PATCH_VERSION [0-9]\+/breakhack_PATCH_VERSION $NEXT_VERSION/" CMakeLists.txt
git log --oneline $LAST_TAG..$NEXT_TAG > build/releasenotes/$NEXT_TAG.txt
git add build/releasenotes/$NEXT_TAG.txt
git commit -a -m"Patch version raised to $NEXT_VERSION"

# Create the tag
git tag $NEXT_TAG

# Push to repo
git push
git push origin $NEXT_TAG

