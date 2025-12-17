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

#include <violet/Container/Optional.h>
#include <violet/Filesystem/Path.h>
#include <violet/IO/Descriptor.h>
#include <violet/IO/Error.h>
#include <violet/Violet.h>

namespace violet {

struct ExitStatus final {
#ifdef VIOLET_UNIX
    using value_t = Int32;
#elif defined(VIOLET_WINDOWS)
    using value_t = DWORD;
#endif

    constexpr VIOLET_IMPLICIT ExitStatus() noexcept = default;
    constexpr VIOLET_IMPLICIT ExitStatus(value_t value) noexcept
        : n_code(value)
    {
    }

    [[nodiscard]] constexpr auto Success() const noexcept -> bool
    {
        return this->n_code == 0;
    }

    [[nodiscard]] auto ToString() const noexcept -> String;
    auto operator<<(std::ostream& os) const noexcept -> std::ostream&;

    constexpr VIOLET_IMPLICIT operator bool() const noexcept
    {
        return this->Success();
    }

private:
    value_t n_code = -1;
};

struct Output final {
    ExitStatus Status;
    Vec<UInt8> Stdout;
    Vec<UInt8> Stderr;
};

struct Subprocess final {
#ifdef VIOLET_UNIX
    using pre_exec_t = std::function<void()>;
#endif

    constexpr VIOLET_IMPLICIT Subprocess() noexcept = delete;

    VIOLET_IMPLICIT Subprocess(Str command) noexcept;
    VIOLET_IMPLICIT Subprocess(Str command, std::initializer_list<String> args) noexcept;
    VIOLET_EXPLICIT Subprocess(Str command, Vec<String> args) noexcept;

    auto WithArg(Str arg) noexcept -> Subprocess&;
    auto WithArgs(std::initializer_list<String> args) noexcept -> Subprocess&;
    auto WithArgs(Vec<String> args) noexcept -> Subprocess&;
    auto WithEnv(Str key, Str value) noexcept -> Subprocess&;
    auto WithWorkingDirectory(filesystem::PathRef path) noexcept -> Subprocess&;
    // auto WithStdin(subprocess::Stdio cfg) noexcept -> Subprocess&;
    // auto WithStdout(subprocess::Stdio cfg) noexcept -> Subprocess&;
    // auto WithStderr(subprocess::Stdio cfg) noexcept -> Subprocess&;

    [[nodiscard]] auto Output() const noexcept -> io::Result<Output>;
    [[nodiscard]] auto Status() const noexcept -> io::Result<ExitStatus>;

    [[nodiscard]] auto Descriptor() const noexcept -> io::FileDescriptor;

    VIOLET_EXPLICIT operator bool() const noexcept;
    VIOLET_EXPLICIT operator io::FileDescriptor() const noexcept;

#ifdef VIOLET_UNIX
    auto WithUID(uid_t uid) noexcept -> Subprocess&;
    auto WithGID(gid_t gid) noexcept -> Subprocess&;
    auto WithGroups(Vec<gid_t> groups) noexcept -> Subprocess&;
    auto WithPreExec(pre_exec_t exec) noexcept -> Subprocess&;
#endif

private:
    struct Impl;

    Impl* n_impl; // the inner, platform-dependent implementation
};

} // namespace violet
