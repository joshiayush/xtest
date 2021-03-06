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

#ifndef XTEST_INCLUDE_INTERNAL_XTEST_STRING_HH_
#define XTEST_INCLUDE_INTERNAL_XTEST_STRING_HH_

#include <cstdio>
#include <memory>
#include <string>

namespace xtest {
class String {
 private:
  // Escapes control characters and quotes in a string and appends it to the
  // given buffer.
  static void ChrCStrLiteral(const char ch, std::string *const buffer);

 public:
  String() = delete;

  // Returns a string with printable representation of escape sequences.
  static std::string Repr(std::string str);

  // Checks if the given string starts with the given prefix.
  static bool StartsWith(const std::string &str, const std::string &prefix);

  // Compares two C strings.  Returns true if and only if they have the same
  // content.
  //
  // Unlike strcmp(), this function can handle NULL argument(s).  A NULL C
  // string is considered different to any non-NULL C string, including the
  // empty string.
  static bool CStringEquals(const char *lhs, const char *rhs);

  // Compares two C strings, ignoring case.  Returns true if and only if they
  // have the same content.
  //
  // Unlike strcasecmp(), this function can handle NULL argument(s).  A NULL C
  // string is considered different to any non-NULL C string, including the
  // empty string.
  static bool CaseInsensitiveCStringEquals(const char *lhs, const char *rhs);
};
}  // namespace xtest

#endif  // XTEST_INCLUDE_INTERNAL_XTEST_STRING_HH_
