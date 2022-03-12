// Copyright 2021, The xtest authors Inc.
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

#include "internal/xtest-printers.hh"

#include <cstdarg>
#include <string>

#include "internal/xtest-port.hh"

namespace xtest {
namespace internal {
// Returns the ANSI color code for the given color.  `XTestColor::kDefault` is
// an invalid input.
std::string GetAnsiColorCode(const XTestColor& color) {
  switch (color) {
    case XTestColor::kRed:
      return "1";
    case XTestColor::kGreen:
      return "2";
    case XTestColor::kYellow:
      return "3";
    default:
      return nullptr;
  }
}

void ColoredPrintf(const XTestColor& color, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

#if defined(__linux__)
  printf("\033[0;3%sm", GetAnsiColorCode(color).c_str());
#endif
  vprintf(fmt, args);
#if defined(__linux__)
  printf("\033[m");  // Resets the terminal to default.
#endif

  va_end(args);
}
}  // namespace internal
}  // namespace xtest
