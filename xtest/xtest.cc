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

#include <chrono>  // NOLINT
#include <cinttypes>
#include <csetjmp>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "internal/xtest-port.hh"
#include "internal/xtest-printers.hh"
#include "xtest-message.hh"

// When this flag is specified, the xtest's help message is printed on the
// console.
XTEST_FLAG_DEFINE_bool_(help, false, "Prints the help message for xtest.");

// When this flag is specified, tests' order is randomized on every iteration.
XTEST_FLAG_DEFINE_bool_(shuffle, false,
                        "True if and only if " XTEST_NAME_
                        " should randomize tests' order on every run.");

// This flag enables using colors in terminal output. Available values are "yes"
// to enable colors, "no" (disable colors), or "auto" (the default) to let XTest
// decide.
XTEST_FLAG_DEFINE_string_(
    color, "auto",
    "Whether to use colors in the output.  Valid values: yes, no, "
    "and auto.  'auto' means to use colors if the output is "
    "being sent to a terminal and the TERM environment variable "
    "is set to a terminal type that supports colors.");

XTEST_GLOBAL_DEFINE_uint64_(failure_count, 0,
                            "Global counter for the number of failed tests.");

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
  std::longjmp(XTestRegistryInstance.M_jumpOutOfTest, 1);
}
}  // namespace impl

namespace internal {
Timer::Timer() : start_(std::chrono::steady_clock::now()) {}

// Return time elapsed in milliseconds since the timer was created.
TimeInMillis Timer::Elapsed() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now() - start_)
      .count();
}
}  // namespace internal

std::uint64_t G_n_tests = 0;
std::uint64_t G_n_testSuites = 0;

std::uint64_t G_n_failedTests = 0;

static const char kColorEncodedHelpMessage[] =
    "This program contains tests written using xtest.  You can use the\n"
    "following command line flags to control its behaviour:\n"
    "\n"
    "Test Execution:\n"
    "   @G--shuffle@D\n"
    "     Randomize tests' order. (In development)\n";

// A copy of all command line arguments.  Set by InitXTest().
static std::vector<std::string> G_argvs;

// XTestIsInitialized() returns true if and only if the user has initialized
// xtest.  Useful for catching the user mistake of not initializing xtest
// before calling RUN_ALL_TESTS().
static bool XTestIsInitialized() { return G_argvs.size() > 0; }

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
std::string GetStrFilledWith(const char& chr, std::size_t width) {
  std::stringstream sstream;
  for (std::size_t i = 0; i < width; ++i)
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
// In order to align the letter `FAILED` to the center we use this function
// with a 'alignSide' of 'ALIGN_CENTER'.
//
// If not given this function will use the default values for `newStrWidth`
// and `alignSide` i.e., `XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_` and
// `ALIGN_CENTER` respectively.
std::string GetStringAlignedTo(const std::string& str,
                               const std::size_t& newStrWidth,
                               const StringAlignValues& alignSide) {
  std::size_t strlen = str.length();
  if (strlen >= newStrWidth)
    return str.c_str();

  std::stringstream sstream;
  std::size_t whiteSpacesNum = newStrWidth - strlen;
  std::size_t suffixlen, preffixlen;

  switch (alignSide) {
    case ALIGN_LEFT:
      suffixlen = (whiteSpacesNum / 5);
      preffixlen = whiteSpacesNum - suffixlen;
      break;
    case ALIGN_CENTER:
      suffixlen = whiteSpacesNum / 2;
      preffixlen = whiteSpacesNum - suffixlen;
      break;
    case ALIGN_RIGHT:
      preffixlen = (whiteSpacesNum / 5);
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
// stack frame leading to huge the stack space.
Message::Message() : _M_sstream(new std::stringstream) {
  // By default, we want there to be enough precision when printing a double
  // to a Message.
  *_M_sstream << std::setprecision(std::numeric_limits<double>::digits10 + 2);
}

// Gets the text streamed to this object so far as an std::string.  Each '\0'
// character in the buffer is replaced with "\\0".
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
std::string Message::GetString() const {
  const std::string& str = _M_sstream->str();
  const char* const start = str.c_str();
  const char* const end = start + str.length();

  std::string result;
  result.reserve(static_cast<size_t>(2 * (end - start)));
  for (const char* ch = start; ch != end; ++ch)
    result += *ch == '\0' ? std::string("\\0") : std::string(sizeof(char), *ch);

  return result;
}

// Returns the total number of test cases and tests registered.
//
// The returned pair has the total number of test cases as its first element
// and the total number of tests as its second element.
TestSuiteAndTestNumberPair GetTestSuiteAndTestNumber() {
  if (G_n_testSuites != 0 && G_n_tests != 0)
    return TestSuiteAndTestNumberPair{G_n_testSuites, G_n_tests};
  G_n_testSuites = G_n_tests = 0;
  for (const auto& testCase : XTestRegistryInstance.M_testRegistryTable) {
    ++G_n_testSuites;
    G_n_tests += testCase.second.size();
  }
  return TestSuiteAndTestNumberPair{G_n_testSuites, G_n_tests};
}

// Returns the `XTestUnitTest` instance of failed tests.
//
// Iterates over the `XTestRegistryInstance.M_testRegistryTable` instance and
// adds the pair who's `M_testResult` equals to `TestResult::FAILED` in the
// `failed_tests` container.
XTestUnitTest GetFailedTests() {
  XTestUnitTest failed_tests;
  for (const XTestUnitTestPair& testCase :
       XTestRegistryInstance.M_testRegistryTable) {
    for (const auto& test : testCase.second) {
      if (test->M_testResult != TestResult::FAILED)
        continue;
      failed_tests[testCase.first].push_back(test);
    }
  }
  return failed_tests;
}

// Returns the number of tests that failed during test execution.
//
// This function keeps a count of the number of test suites that called the
// `abort()` function in case of test failure assertion.
std::uint64_t GetFailedTestCount() {
  if (G_n_failedTests != 0)
    return G_n_failedTests;
  XTestUnitTest failedTests = GetFailedTests();
  for (const XTestUnitTestPair& test_pair : failedTests)
    for (const TestRegistrar* const& test : test_pair.second)
      ++G_n_failedTests;
  return G_n_failedTests;
}

// Joins the test suite and test name at `.` and prints on the console using
// `printf()`.
void PrettyUnitTestResultPrinter::PrintTestName(const char* test_suite,
                                                const char* test_name) {
  std::printf("%s.%s", test_suite, test_name);
}

// Prints out information related to the number of test suites and tests
// before executing the registered tests.
void PrettyUnitTestResultPrinter::OnTestExecutionStart() {
  PrettyUnitTestResultPrinter::OnTestIterationStart();
  PrettyUnitTestResultPrinter::OnEnvironmentsSetUpStart();
}

// Prints out information related to the number of test failed ans passed
// after executing all the registered tests.
void PrettyUnitTestResultPrinter::OnTestExecutionEnd() {
  PrettyUnitTestResultPrinter::OnEnvironmentsTearDownStart();
  PrettyUnitTestResultPrinter::OnTestIterationEnd();
}

// Prints out the information related to the number of tests a test suite
// shares.
void PrettyUnitTestResultPrinter::OnTestStart(
    const XTestUnitTestPair& testSuite) {
  std::printf("\n");
  internal::ColoredPrintf(internal::XTestColor::kGreen, "[%s] ",
                          GetStrFilledWith('-').c_str());
  std::printf("%lu tests from %s\n", testSuite.second.size(), testSuite.first);
  std::fflush(stdout);
}

// Prints out the information related to the number of tests a test suite
// shares.  This function is very much similar to
// `PrettyUnitTestResultPrinter::OnTestStart()` but should be ran after
// executing all the tests of a test suite.
void PrettyUnitTestResultPrinter::OnTestEnd(
    const XTestUnitTestPair& testSuite) {
  internal::ColoredPrintf(internal::XTestColor::kGreen, "[%s] ",
                          GetStrFilledWith('-').c_str());
  std::printf("%lu tests from %s", testSuite.second.size(), testSuite.first);
  TimeInMillis elapsedTime = 0;
  for (const TestRegistrar* const& test : testSuite.second)
    elapsedTime += test->M_elapsedTime;
  std::printf(" (%lu ms total)", elapsedTime);
  std::printf("\n");
  std::fflush(stdout);
}

// Prints the number of tests that failed after executing all the registered
// tests.  If none of the test failed and this function is somehow got called
// then this function prints `0 FAILED TESTS` and flushes the stream buffer.
// This function should only be called when there are failed tests.
void PrettyUnitTestResultPrinter::PrintFailedTests() {
  internal::ColoredPrintf(
      internal::XTestColor::kRed, "[%s] ",
      GetStringAlignedTo("FAILED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
                         ALIGN_CENTER)
          .c_str());
  std::printf("%lu test, listed below:\n", GetFailedTestCount());

  XTestUnitTest failedTests = GetFailedTests();
  for (const XTestUnitTestPair& testCase : failedTests) {
    for (const TestRegistrar* const& test : testCase.second) {
      internal::ColoredPrintf(
          internal::XTestColor::kRed, "[%s] ",
          GetStringAlignedTo("FAILED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
                             ALIGN_CENTER)
              .c_str());
      PrettyUnitTestResultPrinter::PrintTestName(test->M_suiteName,
                                                 test->M_testName);
      std::printf("\n");
    }
  }

  std::printf("\n");
  std::printf("%lu FAILED %s\n", GetFailedTestCount(),
              (GetFailedTestCount() == 1 ? "TEST" : "TESTS"));
  std::fflush(stdout);
}

// Prints the number of test suites and tests to run.  Should only be called
// before starting iteration over the registered test suites.
void PrettyUnitTestResultPrinter::OnTestIterationStart() {
  internal::ColoredPrintf(internal::XTestColor::kGreen, "[%s] ",
                          GetStrFilledWith('=').c_str());
  std::printf("Running %lu tests from %lu test suites.\n",
              GetTestSuiteAndTestNumber().second,
              GetTestSuiteAndTestNumber().first);
  std::fflush(stdout);
}

// Prints number of test suites and tests ran.  Should only be called after
// iterating over all the registered test suites.
void PrettyUnitTestResultPrinter::OnTestIterationEnd() {
  internal::ColoredPrintf(internal::XTestColor::kGreen, "[%s] ",
                          GetStrFilledWith('=').c_str());
  std::printf("Ran %lu tests from %lu test suites.\n",
              GetTestSuiteAndTestNumber().second,
              GetTestSuiteAndTestNumber().first);

  internal::ColoredPrintf(
      internal::XTestColor::kGreen, "[%s] ",
      GetStringAlignedTo("PASSED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
                         ALIGN_CENTER)
          .c_str());
  std::printf("%lu test.\n",
              GetTestSuiteAndTestNumber().second - GetFailedTestCount());

  if (GetFailedTestCount() != 0)
    PrettyUnitTestResultPrinter::PrintFailedTests();

  std::fflush(stdout);
}

// Should be called for printing the status of the global environment set-up.
// (Global environment set-up in `xtest` is a myth, this function just prints
// a line and flushes the output stream).  This line is needed to give our
// users a complete feel of `googletest`.
void PrettyUnitTestResultPrinter::OnEnvironmentsSetUpStart() {
  internal::ColoredPrintf(internal::XTestColor::kGreen, "[%s] ",
                          GetStrFilledWith('-').c_str());
  std::printf("Global test environment set-up.");
  std::fflush(stdout);
}

// Should be called for printing the status of the global environment
// tear-down. (Global environment tear-down in `xtest` is a myth, this
// function just prints a line and flushes the output stream).  This line is
// needed to give our users a complete feel of `googletest`.
void PrettyUnitTestResultPrinter::OnEnvironmentsTearDownStart() {
  std::printf("\n");
  internal::ColoredPrintf(internal::XTestColor::kGreen, "[%s] ",
                          GetStrFilledWith('-').c_str());
  std::printf("Global test environment tear-down.\n");
  std::fflush(stdout);
}

// Runs the registered test suite.
//
// This function runs the registered test suite in the
// `xtest::XTestRegistryInstance.M_testRegistryTable` instance while also
// handling the abort signals raised by `ASSERT_*` assertions.
//
// In case an assertion fails then this function marks that test suite as
// `FAILED` while silently continuing executing rest of the test suites.
static void RunRegisteredTestSuite(const std::vector<TestRegistrar*>& tests) {
  void (*SavedSignalHandler)(int);
  SavedSignalHandler = std::signal(SIGABRT, impl::SignalHandler);
  for (TestRegistrar* const& test : tests) {
    if (test->M_testFunc == nullptr)
      continue;
    internal::Timer timer;
    // We are setting a jump here to later mark the test result as `FAILED` in
    // case the `test->M_testFunc` raised an abort signal result of an
    // `ASSERT_*` assertion.  This step is reduntant but it is done to make
    // readers understand that the `abort` signal will be caught here and the
    // test suite will be exited.
    if (setjmp(XTestRegistryInstance.M_jumpOutOfTest)) {
      test->M_testResult = TestResult::FAILED;
    } else {
      test->M_testFunc(test);
      if (test->M_testResult == TestResult::UNKNOWN)
        test->M_testResult = TestResult::PASSED;
    }
    test->M_elapsedTime = timer.Elapsed();
  }
}

// Runs all the registered test suites and returns the failure count.
//
// This function runs all the registered test suites in the
// `xtest::XTestRegistryInstance.M_testRegistryTable` instance while also
// handling the abort signals raised by `ASSERT_*` assertions.
uint64_t RunRegisteredTests() {
  PrettyUnitTestResultPrinter::OnTestExecutionStart();
  for (const XTestUnitTestPair& testSuite :
       XTestRegistryInstance.M_testRegistryTable) {
    PrettyUnitTestResultPrinter::OnTestStart(testSuite);
    RunRegisteredTestSuite(testSuite.second);
    PrettyUnitTestResultPrinter::OnTestEnd(testSuite);
  }
  PrettyUnitTestResultPrinter::OnTestExecutionEnd();
  return XTEST_GLOBAL_INSTANCE_GET_(failure_count);
}

// Checks if the string given is preffixed by valid characters or not.
//
// This function is maily used to validate command line flags given to the
// main executable.
static uint8_t ValidFlagPreffixLength(const char* flag) {
  if (std::strncmp(flag, "--", 2) == 0)
    return 2;
  if (std::strncmp(flag, "-", 1) == 0)
    return 1;
  return 0;
}

// Returns the value of the xtest command line flag.
//
// Parses the value after '=' character over the command line.  If
// `defOptional` is given then the flag value is treated as a boolen true and
// returend.
static const std::string ParseFlagValue(const char* const flag,
                                        const char* flagName,
                                        bool defOptional) {
  if (flag == nullptr || flagName == nullptr)
    return std::string();

  uint8_t preffixLen = ValidFlagPreffixLength(flag);
  if (preffixLen == 0)
    return std::string();

  const char* flagEnd = flag + (preffixLen + std::strlen(flagName) + 1);
  if (defOptional && *flagEnd == '\0')
    return "true";

  if (*flagEnd != '=')
    return std::string();

  return ++flagEnd;
}

// Parses the given flag i.e., `flagName` from the command line argument
// string i.e., `flag`.
//
// Parses the given `flagName` from the string `flag` and sets the value in
// the address `value`.
//
// Note: This function is just an overload for the boolean type command line
// flags like `--help`, `--debug`, etc in this case the address that the
// `value` is holding will have a bool value set at the end of parsing.
static bool ParseFlag(const char* const flag, const char* const flagName,
                      bool* value) {
  const std::string valueStr = ParseFlagValue(flag, flagName, true);
  const char* valueCStr = valueStr.c_str();
  if (valueCStr == nullptr)
    return false;
  *value = std::strcmp(valueCStr, "true") == 0;
  return true;
}

// Parses a single xtest command line flag at a time.
//
// This function parses value of a command line flag and sets the value of the
// global variable that represents that flag with the required type.
static void ParseXTestFlag(const char* const flag) {
#define XTEST_INTERNAL_PARSE_FLAG(flagName)   \
  do {                                        \
    auto value = XTEST_FLAG_GET_(flagName);   \
    if (ParseFlag(flag, #flagName, &value)) { \
      XTEST_FLAG_SET_(flagName, value);       \
    }                                         \
  } while (false)

  XTEST_INTERNAL_PARSE_FLAG(help);
  XTEST_INTERNAL_PARSE_FLAG(shuffle);
}

// Parses all the xtest command line flags.
//
// This function should be called only at the initialization step of `xtest`
// library.
void ParseXTestFlags(int32_t* argc, char** argv) {
  for (int32_t i = 1; i < *argc; ++i) {
    const std::string argString = internal::StreamableToString(argv[i]);
    ParseXTestFlag(argString.c_str());
  }
}

// Invokes functions corresponding to the command line flags given.
//
// This function should be called after the `ParseXTestFlags()` function.
void PostFlagParsing() {
  if (XTEST_FLAG_GET_(help)) {
    internal::PrintColorEncoded(kColorEncodedHelpMessage);
    std::exit(EXIT_SUCCESS);
  }
}

// Initializes xtest.  This must be called before calling `RUN_ALL_TESTS()`.
//
// In particular, it parses a command line for the flags that xtest
// recognizes. Whenever a Google Test flag is seen, it is removed from argv,
// and *argc is decremented.  Calling the function for the second time has no
// user-visible effect.
void InitXTest(int32_t* argc, char** argv) {
  if (XTestIsInitialized())
    return;

  if (*argc <= 0)
    return;

  G_argvs.clear();
  for (int32_t i = 0; i < *argc; ++i)
    G_argvs.push_back(internal::StreamableToString(argv[i]));

  ParseXTestFlags(argc, argv);
  PostFlagParsing();
}
}  // namespace xtest
