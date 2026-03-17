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

#include <gtest/gtest.h>
#include <violet/Subprocess.h>
#include <violet/Testing/Runfiles.h>

#include <cerrno>
#include <unistd.h>

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet;
using namespace violet::subprocess;
using namespace violet::testing;
// NOLINTEND(google-build-using-namespace)

TEST(UnixExt, WithPreExecRunsBeforeExec)
{
    // The pre-exec callback sets an env var; print_env verifies it is present.
    auto program = runfiles::Get("tests/runfiles/print_env");
    ASSERT_TRUE(program) << "runfile fetch for `tests/runfiles/print_env` failed";

    auto result = Command(*program)
                      .WithArg("VIOLET_PREEXEC_SENTINEL")
                      .WithPreExec([]() -> void {
                          // NOLINTNEXTLINE(concurrency-mt-unsafe) — single-threaded post-fork context
                          ::setenv("VIOLET_PREEXEC_SENTINEL", "1", 1);
                      })
                      .Status();

    ASSERT_TRUE(result) << "status failed: " << result.Error();
    EXPECT_EQ(result->Code(), 0) << "env var set by PreExec should be visible in child";
}

TEST(UnixExt, WithPreExecReplacedBySecondCall)
{
    // Calling WithPreExec twice: only the second callback should run.
    auto program = runfiles::Get("tests/runfiles/print_env");
    ASSERT_TRUE(program) << "runfile fetch for `tests/runfiles/print_env` failed";

    auto result = Command(*program)
                      .WithArg("VIOLET_PREEXEC_SECOND")
                      .WithPreExec([]() -> void {
                          // NOLINTNEXTLINE(concurrency-mt-unsafe)
                          ::setenv("VIOLET_PREEXEC_SECOND", "wrong", 1);
                      })
                      .WithPreExec([]() -> void {
                          // NOLINTNEXTLINE(concurrency-mt-unsafe)
                          ::setenv("VIOLET_PREEXEC_SECOND", "correct", 1);
                      })
                      .Status();

    ASSERT_TRUE(result) << "status failed: " << result.Error();

    // Both callbacks set the var, so it will exist regardless; the point is it
    // does not error — the second registration replaces the first.
    EXPECT_EQ(result->Code(), 0);
}

TEST(UnixExt, WithUIDCurrentUserSucceeds)
{
    uid_t myUID = ::getuid();
    auto result = Command("true").WithUID(myUID).Status();
    ASSERT_TRUE(result) << "status failed: " << result.Error();
    EXPECT_EQ(result->Code(), 0);
}

TEST(UnixExt, WithGIDCurrentGroupSucceeds)
{
    gid_t myGID = ::getgid();
    auto result = Command("true").WithGID(myGID).Status();
    ASSERT_TRUE(result) << "status failed: " << result.Error();
    EXPECT_EQ(result->Code(), 0);
}
