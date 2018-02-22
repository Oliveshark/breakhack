#include <physfs.h>
#include "physfsrwops.h"
#include "io_util.h"
#include "util.h"

SDL_RWops *
io_load_rwops(const char *path)
{
	if (!PHYSFS_exists(path))
		fatal("Unable to open file %s: %s", path, PHYSFS_getLastError());

	return PHYSFSRWOPS_openRead(path);
}