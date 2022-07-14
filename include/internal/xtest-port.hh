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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "internal/xtest-port-arch.hh"

#if XTEST_OS_LINUX || XTEST_OS_MAC
#include <unistd.h>
#elif XTEST_OS_WINDOWS
#include <io.h>
#endif

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

namespace posix {
// Posix compatible function to return the file number associated with the given
// stream.
//
// Returns the integer value of the file descriptor associated with `stream`.
// Otherwise, the value -1 shall be returned and `errno` set to indicate the
// error.
inline int32_t FileNo(FILE* stream) {
#if XTEST_OS_WINDOWS
  return _fileno(stream);
#else
  return fileno(stream);
#endif
}

// Posix compatible function to test whether `fd` is an open file descriptor
// referring to a terminal.
//
// Returns 1 if fd is an open file descriptor referring to a terminal; otherwise
// 0 is returned, and `errno` is set to indicate the error.
inline int32_t IsAtty(int32_t fd) {
#if XTEST_OS_WINDOWS
  return _isatty(fd);
#else
  return isatty(fd);
#endif
}

inline int32_t StrCaseCmp(const char* lhs, const char* rhs) {
  return strcasecmp(lhs, rhs);
}

inline const char* GetEnv(const char* name) { return std::getenv(name); }
}  // namespace posix
}  // namespace xtest

#define XTEST_NAME_ "Xtest"
#define XTEST_FLAG_PREFIX_ "xtest_"

#define XTEST_FLAG_(flagName) flag_xtest_##flagName
#define XTEST_FLAG_GET_(flagName) ::xtest::XTEST_FLAG_(flagName)
#define XTEST_FLAG_SET_(flagName, value) (void)(XTEST_FLAG_(flagName) = (value))

#define XTEST_FLAG_DECLARE_bool_(flagName) \
  namespace xtest {                        \
  extern bool XTEST_FLAG_(flagName);       \
  }                                        \
  static_assert(true, "no-op to require trailing semicolon")
#define XTEST_FLAG_DECLARE_int32_(flagName) \
  namespace xtest {                         \
  extern int32_t XTEST_FLAG_(flagName);     \
  }                                         \
  static_assert(true, "no-op to require trailing semicolon")
#define XTEST_FLAG_DECLARE_uint32_(flagName) \
  namespace xtest {                          \
  extern uint32_t XTEST_FLAG_(flagName);     \
  }                                          \
  static_assert(true, "no-op to require trailing semicolon")
#define XTEST_FLAG_DECLARE_string_(flagName) \
  namespace xtest {                          \
  extern std::string XTEST_FLAG_(flagName);  \
  }                                          \
  static_assert(true, "no-op to require trailing semicolon")

#define XTEST_FLAG_DEFINE_bool_(flagName, value, doc) \
  namespace xtest {                                   \
  bool XTEST_FLAG_(flagName) = (value);               \
  }                                                   \
  static_assert(true, "no-op to require trailing semicolon")
#define XTEST_FLAG_DEFINE_int32_(flagName, value, doc) \
  namespace xtest {                                    \
  int32_t XTEST_FLAG_(flagName) = (value);             \
  }                                                    \
  static_assert(true, "no-op to require trailing semicolon")
#define XTEST_FLAG_DEFINE_uint32_(flagName, value, doc) \
  namespace xtest {                                     \
  uint32_t XTEST_FLAG_(flagName) = (value);             \
  }                                                     \
  static_assert(true, "no-op to require trailing semicolon")
#define XTEST_FLAG_DEFINE_string_(flagName, value, doc) \
  namespace xtest {                                     \
  std::string XTEST_FLAG_(flagName) = (value);          \
  }                                                     \
  static_assert(true, "no-op to require trailing semicolon")

#define XTEST_GLOBAL_INSTANCE_(name) xtest_global_##name
#define XTEST_GLOBAL_INSTANCE_GET_(name) ::xtest::XTEST_GLOBAL_INSTANCE_(name)
#define XTEST_GLOBAL_INSTANCE_SET_(name, value) \
  (void)(XTEST_GLOBAL_INSTANCE_(name) = (value))

#define XTEST_GLOBAL_DECLARE_uint64_(name)      \
  namespace xtest {                             \
  extern uint64_t XTEST_GLOBAL_INSTANCE_(name); \
  }                                             \
  static_assert(true, "no-op to require trailing semicolon")

#define XTEST_GLOBAL_DEFINE_uint64_(name, value, doc) \
  namespace xtest {                                   \
  uint64_t XTEST_GLOBAL_INSTANCE_(name) = (value);    \
  }                                                   \
  static_assert(true, "no-op to require trailing semicolon")

#define XTEST_STATIC_DECLARE_vector_(name, T, doc)    \
  namespace xtest {                                   \
  static std::vector<T> XTEST_GLOBAL_INSTANCE_(name); \
  }

// Global counter for non-fatal test failures.
//
// This global counter is defined in the object file xtest.cc and incremented
// every time a non-fatal test assertion fails.
XTEST_GLOBAL_DECLARE_uint64_(failure_count);

XTEST_GLOBAL_DECLARE_uint64_(test_count);
XTEST_GLOBAL_DECLARE_uint64_(test_suite_count);
XTEST_GLOBAL_DECLARE_uint64_(failed_test_count);

// When this flag is specified, the xtest's help message is printed on the
// console.
XTEST_FLAG_DECLARE_bool_(help);

// When this flag is specified, tests' order is randomized on every iteration.
XTEST_FLAG_DECLARE_bool_(shuffle);

// Prints the list of all tests with there suite names.
XTEST_FLAG_DECLARE_bool_(list_tests);

// This flag enables using colors in terminal output. Available values are "yes"
// to enable colors, "no" (disable colors), or "auto" (the default) to let XTest
// decide.
XTEST_FLAG_DECLARE_string_(color);

#endif  // XTEST_INCLUDE_INTERNAL_XTEST_PORT_HH_
