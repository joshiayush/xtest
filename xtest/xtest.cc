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
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

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

// Filled with true if user provides '--help' flag over the command line.
bool FLAG_xtest_help = false;
bool FLAG_xtest_shuffle = false;

#define XTEST_FLAG_GET(flagName) FLAG_xtest_##flagName
#define XTEST_FLAG_SET(flagName, value) (void)(XTEST_FLAG_GET(flagName) = value)

// Global counter for non-fatal test failures.
//
// This global counter is defined in the object file xtest.cc and incremented
// every time a non-fatal test assertion fails.
uint64_t G_n_testFailures = 0;

static const char HelpMessage[] =
    "This program contains tests written using xtest. You can use the\n"
    "following command line flags to control its behaviour:\n"
    "\n"
    "Test Execution:\n"
    "   --shuffle\n"
    "     Randomize tests' orders.\n";

// A copy of all command line arguments.  Set by InitXTest().
static std::vector<std::string> G_argvs;

// XTestIsInitialized() returns true if and only if the user has initialized
// xtest.  Useful for catching the user mistake of not initializing xtest before
// calling RUN_ALL_TESTS().
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
// In order to align the letter `FAILED` to the center we use this function with
// a 'alignSide' of 'ALIGN_CENTER'.
//
// If not given this function will use the default values for `newStrWidth` and
// `alignSide` i.e., `XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_` and
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
// stack frame leading to huge stack frames in some cases; gcc does not reuse
// the stack space.
Message::Message() : _M_sstream(new std::stringstream) {
  // By default, we want there to be enough precision when printing a double to
  // a Message.
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
  for (const char* ch = start; ch != end; ++ch) {
    if (*ch == '\0')
      result += "\\0";
    else
      result += *ch;
  }

  return result;
}

// Returns the total number of test cases and tests registered.
//
// The returned pair has the total number of test cases as its first element and
// the total number of tests as its second element.
static std::pair<std::uint64_t, std::uint64_t> GetTestCaseAndTestsNumber() {
  std::uint64_t testCaseNum = 0;
  std::uint64_t testsNum = 0;

  for (const auto& testCase : GTestRegistryInst.M_testRegistryTable) {
    ++testCaseNum;
    testsNum += testCase.second.size();
  }

  return std::pair<std::uint64_t, std::uint64_t>{testCaseNum, testsNum};
}

// Global test environment setup step.
//
// As the project's README clearly says that this project is inspired by
// googletest so I'm still trying to understand what google does in this step so
// I can also implement it here.
static void GlobalTestEnvSetup() {
  std::pair<std::uint64_t, std::uint64_t> testCaseAndTestNums =
      GetTestCaseAndTestsNumber();

  MESSAGE() << "[" << GetStrFilledWith('=') << "] Running "
            << testCaseAndTestNums.second << " tests from "
            << testCaseAndTestNums.first << " test case." << std::endl;
  MESSAGE() << "[" << GetStrFilledWith('-')
            << "] Global test environment setup." << std::endl;
}

// Global test environment tear down.
//
// Displays how many tests got passed and how many failed.
// Again I'm still trying to understand what else google does in this step so
// I can also implement it here.
static void GlobalTestEnvTearDown() {
  std::pair<std::uint64_t, std::uint64_t> testCaseAndTestNums =
      GetTestCaseAndTestsNumber();

  std::map<const char*, std::vector<TestRegistrar*>> failedTests;
  std::uint64_t failedTestsNum = 0;

  for (const auto& testCase : GTestRegistryInst.M_testRegistryTable) {
    for (const auto& test : testCase.second) {
      if (test->M_testResult != TestResult::FAILED)
        continue;
      failedTests[testCase.first].push_back(test);
      ++failedTestsNum;
    }
  }

  MESSAGE() << std::endl;
  MESSAGE() << "[" << GetStrFilledWith('-')
            << "] Global test environment tear-down." << std::endl;
  MESSAGE() << "[" << GetStrFilledWith('=') << "] "
            << testCaseAndTestNums.second << " tests from "
            << testCaseAndTestNums.first << " test case ran." << std::endl;
  MESSAGE() << "[" << GetStringAlignedTo("PASSED", 10, ALIGN_CENTER) << "] "
            << testCaseAndTestNums.second - failedTestsNum << " test."
            << std::endl;

  if (failedTestsNum == 0)
    return;

  MESSAGE() << "[" << GetStringAlignedTo("FAILED") << "] " << failedTestsNum
            << " test, listed below:" << std::endl;
  for (const auto& testCase : failedTests) {
    for (const auto& test : testCase.second) {
      MESSAGE() << "[" << GetStringAlignedTo("FAILED") << "] "
                << test->M_suiteName << "." << test->M_testName << std::endl;
    }
  }

  MESSAGE() << std::endl;
  if (failedTestsNum == 1)
    MESSAGE() << failedTestsNum << " FAILED TEST" << std::endl;
  else
    MESSAGE() << failedTestsNum << " FAILED TESTS" << std::endl;
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
  void (*SavedSignalHandler)(int);
  SavedSignalHandler = std::signal(SIGABRT, impl::SignalHandler);

  GlobalTestEnvSetup();
  for (auto& testSuite : GTestRegistryInst.M_testRegistryTable) {
    MESSAGE() << "[" << GetStrFilledWith('-') << "] " << testSuite.second.size()
              << " tests from " << testSuite.first << std::endl;
    for (auto& testCase : testSuite.second) {
      if (testCase->M_testFunc) {
        // We are setting a jump here to later mark the test result as FAILED in
        // case the testCase->M_testFunc raised an abort signal result of an
        // ASSERT_* assertion.
        if (setjmp(GTestRegistryInst.M_jumpOutOfTest)) {
          testCase->M_testResult = TestResult::FAILED;
        } else {
          testCase->M_testFunc(&GTestRegistryInst, testCase);
          if (testCase->M_testResult == TestResult::UNKNOWN)
            testCase->M_testResult = TestResult::PASSED;
        }
      }
    }
    MESSAGE() << "[" << GetStrFilledWith('-') << "] " << testSuite.second.size()
              << " tests from " << testSuite.first << std::endl
              << std::endl;
  }

  GlobalTestEnvTearDown();
  std::signal(SIGABRT, SavedSignalHandler);
  return G_n_testFailures;
}

// Checks if the string given is preffixed by valid characters or not.
//
// This function is maily used to validate command line flags given to the main
// executable.
static uint8_t ValidFlagPreffixLength(const char* flag) {
  if (std::strncmp(flag, "--", 2) == 0)
    return 2;
  if (std::strncmp(flag, "-", 1) == 0)
    return 1;
  return 0;
}

// Returns the value of the xtest command line flag.
//
// Parses the value after '=' character over the command line.  If `defOptional`
// is given then the flag value is treated as a boolen true and returend.
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

// Parses the given flag i.e., `flagName` from the command line argument string
// i.e., `flag`.
//
// Parses the given `flagName` from the string `flag` and sets the value in the
// address `value`.
//
// Note: This function is just an overload for the boolean type command line
// flags like `--help`, `--debug`, etc in this case the address that the `value`
// is holding will have a bool value set at the end of parsing.
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
    auto value = XTEST_FLAG_GET(flagName);    \
    if (ParseFlag(flag, #flagName, &value)) { \
      XTEST_FLAG_SET(flagName, value);        \
    }                                         \
  } while (false)

  XTEST_INTERNAL_PARSE_FLAG(help);
  XTEST_INTERNAL_PARSE_FLAG(shuffle);
}

// Parses all the xtest command line flags.
//
// Note: This function should be called only at the initialization step of
// 'xtest' library.
void ParseXTestFlags(int32_t* argc, char** argv) {
  for (int32_t i = 1; i < *argc; ++i) {
    const std::string argString = internal::StreamableToString(argv[i]);
    ParseXTestFlag(argString.c_str());
  }
}

// Invokes functions corresponding to the command line flags given.
//
// Note: This function should be called after the 'ParseXTestFlags()' function.
void PostFlagParsing() {
  if (XTEST_FLAG_GET(help)) {
    MESSAGE() << HelpMessage;
    std::exit(EXIT_SUCCESS);
  }

  if (XTEST_FLAG_GET(shuffle)) {
    MESSAGE() << "Shuffling is not available for now, be kind and make a PR.\n";
    std::exit(EXIT_SUCCESS);
  }
}

// Initializes xtest.  This must be called before calling RUN_ALL_TESTS().  In
// particular, it parses a command line for the flags that xtest recognizes.
// Whenever a Google Test flag is seen, it is removed from argv, and *argc is
// decremented.
//
// No value is returned.  Instead, the xtest flag variables are updated.
//
// Calling the function for the second time has no user-visible effect.
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
