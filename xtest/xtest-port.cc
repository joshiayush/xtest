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

#include "internal/xtest-port.hh"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "xtest-message.hh"

namespace xtest {
namespace internal {
const char kUnknownFile[] = "unknown file";

// Formats a source file path and a line number as they would appear
// in an error message from the compiler used to compile this code.
std::string FormatFileLocation(const char* file, const uint64_t& line) {
  const std::string fileName(file == nullptr ? kUnknownFile : file);
  return fileName + ":" + StreamableToString(line) + ":";
}

// Logs the file and line number to the stderr.
XTestLog::XTestLog(const XTestLogSeverity& severity, const char* file,
                   const uint64_t& line)
    : _M_severity(severity) {
  const char* const marker = severity == XTEST_INFO      ? "[  INFO ]"
                             : severity == XTEST_WARNING ? "[WARNING]"
                             : severity == XTEST_ERROR   ? "[ ERROR ]"
                                                         : "[ FATAL ]";
  GetStream() << marker << " " << FormatFileLocation(file, line) << ": ";
}

// Flushes the buffers and, if severity is XTEST_FATAL, aborts the program.
XTestLog::~XTestLog() {
  GetStream() << std::endl;
  if (_M_severity == XTEST_FATAL) {
    std::fflush(stderr);
    std::abort();
  }
}
}  // namespace internal
}  // namespace xtest
