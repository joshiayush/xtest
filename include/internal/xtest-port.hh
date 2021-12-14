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

#include <iostream>

// Disallow copy constructor and operator=
// This should be used in the private: declarations for a class.
#define XTEST_DISALLOW_COPY_AND_ASSIGN_(type) \
  type(type const&) = delete;                 \
  type& operator=(type const&) = delete

namespace xtest {
namespace internal {
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

  ::std::ostream& GetStream() { return ::std::cerr; }

 private:
  const XTestLogSeverity _M_severity;

  XTEST_DISALLOW_COPY_AND_ASSIGN_(XTestLog);
};

#define XTEST_LOG_(severity)                                                 \
  ::xtest::internal::XTestLog(::xtest::internal::XTEST_##severity, __FILE__, \
                              __LINE__)                                      \
      .GetStream()
}  // namespace internal

};  // namespace xtest

#endif  // XTEST_INCLUDE_INTERNAL_XTEST_PORT_HH_
