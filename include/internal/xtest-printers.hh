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

#ifndef XTEST_INCLUDE_INTERNAL_XTEST_PRINTERS_HH_
#define XTEST_INCLUDE_INTERNAL_XTEST_PRINTERS_HH_

#include <cstdarg>
#include <string>

namespace xtest {
namespace internal {
enum class XTestColor { kDefault, kRed, kGreen, kYellow };

// Returns true only if the output stream is a TTY.
//
// Uses posix compatible functions `IsAtty()` and `FileNo()` to check if the
// output stream is a TTY.
bool ShouldUseColor(bool stdout_is_tty);

// Prints text with colors in both Windows and Unix-like systems by setting the
// console text attributes and emitting colors respectively.  If the output is
// redirected to a file then we disable color output.
void ColoredPrintf(const XTestColor& color, const char* fmt, ...);

// Prints a string containing code-encoded text.  The following escape sequences
// can be used in the string to control the text color:
//
//   @@    prints a single '@' character.
//   @R    changes the color to red.
//   @G    changes the color to green.
//   @Y    changes the color to yellow.
//   @D    changes to the default terminal text color.
void PrintColorEncoded(const char* str);
}  // namespace internal
}  // namespace xtest

#endif  // XTEST_INCLUDE_INTERNAL_XTEST_PRINTERS_HH_
