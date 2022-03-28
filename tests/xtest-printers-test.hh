// Copyright 2021, The xtest authors.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of The xtest authors. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef XTEST_TESTS_XTEST_PRINTERS_HH_
#define XTEST_TESTS_XTEST_PRINTERS_HH_

#include "internal/xtest-port-arch.hh"
#include "internal/xtest-printers.hh"
#include "redirector.hh"
#include "xtest.hh"

TEST(ShouldUseColorTest, TestBeforeAndAfterRedirection) {
  // Test before redirecting `stdout`; `ShouldUseColor()` must return `true`.
  EXPECT_TRUE(xtest::internal::ShouldUseColor());
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  // Test after redirecting `stdout`; `ShouldUseColor()` must return `false`.
  EXPECT_FALSE(xtest::internal::ShouldUseColor());
  stdout_redirector_context.RestoreStream();
  // Test after restoring `stdout`; `ShouldUseColor()` must return `true`.
  EXPECT_TRUE(xtest::internal::ShouldUseColor());
}

TEST(ColoredPrintfTest,
     TestIfItPrintsColoredOutputInBothWindowsAndUnixSystems) {
#if XTEST_OS_WINDOWS
  // In order to test ColoredPrintf's output we need to redirect `stdout` which
  // will cause ColoredPrintf() to disable color output.
  //
  // On windows we need to read the console text attributes in order to test
  // them. Do we have a way to do capture the console text attributes?

  // Until we figure it out use a EXPECT_TRUE assertion with a argument `true`.
  EXPECT_TRUE(true);
#else
  // In order to test ColoredPrintf's output we need to redirect `stdout` which
  // will cause ColoredPrintf() to disable color output.
  //
  // Do we have a way to test if ColoredPrintf() does what it supposed to do
  // regardless of the operating system?

  // Until we figure it out use a EXPECT_TRUE assertion with a argument `true`.
  EXPECT_TRUE(true);
#endif
}

#endif  // XTEST_TESTS_XTEST_PRINTERS_HH_
