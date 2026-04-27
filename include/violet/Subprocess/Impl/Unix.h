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

#include "violet/Subprocess/detail/config.h"

#include <violet/Subprocess.h>

namespace violet::subprocess {

/// Implementation for [`Command`] and its state for Unix-like systems.
struct VIOLET_SUBPROCESS_API Command::Impl final {
    VIOLET_DISALLOW_CONSTRUCTOR(Impl);

private:
    friend struct Command;

    VIOLET_IMPLICIT Impl(Str program) noexcept;
    VIOLET_IMPLICIT Impl(Str program, const Vec<String>& args) noexcept;
    VIOLET_IMPLICIT Impl(Str program, std::initializer_list<String> args) noexcept;

    String n_program;
    Vec<String> n_args;
    UnorderedMap<String, String> n_environ;
    Optional<filesystem::Path> n_wd;
    Stdio n_stdin = Stdio::Inherit();
    Stdio n_stdout = Stdio::Inherit();
    Stdio n_stderr = Stdio::Inherit();
    Optional<uid_t> n_uid;
    Optional<gid_t> n_gid;
    Vec<gid_t> n_extraGroupIDs;
    Optional<PreExecFun> n_exec;
    Optional<std::chrono::milliseconds> n_deathTimeout;
};

} // namespace violet::subprocess
