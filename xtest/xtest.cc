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

#include "xtest.hh"

#include <cinttypes>
#include <csetjmp>
#include <csignal>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "internal/xtest-port.hh"
#include "xtest-message.hh"

namespace xtest {
namespace impl {
// Calls std::longjmp() with M_jumpOutOfTest instance as its first
// argument.
//
// This function calls the std::longjmp() function with the std::jmp_buf
// instance M_jumpOutOfTest as its first argument when the SIGABRT is raised
// inside of the function run_registered_test() that runs the registered test
// suites.
void SignalHandler(int param) {
  std::longjmp(GTestRegistryInst.M_jumpOutOfTest, 1);
}
}  // namespace impl
// Global counter for non-fatal test failures.
//
// This global counter is defined in the object file xtest.cc and incremented
// every time a non-fatal test assertion fails.
uint64_t G_n_testFailures = 0;

// Returns a string of length `width` all filled with the character `chr`.
//
// This function is mainly used to decorate the box used in the test summary
// like show below where we are filling the boxes with either `-` or `=`
// character,
//
// ```shell
// [‑‑‑‑‑‑‑‑‑‑] Global test environment tear‑down
// [==========] 2 tests from 1 test case ran. (10 ms total)
// [  PASSED  ] 1 test.
// [  FAILED  ] 1 test, listed below:
// [  FAILED  ] SquareRootTest.PositiveNos
//
//  1 FAILED TEST
// ```
std::string GetStrFilledWith(const char& chr, ::std::size_t width) {
  ::std::stringstream sstream;
  for (::std::size_t i = 0; i < width; ++i)
    sstream << chr;
  return sstream.str();
}

// Align the original string to the given alignment inside of the new sized
// buffer.
//
// This function is mainly used to print out nice visual representation of the
// test results like the following,
//
// ```shell
// [  FAILED  ] SquareRootTest.PositiveNos
// ```
//
// In order to align the letter `FAILED` to the center we use this function with
// a 'alignSide' of 'ALIGN_CENTER'.
//
// If not given this function will use the default values for `newStrWidth` and
// `alignSide` i.e., `XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_` and
// `ALIGN_CENTER` respectively.
std::string GetStringAlignedTo(const ::std::string& str,
                               const ::std::size_t& newStrWidth,
                               const StringAlignValues& alignSide) {
  ::std::size_t strlen = str.length();
  if (strlen >= newStrWidth)
    return str.c_str();

  ::std::stringstream sstream;
  ::std::size_t whiteSpacesNum = newStrWidth - strlen;
  ::std::size_t suffixlen, preffixlen;

  switch (alignSide) {
    case ALIGN_LEFT:
      suffixlen = (whiteSpacesNum / 5) + 1;
      preffixlen = whiteSpacesNum - suffixlen;
      break;
    case ALIGN_CENTER:
      suffixlen = whiteSpacesNum / 2;
      preffixlen = whiteSpacesNum - suffixlen;
      break;
    case ALIGN_RIGHT:
      preffixlen = (whiteSpacesNum / 5) + 1;
      suffixlen = whiteSpacesNum - preffixlen;
      break;
  }

  sstream << GetStrFilledWith(' ', suffixlen) << str
          << GetStrFilledWith(' ', preffixlen);
  return sstream.str();
}

// Constructs an empty Message.
//
// We allocate the stringstream separately because otherwise each use of
// ASSERT_* or EXPECT_* in a procedure adds over 200 bytes to the procedure's
// stack frame leading to huge stack frames in some cases; gcc does not reuse
// the stack space.
Message::Message() : _M_sstream(new ::std::stringstream) {
  // By default, we want there to be enough precision when printing a double to
  // a Message.
  *_M_sstream << std::setprecision(std::numeric_limits<double>::digits10 + 2);
}

// Gets the text streamed to this object so far as an std::string.  Each '\0'
// character in the buffer is replaced with "\\0".
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
::std::string Message::GetString() const {
  const ::std::string& str = _M_sstream->str();
  const char* const start = str.c_str();
  const char* const end = start + str.length();

  std::string result;
  result.reserve(static_cast<size_t>(2 * (end - start)));
  for (const char* ch = start; ch != end; ++ch) {
    if (*ch == '\0')
      result += "\\0";
    else
      result += *ch;
  }

  return result;
}

// Runs all the registered test suites and returns the failure count.
//
// This function runs all the registered test suites in the
// xtest::GTestRegistryInst.M_head instance while also handling the abort
// signals raised by ASSERT_* assertions.
//
// In case an assertion fails then this function marks that test suite as
// FAILED while silently continuing executing rest of the test suites.
uint64_t RunRegisteredTests() {
  TestRegistrar* node = GTestRegistryInst.M_head;

  void (*SavedSignalHandler)(int);
  SavedSignalHandler = std::signal(SIGABRT, impl::SignalHandler);

  while (node) {
    if (node->M_testFunc) {
      // We are setting a jump here to later mark the test result as FAILED in
      // case the node->M_testFunc raised an abort signal result of an ASSERT_*
      // assertion.
      if (setjmp(GTestRegistryInst.M_jumpOutOfTest)) {
        node->M_testResult = TestResult::FAILED;
      } else {
        node->M_testFunc(&GTestRegistryInst, node);
        if (node->M_testResult == TestResult::UNKNOWN)
          node->M_testResult = TestResult::PASSED;
      }
    }
    node = node->M_nextTestSuite;
  }

  std::signal(SIGABRT, SavedSignalHandler);

  return G_n_testFailures;
}

void InitXtest() { _DebugListRegisteredTests(::std::cout); }
}  // namespace xtest
