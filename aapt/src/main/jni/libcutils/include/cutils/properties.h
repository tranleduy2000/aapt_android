/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CUTILS_PROPERTIES_H
#define __CUTILS_PROPERTIES_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <sys/system_properties.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* System properties are *small* name value pairs managed by the
** property service.  If your data doesn't fit in the provided
** space it is not appropriate for a system property.
**
** WARNING: system/bionic/include/sys/system_properties.h also defines
**          these, but with different names.  (TODO: fix that)
*/
#define PROPERTY_KEY_MAX   PROP_NAME_MAX
#define PROPERTY_VALUE_MAX  PROP_VALUE_MAX

/* property_get: returns the length of the value which will never be
** greater than PROPERTY_VALUE_MAX - 1 and will always be zero terminated.
** (the length does not include the terminating zero).
**
** If the property read fails or returns an empty value, the default
** value is used (if nonnull).
*/
int property_get(const char *key, char *value, const char *default_value);

/* property_set: returns 0 on success, < 0 on failure
*/
int property_set(const char *key, const char *value);

int property_list(void (*propfn)(const char *key, const char *value, void *cookie), void *cookie);

#if defined(__BIONIC_FORTIFY)
#define __property_get_err_str "property_get() called with too small of a buffer"

#if defined(__clang__)

/* Some projects use -Weverything; diagnose_if is clang-specific. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgcc-compat"
int property_get(const char* key, char* value, const char* default_value)
    __clang_error_if(__bos(value) != __BIONIC_FORTIFY_UNKNOWN_SIZE &&
                         __bos(value) < PROPERTY_VALUE_MAX,
                     __property_get_err_str);
#pragma clang diagnostic pop

#else /* defined(__clang__) */

extern int __property_get_real(const char *, char *, const char *)
    __asm__(__USER_LABEL_PREFIX__ "property_get");
__errordecl(__property_get_too_small_error, __property_get_err_str);

__BIONIC_FORTIFY_INLINE
int property_get(const char *key, char *value, const char *default_value) {
    size_t bos = __bos(value);
    if (bos < PROPERTY_VALUE_MAX) {
        __property_get_too_small_error();
    }
    return __property_get_real(key, value, default_value);
}

#endif /* defined(__clang__) */

#undef __property_get_err_str
#endif /* defined(__BIONIC_FORTIFY) */

#ifdef __cplusplus
}
#endif

#endif
