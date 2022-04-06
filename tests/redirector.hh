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

#ifndef XTEST_TESTS_REDIRECTOR_HH_
#define XTEST_TESTS_REDIRECTOR_HH_

#include <unistd.h>

#include <cstdint>
#include <cstdio>

#define REDIRECTOR_BUFFER_SIZE 1024

namespace xtest {
namespace testing {
enum class RedirectorContextStream {
  kStdout,
  kStderr,
};

class RedirectorContext {
 public:
  // Construct a `RedirectorContext` instance for the given context stream.
  //
  // Filles `M_output_buffer_` variable with all `NULL` to over-write garbage
  // values.
  explicit RedirectorContext(const RedirectorContextStream& stream);

  // Removes the file created for redirecting data from the standard streams.
  ~RedirectorContext();

  // Replaces given standard stream with context buffer for later retrieval of
  // data.  This method is marked as `public` and meant to be used from the user
  // code.
  void ReplaceStreamWithContextBuffer();

  // This method is used to restore the standard stream that was replaced by
  // the context buffer.  This method is marked as `public` and meant to be used
  // from the user code.
  void RestoreStream();

 private:
  // Replaces given standard stream with context buffer for later retrieval of
  // data.  This method is marked as `private` and meant to be used within the
  // public methods.  This is the internal of `ReplaceStreamWithContextBuffer`
  // method; allowing us to replace the given standard stream with the context
  // buffer i.e., `M_output_buffer_`.
  void ReplaceStreamWithContextBuffer(FILE* stream);

  // This method is used to restore the standard stream that was replaced by
  // the context buffer.  This method is marked as `private` and meant to be
  // used within the public methods.  This is the internal of `RestoreStream`
  // method; allowing us to restore the standard streams at any point in our
  // code.
  void RestoreStream(FILE* stream);

 private:
  RedirectorContextStream M_stream_;
  int32_t M_out_err_fd_;

 public:
  char M_output_buffer_[REDIRECTOR_BUFFER_SIZE];
  const char* M_redirector_file_name_;
  const char* M_redirector_file_open_mode_;
};
}  // namespace testing
}  // namespace xtest

#endif  // XTEST_TESTS_REDIRECTOR_HH_
