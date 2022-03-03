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
#include <cstdlib>

namespace xtest {
namespace testing {
// Construct a `RedirectorContext` instance for the given context stream.
//
// Filles `M_output_buffer_` variable with all `NULL` to over-write garbage
// values.
RedirectorContext::RedirectorContext(const RedirectorContextStream& stream)
    : M_stream_(stream),
      M_redirector_file_name_("NULL"),
      M_redirector_file_open_mode_("a") {
  for (int32_t i = 0; i < REDIRECTOR_BUFFER_SIZE; ++i)
    M_output_buffer_[i] = '\0';
}

// Replaces given standard stream with context buffer for later retrieval of
// data.  This method is marked as `private` and meant to be used within the
// public methods.  This is the internal of `ReplaceStreamWithContextBuffer`
// method; allowing us to replace the given standard stream with the context
// buffer i.e., `M_output_buffer_`.
void RedirectorContext::ReplaceStreamWithContextBuffer(FILE*& stream) {
  std::fflush(stream);
  switch (M_stream_) {
    case RedirectorContextStream::kStdout:
      M_out_err_fd_ = dup(STDOUT_FILENO);
      break;
    case RedirectorContextStream::kStderr:
      M_out_err_fd_ = dup(STDERR_FILENO);
      break;
  }
  std::freopen(M_redirector_file_name_, M_redirector_file_open_mode_, stream);
  std::setvbuf(stream, M_output_buffer_, _IOFBF, REDIRECTOR_BUFFER_SIZE);
}

// Replaces given standard stream with context buffer for later retrieval of
// data.  This method is marked as `public` and meant to be used from the user
// code.
void RedirectorContext::ReplaceStreamWithContextBuffer() {
  switch (M_stream_) {
    case RedirectorContextStream::kStdout:
      ReplaceStreamWithContextBuffer(stdout);
      break;
    case RedirectorContextStream::kStderr:
      ReplaceStreamWithContextBuffer(stderr);
      break;
  }
}

// This method is used to restore the standard stream that was replaced by the
// context buffer.  This method is marked as `private` and meant to be used
// within the public methods.  This is the internal of `RestoreStream` method;
// allowing us to restore the standard streams at any point in our code.
void RedirectorContext::RestoreStream(FILE*& stream) {
  std::freopen(M_redirector_file_name_, M_redirector_file_open_mode_, stream);
  switch (M_stream_) {
    case RedirectorContextStream::kStdout:
      dup2(M_out_err_fd_, STDOUT_FILENO);
      break;
    case RedirectorContextStream::kStderr:
      dup2(M_out_err_fd_, STDERR_FILENO);
      break;
  }
  std::setvbuf(stream, NULL, _IOFBF, REDIRECTOR_BUFFER_SIZE);
}

// This method is used to restore the standard stream that was replaced by the
// context buffer.  This method is marked as `public` and meant to be used from
// the user code.
void RedirectorContext::RestoreStream() {
  switch (M_stream_) {
    case RedirectorContextStream::kStdout:
      RestoreStream(stdout);
      break;
    case RedirectorContextStream::kStderr:
      RestoreStream(stderr);
      break;
  }
}

// Removes the file created for redirecting data from the standard streams.
RedirectorContext::~RedirectorContext() {
  std::remove(M_redirector_file_name_);
}
}  // namespace testing
}  // namespace xtest
