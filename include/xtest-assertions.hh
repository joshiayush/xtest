// Copyright 2021, xtest Inc.
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
//     * Neither the name of xtest Inc. nor the names of its
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

#ifndef XTEST_INCLUDE_XTEST_ASSERTIONS_HH_
#define XTEST_INCLUDE_XTEST_ASSERTIONS_HH_

#include <cstdlib>
#include <iostream>

// Aborts the current process if fatal is true.
//
// This implementation detail macro IMPL__TEST_FAILED aborts the current
// process if the fatal is true, otherwise increments the global variable
// xtest::g_n_test_failures by 1 which keeps a counter of the number of
// non-fatal tests that failed so far.
//
// In case of a test failure this function also marks the result of the test
// suite as FAILED by setting up the current_test->m_result variable to
// ::xtest::TestResult::FAILED.
//
// Variable current_test is passed to the test suite from the function
// ::xtest::run_registered_tests().
//
// fatal can be anything that evaluates to true when used in a if clause
// so make sure that you provide a valid boolean value to this macro.
#define IMPL__TEST_FAILED(fatal)                             \
  do {                                                       \
    currentTest->M_testResult = ::xtest::TestResult::FAILED; \
    ++::xtest::G_n_testFailures;                             \
    if (fatal)                                               \
      abort();                                               \
  } while (false)

// Trace out the test failure with information like the source file, line
// number, received object and what we're expecting.
#define IMPL__TEST_FAILURE_TRACE(actual, expected)                             \
  ::xtest::impl::AssertionFailure()                                            \
      << __FILE__ << '(' << __LINE__ << "): error: Value of: " << #actual      \
      << '\n'                                                                  \
      << "  Actual: " << ::xtest::internal::StreamableToString(actual) << '\n' \
      << "Expected: " << ::xtest::internal::StreamableToString(expected)       \
      << '\n';

#define IMPL__TEST_EQUALITY_FAILURE_MSG()                                 \
  ::xtest::impl::AssertionFailure()                                       \
      << '['                                                              \
      << ::xtest::GetStringAlignedTo("FAILED", 10, ::xtest::ALIGN_CENTER) \
      << ']' << ' ' << currentTest->M_suiteName << '.'                    \
      << currentTest->M_testName << '\n';

#define IMPL__TEST_EQUALITY_SUCCESS_MSG()                                    \
  ::std::cerr << '['                                                         \
              << ::xtest::GetStringAlignedTo("OK", 10, ::xtest::ALIGN_RIGHT) \
              << ']' << ' ' << currentTest->M_suiteName << '.'               \
              << currentTest->M_testName << '\n';

#define IMPL__TEST_ASSERTION_SETUP()                                         \
  ::std::cerr << '['                                                         \
              << ::xtest::GetStringAlignedTo("RUN", 10, ::xtest::ALIGN_LEFT) \
              << ']' << ' ' << currentTest->M_suiteName << '.'               \
              << currentTest->M_testName << '\n';

// Does a equality check between the actual and the expected value.
//
// This implementation detail macro IMPL__TEST_EQ does a equality check between
// the value actual and expected provided that both of the operands must have a
// overridden implementation for the equality check operator i.e., (==).
//
// In case the expressions are not equal this macro prints a failure message on
// the stderr file stream and calls the operations defined in IMPL__TEST_FAILED
// macro to decide whether to abort the current process or just simply increment
// xtest::g_n_test_failures.
#define IMPL__TEST_EQ(actual, expected, fatal)    \
  do {                                            \
    IMPL__TEST_ASSERTION_SETUP();                 \
    if ((actual) != (expected)) {                 \
      IMPL__TEST_FAILURE_TRACE(actual, expected); \
      IMPL__TEST_EQUALITY_FAILURE_MSG();          \
      IMPL__TEST_FAILED(fatal);                   \
    } else {                                      \
      IMPL__TEST_EQUALITY_SUCCESS_MSG();          \
    }                                             \
  } while (false)

#define EXPECT_EQ(actual, expected) IMPL__TEST_EQ(actual, expected, false)
#define ASSERT_EQ(actual, expected) IMPL__TEST_EQ(actual, expected, true)

// Does a non-equality check between the actual and the expected value.
//
// This implementation detail macro IMPL__TEST_NE does a non-equality check
// between the value actual and expected provided that both of the operands must
// have a overridden implementation for the non-equality check operator i.e.,
// (!=).
//
// In case the expressions are equal this macro prints a failure message on the
// stderr file stream and calls the operations defined in IMPL__TEST_FAILED
// macro to decide whether to abort the current process or just simply increment
// xtest::g_n_test_failures.
#define IMPL__TEST_NE(actual, expected, fatal)    \
  do {                                            \
    IMPL__TEST_ASSERTION_SETUP();                 \
    if ((actual) == (expected)) {                 \
      IMPL__TEST_FAILURE_TRACE(actual, expected); \
      IMPL__TEST_EQUALITY_FAILURE_MSG();          \
      IMPL__TEST_FAILED(fatal);                   \
    } else {                                      \
      IMPL__TEST_EQUALITY_SUCCESS_MSG();          \
    }                                             \
  } while (false)

#define EXPECT_NE(actual, expected) IMPL__TEST_NE(actual, expected, false)
#define ASSERT_NE(actual, expected) IMPL__TEST_NE(actual, expected, true)

// Checks if the given value evaluates to true or not.
//
// This implementation detail macro IMPL__TEST_TRUE checks if the given value
// i.e., actual evaluates to true or not.
//
// Note: The value given can be anything that evaluates to true when used inside
// of a if clause.
//
// In case the value given does not evaluates to true, this macro calls the
// operations defined in the IMPL__TEST_FAILED macro to decide whether to abort
// the current process or just simply increment xtest::g_n_test_failures.
#define IMPL__TEST_TRUE(actual, fatal)        \
  do {                                        \
    IMPL__TEST_ASSERTION_SETUP();             \
    if (!(actual)) {                          \
      IMPL__TEST_FAILURE_TRACE(actual, true); \
      IMPL__TEST_EQUALITY_FAILURE_MSG();      \
      IMPL__TEST_FAILED(fatal);               \
    } else {                                  \
      IMPL__TEST_EQUALITY_SUCCESS_MSG();      \
    }                                         \
  } while (false)

#define EXPECT_TRUE(actual) IMPL__TEST_TRUE(actual, false)
#define ASSERT_TRUE(actual) IMPL__TEST_TRUE(actual, true)

// Checks if the given value evaluates to false or not.
//
// This implementation detail macro IMPL__TEST_FALSE checks if the given value
// i.e., actual evaluates to false or not.
//
// Note: The value given can be anything that evaluates to false when used
// inside of a if clause.
//
// In case the value given does not evaluates to false, this macro calls the
// operations defined in the IMPL__TEST_FAILED macro to decide whether to abort
// the current process or just simply increment xtest::g_n_test_failures.
#define IMPL__TEST_FALSE(actual, fatal)        \
  do {                                         \
    IMPL__TEST_ASSERTION_SETUP();              \
    if (actual) {                              \
      IMPL__TEST_FAILURE_TRACE(actual, false); \
      IMPL__TEST_EQUALITY_FAILURE_MSG();       \
      IMPL__TEST_FAILED(fatal);                \
    } else {                                   \
      IMPL__TEST_EQUALITY_SUCCESS_MSG();       \
    }                                          \
  } while (false)

#define EXPECT_FALSE(actual) IMPL__TEST_FALSE(actual, false)
#define ASSERT_FALSE(actual) IMPL__TEST_FALSE(actual, true)

#endif  // XTEST_INCLUDE_XTEST_ASSERTIONS_HH_
