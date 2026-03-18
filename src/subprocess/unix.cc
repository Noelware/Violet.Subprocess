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

#include <violet/Violet.h>

#ifdef VIOLET_UNIX

#include <violet/Subprocess.h>
#include <violet/Subprocess/Impl/Unix.h>

using violet::Array;
using violet::UInt8;
using violet::Vec;
using violet::subprocess::Command;
using violet::subprocess::Stdio;

Command::Impl::Impl(Str program) noexcept
    : Impl(program, {})
{
}

Command::Impl::Impl(Str program, const Vec<String>& args) noexcept
    : n_program(program)
    , n_args(args)
{
}

Command::Impl::Impl(Str program, std::initializer_list<String> args) noexcept
    : n_program(program)
    , n_args(args)
{
}

Command::Command(Str program) noexcept
    : n_impl(new Impl(program))
{
}

Command::Command(Str program, std::initializer_list<String> args) noexcept
    : n_impl(new Impl(program, args))
{
}

Command::Command(Str program, const Vec<String>& args) noexcept
    : n_impl(new Impl(program, args))
{
}

Command::~Command()
{
    if (this->n_impl != nullptr) {
        delete this->n_impl;
        this->n_impl = nullptr;
    }
}

auto Command::WithArg(Str arg) noexcept -> Command&
{
    this->n_impl->n_args.emplace_back(arg);
    return *this;
}

auto Command::WithArgs(std::initializer_list<String> args) noexcept -> Command&
{
    this->n_impl->n_args.insert(this->n_impl->n_args.end(), args.begin(), args.end());
    return *this;
}

auto Command::WithArgs(Span<String> args) noexcept -> Command&
{
    this->n_impl->n_args.insert(this->n_impl->n_args.end(), args.begin(), args.end());
    return *this;
}

auto Command::WithEnv(Str key, Str value) noexcept -> Command&
{
    this->n_impl->n_environ.insert(std::make_pair(key, value));
    return *this;
}

auto Command::WithEnvs(std::initializer_list<Pair<String, String>> envs) noexcept -> Command&
{
    this->n_impl->n_environ.insert(envs.begin(), envs.end());
    return *this;
}

auto Command::WithEnvs(Span<Pair<String, String>> envs) noexcept -> Command&
{
    this->n_impl->n_environ.insert(envs.begin(), envs.end());
    return *this;
}

auto Command::WithStdin(const Stdio& cfg) noexcept -> Command&
{
    this->n_impl->n_stdin = cfg;
    return *this;
}

auto Command::WithStdout(const Stdio& cfg) noexcept -> Command&
{
    this->n_impl->n_stdout = cfg;
    return *this;
}

auto Command::WithStderr(const Stdio& cfg) noexcept -> Command&
{
    this->n_impl->n_stderr = cfg;
    return *this;
}

auto Command::WithUID(uid_t uid) noexcept -> Command&
{
    this->n_impl->n_uid = uid;
    return *this;
}

auto Command::WithGID(gid_t gid) noexcept -> Command&
{
    this->n_impl->n_gid = gid;
    return *this;
}

auto Command::WithGroups(std::initializer_list<gid_t> groups) noexcept -> Command&
{
    this->n_impl->n_extraGroupIDs.insert(this->n_impl->n_extraGroupIDs.end(), groups.begin(), groups.end());
    return *this;
}

auto Command::WithGroups(Span<gid_t> groups) noexcept -> Command&
{
    this->n_impl->n_extraGroupIDs.insert(this->n_impl->n_extraGroupIDs.end(), groups.begin(), groups.end());
    return *this;
}

auto Command::WithPreExec(const PreExecFun& exec) noexcept -> Command&
{
    this->n_impl->n_exec.Replace(exec);
    return *this;
}

auto Command::WithWorkingDirectory(filesystem::PathRef path) noexcept -> Command&
{
    this->n_impl->n_wd = filesystem::Path(path.ToString());
    return *this;
}

#endif
