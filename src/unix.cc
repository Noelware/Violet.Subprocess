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

#include <grp.h>
#include <unistd.h>

using violet::Str;
using violet::Subprocess;
using violet::Vec;

struct Subprocess::Impl final {
    VIOLET_IMPLICIT Impl(Str command) noexcept
        : n_command(command)
    {
    }

    VIOLET_IMPLICIT Impl(Str command, std::initializer_list<String> args) noexcept
        : n_command(command)
        , n_args(args)
    {
    }

    VIOLET_EXPLICIT Impl(Str command, Vec<String> args) noexcept
        : n_command(command)
        , n_args(VIOLET_MOVE(args))
    {
    }

    auto doSpawn() noexcept -> io::Result<void>;

private:
    friend struct Subprocess;

    io::FileDescriptor n_descriptor;
    FlatHashMap<String, String> n_env;
    String n_command;
    Vec<String> n_args;
    uid_t n_uid = -1;
    gid_t n_gid = -1;
    Vec<gid_t> n_groups;
    pre_exec_t n_pre_exec;

    Optional<filesystem::Path> n_wd = Nothing;
};

Subprocess::Subprocess(Str command) noexcept
    : n_impl(new Impl(command))
{
}

auto Subprocess::WithArg(Str arg) noexcept -> Subprocess&
{
    this->n_impl->n_args.emplace_back(arg);
    return *this;
}

auto Subprocess::WithArgs(std::initializer_list<String> args) noexcept -> Subprocess&
{
    this->n_impl->n_args.insert(this->n_impl->n_args.end(), args);
    return *this;
}

auto Subprocess::WithArgs(Vec<String> args) noexcept -> Subprocess&
{
    this->n_impl->n_args.insert(this->n_impl->n_args.end(), args.begin(), args.end());
    return *this;
}

auto Subprocess::WithEnv(Str key, Str value) noexcept -> Subprocess&
{
    this->n_impl->n_env.emplace(std::make_pair(key, value));
    return *this;
}

auto Subprocess::WithWorkingDirectory(filesystem::PathRef path) noexcept -> Subprocess&
{
    this->n_impl->n_wd = Some<filesystem::Path>(path.Data());
    return *this;
}

auto Subprocess::WithUID(uid_t uid) noexcept -> Subprocess&
{
    this->n_impl->n_uid = uid;
    return *this;
}

auto Subprocess::WithGID(gid_t gid) noexcept -> Subprocess&
{
    this->n_impl->n_gid = gid;
    return *this;
}

auto Subprocess::WithGroups(Vec<gid_t> groups) noexcept -> Subprocess&
{
    this->n_impl->n_groups.insert(this->n_impl->n_groups.end(), groups.begin(), groups.end());
    return *this;
}

auto Subprocess::WithPreExec(Subprocess::pre_exec_t exec) noexcept -> Subprocess&
{
    this->n_impl->n_pre_exec = VIOLET_MOVE(exec);
    return *this;
}

auto Subprocess::Descriptor() const noexcept -> io::FileDescriptor
{
    auto pid = this->n_impl->n_descriptor.Get();
    return { pid };
}

Subprocess::operator bool() const noexcept
{
    return this->n_impl->n_descriptor.Valid();
}

Subprocess::operator io::FileDescriptor() const noexcept
{
    return this->Descriptor();
}

namespace {

using violet::String;

auto buildArgv(Str command, const violet::Vec<String>& args) noexcept -> Vec<char*>
{
    Vec<char*> argv;
    argv.reserve(args.size() + 2);
    argv.push_back(const_cast<char*>(command.data())); // NOLINT(cppcoreguidelines-pro-type-const-cast)

    for (const auto& arg: args) {
        argv.push_back(const_cast<char*>(arg.c_str())); // NOLINT(cppcoreguidelines-pro-type-const-cast)
    }

    argv.push_back(nullptr);
    return argv;
}

struct EnvBlock {
    Vec<String> storage;
    Vec<char*> envp;
};

auto buildEnvp(const violet::FlatHashMap<String, String>& env) noexcept -> EnvBlock
{
    EnvBlock out;
    out.storage.reserve(env.size());
    out.envp.reserve(env.size() + 1);

    for (const auto& [key, value]: env) {
        out.storage.emplace_back(std::format("{}={}", key, value));
    }

    for (auto& arg: out.storage) {
        out.envp.push_back(arg.data());
    }

    out.envp.push_back(nullptr);
    return out;
}

void applyCredentials(uid_t uid, gid_t gid, const Vec<gid_t>& groups) noexcept
{
    if (gid != -1) {
        if (::setgid(gid) != 0) {
            _exit(127);
        }
    }

    if (!groups.empty()) {
        auto* gids = const_cast<gid_t*>(groups.data()); // NOLINT(cppcoreguidelines-pro-type-const-cast)
        if (::setgroups(static_cast<violet::Int32>(groups.size()), gids) != 0) {}
    }

    if (uid != -1) {
        if (::setuid(uid) != 0) {
            _exit(127);
        }
    }
}

} // namespace

auto Subprocess::Impl::doSpawn() noexcept -> io::Result<void>
{
    auto argv = buildArgv(this->n_command, this->n_args);

    EnvBlock block;
    if (!this->n_env.empty()) {
        block = buildEnvp(this->n_env);
    }

    pid_t pid = ::fork();
    if (pid < 0) {
        return Err(io::Error::OSError());
    }

    if (pid == 0) {
        // We are the child, let's bootstrap this process
        if (auto path = this->n_wd) {
            if (::chdir(path->Data().c_str()) != 0) {
                _exit(127);
            }
        }

        applyCredentials(this->n_uid, this->n_gid, this->n_groups);
        if (this->n_pre_exec) {
            std::invoke(this->n_pre_exec);
        }

        if (!this->n_env.empty()) {
            ::execve(this->n_command.c_str(), argv.data(), block.envp.data());
        } else {
            ::execvp(this->n_command.c_str(), argv.data());
        }

        _exit(127);
    }

    this->n_descriptor = io::FileDescriptor(pid);
    return {};
}

#endif
