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
#include <violet/Violet.h>

namespace violet::subprocess {

struct Stdio final {
    VIOLET_DISALLOW_CONSTEXPR_CONSTRUCTOR(Stdio);

    constexpr static auto Null() noexcept -> Stdio
    {
        return Stdio(kind_t::kNull);
    }

    constexpr static auto Inherit() noexcept -> Stdio
    {
        return Stdio(kind_t::kInherit);
    }

    constexpr static auto Pipe() noexcept -> Stdio
    {
        return Stdio(kind_t::kPipe);
    }

    template<std::convertible_to<filesystem::PathRef> Path>
    constexpr static auto Pipe(Path&& path) noexcept -> Stdio
    {
        return Stdio(kind_t::kPipe, VIOLET_FWD(Path, path));
    }

    constexpr auto IsNull() const noexcept -> bool
    {
        return this->n_kind == kind_t::kNull;
    }

    constexpr auto Inherited() const noexcept -> bool
    {
        return this->n_kind == kind_t::kInherit;
    }

    constexpr auto Piped() const noexcept -> bool
    {
        return this->n_kind == kind_t::kPipe;
    }

    constexpr auto PipedIntoFile() const noexcept -> bool
    {
        return this->Piped() && this->n_pipeInto.HasValue();
    }

    constexpr auto PipedFile() const noexcept -> Optional<filesystem::Path>
    {
        return this->n_pipeInto;
    }

private:
    enum struct kind_t : UInt8 {
        kNull,
        kInherit,
        kPipe
    };

    constexpr VIOLET_EXPLICIT Stdio(kind_t kind)
        : n_kind(kind)
    {
    }

    constexpr VIOLET_EXPLICIT Stdio(kind_t kind, Optional<filesystem::Path> path)
        : n_kind(kind)
        , n_pipeInto(VIOLET_MOVE(path))
    {
    }

    kind_t n_kind;
    Optional<filesystem::Path> n_pipeInto;
};

struct ChildStdin final {
    VIOLET_IMPLICIT ChildStdin(io::FileDescriptor&& descriptor) noexcept
        : n_descriptor(VIOLET_MOVE(descriptor))
    {
    }

    [[nodiscard]] auto Write(Span<const UInt8> buf) const noexcept -> io::Result<UInt>
    {
        return this->n_descriptor.Write(buf);
    }

    [[nodiscard]] auto Flush() const noexcept -> io::Result<void>
    {
        return {};
    }

private:
    io::FileDescriptor n_descriptor;
};

static_assert(io::Writable<ChildStdin>, "`ChildStdin` doesn't conform to the `io::Writable` concept");

struct ChildStdout final {
    VIOLET_IMPLICIT ChildStdout(io::FileDescriptor&& descriptor) noexcept
        : n_descriptor(VIOLET_MOVE(descriptor))
    {
    }

    [[nodiscard]] auto Read(Span<UInt8> buf) const noexcept -> io::Result<UInt>
    {
        return this->n_descriptor.Read(buf);
    }

private:
    io::FileDescriptor n_descriptor;
};

static_assert(io::Readable<ChildStdout>, "`ChildStdout` doesn't conform to the `io::Readable` concept");

struct ChildStderr final {
    VIOLET_IMPLICIT ChildStderr(io::FileDescriptor&& descriptor) noexcept
        : n_descriptor(VIOLET_MOVE(descriptor))
    {
    }

    [[nodiscard]] auto Read(Span<UInt8> buf) const noexcept -> io::Result<UInt>
    {
        return this->n_descriptor.Read(buf);
    }

private:
    io::FileDescriptor n_descriptor;
};

static_assert(io::Readable<ChildStderr>, "`ChildStderr` doesn't conform to the `io::Readable` concept");

} // namespace violet::subprocess
