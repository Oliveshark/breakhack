#include <physfs.h>
#include "physfsrwops.h"
#include "io_util.h"
#include "util.h"

static void
file_error(const char *path)
{
	PHYSFS_ErrorCode code = PHYSFS_getLastErrorCode();
	fatal("Unable to open file %s: (%d) %s", path, code, PHYSFS_getErrorByCode(code));
}

SDL_IOStream *
io_load_rwops(const char *path)
{
	if (!PHYSFS_exists(path))
		file_error(path);

	return PHYSFSIO_openRead(path);
}

void
io_load_file_buffer(char **dest, unsigned long *len, const char *filepath)
{
	if (!PHYSFS_exists(filepath))
		file_error(filepath);

	debug("PHYSFS: Loading file %s", filepath);
	PHYSFS_File *file = PHYSFS_openRead(filepath);
	PHYSFS_sint64 size = (unsigned long) PHYSFS_fileLength(file);
	char *buffer = ec_malloc(sizeof(char) * (unsigned long) (size + 1));
	PHYSFS_readBytes(file, buffer, (PHYSFS_uint32) size);
	buffer[size] = '\0';
	PHYSFS_close(file);

	*len = (unsigned long) size;
	*dest = buffer;
}

