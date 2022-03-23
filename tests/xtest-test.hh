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

#ifndef XTEST_TESTS_XTEST_TEST_HH_
#define XTEST_TESTS_XTEST_TEST_HH_

#include <cstdio>
#include <cstring>
#include <string>

#include "internal/xtest-printers.hh"
#include "redirector.hh"
#include "xtest.hh"

// Verifies that the command line flag variables can be accessed in code once
// `xtest.hh` has been included.
TEST(CommandLineFlagsTest, CanBeAccessedInCodeOnceXTestHIsIncluded) {
  bool dummy = XTEST_FLAG_GET_(help) || XTEST_FLAG_GET_(shuffle) ||
               XTEST_FLAG_GET_(color) != "unknown";
  EXPECT_TRUE(dummy || !dummy);  // Suppresses warning that dummy is unused.
}

TEST(XtestDefaultSummaryStatusStrWidthTest, CheckIfVisible) {
  EXPECT_EQ(XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_, 10);
}

TEST(PrettyUnitTestResultPrinterTest, StaticMethodPrintTestName) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::PrettyUnitTestResultPrinter::PrintTestName(currentTest->M_suiteName,
                                                    currentTest->M_testName);
  stdout_redirector_context.RestoreStream();
  const std::string actual(stdout_redirector_context.M_output_buffer_);
  const std::string expected(
      "PrettyUnitTestResultPrinterTest.StaticMethodPrintTestName");
  EXPECT_EQ(actual, expected);
}

TEST(PrettyUnitTestResultPrinterTest, StaticMethodOnTestIterationStart) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::PrettyUnitTestResultPrinter::OnTestIterationStart();
  stdout_redirector_context.RestoreStream();
  const std::string actual(stdout_redirector_context.M_output_buffer_);
  char expected[REDIRECTOR_BUFFER_SIZE];
  std::snprintf(expected, REDIRECTOR_BUFFER_SIZE,
                "[%s] Running %lu tests from %lu test suites.\n",
                xtest::GetStrFilledWith('=').c_str(),
                xtest::GetTestSuiteAndTestNumber().second,
                xtest::GetTestSuiteAndTestNumber().first);
  EXPECT_EQ(actual, expected);
}

// Testing static method `OnTestIterationEnd` is one of the most trickiest test
// in library `xtest`.  You'll better understand it when you'll walk down the
// test function.
TEST(PrettyUnitTestResultPrinterTest, StaticMethodOnTestIterationEnd) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::PrettyUnitTestResultPrinter::OnTestIterationEnd();
  stdout_redirector_context.RestoreStream();
  const std::string actual(stdout_redirector_context.M_output_buffer_);

  // We have to create a string identical to the result of
  // `PrettyUnitTestResultPrinter::OnTestIterationEnd()` function.  Calling
  // `PrettyUnitTestResultPrinter::OnTestIterationEnd()` function is going to
  // produce a result for these tests so far with their passed and failed
  // status so we have to simulate the behaviour of function
  // `PrettyUnitTestResultPrinter::OnTestIterationEnd()` here inside this test
  // suite.
  char expected[REDIRECTOR_BUFFER_SIZE];

  // Create the `expected` string with the summary of all the tests and passed
  // tests.
  std::snprintf(
      expected, REDIRECTOR_BUFFER_SIZE,
      "[%s] Ran %lu tests from %lu test suites.\n[%s] %lu test.\n",
      xtest::GetStrFilledWith('=').c_str(),
      xtest::GetTestSuiteAndTestNumber().second,
      xtest::GetTestSuiteAndTestNumber().first,
      xtest::GetStringAlignedTo("PASSED",
                                XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
                                xtest::StringAlignValues::ALIGN_CENTER)
          .c_str(),
      xtest::GetTestSuiteAndTestNumber().second - xtest::GetFailedTestCount());

  // Now here comes the trickiest part of this test suite, we are going to
  // simulate the behaviour of function
  // `PrettyUnitTestResultPrinter::PrintFailedTests()` that is called inside of
  // the function `PrettyUnitTestResultPrinter::OnTestIterationEnd()` when there
  // are some failed tests.
  if (xtest::GetFailedTestCount() != 0) {
    // Because of limited memory
    // `REDIRECTOR_BUFFER_SIZE - std::strlen(expected)` is going to be the
    // amount of room left for failed tests output.
    char* failed_tests_output =
        new char[REDIRECTOR_BUFFER_SIZE - std::strlen(expected)];

    // Create a string with the number of failed tests to later concatenate with
    // the `expected` string.
    std::snprintf(failed_tests_output,
                  REDIRECTOR_BUFFER_SIZE - std::strlen(expected),
                  "[%s] %lu test, listed below:\n",
                  xtest::GetStringAlignedTo(
                      "FAILED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
                      xtest::StringAlignValues::ALIGN_CENTER)
                      .c_str(),
                  xtest::GetFailedTestCount());

    xtest::XTestUnitTest failedTests = xtest::GetFailedTests();
    for (const xtest::XTestUnitTestPair& testCase : failedTests) {
      for (const xtest::TestRegistrar* const& test : testCase.second) {
        // Because of limited memory
        // `REDIRECTOR_BUFFER_SIZE - std::strlen(expected) -
        // std::strlen(failed_tests_output)` is going to be the amount of room
        // left for failed tests description.
        char* failed_tests =
            new char[REDIRECTOR_BUFFER_SIZE - std::strlen(expected) -
                     std::strlen(failed_tests_output)];

        // When there are failed tests we want to see the name of there test
        // suite and there test name.
        std::snprintf(failed_tests,
                      REDIRECTOR_BUFFER_SIZE - std::strlen(expected) -
                          std::strlen(failed_tests_output),
                      "[%s] %s.%s\n",
                      xtest::GetStringAlignedTo("FAILED").c_str(),
                      test->M_suiteName, test->M_testName);

        // Concatenate `failed_tests` string with `failed_tests_output` to see a
        // complete description of tests that failed.
        std::strcat(failed_tests_output, failed_tests);
        delete[] failed_tests;
      }
    }

    // Because of limited memory
    // `REDIRECTOR_BUFFER_SIZE - std::strlen(expected) -
    // std::strlen(failed_tests_output)` is going to be the amount of room left
    // for failed tests summary.
    char* summarize_failed_tests =
        new char[REDIRECTOR_BUFFER_SIZE - std::strlen(expected) -
                 std::strlen(failed_tests_output)];

    // Create a summary string to see how many number of tests failed.
    std::snprintf(summarize_failed_tests,
                  REDIRECTOR_BUFFER_SIZE - std::strlen(expected) -
                      std::strlen(failed_tests_output),
                  "\n%lu FAILED %s\n", xtest::GetFailedTestCount(),
                  (xtest::GetFailedTestCount() == 1 ? "TEST" : "TESTS"));

    // Concatenate summary onto the `failed_tests_output` string.
    std::strcat(failed_tests_output, summarize_failed_tests);
    delete[] summarize_failed_tests;
    // Finally concatenate `failed_tests_output` string onto `expected`.
    std::strcat(expected, failed_tests_output);
    delete[] failed_tests_output;
  }
  EXPECT_EQ(actual, expected);
}

TEST(PrettyUnitTestResultPrinterTest, StaticMethodOnEnvironmentsSetUpStart) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::PrettyUnitTestResultPrinter::OnEnvironmentsSetUpStart();
  stdout_redirector_context.RestoreStream();
  const std::string actual(stdout_redirector_context.M_output_buffer_);
  char expected[REDIRECTOR_BUFFER_SIZE];
  std::snprintf(expected, REDIRECTOR_BUFFER_SIZE,
                "[%s] Global test environment set-up.",
                xtest::GetStrFilledWith('-').c_str());
  EXPECT_EQ(actual, expected);
}

TEST(PrettyUnitTestResultPrinterTest, StaticMethodOnEnvironmentsTearDownStart) {
  xtest::testing::RedirectorContext stdout_redirector_context(
      xtest::testing::RedirectorContextStream::kStdout);
  stdout_redirector_context.ReplaceStreamWithContextBuffer();
  xtest::PrettyUnitTestResultPrinter::OnEnvironmentsTearDownStart();
  stdout_redirector_context.RestoreStream();
  const std::string actual(stdout_redirector_context.M_output_buffer_);
  char expected[REDIRECTOR_BUFFER_SIZE];
  std::snprintf(expected, REDIRECTOR_BUFFER_SIZE,
                "\n[%s] Global test environment tear-down.\n",
                xtest::GetStrFilledWith('-').c_str());
  EXPECT_EQ(actual, expected);
}

#endif  // XTEST_TESTS_XTEST_TEST_HH_
