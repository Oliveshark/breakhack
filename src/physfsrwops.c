/*
 * This file was originally authored by Ryan C. Gordon (icculus@icculus.org).
 * It's been patched by Linus Probert (linus.probert@gmail.com) to work for
 * SDL3 IOStreams which have replaced rwops.
 *
 * The file is still under no warranty or license. Do what you like with it.
 *
 * The original file taken from: https://github.com/icculus/physfs
 */

#include <stdio.h>
#include "physfsrwops.h"

static const char *getLastPhysfsError(void)
{
	const PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
	return (err) ? PHYSFS_getErrorByCode(err) : NULL;
}

static Sint64 physfsrwops_size(void *userdata)
{
	PHYSFS_File *handle = (PHYSFS_File *) userdata;
	return (Sint64) PHYSFS_fileLength(handle);
}


static Sint64 physfsrwops_seek(void *userdata, Sint64 offset, enum SDL_IOWhence whence)
{
	PHYSFS_File *handle = (PHYSFS_File *) userdata;
	PHYSFS_sint64 pos = 0;

	if (whence == SDL_IO_SEEK_SET) {
		pos = (PHYSFS_sint64) offset;
	} else if (whence == SDL_IO_SEEK_CUR) {
		const PHYSFS_sint64 current = PHYSFS_tell(handle);
		if (current == -1) {

			SDL_SetError("Can't find position in file: %s",
				     getLastPhysfsError());
			return -1;
		}

		if (offset == 0) {
			return (Sint64) current;
		}

		pos = current + ((PHYSFS_sint64) offset);
	} else if (whence == SDL_IO_SEEK_END) {
		const PHYSFS_sint64 len = PHYSFS_fileLength(handle);
		if (len == -1) {
			SDL_SetError("Can't find end of file: %s", getLastPhysfsError());
			return -1;
		}

		pos = len + ((PHYSFS_sint64) offset);
	} else {
		SDL_SetError("Invalid 'whence' parameter.");
		return -1;
	}

	if ( pos < 0 ) {
		SDL_SetError("Attempt to seek past start of file.");
		return -1;
	}

	if (!PHYSFS_seek(handle, (PHYSFS_uint64) pos)) {
		SDL_SetError("PhysicsFS error: %s", getLastPhysfsError());
		return -1;
	}

	return (Sint64) pos;
}


static size_t physfsrwops_read(void *userdata, void *ptr, size_t size, SDL_IOStatus *status)
{
	PHYSFS_File *handle = (PHYSFS_File *) userdata;
	const PHYSFS_uint64 readlen = (PHYSFS_uint64) size;
	const PHYSFS_sint64 rc = PHYSFS_readBytes(handle, ptr, readlen);
	if (rc != ((PHYSFS_sint64) readlen)) {
		if (!PHYSFS_eof(handle)) {
			SDL_SetError("PhysicsFS error: %s", getLastPhysfsError());
			*status = SDL_IO_STATUS_ERROR;
		} else {
			*status = SDL_IO_STATUS_EOF;
		}
	} else {
		*status = SDL_IO_STATUS_READY;
	}

	return (size_t) rc;
}


static size_t SDLCALL physfsrwops_write(void *userdata, const void *ptr,
					size_t size, SDL_IOStatus *status)
{
	PHYSFS_File *handle = (PHYSFS_File *) userdata;
	const PHYSFS_uint64 writelen = (PHYSFS_uint64) size;
	const PHYSFS_sint64 rc = PHYSFS_writeBytes(handle, ptr, writelen);
	if (rc != ((PHYSFS_sint64) writelen)) {
		SDL_SetError("PhysicsFS error: %s", getLastPhysfsError());
		*status = SDL_IO_STATUS_ERROR;
	} else {
		*status = SDL_IO_STATUS_READY;
	}

	return (size_t) rc;
}

static bool physfsrwops_close(void *userdata)
{
	PHYSFS_File *handle = (PHYSFS_File *) userdata;
	if (!PHYSFS_close(handle)) {
		SDL_SetError("PhysicsFS error: %s", getLastPhysfsError());
		return false;
	} /* if */

	return true;
}

static bool physfsrwops_flush(void *userdata, enum SDL_IOStatus *status)
{
	bool result = true;
	PHYSFS_File *handle = (PHYSFS_File *) userdata;
	if (!PHYSFS_flush(handle)) {
		SDL_SetError("PhysicsFS error: %s", getLastPhysfsError());
		*status = SDL_IO_STATUS_ERROR;
		result = false;
	} else {
		*status = SDL_IO_STATUS_READY;
	}
	return result;
}


static SDL_IOStream *create_rwops(PHYSFS_File *handle)
{
	SDL_IOStream *retval = NULL;

	if (handle == NULL) {
		SDL_SetError("PhysicsFS error: %s", getLastPhysfsError());
	} else {
		SDL_IOStreamInterface iface;
		SDL_INIT_INTERFACE(&iface);
		iface.size  = physfsrwops_size;
		iface.seek  = physfsrwops_seek;
		iface.read  = physfsrwops_read;
		iface.write = physfsrwops_write;
		iface.close = physfsrwops_close;
		iface.flush = physfsrwops_flush;
		retval = SDL_OpenIO(&iface, handle);
	}

	return retval;
}


SDL_IOStream *PHYSFSIO_makeRWops(PHYSFS_File *handle)
{
	SDL_IOStream *retval = NULL;
	if (handle == NULL)
		SDL_SetError("NULL pointer passed to PHYSFSIO_makeRWops().");
	else
		retval = create_rwops(handle);

	return retval;
}


SDL_IOStream *PHYSFSIO_openRead(const char *fname)
{
	return create_rwops(PHYSFS_openRead(fname));
}


SDL_IOStream *PHYSFSIO_openWrite(const char *fname)
{
	return create_rwops(PHYSFS_openWrite(fname));
}


SDL_IOStream *PHYSFSIO_openAppend(const char *fname)
{
	return create_rwops(PHYSFS_openAppend(fname));
}
