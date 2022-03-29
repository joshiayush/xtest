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

#include <string>

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

TEST(ColoredPrintfTest, TestWhenDefaultIsUsedInBothWindowsAndUnixSystems) {
#if XTEST_OS_WINDOWS
  // On windows we need to read the console text attributes in order to test
  // them. Do we have a way to capture the console text attributes?

  // Until we figure it out use a EXPECT_TRUE assertion with a argument `true`.
  EXPECT_TRUE(true);
#else
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::ColoredPrintf(xtest::internal::XTestColor::kDefault,
                                 "Testing for %s color.", "default");
  stdout_redirector_context.RestoreStream();
  std::string actual(stdout_redirector_context.M_output_buffer_);
  std::string expected("\x1b[0;3mTesting for default color.\x1b[m");
  EXPECT_EQ(actual, expected);
#endif
}

TEST(ColoredPrintfTest, TestWhenRedIsUsedInBothWindowsAndUnixSystems) {
#if XTEST_OS_WINDOWS
  // On windows we need to read the console text attributes in order to test
  // them. Do we have a way to capture the console text attributes?

  // Until we figure it out use a EXPECT_TRUE assertion with a argument `true`.
  EXPECT_TRUE(true);
#else
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::ColoredPrintf(xtest::internal::XTestColor::kRed,
                                 "Testing for %s color.", "default");
  stdout_redirector_context.RestoreStream();
  std::string actual(stdout_redirector_context.M_output_buffer_);
  std::string expected("\x1b[0;31mTesting for default color.\x1b[m");
  EXPECT_EQ(actual, expected);
#endif
}

TEST(ColoredPrintfTest, TestWhenGreenIsUsedInBothWindowsAndUnixSystems) {
#if XTEST_OS_WINDOWS
  // On windows we need to read the console text attributes in order to test
  // them. Do we have a way to capture the console text attributes?

  // Until we figure it out use a EXPECT_TRUE assertion with a argument `true`.
  EXPECT_TRUE(true);
#else
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::ColoredPrintf(xtest::internal::XTestColor::kGreen,
                                 "Testing for %s color.", "default");
  stdout_redirector_context.RestoreStream();
  std::string actual(stdout_redirector_context.M_output_buffer_);
  std::string expected("\x1b[0;32mTesting for default color.\x1b[m");
  EXPECT_EQ(actual, expected);
#endif
}
TEST(ColoredPrintfTest, TestWhenYellowIsUsedInBothWindowsAndUnixSystems) {
#if XTEST_OS_WINDOWS
  // On windows we need to read the console text attributes in order to test
  // them. Do we have a way to capture the console text attributes?

  // Until we figure it out use a EXPECT_TRUE assertion with a argument `true`.
  EXPECT_TRUE(true);
#else
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::ColoredPrintf(xtest::internal::XTestColor::kYellow,
                                 "Testing for %s color.", "default");
  stdout_redirector_context.RestoreStream();
  std::string actual(stdout_redirector_context.M_output_buffer_);
  std::string expected("\x1b[0;33mTesting for default color.\x1b[m");
  EXPECT_EQ(actual, expected);
#endif
}

TEST(PrintColorEncodedTest, TestWithXTestColorEncodedHelpMessage) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::PrintColorEncoded(
      "This program contains tests written using xtest.  You can use the\n"
      "following command line flags to control its behaviour:\n"
      "\n"
      "Test Execution:\n"
      "   @G--shuffle@D\n"
      "     Randomize tests' order. (In development)\n");
  stdout_redirector_context.RestoreStream();
  std::string actual(stdout_redirector_context.M_output_buffer_);
  std::string expected(
      "\x1b[0;3mThis program contains tests written using xtest.  You can use "
      "the\n"
      "following command line flags to control its behaviour:\n"
      "\n"
      "Test Execution:\n"
      "   \x1b[m\x1b[0;32m--shuffle\x1b[m\x1b[0;3m\n"
      "     Randomize tests' order. (In development)\n\x1b[m");
  EXPECT_EQ(actual, expected);
}

#endif  // XTEST_TESTS_XTEST_PRINTERS_HH_
