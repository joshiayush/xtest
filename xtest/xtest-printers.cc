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
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

#include "internal/xtest-port-arch.hh"
#include "internal/xtest-port.hh"
#include "internal/xtest-string.hh"

#if XTEST_OS_WINDOWS
#include <windows.h>
#endif

namespace xtest {
namespace internal {
#if XTEST_OS_WINDOWS
// Returns the character attribute for the given color.
static WORD GetColorAttribute(XTestColor color) {
  switch (color) {
    case XTestColor::kRed:
      return FOREGROUND_RED;
    case XTestColor::kGreen:
      return FOREGROUND_GREEN;
    case XTestColor::kYellow:
      return FOREGROUND_RED | FOREGROUND_GREEN;
    default:
      return 0;
  }
}

static int32_t GetBitOffset(WORD color_mask) {
  if (color_mask == 0)
    return 0;

  int32_t bitOffset = 0;
  while ((color_mask & 1) == 0) {
    color_mask >>= 1;
    ++bitOffset;
  }
  return bitOffset;
}

// Returns the character attribute for the given color.
static WORD GetNewColor(XTestColor color, WORD old_color_attrs) {
  // Let's reuse the BG and the FG.
  static const WORD background_mask = BACKGROUND_BLUE | BACKGROUND_GREEN |
                                      BACKGROUND_RED | BACKGROUND_INTENSITY;
  static const WORD foreground_mask = FOREGROUND_BLUE | FOREGROUND_GREEN |
                                      FOREGROUND_RED | FOREGROUND_INTENSITY;
  const WORD existing_bg = old_color_attrs & background_mask;

  WORD new_color =
      GetColorAttribute(color) | existing_bg | FOREGROUND_INTENSITY;
  static const int32_t bg_bitOffset = GetBitOffset(background_mask);
  static const int32_t fg_bitOffset = GetBitOffset(foreground_mask);

  if (((new_color & background_mask) >> bg_bitOffset) ==
      ((new_color & foreground_mask) >> fg_bitOffset))
    new_color ^= FOREGROUND_INTENSITY;  // invert intensity
  return new_color;
}
#else
// Returns the ANSI color code for the given color.  `XTestColor::kDefault`
// result in an empty string.  This function should only be used in Unix-like
// operating systems where color output just requires emitting escape codes.
static std::string GetAnsiColorCode(const XTestColor& color) {
  switch (color) {
    case XTestColor::kRed:
      return "1";
    case XTestColor::kGreen:
      return "2";
    case XTestColor::kYellow:
      return "3";
    default:
      return "";
  }
}
#endif

// Returns true only if the output stream is a TTY.
//
// Uses posix compatible functions `IsAtty()` and `FileNo()` to check if the
// output stream is a TTY.
bool ShouldUseColor(bool stdout_is_tty) {
  std::string color = XTEST_FLAG_GET_(color);
  const char* const xtest_color = color.c_str();

  if (String::CaseInsensitiveCStringEquals(xtest_color, "auto")) {
#if XTEST_OS_WINDOWS && !XTEST_OS_WINDOWS_MINGW
    // On Windows the TERM variable is usually not set, but the
    // console there does support colors.
    return stdout_is_tty;
#else
    // On non-Windows platforms, we rely on the TERM variable.
    const char* const term = posix::GetEnv("TERM");
    const bool term_supports_color =
        String::CStringEquals(term, "xterm") ||
        String::CStringEquals(term, "xterm-color") ||
        String::CStringEquals(term, "xterm-256color") ||
        String::CStringEquals(term, "screen") ||
        String::CStringEquals(term, "screen-256color") ||
        String::CStringEquals(term, "tmux") ||
        String::CStringEquals(term, "tmux-256color") ||
        String::CStringEquals(term, "rxvt-unicode") ||
        String::CStringEquals(term, "rxvt-unicode-256color") ||
        String::CStringEquals(term, "linux") ||
        String::CStringEquals(term, "cygwin");
    return stdout_is_tty && term_supports_color;
#endif  // XTEST_OS_WINDOWS
  }

  // We take "yes", "true", "t", and "1" as meaning "yes".  If the value is
  // neither one of these nor "auto", we treat it as "no" to be conservative.
  return String::CaseInsensitiveCStringEquals(xtest_color, "yes") ||
         String::CaseInsensitiveCStringEquals(xtest_color, "true") ||
         String::CaseInsensitiveCStringEquals(xtest_color, "t") ||
         String::CStringEquals(xtest_color, "1");
}

// Prints text with colors in both Windows and Unix-like systems by setting the
// console text attributes and emitting colors respectively.  If the output is
// redirected to a file then we disable color output.
void ColoredPrintf(const XTestColor& color, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

// The purpose why we use `XTEST_TESTING_ENABLED` macro to add the following
// code is because we need to test `ColoredPrintf()` function's output with
// escape sequences and different console text attributes which is only possible
// if we disable `ShouldUseColor()` function's result, because
// `ShouldUseColor()` will always return `false` in testing because we
// intentionally redirect `ColoredPrintf()` function's output to a context
// buffer in order to check it.
#if defined(XTEST_TESTING_ENABLED)
  if (!ShouldUseColor(posix::IsAtty(posix::FileNo(stdout)) != 0)) {
    std::vprintf(fmt, args);
    va_end(args);
    return;
  }
#endif

#if XTEST_OS_WINDOWS
  // Get a `HANDLE` type to `STD_OUTPUT_HANDLE` which is 4294967285 and later is
  // cast to (signed) integer.
  const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if (stdout_handle == INVALID_HANDLE_VALUE)
    std::exit(GetLastError());

  // Gets the current text color.
  CONSOLE_SCREEN_BUFFER_INFO buffer_info;
  GetConsoleScreenBufferInfo(stdout_handle, &buffer_info);
  const WORD old_color_attrs = buffer_info.wAttributes;
  const WORD new_color = GetNewColor(color, old_color_attrs);

  // We need to flush the stream buffers into the console before each
  // SetConsoleTextAttribute call lest it affect the text that is already
  // printed but has not yet reached the console.
  std::fflush(stdout);
  SetConsoleTextAttribute(stdout_handle, new_color);

  std::vprintf(fmt, args);

  std::fflush(stdout);
  // Restores the text color.
  SetConsoleTextAttribute(stdout_handle, old_color_attrs);
#else
  std::printf("\x1b[0;3%sm", GetAnsiColorCode(color).c_str());
  std::vprintf(fmt, args);
  std::printf("\x1b[m");  // Resets the terminal to default.
#endif

  va_end(args);
}

// Prints a string containing code-encoded text.  The following escape sequences
// can be used in the string to control the text color:
//
//   @@    prints a single '@' character.
//   @R    changes the color to red.
//   @G    changes the color to green.
//   @Y    changes the color to yellow.
//   @D    changes to the default terminal text color.
void PrintColorEncoded(const char* str) {
  XTestColor color = XTestColor::kDefault;  // The current color.

  // Conceptually, we split the string into segments divided by escape
  // sequences.  Then we print one segment at a time.  At the end of each
  // iteration, the str pointer advances to the beginning of the next segment.
  for (;;) {
    const char* p = std::strchr(str, '@');
    if (p == nullptr) {
      ColoredPrintf(color, "%s", str);
      return;
    }

    ColoredPrintf(color, "%s", std::string(str, p).c_str());

    const char ch = p[1];
    str = p + 2;
    if (ch == '@') {
      ColoredPrintf(color, "@");
    } else if (ch == 'D') {
      color = XTestColor::kDefault;
    } else if (ch == 'R') {
      color = XTestColor::kRed;
    } else if (ch == 'G') {
      color = XTestColor::kGreen;
    } else if (ch == 'Y') {
      color = XTestColor::kYellow;
    } else {
      --str;
    }
  }
}
}  // namespace internal
}  // namespace xtest
