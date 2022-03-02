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

#include "redirector.hh"

#include <unistd.h>

#include <cstdio>

namespace xtest {
namespace testing {
void StdoutRedirectorContext::ReplaceStdoutWithContextBuffer() {
  std::fflush(stdout);  // Clear everyting first.
  m_stdout_ = dup(STDOUT_FILENO);
  std::freopen("NUL", "a", stdout);  // Redirect `stdout` to `NULL` so to
                                     // prevent output on the console.
  std::setvbuf(stdout, m_output_buffer_, _IOFBF, REDIRECTOR_BUFFER_SIZE);
}

void StdoutRedirectorContext::RestoreStdout() {
  std::freopen("NUL", "a", stdout);
  dup2(m_stdout_, STDOUT_FILENO);
  std::setvbuf(stdout, NULL, _IOFBF, REDIRECTOR_BUFFER_SIZE);
}

void StderrRedirectorContext::ReplaceStderrWithContextBuffer() {
  std::fflush(stderr);  // Clear everyting first.
  m_stderr_ = dup(STDERR_FILENO);
  std::freopen("NUL", "a", stderr);  // Redirect `stderr` to `NULL` so to
                                     // prevent output on the console.
  std::setvbuf(stderr, m_output_buffer_, _IOFBF, REDIRECTOR_BUFFER_SIZE);
}

void StderrRedirectorContext::RestoreStderr() {
  std::freopen("NUL", "a", stderr);
  dup2(m_stderr_, STDERR_FILENO);
  std::setvbuf(stderr, NULL, _IOFBF, REDIRECTOR_BUFFER_SIZE);
}
}  // namespace testing
}  // namespace xtest
