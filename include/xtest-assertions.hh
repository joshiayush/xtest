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

#include "internal/xtest-port.hh"
#include "xtest-message.hh"
#include "xtest-registrar.hh"
#include "xtest.hh"

namespace xtest {
namespace internal {
class AssertionContext {
 public:
  AssertionContext(const char* file, uint64_t line,
                   TestRegistrar* const& current_test)
      : file_(file), line_(line), current_test_(current_test) {}
  const char* file() const { return file_; }
  uint64_t line() const { return line_; }
  TestRegistrar* const current_test() const { return current_test_; }

 private:
  const char* file_;
  const uint64_t line_;
  TestRegistrar* const current_test_;
};

void OnTestAssertionStart(const TestRegistrar* const& test);
void OnTestAssertionEnd(const TestRegistrar* const& test,
                        const TimeInMillis& elapsed_time);

template <typename T1, typename T2>
void OnTestAssertionFailure(const char* lhs_expr, const char* rhs_expr,
                            const T1& lhs, const T2& rhs,
                            const AssertionContext& assertion_context,
                            const bool& is_fatal) {
  std::fprintf(stderr,
               "%s(%lu): error: Value of: %s\n  Actual: %s\nExpected: %s\n",
               assertion_context.file(), assertion_context.line(), lhs_expr,
               xtest::internal::StreamableToString(lhs).c_str(),
               xtest::internal::StreamableToString(rhs).c_str());
  std::fflush(stderr);
  assertion_context.current_test()->M_testResult = xtest::TestResult::FAILED;
  ++::xtest::G_n_testFailures;
  if (is_fatal)
    std::abort();
}

class AssertionResult {
 public:
  explicit AssertionResult(const bool& success) : success_(success) {}

  template <typename Streamable>
  AssertionResult operator<<(const Streamable& streamable) {
    if (!success_) {
      std::fprintf(stderr, "%s\n",
                   xtest::internal::StreamableToString(streamable).c_str());
      std::fflush(stderr);
    }
    return *this;
  }

 private:
  bool success_;
};

AssertionResult AssertionSuccess();
AssertionResult AssertionFailure();
}  // namespace internal

class BoolTrueHelper {
 public:
  template <typename T>
  static internal::AssertionResult Check(
      const char* actual_expr, const T& actual,
      internal::AssertionContext&& assertion_context, const bool& is_fatal) {
    internal::Timer timer;
    internal::OnTestAssertionStart(assertion_context.current_test());
    if (!actual)
      internal::OnTestAssertionFailure(actual_expr, "true", actual, true,
                                       assertion_context, is_fatal);
    else
      assertion_context.current_test()->M_testResult =
          ::xtest::TestResult::PASSED;
    internal::OnTestAssertionEnd(assertion_context.current_test(),
                                 timer.Elapsed());
    return actual ? internal::AssertionSuccess() : internal::AssertionFailure();
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
    internal::OnTestAssertionStart(assertion_context.current_test());
    if (actual)
      internal::OnTestAssertionFailure(actual_expr, "false", actual, false,
                                       assertion_context, is_fatal);
    else
      assertion_context.current_test()->M_testResult =
          ::xtest::TestResult::PASSED;
    internal::OnTestAssertionEnd(assertion_context.current_test(),
                                 timer.Elapsed());
    return !actual ? internal::AssertionSuccess()
                   : internal::AssertionFailure();
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
  template <typename T1, typename T2>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, const T1& lhs, const T2& rhs,
      internal::AssertionContext&& assertion_context, const bool& is_fatal) {
    internal::Timer timer;
    bool result = lhs == rhs;
    internal::OnTestAssertionStart(assertion_context.current_test());
    if (result)
      internal::OnTestAssertionFailure(lhs_expr, rhs_expr, lhs, rhs,
                                       assertion_context, is_fatal);
    else
      assertion_context.current_test()->M_testResult =
          ::xtest::TestResult::PASSED;
    internal::OnTestAssertionEnd(assertion_context.current_test(),
                                 timer.Elapsed());
    return result ? internal::AssertionSuccess() : internal::AssertionFailure();
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
  template <typename T1, typename T2>
  static internal::AssertionResult Compare(
      const char* lhs_expr, const char* rhs_expr, const T1& lhs, const T2& rhs,
      internal::AssertionContext&& assertion_context, const bool& is_fatal) {
    internal::Timer timer;
    bool result = lhs == rhs;
    internal::OnTestAssertionStart(assertion_context.current_test());
    if (!result)
      internal::OnTestAssertionFailure(lhs_expr, rhs_expr, lhs, rhs,
                                       assertion_context, is_fatal);
    else
      assertion_context.current_test()->M_testResult =
          ::xtest::TestResult::PASSED;
    internal::OnTestAssertionEnd(assertion_context.current_test(),
                                 timer.Elapsed());
    return result ? internal::AssertionSuccess() : internal::AssertionFailure();
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
