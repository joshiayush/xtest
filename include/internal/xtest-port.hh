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

#ifndef XTEST_INCLUDE_INTERNAL_XTEST_PORT_HH_
#define XTEST_INCLUDE_INTERNAL_XTEST_PORT_HH_

#include <chrono>  // NOLINT
#include <cstdint>
#include <iostream>

namespace xtest {
namespace internal {
// Disallow copy constructor and operator=
// This should be used in the private: declarations for a class.
#define XTEST_DISALLOW_COPY_AND_ASSIGN_(type) \
  type(type const&) = delete;                 \
  type& operator=(type const&) = delete

using TimeInMillis = int64_t;

// The biggest signed integer type the compiler supports.
using BiggestInt = int64_t;

// Severity types for custom logging class.
//
// This enum specifies severity types for custom logging class XTestLog to use
// for logging messages.
enum XTestLogSeverity { XTEST_INFO, XTEST_WARNING, XTEST_ERROR, XTEST_FATAL };

// Custom class to provide logging.
//
// Formats log entry severity, provides a stream object for streaming the
// log message, and terminates the message with a newline when going out of
// scope.
class XTestLog {
 public:
  XTestLog(const XTestLogSeverity& severity, const char* file,
           const uint64_t& line);

  // Flushes the buffers and, if severity is XTEST_FATAL, aborts the program.
  ~XTestLog();

  std::ostream& GetStream() { return std::cerr; }

 private:
  const XTestLogSeverity _M_severity;

  XTEST_DISALLOW_COPY_AND_ASSIGN_(XTestLog);
};

#define XTEST_LOG_(severity)                                             \
  xtest::internal::XTestLog(xtest::internal::XTEST_##severity, __FILE__, \
                            __LINE__)                                    \
      .GetStream()

// A helper class for measuring elapsed times.
class Timer {
 public:
  Timer();

  // Return time elapsed in milliseconds since the timer was created.
  TimeInMillis Elapsed();

 private:
  std::chrono::steady_clock::time_point start_;
};
}  // namespace internal

// clang-format off
#define XTEST_NAME_ "Xtest"
#define XTEST_FLAG_PREFIX_ "xtest_"

#define XTEST_FLAG(flagName)            ::xtest::FLAG_xtest_##flagName
#define XTEST_FLAG_GET(flagName)        XTEST_FLAG(flagName)
#define XTEST_FLAG_SET(flagName, value) (void)(XTEST_FLAG(flagName) = (value))

#define XTEST_GLOBAL_INSTANCE_PREFIX_           xtest_global_
#define XTEST_GLOBAL_INSTANCE_(name)            XTEST_GLOBAL_INSTANCE_PREFIX_##name
#define XTEST_GLOBAL_INSTANCE_GET_(name)        ::xtest::XTEST_GLOBAL_INSTANCE_(name)
#define XTEST_GLOBAL_INSTANCE_SET_(name, value) (void)(::xtest::XTEST_GLOBAL_INSTANCE_(name) = (value))
// clang-format on

// Global counter for non-fatal test failures.
//
// This global counter is defined in the object file xtest.cc and incremented
// every time a non-fatal test assertion fails.
extern uint64_t G_n_testFailures;

// New string width for the aligned string returned by the function
// `GetStringAlignedTo()`.
#define XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_ 10

// Returns a string of length `width` all filled with the character `chr`.
//
// This function is mainly used to decorate the box used in the test summary
// like show below where we are filling the boxes with either `-` or `=`
// character,
//
// ```shell
// [‑‑‑‑‑‑‑‑‑‑] Global test environment tear‑down
// [==========] 2 tests from 1 test case ran. (10 ms total)
// [  PASSED  ] 1 test.
// [  FAILED  ] 1 test, listed below:
// [  FAILED  ] SquareRootTest.PositiveNos
//
// 1 FAILED TEST
// ```
std::string GetStrFilledWith(
    const char& chr,
    std::size_t width = XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_);

enum StringAlignValues { ALIGN_RIGHT, ALIGN_LEFT, ALIGN_CENTER };

// Align the original string to the given alignment inside of the new sized
// buffer.
//
// This function is mainly used to print out nice visual representation of the
// test results like the following,
//
// ```shell
// [  FAILED  ] SquareRootTest.PositiveNos
// ```
//
// In order to align the letter `FAILED` to the center we use this function with
// a 'alignSide' of 'ALIGN_CENTER'.
//
// If not given this function will use the default values for `newStrWidth` and
// `alignSide` i.e., `XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_` and
// `ALIGN_CENTER` respectively.
std::string GetStringAlignedTo(
    const std::string& str,
    const std::size_t& newStrWidth = XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
    const StringAlignValues& alignSide = ALIGN_CENTER);
}  // namespace xtest

#endif  // XTEST_INCLUDE_INTERNAL_XTEST_PORT_HH_
