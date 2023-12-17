/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2024 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/**
 *  \file SDL_camera.h
 *
 *  Video Capture for the SDL library.
 */

#ifndef SDL_camera_h_
#define SDL_camera_h_

#include "SDL3/SDL_video.h"

#include <SDL3/SDL_begin_code.h>
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is a unique ID for a camera device for the time it is connected to the system,
 * and is never reused for the lifetime of the application. If the device is
 * disconnected and reconnected, it will get a new ID.
 *
 * The ID value starts at 1 and increments from there. The value 0 is an invalid ID.
 *
 * \sa SDL_GetCameraDevices
 */
typedef Uint32 SDL_CameraDeviceID;


/**
 * The structure used to identify an opened SDL camera
 */
struct SDL_Camera;
typedef struct SDL_Camera SDL_Camera;

/**
 *  SDL_CameraSpec structure
 *
 * \sa SDL_GetCameraDeviceSupportedFormats
 * \sa SDL_GetCameraFormat
 *
 */
typedef struct SDL_CameraSpec
{
    Uint32 format;          /**< Frame SDL_PixelFormatEnum format */
    int width;              /**< Frame width */
    int height;             /**< Frame height */
    int interval_numerator;  /**< Frame rate numerator ((dom / num) == fps) */
    int interval_denominator;  /**< Frame rate demoninator ((dom / num) == fps)*/
} SDL_CameraSpec;

/**
 * Use this function to get the number of built-in camera drivers.
 *
 * This function returns a hardcoded number. This never returns a negative
 * value; if there are no drivers compiled into this build of SDL, this
 * function returns zero. The presence of a driver in this list does not mean
 * it will function, it just means SDL is capable of interacting with that
 * interface. For example, a build of SDL might have v4l2 support, but if
 * there's no kernel support available, SDL's v4l2 driver would fail if used.
 *
 * By default, SDL tries all drivers, in its preferred order, until one is
 * found to be usable.
 *
 * \returns the number of built-in camera drivers.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_GetCameraDriver
 */
extern DECLSPEC int SDLCALL SDL_GetNumCameraDrivers(void);

/**
 * Use this function to get the name of a built in camera driver.
 *
 * The list of camera drivers is given in the order that they are normally
 * initialized by default; the drivers that seem more reasonable to choose
 * first (as far as the SDL developers believe) are earlier in the list.
 *
 * The names of drivers are all simple, low-ASCII identifiers, like "v4l2",
 * "coremedia" or "android". These never have Unicode characters, and are not
 * meant to be proper names.
 *
 * \param index the index of the camera driver; the value ranges from 0 to
 *              SDL_GetNumCameraDrivers() - 1
 * \returns the name of the camera driver at the requested index, or NULL if an
 *          invalid index was specified.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_GetNumCameraDrivers
 */
extern DECLSPEC const char *SDLCALL SDL_GetCameraDriver(int index);

/**
 * Get the name of the current camera driver.
 *
 * The returned string points to internal static memory and thus never becomes
 * invalid, even if you quit the camera subsystem and initialize a new driver
 * (although such a case would return a different static string from another
 * call to this function, of course). As such, you should not modify or free
 * the returned string.
 *
 * \returns the name of the current camera driver or NULL if no driver has been
 *          initialized.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC const char *SDLCALL SDL_GetCurrentCameraDriver(void);

/**
 * Get a list of currently connected camera devices.
 *
 * \param count a pointer filled in with the number of camera devices. Can be NULL.
 * \returns a 0 terminated array of camera instance IDs which should be
 *          freed with SDL_free(), or NULL on error; call SDL_GetError() for
 *          more details.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_OpenCamera
 */
extern DECLSPEC SDL_CameraDeviceID *SDLCALL SDL_GetCameraDevices(int *count);

/**
 * Get the list of native formats/sizes a camera supports.
 *
 * This returns a list of all formats and frame sizes that a specific
 * camera can offer. This is useful if your app can accept a variety
 * of image formats and sizes and so want to find the optimal spec
 * that doesn't require conversion.
 *
 * This function isn't strictly required; if you call SDL_OpenCameraDevice
 * with a NULL spec, SDL will choose a native format for you, and if you
 * instead specify a desired format, it will transparently convert to the
 * requested format on your behalf.
 *
 * If `count` is not NULL, it will be filled with the number of elements
 * in the returned array. Additionally, the last element of the array
 * has all fields set to zero (this element is not included in `count`).
 *
 * The returned list is owned by the caller, and should be released with
 * SDL_free() when no longer needed.
 *
 * \param devid the camera device instance ID to query.
 * \param count a pointer filled in with the number of elements in the list. Can be NULL.
 * \returns a 0 terminated array of SDL_CameraSpecs, which should be
 *          freed with SDL_free(), or NULL on error; call SDL_GetError() for
 *          more details.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_GetCameraDevices
 * \sa SDL_OpenCameraDevice
 */
extern DECLSPEC SDL_CameraSpec *SDLCALL SDL_GetCameraDeviceSupportedFormats(SDL_CameraDeviceID devid, int *count);

/**
 * Get human-readable device name for a camera.
 *
 * The returned string is owned by the caller; please release it with
 * SDL_free() when done with it.
 *
 * \param instance_id the camera device instance ID
 * \returns Human-readable device name, or NULL on error; call SDL_GetError() for more information.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_GetCameraDevices
 */
extern DECLSPEC char * SDLCALL SDL_GetCameraDeviceName(SDL_CameraDeviceID instance_id);

/**
 * Open a video capture device (a "camera").
 *
 * You can open the device with any reasonable spec, and if the hardware can't
 * directly support it, it will convert data seamlessly to the requested
 * format. This might incur overhead, including scaling of image data.
 *
 * If you would rather accept whatever format the device offers, you can
 * pass a NULL spec here and it will choose one for you (and you can use
 * SDL_Surface's conversion/scaling functions directly if necessary).
 *
 * You can call SDL_GetCameraFormat() to get the actual data format if
 * passing a NULL spec here. You can see the exact specs a device can
 * support without conversion with SDL_GetCameraSupportedSpecs().
 *
 * SDL will not attempt to emulate framerate; it will try to set the
 * hardware to the rate closest to the requested speed, but it won't
 * attempt to limit or duplicate frames artificially; call
 * SDL_GetCameraFormat() to see the actual framerate of the opened the device,
 * and check your timestamps if this is crucial to your app!
 *
 * \param instance_id the camera device instance ID
 * \param spec The desired format for data the device will provide. Can be NULL.
 * \returns device, or NULL on failure; call SDL_GetError() for more
 *          information.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_GetCameraDevices
 * \sa SDL_GetCameraFormat
 */
extern DECLSPEC SDL_Camera *SDLCALL SDL_OpenCameraDevice(SDL_CameraDeviceID instance_id, const SDL_CameraSpec *spec);

/**
 * Get the instance ID of an opened camera.
 *
 * \param device an SDL_Camera to query
 * \returns the instance ID of the specified camera on success or 0 on
 *          failure; call SDL_GetError() for more information.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_OpenCameraDevice
 */
extern DECLSPEC SDL_CameraDeviceID SDLCALL SDL_GetCameraInstanceID(SDL_Camera *camera);

/**
 * Get the properties associated with an opened camera.
 *
 * \param device the SDL_Camera obtained from SDL_OpenCameraDevice()
 * \returns a valid property ID on success or 0 on failure; call
 *          SDL_GetError() for more information.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_GetProperty
 * \sa SDL_SetProperty
 */
extern DECLSPEC SDL_PropertiesID SDLCALL SDL_GetCameraProperties(SDL_Camera *camera);

/**
 * Get the spec that a camera is using when generating images.
 *
 * Note that this might not be the native format of the hardware, as SDL
 * might be converting to this format behind the scenes.
 *
 * \param device opened camera device
 * \param spec The SDL_CameraSpec to be initialized by this function.
 * \returns 0 on success or a negative error code on failure; call
 *          SDL_GetError() for more information.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_OpenCameraDevice
 */
extern DECLSPEC int SDLCALL SDL_GetCameraFormat(SDL_Camera *camera, SDL_CameraSpec *spec);

/**
 * Acquire a frame.
 *
 * The frame is a memory pointer to the image data, whose size and format are
 * given by the spec requested when opening the device.
 *
 * This is a non blocking API. If there is a frame available, a non-NULL surface is
 * returned, and timestampNS will be filled with a non-zero value.
 *
 * Note that an error case can also return NULL, but a NULL by itself is normal
 * and just signifies that a new frame is not yet available. Note that even if a
 * camera device fails outright (a USB camera is unplugged while in use, etc), SDL
 * will send an event separately to notify the app, but continue to provide blank
 * frames at ongoing intervals until SDL_CloseCamera() is called, so real
 * failure here is almost always an out of memory condition.
 *
 * After use, the frame should be released with SDL_ReleaseCameraFrame(). If you
 * don't do this, the system may stop providing more video! If the hardware is
 * using DMA to write directly into memory, frames held too long may be overwritten
 * with new data.
 *
 * Do not call SDL_FreeSurface() on the returned surface! It must be given back
 * to the camera subsystem with SDL_ReleaseCameraFrame!
 *
 * \param device opened camera device
 * \param timestampNS a pointer filled in with the frame's timestamp, or 0 on error. Can be NULL.
 * \returns A new frame of video on success, NULL if none is currently available.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_ReleaseCameraFrame
 */
extern DECLSPEC SDL_Surface * SDLCALL SDL_AcquireCameraFrame(SDL_Camera *camera, Uint64 *timestampNS);

/**
 * Release a frame of video acquired from a camera.
 *
 * Let the back-end re-use the internal buffer for camera.
 *
 * This function _must_ be called only on surface objects returned by
 * SDL_AcquireCameraFrame(). This function should be called as quickly as
 * possible after acquisition, as SDL keeps a small FIFO queue of surfaces
 * for video frames; if surfaces aren't released in a timely manner, SDL
 * may drop upcoming video frames from the camera.
 *
 * If the app needs to keep the surface for a significant time, they should
 * make a copy of it and release the original.
 *
 * The app should not use the surface again after calling this function;
 * assume the surface is freed and the pointer is invalid.
 *
 * \param device opened camera device
 * \param frame The video frame surface to release.
 * \returns 0 on success or a negative error code on failure; call
 *          SDL_GetError() for more information.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_AcquireCameraFrame
 */
extern DECLSPEC int SDLCALL SDL_ReleaseCameraFrame(SDL_Camera *camera, SDL_Surface *frame);

/**
 * Use this function to shut down camera processing and close the
 * camera device.
 *
 * \param device opened camera device
 *
 * \threadsafety It is safe to call this function from any thread, but
 *               no thread may reference `device` once this function
 *               is called.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_OpenCameraWithSpec
 * \sa SDL_OpenCamera
 */
extern DECLSPEC void SDLCALL SDL_CloseCamera(SDL_Camera *camera);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include <SDL3/SDL_close_code.h>

#endif /* SDL_camera_h_ */