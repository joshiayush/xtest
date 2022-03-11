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

#ifndef XTEST_INCLUDE_XTEST_HH_
#define XTEST_INCLUDE_XTEST_HH_

#include <cinttypes>
#include <iostream>
#include <string>

#include "internal/xtest-port.hh"
#include "xtest-message.hh"
#include "xtest-registrar.hh"

namespace xtest {
namespace impl {
// Calls `std::longjmp()` with `M_jumpOutOfTest` instance as its first argument.
//
// This function calls the `std::longjmp()` function with the `std::jmp_buf`
// instance `M_jumpOutOfTest` as its first argument when the `SIGABRT` is raised
// inside of the function `RunRegisteredTests()` that runs the registered test
// suites.
void SignalHandler(int param);

// Special class to take streamables to the console and highlight them with
// green color.
//
// This class will print the streamable object in 'green' color,  At this moment
// we don't have any functionality to colorify the streamable object before
// putting it on the console.
class MessageStream {
 private:
  // The type of basic IO manipulators (endl, ends, and flush) for narrow
  // streams.
  //
  // @TODO(joshiayush): Copy of the Message implementation of this type, if
  // possible refactor into one single definition.
  typedef std::ostream& (*BasicNarrowIoManip)(std::ostream&);

  std::ostream& _M_stream;

 public:
  MessageStream() : _M_stream(std::cout) {}

  template <typename Type>
  MessageStream operator<<(const Type& streamable) {
    std::cout << internal::StreamableToString(streamable);
    return *this;
  }

  // Since the basic IO manipulators are overloaded for both narrow and wide
  // streams, we have to provide this specialized definition of operator <<,
  // even though its body is the same as the templatized version above.  Without
  // this definition, streaming endl or other basic IO manipulators to Message
  // will confuse the compiler.
  MessageStream& operator<<(BasicNarrowIoManip val) {
    std::cout << val;
    return *this;
  }
};

#define MESSAGE() ::xtest::impl::MessageStream()
}  // namespace impl

// When this flag is specified, the xtest's help message is printed on the
// console.
extern bool FLAG_xtest_help;

// When this flag is specified, tests' order is randomized on every iteration.
extern bool FLAG_xtest_shuffle;

// This flag enables using colors in terminal output. Available values are "yes"
// to enable colors, "no" (disable colors), or "auto" (the default) to let
// Google Test decide.
extern bool FLAG_xtest_color;

using TestSuiteAndTestNumberPair = std::pair<std::uint64_t, std::uint64_t>;

// Returns the total number of test cases and tests registered.
//
// The returned pair has the total number of test cases as its first element and
// the total number of tests as its second element.
TestSuiteAndTestNumberPair GetTestSuiteAndTestNumber();

// Returns the number of tests that failed during test execution.
//
// This function keeps a count of the number of test suites that called the
// `abort()` function in case of test failure assertion.
std::uint64_t GetFailedTestCount();

// Returns the `UnitTest` instance of failed tests.
//
// Iterates over the `GTestRegistryInst.M_testRegistryTable` instance and adds
// the pair who's `M_testResult` equals to `TestResult::FAILED` in the
// `failed_tests` container.
XTestUnitTest GetFailedTests();

// Runs all the registered test suites and returns the failure count.
//
// This function runs all the registered test suites in the
// `xtest::GTestRegistryInst.M_head instance` while also handling the abort
// signals raised by `ASSERT_*` assertions.
uint64_t RunRegisteredTests();

#define RUN_ALL_TESTS() xtest::RunRegisteredTests()

class PrettyUnitTestResultPrinter {
 public:
  // No instance should instantiate from this class.
  PrettyUnitTestResultPrinter() = delete;

  // Joins the test suite and test name at `.` and prints on the console using
  // `printf()`.
  static void PrintTestName(const char* test_suite, const char* test_name);

  // Prints out the information related to the number of tests a test suite
  // shares.
  static void OnTestStart(const XTestUnitTestPair& testSuite);

  // Prints the number of test suites and tests to run.  Should only be called
  // before starting iteration over the registered test suites.
  static void OnTestIterationStart();

  // Should be called for printing the status of the global environment set-up.
  // (Global environment set-up in `xtest` is a myth, this function just prints
  // a line and flushes the output stream).  This line is needed to give our
  // users a complete feel of `googletest`.
  static void OnEnvironmentsSetUpStart();

  // Prints out information related to the number of test suites and tests
  // before executing the registered tests.
  static void OnTestExecutionStart();

  // Prints out the information related to the number of tests a test suite
  // shares.  This function is very much similar to
  // `PrettyUnitTestResultPrinter::OnTestStart()` but should be ran after
  // executing all the tests of a test suite.
  static void OnTestEnd(const XTestUnitTestPair& testSuite);

  // Prints number of test suites and tests ran.  Should only be called after
  // iterating over all the registered test suites.
  static void OnTestIterationEnd();

  // Prints out information related to the number of test failed ans passed
  // after executing all the registered tests.
  static void OnTestExecutionEnd();

  // Should be called for printing the status of the global environment
  // tear-down. (Global environment tear-down in `xtest` is a myth, this
  // function just prints a line and flushes the output stream).  This line is
  // needed to give our users a complete feel of `googletest`.
  static void OnEnvironmentsTearDownStart();

 private:
  // Prints the number of tests that failed after executing all the registered
  // tests.  If none of the test failed and this function is somehow got called
  // then this function prints `0 FAILED TESTS` and flushes the stream buffer.
  //
  // Note: This function should only be called when there are failed tests.
  static void PrintFailedTests();
};

// Parses all the xtest command line flags.
//
// This function should be called only at the initialization step of `xtest`
// library.
void ParseXTestFlags(int32_t* argc, char** argv);

// Invokes functions corresponding to the command line flags given.
//
// This function should be called after the `ParseXTestFlags()` function.
void PostFlagParsing();

// Initializes xtest.  This must be called before calling `RUN_ALL_TESTS()`.
//
// In particular, it parses a command line for the flags that xtest recognizes.
// Whenever a Google Test flag is seen, it is removed from argv, and *argc is
// decremented.  Calling the function for the second time has no user-visible
// effect.
void InitXTest(int32_t* argc, char** argv);
}  // namespace xtest

#include "xtest-assertions.hh"

#endif  // XTEST_INCLUDE_XTEST_HH_
