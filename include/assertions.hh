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

#include <cstdio>
#include <cstdlib>

#include <cinttypes>

/**
 * @brief Aborts the current process if fatal is true.
 *
 * This implementation detail macro IMPL__TEST_FAILED aborts the current
 * process if the fatal is true, otherwise increments the global variable
 * xtest::g_n_test_failures by 1 which keeps a counter of the number of
 * non-fatal tests that failed so far.
 * 
 * fatal can be anything that evaluates to true when used in a if clause
 * so make sure that you provide a valid boolean value to this macro.
 */
#define IMPL__TEST_FAILED(fatal)  \
  do {                            \
    if (fatal)                    \
      abort();                    \
    else                          \
      ++xtest::g_n_test_failures; \
  } while (0)

/**
 * @brief Does a equality check between the actual and the expected value.
 * 
 * This implementation detail macro IMPL__TEST_EQ does a equality check between
 * the value actual and expected provided that both of the operands must have a
 * overridden implementation for the equality check operator i.e., (==).
 * 
 * In case the expressions are not equal this macro prints a failure message on
 * the stderr file stream and calls the operations defined in IMPL__TEST_FAILED
 * macro to decide whether to abort the current process or just simply increment
 * xtest::g_n_test_failures.
 */
#define IMPL__TEST_EQ(actual, expected, format, fatal)                         \
  do {                                                                         \
    if ((actual) != (expected)) {                                              \
      fprintf(stderr,                                                          \
              "%s:%lu: FAIL: expected %s == %s; but is actually " format "\n", \
              __FILE__, __LINE__, #actual, #expected, actual);                 \
      IMPL__TEST_FAILED(fatal);                                                \
    }                                                                          \
  } while (0)

#define EXPECT_EQ_STR(actual, expected) IMPL__TEST_EQ(actual, expected, "%s", false)
#define ASSERT_EQ_STR(actual, expected) IMPL__TEST_EQ(actual, expected, "%s", true)

#define EXPECT_EQ_INT32(actual, expected) IMPL__TEST_EQ(actual, expected, "%d", false)
#define EXPECT_EQ_INT64(actual, expected) IMPL__TEST_EQ(actual, expected, "%" PRId64, false)
#define ASSERT_EQ_INT32(actual, expected) IMPL__TEST_EQ(actual, expected, "%d", true)
#define ASSERT_EQ_INT64(actual, expected) IMPL__TEST_EQ(actual, expected, "%" PRId64, true)

#define EXPECT_EQ_UINT32(actual, expected) IMPL__TEST_EQ(actual, expected, "%u", false)
#define EXPECT_EQ_UINT64(actual, expected) IMPL__TEST_EQ(actual, expected, "%" PRIu64, false)
#define ASSERT_EQ_UINT32(actual, expected) IMPL__TEST_EQ(actual, expected, "%u", true)
#define ASSERT_EQ_UINT64(actual, expected) IMPL__TEST_EQ(actual, expected, "%" PRIu64, true)

#define EXPECT_EQ_XINT32(actual, expected) IMPL__TEST_EQ(actual, expected, "0x%08x", false)
#define EXPECT_EQ_XINT64(actual, expected) IMPL__TEST_EQ(actual, expected, "0x%016" PRIx64, false)
#define ASSERT_EQ_XINT32(actual, expected) IMPL__TEST_EQ(actual, expected, "0x%08x", true)
#define ASSERT_EQ_XINT64(actual, expected) IMPL__TEST_EQ(actual, expected, "0x%016" PRIx64, true)

/**
 * @brief Does a non-equality check between the actual and the expected value.
 * 
 * This implementation detail macro IMPL__TEST_NE does a non-equality check
 * between the value actual and expected provided that both of the operands must
 * have a overridden implementation for the non-equality check operator i.e., (!=).
 * 
 * In case the expressions are equal this macro prints a failure message on the
 * stderr file stream and calls the operations defined in IMPL__TEST_FAILED macro
 * to decide whether to abort the current process or just simply increment
 * xtest::g_n_test_failures.
 */
#define IMPL__TEST_NE(actual, expected, format, fatal)                         \
  do {                                                                         \
    if ((actual) != (expected)) {                                              \
      fprintf(stderr,                                                          \
              "%s:%lu: FAIL: expected %s != %s; but is actually " format "\n", \
              __FILE__, __LINE__, #actual, #expected, actual);                 \
      IMPL__TEST_FAILED(fatal);                                                \
    }                                                                          \
  } while (0)

#define EXPECT_NE_STR(actual, expected) IMPL__TEST_NE(actual, expected, "%s", false)
#define ASSERT_NE_STR(actual, expected) IMPL__TEST_NE(actual, expected, "%s", true)

#define EXPECT_NE_INT32(actual, expected) IMPL__TEST_NE(actual, expected, "%d", false)
#define EXPECT_NE_INT64(actual, expected) IMPL__TEST_NE(actual, expected, "%" PRId64, false)
#define ASSERT_NE_INT32(actual, expected) IMPL__TEST_NE(actual, expected, "%d", true)
#define ASSERT_NE_INT64(actual, expected) IMPL__TEST_NE(actual, expected, "%" PRId64, true)

#define EXPECT_NE_UINT32(actual, expected) IMPL__TEST_NE(actual, expected, "%u", false)
#define EXPECT_NE_UINT64(actual, expected) IMPL__TEST_NE(actual, expected, "%" PRIu64, false)
#define ASSERT_NE_UINT32(actual, expected) IMPL__TEST_NE(actual, expected, "%u", true)
#define ASSERT_NE_UINT64(actual, expected) IMPL__TEST_NE(actual, expected, "%" PRIu64, true)

#define EXPECT_NE_XINT32(actual, expected) IMPL__TEST_NE(actual, expected, "0x%08x", false)
#define EXPECT_NE_XINT64(actual, expected) IMPL__TEST_NE(actual, expected, "0x%016" PRIx64, false)
#define ASSERT_NE_XINT32(actual, expected) IMPL__TEST_NE(actual, expected, "0x%08x", true)
#define ASSERT_NE_XINT64(actual, expected) IMPL__TEST_NE(actual, expected, "0x%016" PRIx64, true)

/**
 * @brief Checks if the given value evaluates to true or not.
 * 
 * This implementation detail macro IMPL__TEST_TRUE checks if the given value
 * i.e., actual evaluates to true or not.
 * 
 * Note: The value given can be anything that evaluates to true when used inside
 * of a if clause.
 * 
 * In case the value given does not evaluates to true, this macro calls the
 * operations defined in the IMPL__TEST_FAILED macro to decide whether to abort
 * the current process or just simply increment xtest::g_n_test_failures.
 */
#define IMPL__TEST_TRUE(actual, fatal)                                         \
  do {                                                                         \
    if (!(actual)) {                                                           \
      fprintf(stderr,                                                          \
              "%s:%lu: FAIL: expected %s to be true; but is actually false\n", \
              __FILE__, __LINE__, #actual);                                    \
      IMPL__TEST_FAILED(fatal);                                                \
    }                                                                          \
  } while (0)

#define EXPECT_TRUE(actual) IMPL__TEST_TRUE(actual, false)
#define ASSERT_TRUE(actual) IMPL__TEST_TRUE(actual, true)

/**
 * @brief Checks if the given value evaluates to false or not.
 * 
 * This implementation detail macro IMPL__TEST_FALSE checks if the given value
 * i.e., actual evaluates to false or not.
 * 
 * Note: The value given can be anything that evaluates to false when used inside
 * of a if clause.
 * 
 * In case the value given does not evaluates to false, this macro calls the
 * operations defined in the IMPL__TEST_FAILED macro to decide whether to abort
 * the current process or just simply increment xtest::g_n_test_failures.
 */
#define IMPL__TEST_FALSE(actual, fatal)                                        \
  do {                                                                         \
    if ((actual)) {                                                            \
      fprintf(stderr,                                                          \
              "%s:%lu: FAIL: expected %s to be false; but is actually true\n", \
              __FILE__, __LINE__, #actual);                                    \
      IMPL__TEST_FAILED(fatal);                                                \
    }                                                                          \
  } while (0)

#define EXPECT_FALSE(actual) IMPL__TEST_FALSE(actual, false)
#define ASSERT_FALSE(actual) IMPL__TEST_FALSE(actual, true)

#endif