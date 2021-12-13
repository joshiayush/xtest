// Copyright (C) 2021  Ayush Joshi
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program. If not, see <https://www.gnu.org/licenses/>.

#ifndef _ASSERTIONS_HH_
#define _ASSERTIONS_HH_

#include <cstdlib>
#include <iostream>

// @brief Aborts the current process if fatal is true.
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
#define IMPL__TEST_FAILED(fatal)                          \
  do {                                                    \
    current_test->m_result = ::xtest::TestResult::FAILED; \
    ++::xtest::G_n_testFailures;                          \
    if (fatal)                                            \
      abort();                                            \
  } while (false)

// @brief Does a equality check between the actual and the expected value.
//
// This implementation detail macro IMPL__TEST_EQ does a equality check between
// the value actual and expected provided that both of the operands must have a
// overridden implementation for the equality check operator i.e., (==).
//
// In case the expressions are not equal this macro prints a failure message on
// the stderr file stream and calls the operations defined in IMPL__TEST_FAILED
// macro to decide whether to abort the current process or just simply increment
// xtest::g_n_test_failures.
#define IMPL__TEST_EQ(actual, expected, format, fatal)                    \
  do {                                                                    \
    if ((actual) != (expected)) {                                         \
      std::cerr << __FILE__ << ':' << __LINE__ << ": FAIL: expected "     \
                << #actual << " == " << #expected << "; but is actually " \
                << actual << std::endl;                                   \
      IMPL__TEST_FAILED(fatal);                                           \
    }                                                                     \
  } while (false)

#define EXPECT_EQ(actual, expected) IMPL__TEST_EQ(actual, expected, false)
#define ASSERT_EQ(actual, expected) IMPL__TEST_EQ(actual, expected, true)

// @brief Does a non-equality check between the actual and the expected value.
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
#define IMPL__TEST_EQ(actual, expected, format, fatal)                    \
  do {                                                                    \
    if ((actual) != (expected)) {                                         \
      std::cerr << __FILE__ << ':' << __LINE__ << ": FAIL: expected "     \
                << #actual << " != " << #expected << "; but is actually " \
                << actual << std::endl;                                   \
      IMPL__TEST_FAILED(fatal);                                           \
    }                                                                     \
  } while (false)

#define EXPECT_NE(actual, expected) IMPL__TEST_NE(actual, expected, false)
#define ASSERT_NE(actual, expected) IMPL__TEST_NE(actual, expected, true)

// @brief Checks if the given value evaluates to true or not.
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
#define IMPL__TEST_TRUE(actual, fatal)                                \
  do {                                                                \
    if (!(actual)) {                                                  \
      std::cerr << __FILE__ << ':' << __LINE__ << ": FAIL: expected " \
                << #actual << " to be true; but is actually false"    \
                << std::endl;                                         \
      IMPL__TEST_FAILED(fatal);                                       \
    }                                                                 \
  } while (false)

#define EXPECT_TRUE(actual) IMPL__TEST_TRUE(actual, false)
#define ASSERT_TRUE(actual) IMPL__TEST_TRUE(actual, true)

// @brief Checks if the given value evaluates to false or not.
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
#define IMPL__TEST_TRUE(actual, fatal)                                \
  do {                                                                \
    if (!(actual)) {                                                  \
      std::cerr << __FILE__ << ':' << __LINE__ << ": FAIL: expected " \
                << #actual << " to be false; but is actually true"    \
                << std::endl;                                         \
      IMPL__TEST_FAILED(fatal);                                       \
    }                                                                 \
  } while (false)

#define EXPECT_FALSE(actual) IMPL__TEST_FALSE(actual, false)
#define ASSERT_FALSE(actual) IMPL__TEST_FALSE(actual, true)

#endif