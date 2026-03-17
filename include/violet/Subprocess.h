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
#include <violet/Subprocess/ExitStatus.h>
#include <violet/Subprocess/PID.h>
#include <violet/Subprocess/Stdio.h>
#include <violet/Violet.h>

namespace violet::subprocess {

struct Child final {
    struct PID PID;
    Optional<ChildStdin> Stdin;
    Optional<ChildStdout> Stdout;
    Optional<ChildStderr> Stderr;

    auto Wait() const noexcept -> io::Result<ExitStatus>;
    auto ToString() const noexcept -> String;

    friend auto operator<<(std::ostream& os, const Child& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }
};

/// Captures the output of a [`Subprocess`].
///
/// ## Example
/// ```cpp
/// #include <violet/Subprocess.h>
///
/// violet::Command cmd("echo");
/// cmd = cmd.WithArg("hello");
///
/// if (auto output = cmd.Output()) {
///     violet::String out(output->Stdout.begin(), output->Stdout.end());
///     std::cout << "captured stdout: " << captured << '\n';
/// }
/// ```
struct Output final {
    ExitStatus Status; ///< the exit code of the subprocess
    Vec<UInt8> Stdout; ///< the standard output from the process
    Vec<UInt8> Stderr; ///< the standard error from the process
};

#ifdef VIOLET_UNIX
using PreExecFun = std::function<void()>;
#endif

struct Command final {
    VIOLET_DISALLOW_CONSTRUCTOR(Command);

    VIOLET_IMPLICIT Command(Str program) noexcept;
    VIOLET_IMPLICIT Command(Str program, std::initializer_list<String> args) noexcept;
    VIOLET_IMPLICIT Command(Str program, Vec<String> args) noexcept;

    auto WithArg(Str arg) noexcept -> Command&;
    auto WithArgs(std::initializer_list<String> args) noexcept -> Command&;
    auto WithArgs(Span<String> args) noexcept -> Command&;
    auto WithEnv(Str key, Str value) noexcept -> Command&;
    auto WithEnvs(std::initializer_list<Pair<String, String>> envs) noexcept -> Command&;
    auto WithEnvs(Span<Pair<String, String>> envs) noexcept -> Command&;
    auto WithWorkingDirectory(filesystem::PathRef path) noexcept -> Command&;
    auto WithStdin(Stdio cfg) noexcept -> Command&;
    auto WithStdout(Stdio cfg) noexcept -> Command&;
    auto WithStderr(Stdio cfg) noexcept -> Command&;

#ifdef VIOLET_UNIX
    auto WithUID(uid_t uid) noexcept -> Command&;
    auto WithGID(gid_t gid) noexcept -> Command&;
    auto WithGroups(std::initializer_list<gid_t> groups) noexcept -> Command&;
    auto WithGroups(Span<gid_t> groups) noexcept -> Command&;
    auto WithPreExec(PreExecFun exec) noexcept -> Command&;
#endif

    [[nodiscard]] auto Output() const noexcept -> io::Result<Output>;
    [[nodiscard]] auto Status() const noexcept -> io::Result<ExitStatus>;
    [[nodiscard]] auto Spawn() const noexcept -> io::Result<Child>;

private:
    struct Impl;

    Impl* n_impl;
};

} // namespace violet::subprocess
