#!/bin/sh

# Get the current patch version
CURRENT_VERSION=$(egrep -o 'breakhack_PATCH_VERSION [0-9]+' CMakeLists.txt | awk '{print $2}')
git tag early-access-v$CURRENT_VERSION
git push origin early-access-v$CURRENT_VERSION

NEXT_VERSION=$((CURRENT_VERSION + 1))
# Update the version
sed -i -e "s/breakhack_PATCH_VERSION [0-9]\+/breakhack_PATCH_VERSION $NEXT_VERSION/" CMakeLists.txt
git commit -a -m"Patch version raised to $NEXT_VERSION"
"push"
