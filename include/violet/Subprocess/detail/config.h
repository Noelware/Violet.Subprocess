// 🌺💜 Violet.Subprocess: C++20 library for subprocesses in a cross-platform, secure way
// Copyright (c) 2025-2026 Noelware, LLC. <team@noelware.org>, et al.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#if !defined(VIOLET_SUBPROCESS_HAS_ATTRIBUTE) && defined(__has_attribute)
#define VIOLET_SUBPROCESS_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define VIOLET_SUBPROCESS_HAS_ATTRIBUTE(x) 0
#endif

/**
 * @macro VIOLET_SUBPROCESS_API
 */

/**
 * @macro VIOLET_SUBPROCESS_LOCAL
 */

#ifdef VIOLET_SUBPROCESS_BUILDING
#define VIOLET_SUBPROCESS_API
#define VIOLET_SUBPROCESS_LOCAL
#elif defined(_WIN32)
#ifdef VIOLET_SUBPROCESS_BUILD_SHARED
#define VIOLET_SUBPROCESS_API __declspec(dllexport)
#define VIOLET_SUBPROCESS_LOCAL
#elif defined(VIOLET_SUBPROCESS_USING_SHARED)
#define VIOLET_SUBPROCESS_API __declspec(dllimport)
#define VIOLET_SUBPROCESS_LOCAL
#else
#define VIOLET_SUBPROCESS_API __declspec(dllexport)
#define VIOLET_SUBPROCESS_LOCAL
#endif
#elif defined(VIOLET_SUBPROCESS_BUILD_SHARED)
#if VIOLET_SUBPROCESS_HAS_ATTRIBUTE(visibility)
#define VIOLET_SUBPROCESS_API __attribute__((visibility("default")))
#define VIOLET_SUBPROCESS_LOCAL __attribute__((visibility("hidden")))
#else
#define VIOLET_SUBPROCESS_API
#define VIOLET_SUBPROCESS_LOCAL
#endif
#else
#define VIOLET_SUBPROCESS_API
#define VIOLET_SUBPROCESS_LOCAL
#endif
