// Copyright 2022, The xtest authors Inc.
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
//     * Neither the name of The xtest authors Inc. nor the names of its
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

#include "internal/xtest-string.hh"

#include <cstdio>
#include <memory>
#include <string>

namespace xtest {
namespace string {
void ChrCStrLiteral(const char ch, std::string& buffer) {
  if ((ch >= 0x20 && ch <= 0x7E) && ch != '\'' && ch != '\"' && ch != '\\') {
    buffer +=
        ch;  // opposite of control characters can be saved just as they are
  } else {
    switch (ch) {
      case '\a':
        buffer += "\\a";
        break;
      case '\b':
        buffer += "\\b";
        break;
      case '\f':
        buffer += "\\f";
        break;
      case '\n':
        buffer += "\\n";
        break;
      case '\r':
        buffer += "\\r";
        break;
      case '\t':
        buffer += "\\t";
        break;
      case '\v':
        buffer += "\\v";
        break;
      case '\\':
        buffer += "\\\\";
        break;
      case '\'':
        buffer += "\\'";
        break;
      case '\"':
        buffer += "\\\"";
        break;
      default: {
        char buffer_[5];
        std::snprintf(
            buffer_, 5, "\\x%02x",
            static_cast<unsigned char>(ch));  // this handles control
                                              // characters that are not
                                              // printable and does not
                                              // meet any case
        buffer += buffer_;
      } break;
    }
  }
}

std::string Repr(std::string str) {
  std::string buffer;
  buffer.reserve(str.size());  // reserve 'reserved_bits' bytes for buffer
  for (const auto& chr : str)
    ChrCStrLiteral(
        chr,
        buffer);  // escape the character that we took out in this iteration
  str = buffer;
  return str;
}
}  // namespace string
}  // namespace xtest
