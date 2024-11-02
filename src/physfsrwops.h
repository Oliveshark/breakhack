/*
 * This file was originally authored by Ryan C. Gordon (icculus@icculus.org).
 * It's been patched by Linus Probert (linus.probert@gmail.com) to work for
 * SDL3 IOStreams which have replaced rwops.
 *
 * The file is still under no warranty or license. Do what you like with it.
 *
 * The original file taken from: https://github.com/icculus/physfs
 */

#ifndef _INCLUDE_PHYSFSRWOPS_H_
#define _INCLUDE_PHYSFSRWOPS_H_

#include "physfs.h"
#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Open a platform-independent filename for reading, and make it accessible
 *  via an SDL_IOStream structure. The file will be closed in PhysicsFS when the
 *  IOStream is closed. PhysicsFS should be configured to your liking before
 *  opening files through this method.
 *
 *   @param filename File to open in platform-independent notation.
 *  @return A valid SDL_IOStream structure on success, NULL on error. Specifics
 *           of the error can be gleaned from PHYSFS_getLastError().
 */
PHYSFS_DECL SDL_IOStream *PHYSFSIO_openRead(const char *fname);

/**
 * Open a platform-independent filename for writing, and make it accessible
 *  via an SDL_IOStream structure. The file will be closed in PhysicsFS when the
 *  IOStream is closed. PhysicsFS should be configured to your liking before
 *  opening files through this method.
 *
 *   @param filename File to open in platform-independent notation.
 *  @return A valid SDL_IOStream structure on success, NULL on error. Specifics
 *           of the error can be gleaned from PHYSFS_getLastError().
 */
PHYSFS_DECL SDL_IOStream *PHYSFSIO_openWrite(const char *fname);

/**
 * Open a platform-independent filename for appending, and make it accessible
 *  via an SDL_IOStream structure. The file will be closed in PhysicsFS when the
 *  IOStream is closed. PhysicsFS should be configured to your liking before
 *  opening files through this method.
 *
 *   @param filename File to open in platform-independent notation.
 *  @return A valid SDL_IOStream structure on success, NULL on error. Specifics
 *           of the error can be gleaned from PHYSFS_getLastError().
 */
PHYSFS_DECL SDL_IOStream *PHYSFSIO_openAppend(const char *fname);

/**
 * Make a SDL_IOStream from an existing PhysicsFS file handle. You should
 *  dispose of any references to the handle after successful creation of
 *  the IOStream. The actual PhysicsFS handle will be destroyed when the
 *  IOStream is closed.
 *
 *   @param handle a valid PhysicsFS file handle.
 *  @return A valid SDL_IOStream structure on success, NULL on error. Specifics
 *           of the error can be gleaned from PHYSFS_getLastError().
 */
PHYSFS_DECL SDL_IOStream *PHYSFSIO_makeIOStream(PHYSFS_File *handle);

#ifdef __cplusplus
}
#endif

#endif /* include-once blocker */

/* end of physfsrwops.h ... */

