#include <physfs.h>
#include "physfsrwops.h"
#include "io_util.h"
#include "util.h"

SDL_RWops *
io_load_rwops(const char *path)
{
	if (!PHYSFS_exists(path)) {
		PHYSFS_ErrorCode code = PHYSFS_getLastErrorCode();
		fatal("Unable to open file %s: (%d) %s", path, code, PHYSFS_getErrorByCode(code));
	}

	return PHYSFSRWOPS_openRead(path);
}
