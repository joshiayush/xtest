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

#ifndef XTEST_INCLUDE_XTEST_ASSERTIONS_HH_
#define XTEST_INCLUDE_XTEST_ASSERTIONS_HH_

#include <cstdlib>
#include <iostream>

#include "internal/xtest-port.hh"
#include "xtest-message.hh"
#include "xtest-registrar.hh"
#include "xtest.hh"

namespace xtest {
namespace internal {
// Internal implementation of an {EXPECT|ASSERT} assertion context.
//
// Includes:
//  * file_         - File where the {EXPECT|ASSERT} assertion has been used.
//  * line_         - Line number where the {EXPECT|ASSERT} assertion has been
//                    used.
//  * current_test_ - Test suite inside which the {EXPECT|ASSERT} assertion is
//                    present.
class AssertionContext {
 public:
  //  Constructs a AssertionContext.
  AssertionContext(const char* file, uint64_t line,
                   TestRegistrar* const& current_test);

  // Returns file name inside which the {EXPECT|ASSERT} assertion has been used.
  const char* file() const noexcept;

  // Returns line number where the {EXPECT|ASSERT} assertion is placed.
  uint64_t line() const noexcept;

  // Returns the test suite instance inside which the {EXPECT|ASSERT} assertion
  // is present.
  TestRegistrar* const current_test() const noexcept;

 private:
  const char* file_;
  const uint64_t line_;
  TestRegistrar* const current_test_;
};

// Utility class to pretty print {EXPECT|ASSERT} assertion results.
class PrettyAssertionResultPrinter {
 public:
  // No instance should instantiate from this class.
  PrettyAssertionResultPrinter() = delete;

  // Prints out the information of the test suite and the test name.
  static void OnTestAssertionStart(const TestRegistrar* const& test);

  // Prints out the information of the test suite and the test name with the
  // assertion result.
  static void OnTestAssertionEnd(const TestRegistrar* const& test,
                                 const TimeInMillis& elapsed_time);

  // Prints out a trace on {EXPECT|ASSERT} assertion failure with the file and
  // the line number.
  template <typename T1, typename T2>
  static void OnTestAssertionFailure(
      const char* lhs_expr, const char* rhs_expr, const T1& lhs, const T2& rhs,
      const AssertionContext& assertion_context) {
    std::fprintf(stderr,
                 "%s(%lu): error: Value of: %s\n  Actual: %s\nExpected: %s\n",
                 assertion_context.file(), assertion_context.line(), lhs_expr,
                 xtest::internal::StreamableToString(lhs).c_str(),
                 xtest::internal::StreamableToString(rhs).c_str());
    std::fflush(stderr);

    // Mark the current test as `xtest::TestResult::FAILED` on failure.
    assertion_context.current_test()->M_testResult = xtest::TestResult::FAILED;
    // Add this test in the global test failure counter.
    ++::xtest::G_n_testFailures;
  }
};

// Representation of an {EXPECT|ASSERT} assertion result.
class AssertionResult {
 public:
  // Creates an {EXPECT|ASSERT} assertion result instance from the given
  // `success` value.
  explicit AssertionResult(const bool& success)
      : success_(success), fatal_(false) {}

  AssertionResult(const bool& success, const bool& fatal)
      : success_(success), fatal_(fatal) {}

  // Streams data to the `stderr` stream when this instance represents a
  // {EXPECT|ASSERT} assertion failure.  There will be not output in case of
  // {EXPECT|ASSERT} assertion success.
  template <typename Streamable>
  AssertionResult operator<<(const Streamable& streamable) {
    if (!success_) {
      std::fprintf(stderr, "%s\n",
                   xtest::internal::StreamableToString(streamable).c_str());
      std::fflush(stderr);
    }
    return *this;
  }

  ~AssertionResult() {
    // We raise an abort signal in case `is_fatal` is true; this is to catch the
    // abort signal later and mark the result of the test suite as
    // `xtest::TestResult::FAILED`.
    if (fatal_)
      std::abort();
  }

 private:
  bool fatal_;
  bool success_;
};

// Returns a `AssertionResult` instance of success type in case of
// {EXPECT|ASSERT} assertion success.
AssertionResult AssertionSuccess();
// Returns a `AssertionResult` instance of failure type in case of
// {EXPECT|ASSERT} assertion failure.
AssertionResult AssertionFailure(const bool& is_fatal);
}  // namespace internal

class BoolTrueHelper {
 public:
  template <typename T>
  static internal::AssertionResult Check(
      const char* actual_expr, const T& actual,
      internal::AssertionContext&& assertion_context, const bool& is_fatal) {
    internal::Timer timer;
    internal::PrettyAssertionResultPrinter::OnTestAssertionStart(
        assertion_context.current_test());
    if (!actual)
      internal::PrettyAssertionResultPrinter::OnTestAssertionFailure(
          actual_expr, "true", actual, true, assertion_context);
    else
      assertion_context.current_test()->M_testResult =
          ::xtest::TestResult::PASSED;
    internal::PrettyAssertionResultPrinter::OnTestAssertionEnd(
        assertion_context.current_test(), timer.Elapsed());
    return actual ? internal::AssertionSuccess()
                  : internal::AssertionFailure(is_fatal);
  }
};

#define XTEST_ASSERT_TRUE_(actual, fatal)                                   \
  ::xtest::BoolTrueHelper::Check(                                           \
      #actual, actual,                                                      \
      ::xtest::internal::AssertionContext(__FILE__, __LINE__, currentTest), \
      fatal)

#define EXPECT_TRUE(actual) XTEST_ASSERT_TRUE_(actual, false)
#define ASSERT_TRUE(actual) XTEST_ASSERT_TRUE_(actual, true)

class BoolFalseHelper {
 public:
  template <typename T>
  static internal::AssertionResult Check(
      const char* actual_expr, const T& actual,
      internal::AssertionContext&& assertion_context, const bool& is_fatal) {
    internal::Timer timer;
    internal::PrettyAssertionResultPrinter::OnTestAssertionStart(
        assertion_context.current_test());
    if (actual)
      internal::PrettyAssertionResultPrinter::OnTestAssertionFailure(
          actual_expr, "false", actual, false, assertion_context);
    else
      assertion_context.current_test()->M_testResult =
          ::xtest::TestResult::PASSED;
    internal::PrettyAssertionResultPrinter::OnTestAssertionEnd(
        assertion_context.current_test(), timer.Elapsed());
    return !actual ? internal::AssertionSuccess()
                   : internal::AssertionFailure(is_fatal);
  }
};

#define XTEST_ASSERT_FALSE_(actual, fatal)                                  \
  ::xtest::BoolFalseHelper::Check(                                          \
      #actual, actual,                                                      \
      ::xtest::internal::AssertionContext(__FILE__, __LINE__, currentTest), \
      fatal)

#define EXPECT_FALSE(actual) XTEST_ASSERT_FALSE_(actual, false)
#define ASSERT_FALSE(actual) XTEST_ASSERT_FALSE_(actual, true)

class NqHelper {
 public:
  // This function is an overload for r-value `AssertionContext` instance
  // reference.
  template <
      typename T1, typename T2,
      // Disable this overload for cases where one argument is a pointer
      // and the other is the null pointer constant.
      typename std::enable_if<!std::is_integral<T1>::value ||
                              !std::is_pointer<T2>::value>::type* = nullptr>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, const T1& lhs, const T2& rhs,
      internal::AssertionContext&& assertion_context, const bool& is_fatal) {
    return CmpHelperNE(lhs_expr, rhs_expr, lhs, rhs, assertion_context,
                       is_fatal);
  }

  // This function is an overload for l-value `AssertionContext` instance
  // reference.
  template <typename T1, typename T2>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, const T1& lhs, const T2& rhs,
      internal::AssertionContext& assertion_context, const bool& is_fatal) {
    return CmpHelperNE(lhs_expr, rhs_expr, lhs, rhs, assertion_context,
                       is_fatal);
  }

  // With this overloaded version, we allow anonymous enums to be used in
  // {ASSERT|EXPECT}_EQ when compiled with gcc 4, as anonymous enums can be
  // implicitly cast to BiggestInt.
  //
  // Even though its body looks the same as the above version, we cannot merge
  // the two, as it will make anonymous enums unhappy.
  //
  // This function is an overload for r-value `AssertionContext` instance
  // reference.
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, internal::BiggestInt lhs,
      internal::BiggestInt rhs, internal::AssertionContext&& assertion_context,
      const bool& is_fatal) {
    return CmpHelperNE(lhs_expr, rhs_expr, lhs, rhs, assertion_context,
                       is_fatal);
  }

  // With this overloaded version, we allow anonymous enums to be used in
  // {ASSERT|EXPECT}_EQ when compiled with gcc 4, as anonymous enums can be
  // implicitly cast to BiggestInt.
  //
  // Even though its body looks the same as the above version, we cannot merge
  // the two, as it will make anonymous enums unhappy.
  //
  // This function is an overload for l-value `AssertionContext` instance
  // reference.
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, internal::BiggestInt lhs,
      internal::BiggestInt rhs, internal::AssertionContext& assertion_context,
      const bool& is_fatal) {
    return CmpHelperNE(lhs_expr, rhs_expr, lhs, rhs, assertion_context,
                       is_fatal);
  }

  // With this overload version, we allow `0` to be used as a null pointer
  // literal.
  //
  // This function is an overload for r-value `AssertionContext` instance
  // reference.
  template <typename T>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, std::nullptr_t /* lhs */,
      T* rhs, internal::AssertionContext&& assertion_context,
      const bool& if_fatal) {
    return CmpHelperNE(lhs_expr, rhs_expr, static_cast<T*>(nullptr), rhs);
  }

  // With this overload version, we allow `0` to be used as a null pointer
  // literal.
  //
  // This function is an overload for l-value `AssertionContext` instance
  // reference.
  template <typename T>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, std::nullptr_t /* lhs */,
      T* rhs, internal::AssertionContext& assertion_context,
      const bool& if_fatal) {
    return CmpHelperNE(lhs_expr, rhs_expr, static_cast<T*>(nullptr), rhs);
  }

 private:
  template <typename T1, typename T2>
  static internal::AssertionResult CmpHelperNE(
      const char* lhs_expr, const char* rhs_expr, const T1& lhs, const T2& rhs,
      internal::AssertionContext& assertion_context, const bool& is_fatal) {
    internal::Timer timer;
    bool result = lhs == rhs;
    internal::PrettyAssertionResultPrinter::OnTestAssertionStart(
        assertion_context.current_test());
    if (result)
      internal::PrettyAssertionResultPrinter::OnTestAssertionFailure(
          lhs_expr, rhs_expr, lhs, rhs, assertion_context);
    else
      assertion_context.current_test()->M_testResult =
          ::xtest::TestResult::PASSED;
    internal::PrettyAssertionResultPrinter::OnTestAssertionEnd(
        assertion_context.current_test(), timer.Elapsed());
    return result ? internal::AssertionSuccess()
                  : internal::AssertionFailure(is_fatal);
  }
};

#define XTEST_ASSERT_NE_(actual, expected, fatal)                           \
  ::xtest::NqHelper::Compare(                                               \
      #actual, #expected, actual, expected,                                 \
      ::xtest::internal::AssertionContext(__FILE__, __LINE__, currentTest), \
      fatal)

#define EXPECT_NE(actual, expected) XTEST_ASSERT_NE_(actual, expected, false)
#define ASSERT_NE(actual, expected) XTEST_ASSERT_NE_(actual, expected, true)

class EqHelper {
 public:
  // This function is an overload for r-value `AssertionContext` instance
  // reference.
  template <
      typename T1, typename T2,
      // Disable this overload for cases where one argument is a pointer
      // and the other is the null pointer constant.
      typename std::enable_if<!std::is_integral<T1>::value ||
                              !std::is_pointer<T2>::value>::type* = nullptr>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, const T1& lhs, const T2& rhs,
      internal::AssertionContext&& assertion_context, const bool& is_fatal) {
    return CmpHelperEQ(lhs_expr, rhs_expr, lhs, rhs, assertion_context,
                       is_fatal);
  }

  // This function is an overload for l-value `AssertionContext` instance
  // reference.
  template <typename T1, typename T2>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, const T1& lhs, const T2& rhs,
      internal::AssertionContext& assertion_context, const bool& is_fatal) {
    return CmpHelperEQ(lhs_expr, rhs_expr, lhs, rhs, assertion_context,
                       is_fatal);
  }

  // With this overloaded version, we allow anonymous enums to be used in
  // {ASSERT|EXPECT}_EQ when compiled with gcc 4, as anonymous enums can be
  // implicitly cast to BiggestInt.
  //
  // Even though its body looks the same as the above version, we cannot merge
  // the two, as it will make anonymous enums unhappy.
  //
  // This function is an overload for r-value `AssertionContext` instance
  // reference.
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, internal::BiggestInt lhs,
      internal::BiggestInt rhs, internal::AssertionContext&& assertion_context,
      const bool& is_fatal) {
    return CmpHelperEQ(lhs_expr, rhs_expr, lhs, rhs, assertion_context,
                       is_fatal);
  }

  // With this overloaded version, we allow anonymous enums to be used in
  // {ASSERT|EXPECT}_EQ when compiled with gcc 4, as anonymous enums can be
  // implicitly cast to BiggestInt.
  //
  // Even though its body looks the same as the above version, we cannot merge
  // the two, as it will make anonymous enums unhappy.
  //
  // This function is an overload for l-value `AssertionContext` instance
  // reference.
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, internal::BiggestInt lhs,
      internal::BiggestInt rhs, internal::AssertionContext& assertion_context,
      const bool& is_fatal) {
    return CmpHelperEQ(lhs_expr, rhs_expr, lhs, rhs, assertion_context,
                       is_fatal);
  }

  // With this overload version, we allow `0` to be used as a null pointer
  // literal.
  //
  // This function is an overload for r-value `AssertionContext` instance
  // reference.
  template <typename T>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, std::nullptr_t /* lhs */,
      T* rhs, internal::AssertionContext&& assertion_context,
      const bool& if_fatal) {
    return CmpHelperEQ(lhs_expr, rhs_expr, static_cast<T*>(nullptr), rhs);
  }

  // With this overload version, we allow `0` to be used as a null pointer
  // literal.
  //
  // This function is an overload for l-value `AssertionContext` instance
  // reference.
  template <typename T>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, std::nullptr_t /* lhs */,
      T* rhs, internal::AssertionContext& assertion_context,
      const bool& if_fatal) {
    return CmpHelperEQ(lhs_expr, rhs_expr, static_cast<T*>(nullptr), rhs);
  }

 private:
  template <typename T1, typename T2>
  static internal::AssertionResult CmpHelperEQ(
      const char* lhs_expr, const char* rhs_expr, const T1& lhs, const T2& rhs,
      internal::AssertionContext& assertion_context, const bool& is_fatal) {
    internal::Timer timer;
    bool result = lhs == rhs;
    internal::PrettyAssertionResultPrinter::OnTestAssertionStart(
        assertion_context.current_test());
    if (!result)
      internal::PrettyAssertionResultPrinter::OnTestAssertionFailure(
          lhs_expr, rhs_expr, lhs, rhs, assertion_context);
    else
      assertion_context.current_test()->M_testResult =
          ::xtest::TestResult::PASSED;
    internal::PrettyAssertionResultPrinter::OnTestAssertionEnd(
        assertion_context.current_test(), timer.Elapsed());
    return result ? internal::AssertionSuccess()
                  : internal::AssertionFailure(is_fatal);
  }
};

#define XTEST_ASSERT_EQ_(actual, expected, fatal)                           \
  ::xtest::EqHelper::Compare(                                               \
      #actual, #expected, actual, expected,                                 \
      ::xtest::internal::AssertionContext(__FILE__, __LINE__, currentTest), \
      fatal)

#define EXPECT_EQ(actual, expected) XTEST_ASSERT_EQ_(actual, expected, false)
#define ASSERT_EQ(actual, expected) XTEST_ASSERT_EQ_(actual, expected, true)
}  // namespace xtest

#endif  // XTEST_INCLUDE_XTEST_ASSERTIONS_HH_
