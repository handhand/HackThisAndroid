//
// Created by Msii on 11/21/2024.
//

#ifndef ANDROID_NATIVE_GUARD_CONSTANTS_H
#define ANDROID_NATIVE_GUARD_CONSTANTS_H

#define _forceinline __attribute__((always_inline)) inline

#ifdef __LP64__
#define x32_64(_32, _64) _64
#else
#define x32_64(_32, _64) _32
#endif
#include "../../common/common.h"

using Pointer = unsigned long;

constexpr Pointer null = 0UL;

#endif //ANDROID_NATIVE_GUARD_CONSTANTS_H
