#!/bin/sh

# Get the current patch version
LAST_VERSION=$(egrep -o 'breakhack_PATCH_VERSION [0-9]+' CMakeLists.txt | awk '{print $2}')
NEXT_VERSION=$((LAST_VERSION + 1))

# Update the version and create release notes
sed -i -e "s/breakhack_PATCH_VERSION [0-9]\+/breakhack_PATCH_VERSION $NEXT_VERSION/" CMakeLists.txt
git log --oneline early-access-v$LAST_VERSION..early-access-v$NEXT_VERSION >> build/releasenotes/early-access-$NEXT_VERSION
git commit -a -m"Patch version raised to $NEXT_VERSION"

# Create the tag
git tag early-access-v$NEXT_VERSION

# Push to repo
git push --follow-tags
