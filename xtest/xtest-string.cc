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

#include "internal/xtest-port.hh"

namespace xtest {
// Escapes control characters and quotes in a string and appends it to the given
// buffer.
void String::ChrCStrLiteral(const char ch, std::string* const buffer) {
  if ((ch >= 0x20 && ch <= 0x7E) && ch != '\'' && ch != '\"' && ch != '\\') {
    *buffer +=
        ch;  // Opposite of control characters can be saved just as they are.
  } else {
    switch (ch) {
      case '\a':
        *buffer += "\\a";
        break;
      case '\b':
        *buffer += "\\b";
        break;
      case '\f':
        *buffer += "\\f";
        break;
      case '\n':
        *buffer += "\\n";
        break;
      case '\r':
        *buffer += "\\r";
        break;
      case '\t':
        *buffer += "\\t";
        break;
      case '\v':
        *buffer += "\\v";
        break;
      case '\\':
        *buffer += "\\\\";
        break;
      case '\'':
        *buffer += "\\'";
        break;
      case '\"':
        *buffer += "\\\"";
        break;
      default: {
        char buffer_[6];
        std::snprintf(
            buffer_, 6, "\\0x%02x",
            static_cast<unsigned char>(ch));  // This handles control
                                              // characters that are not
                                              // printable and does not
                                              // meet any case.
        *buffer += buffer_;
      } break;
    }
  }
}

// Returns a string with printable representation of escape sequences.
std::string String::Repr(std::string str) {
  std::string buffer;
  buffer.reserve(str.size());  // Reserve bytes for buffer.
  for (const auto& chr : str)
    ChrCStrLiteral(
        chr,
        &buffer);  // Escape the character that we took out in this iteration.
  str = buffer;
  return str;
}

// Checks if the given string starts with the given prefix.
bool String::StartsWith(const std::string& str, const std::string& prefix) {
  return str.compare(0, prefix.size(), prefix) == 0;
}

// Compares two C strings.  Returns true if and only if they have the same
// content.
//
// Unlike strcmp(), this function can handle NULL argument(s).  A NULL C string
// is considered different to any non-NULL C string, including the empty string.
bool String::CStringEquals(const char* lhs, const char* rhs) {
  if (lhs == nullptr)
    return rhs == nullptr;
  if (rhs == nullptr)
    return false;
  return strcmp(lhs, rhs) == 0;
}

// Compares two C strings, ignoring case.  Returns true if and only if they have
// the same content.
//
// Unlike strcasecmp(), this function can handle NULL argument(s).  A NULL C
// string is considered different to any non-NULL C string, including the empty
// string.
bool String::CaseInsensitiveCStringEquals(const char* lhs, const char* rhs) {
  if (lhs == nullptr)
    return rhs == nullptr;
  if (rhs == nullptr)
    return false;
  return posix::StrCaseCmp(lhs, rhs) == 0;
}
}  // namespace xtest
