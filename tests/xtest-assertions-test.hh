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

#ifndef XTEST_TESTS_XTEST_ASSERTIONS_TEST_HH_
#define XTEST_TESTS_XTEST_ASSERTIONS_TEST_HH_

#include <string>

#include "redirector.hh"
#include "xtest-assertions.hh"
#include "xtest.hh"

TEST(AssertionContextTest, ReturnsFileName) {
  const char* file = "tests/xtest-assertions-test.hh";
  xtest::internal::AssertionContext context(file, 0, nullptr);
  EXPECT_EQ(file, context.file());
}

TEST(AssertionContextTest, ReturnsFileNameAndLineNumber) {
  const char* file = "tests/xtest-assertions-test.hh";
  const uint64_t line = 42;
  xtest::internal::AssertionContext context(file, line, nullptr);
  EXPECT_EQ(file, context.file());
  EXPECT_EQ(line, context.line());
}

TEST(AssertionContextTest, ReturnsFileNameLineNumberAndPointerToCurrentTest) {
  const char* file = "tests/xtest-assertions-test.hh";
  const uint64_t line = 42;
  xtest::internal::AssertionContext context(
      file, line,
      current_test);  // `current_test` is in the scope of the test function.
  EXPECT_EQ(file, context.file());
  EXPECT_EQ(line, context.line());
  EXPECT_EQ(current_test, context.current_test());
}

TEST(PrettyAssertionResultPrinterTest, OnTestAssertionStartTest) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::PrettyAssertionResultPrinter::OnTestAssertionStart(
      current_test);
  stdout_redirector_context.RestoreStream();
  std::string actual(stdout_redirector_context.M_output_buffer_);
  std::string expected(
      "\x1b[0;32m[ RUN      ] "
      "\x1b[mPrettyAssertionResultPrinterTest.OnTestAssertionStartTest\n");
  EXPECT_EQ(actual, expected);
}

TEST(PrettyAssertionResultPrinterTest,
     OnTestAssertionEndTestWhenTestResultIsPassed) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  current_test->test_result_ = xtest::TestResult::PASSED;
  xtest::internal::PrettyAssertionResultPrinter::OnTestAssertionEnd(current_test,
                                                                    100);
  stdout_redirector_context.RestoreStream();
  std::string actual(stdout_redirector_context.M_output_buffer_);
  std::string expected(
      "\x1b[0;32m[       OK ] "
      "\x1b[mPrettyAssertionResultPrinterTest."
      "OnTestAssertionEndTestWhenTestResultIsPassed (100 "
      "ms)\n");
  EXPECT_EQ(actual, expected);
}

TEST(PrettyAssertionResultPrinterTest,
     OnTestAssertionEndTestWhenTestResultIsFailed) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  current_test->test_result_ = xtest::TestResult::FAILED;
  xtest::internal::PrettyAssertionResultPrinter::OnTestAssertionEnd(current_test,
                                                                    100);
  stdout_redirector_context.RestoreStream();
  std::string actual(stdout_redirector_context.M_output_buffer_);
  std::string expected(
      "\x1b[0;31m[  FAILED  ] "
      "\x1b[mPrettyAssertionResultPrinterTest."
      "OnTestAssertionEndTestWhenTestResultIsFailed (100 "
      "ms)\n");
  EXPECT_EQ(actual, expected);
}

TEST(PrettyAssertionResultPrinterTest, OnTestAssertionFailureTest) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStderr);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::PrettyAssertionResultPrinter::OnTestAssertionFailure(
      "lhs_expression", "rhs_expression", "lhs_expression", "rhs_expression",
      xtest::internal::AssertionContext(__FILE__, 120, current_test));
  stdout_redirector_context.RestoreStream();
  std::string actual(stdout_redirector_context.M_output_buffer_);
  std::string expected(__FILE__
                       "(120): error: Value of: lhs_expression\n  Actual: "
                       "lhs_expression\nExpected: rhs_expression\n");
  EXPECT_EQ(actual, expected);
}

TEST(AssertionResultTest, WhenAssertionResultIsTrue) {
  xtest::testing::RedirectorContext stderr_redirector_context(
      xtest::testing::RedirectorContextStream::kStderr);
  stderr_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::AssertionResult(true) << "Failed AssertionResult";
  stderr_redirector_context.RestoreStream();
  EXPECT_EQ(stderr_redirector_context.M_output_buffer_[0], '\0');
}

TEST(AssertionResultTest, WhenAssertionResultAndIsFatalIsTrue) {
  xtest::testing::RedirectorContext stderr_redirector_context(
      xtest::testing::RedirectorContextStream::kStderr);
  stderr_redirector_context.ReplaceStreamWithContextBuffer();
  // AssertionResult instance must not raise `abort` signal.
  xtest::internal::AssertionResult(true, true) << "Failed AssertionResult";
  stderr_redirector_context.RestoreStream();
  EXPECT_EQ(stderr_redirector_context.M_output_buffer_[0], '\0');
}

TEST(AssertionResultTest, WhenAssertionResultIsTrueButFatalIsFalse) {
  xtest::testing::RedirectorContext stderr_redirector_context(
      xtest::testing::RedirectorContextStream::kStderr);
  stderr_redirector_context.ReplaceStreamWithContextBuffer();
  // AssertionResult instance must not raise `abort` signal.
  xtest::internal::AssertionResult(true, false) << "Failed AssertionResult";
  stderr_redirector_context.RestoreStream();
  EXPECT_EQ(stderr_redirector_context.M_output_buffer_[0], '\0');
}

TEST(AssertionResultTest, WhenAssertionResultIsFalse) {
  xtest::testing::RedirectorContext stderr_redirector_context(
      xtest::testing::RedirectorContextStream::kStderr);
  stderr_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::AssertionResult(false) << "Failed AssertionResult";
  stderr_redirector_context.RestoreStream();
  std::string actual(stderr_redirector_context.M_output_buffer_);
  std::string expected("Failed AssertionResult\n");
  EXPECT_EQ(actual, expected);
}

TEST(AssertionResultTest, WhenAssertionResultAndIsFatalIsFalse) {
  xtest::testing::RedirectorContext stderr_redirector_context(
      xtest::testing::RedirectorContextStream::kStderr);
  stderr_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::AssertionResult(false, false) << "Failed AssertionResult";
  stderr_redirector_context.RestoreStream();
  std::string actual(stderr_redirector_context.M_output_buffer_);
  std::string expected("Failed AssertionResult\n");
  EXPECT_EQ(actual, expected);
}

TEST(AssertionSuccessTest, WhenAssertionResultIsTrue) {
  xtest::testing::RedirectorContext stderr_redirector_context(
      xtest::testing::RedirectorContextStream::kStderr);
  stderr_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::AssertionSuccess() << "AssertionSuccess";
  stderr_redirector_context.RestoreStream();
  EXPECT_EQ(stderr_redirector_context.M_output_buffer_[0], '\0');
}

TEST(AssertionFailureTest, WhenAssertionResultIsFalseButFatalIsFalse) {
  xtest::testing::RedirectorContext stderr_redirector_context(
      xtest::testing::RedirectorContextStream::kStderr);
  stderr_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::internal::AssertionFailure(false) << "AssertionFailure";
  stderr_redirector_context.RestoreStream();
  std::string actual(stderr_redirector_context.M_output_buffer_);
  std::string expected("AssertionFailure\n");
  EXPECT_EQ(actual, expected);
}

#endif  // XTEST_TESTS_XTEST_ASSERTIONS_TEST_HH_
