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

#include <cstdio>

#include "internal/xtest-port.hh"
#include "xtest-registrar.hh"
#include "xtest.hh"

namespace xtest {
namespace internal {
AssertionResult AssertionSuccess() { return AssertionResult(true); }
AssertionResult AssertionFailure() { return AssertionResult(false); }

void OnTestAssertionStart(const TestRegistrar* const& test) {
  std::printf("[%s] %s.%s",
              ::xtest::GetStringAlignedTo(
                  "RUN", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_, ALIGN_LEFT)
                  .c_str(),
              test->M_suiteName, test->M_testName);
  std::printf("\n");
  std::fflush(stdout);
}

void OnTestAssertionEnd(const TestRegistrar* const& test,
                        const TimeInMillis& elapsed_time) {
  std::printf(
      "[%s] %s.%s (%lu ms)",
      test->M_testResult == ::xtest::TestResult::PASSED
          ? ::xtest::GetStringAlignedTo(
                "OK", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_, ALIGN_RIGHT)
                .c_str()
          : ::xtest::GetStringAlignedTo(
                "FALIED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_, ALIGN_CENTER)
                .c_str(),
      test->M_suiteName, test->M_testName, elapsed_time);
  std::printf("\n");
  std::fflush(stdout);
}
}  // namespace internal
}  // namespace xtest
