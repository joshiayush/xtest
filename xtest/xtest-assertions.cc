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

#include "xtest-assertions.hh"

#include <cstdint>
#include <cstdio>

#include "internal/xtest-port.hh"
#include "internal/xtest-printers.hh"
#include "xtest-registrar.hh"
#include "xtest.hh"

namespace xtest {
namespace internal {
//  Constructs a AssertionContext.
AssertionContext::AssertionContext(const char* file, uint64_t line,
                                   TestRegistrar* const& current_test)
    : file_(file), line_(line), current_test_(current_test) {}

// Returns file name inside which the {EXPECT|ASSERT} assertion has been used.
const char* AssertionContext::file() const noexcept { return file_; }

// Returns line number where the {EXPECT|ASSERT} assertion is placed.
uint64_t AssertionContext::line() const noexcept { return line_; }

// Returns the test suite instance inside which the {EXPECT|ASSERT} assertion is
// present.
TestRegistrar* const AssertionContext::current_test() const noexcept {
  return current_test_;
}

// Returns a `AssertionResult` instance of success type in case of
// {EXPECT|ASSERT} assertion success.
AssertionResult AssertionSuccess() { return AssertionResult(true); }
// Returns a `AssertionResult` instance of failure type in case of
// {EXPECT|ASSERT} assertion failure.
AssertionResult AssertionFailure(const bool& is_fatal) {
  return AssertionResult(false, is_fatal);
}

// Prints out the information of the test suite and the test name.
void PrettyAssertionResultPrinter::OnTestAssertionStart(
    const TestRegistrar* const& test) {
  ColoredPrintf(XTestColor::kGreen, "[%s] ",
                ::xtest::GetStringAlignedTo(
                    "RUN", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_, ALIGN_LEFT)
                    .c_str());
  std::printf("%s.%s", test->M_suiteName, test->M_testName);
  std::printf("\n");
  std::fflush(stdout);
}

// Prints out the information of the test suite and the test name with the
// assertion result.
void PrettyAssertionResultPrinter::OnTestAssertionEnd(
    const TestRegistrar* const& test, const TimeInMillis& elapsed_time) {
  if (test->M_testResult == ::xtest::TestResult::PASSED)
    ColoredPrintf(
        XTestColor::kGreen, "[%s] ",
        ::xtest::GetStringAlignedTo(
            "OK", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_, ALIGN_RIGHT)
            .c_str());
  else
    ColoredPrintf(
        XTestColor::kRed, "[%s] ",
        ::xtest::GetStringAlignedTo(
            "FALIED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_, ALIGN_CENTER)
            .c_str());
  std::printf("%s.%s (%lu ms)", test->M_suiteName, test->M_testName,
              elapsed_time);
  std::printf("\n");
  std::fflush(stdout);
}
}  // namespace internal
}  // namespace xtest
