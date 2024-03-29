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
#include <list>
#include <sstream>
#include <string>
#include <utility>

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

// Prints the list of all tests with there suite names.
XTEST_FLAG_DEFINE_bool_(list_tests, false,
                        "List all tests without running them.");

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
XTEST_GLOBAL_DEFINE_uint64_(test_count, 0,
                            "Global counter for the number of tests run.");
XTEST_GLOBAL_DEFINE_uint64_(test_suite_count, 0,
                            "Global counter for the number of test suites.");
XTEST_GLOBAL_DEFINE_uint64_(failed_test_count, 0,
                            "Global counter for the number of failed tests.");

XTEST_STATIC_DECLARE_vector_(argvs, std::string,
                             "Copy of command line arguments passed to the "
                             "main executable.  Set by InitXTest().");

namespace xtest {
namespace impl {
// Calls std::longjmp() with jump_out_of_test_ instance as its first
// argument.
//
// This function calls the std::longjmp() function with the std::jmp_buf
// instance jump_out_of_test_ as its first argument when the SIGABRT is raised
// inside of the function run_registered_test() that runs the registered test
// suites.
void SignalHandler(int param) {
  std::longjmp(XTestRegistryInstance.jump_out_of_test_, 1);
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

// XTestIsInitialized() returns true if and only if the user has initialized
// xtest.  Useful for catching the user mistake of not initializing xtest
// before calling RUN_ALL_TESTS().
static bool XTestIsInitialized() {
  return XTEST_GLOBAL_INSTANCE_GET_(argvs).size() > 0;
}

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
// with a 'align_side' of 'ALIGN_CENTER'.
//
// If not given this function will use the default values for `new_str_width`
// and `align_side` i.e., `XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_` and
// `ALIGN_CENTER` respectively.
std::string GetStringAlignedTo(const std::string& str,
                               const std::size_t& new_str_width,
                               const StringAlignValues& align_side) {
  std::size_t strlen = str.length();
  if (strlen >= new_str_width)
    return str.c_str();

  std::stringstream sstream;
  std::size_t white_space_num = new_str_width - strlen;
  std::size_t suffix_len, prefix_len;

  switch (align_side) {
    case ALIGN_LEFT:
      suffix_len = (white_space_num / 5);
      prefix_len = white_space_num - suffix_len;
      break;
    case ALIGN_CENTER:
      suffix_len = white_space_num / 2;
      prefix_len = white_space_num - suffix_len;
      break;
    case ALIGN_RIGHT:
      prefix_len = (white_space_num / 5);
      suffix_len = white_space_num - prefix_len;
      break;
  }

  sstream << GetStrFilledWith(' ', suffix_len) << str
          << GetStrFilledWith(' ', prefix_len);
  return sstream.str();
}

// Constructs an empty Message.
//
// We allocate the stringstream separately because otherwise each use of
// ASSERT_* or EXPECT_* in a procedure adds over 200 bytes to the procedure's
// stack frame leading to huge the stack space.
Message::Message() : sstream_(new(std::nothrow) std::stringstream) {
  // By default, we want there to be enough precision when printing a double
  // to a Message.
  *sstream_ << std::setprecision(std::numeric_limits<double>::digits10 + 2);
}

// Gets the text streamed to this object so far as an std::string.  Each '\0'
// character in the buffer is replaced with "\\0".
//
// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
std::string Message::GetString() const {
  const std::string& str = sstream_->str();
  const char* const start = str.c_str();
  const char* const end = start + str.length();

  std::string result;
  result.reserve(static_cast<size_t>(2 * (end - start)));
  for (const char* ch = start; ch != end; ++ch)
    result += *ch == '\0' ? std::string("\\0") : std::string(sizeof(char), *ch);

  return result;
}

// Returns the total number of test functions.
std::uint64_t GetTestNumber() {
  if (XTEST_GLOBAL_INSTANCE_GET_(test_count) != 0)
    return XTEST_GLOBAL_INSTANCE_GET_(test_count);
  XTEST_GLOBAL_INSTANCE_GET_(test_count) = 0;
  for (const XTestUnitTestPair& test_suite :
       XTestRegistryInstance.test_registry_table_)
    XTEST_GLOBAL_INSTANCE_GET_(test_count) += test_suite.second.size();
  return XTEST_GLOBAL_INSTANCE_GET_(test_count);
}

// Returns the total number of test suites.
std::uint64_t GetTestSuiteNumber() {
  if (XTEST_GLOBAL_INSTANCE_GET_(test_suite_count) != 0)
    return XTEST_GLOBAL_INSTANCE_GET_(test_suite_count);
  XTEST_GLOBAL_INSTANCE_GET_(test_suite_count) = 0;
  for (const XTestUnitTestPair& test_suite :
       XTestRegistryInstance.test_registry_table_)
    ++XTEST_GLOBAL_INSTANCE_GET_(test_suite_count);
  return XTEST_GLOBAL_INSTANCE_GET_(test_suite_count);
}

// Returns the `XTestUnitTest` instance of failed tests.
//
// Iterates over the `XTestRegistryInstance.test_registry_table_` instance and
// adds the pair who's `test_result_` equals to `TestResult::FAILED` in the
// `failed_tests` container.
XTestUnitTest GetFailedTests() {
  XTestUnitTest failed_tests;
  for (const XTestUnitTestPair& testCase :
       XTestRegistryInstance.test_registry_table_) {
    for (const auto& test : testCase.second) {
      if (test->test_result_ != TestResult::FAILED)
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
  if (XTEST_GLOBAL_INSTANCE_GET_(failed_test_count) != 0)
    return XTEST_GLOBAL_INSTANCE_GET_(failed_test_count);
  XTestUnitTest failedTests = GetFailedTests();
  for (const XTestUnitTestPair& test_pair : failedTests)
    for (const TestRegistrar* const& test : test_pair.second)
      ++XTEST_GLOBAL_INSTANCE_GET_(failed_test_count);
  return XTEST_GLOBAL_INSTANCE_GET_(failed_test_count);
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
    const XTestUnitTestPair& test_suite) {
  std::printf("\n");
  internal::ColoredPrintf(internal::XTestColor::kGreen, "[%s] ",
                          GetStrFilledWith('-').c_str());
  std::printf("%lu tests from %s\n", test_suite.second.size(),
              test_suite.first);
  std::fflush(stdout);
}

// Prints out the information related to the number of tests a test suite
// shares.  This function is very much similar to
// `PrettyUnitTestResultPrinter::OnTestStart()` but should be ran after
// executing all the tests of a test suite.
void PrettyUnitTestResultPrinter::OnTestEnd(
    const XTestUnitTestPair& test_suite) {
  internal::ColoredPrintf(internal::XTestColor::kGreen, "[%s] ",
                          GetStrFilledWith('-').c_str());
  std::printf("%lu tests from %s", test_suite.second.size(), test_suite.first);
  TimeInMillis elapsedTime = 0;
  for (const TestRegistrar* const& test : test_suite.second)
    elapsedTime += test->elapsed_time_;
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
      PrettyUnitTestResultPrinter::PrintTestName(test->suite_name_,
                                                 test->test_name_);
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
  std::printf("Running %lu tests from %lu test suites.\n", GetTestNumber(),
              GetTestSuiteNumber());
  std::fflush(stdout);
}

// Prints number of test suites and tests ran.  Should only be called after
// iterating over all the registered test suites.
void PrettyUnitTestResultPrinter::OnTestIterationEnd() {
  internal::ColoredPrintf(internal::XTestColor::kGreen, "[%s] ",
                          GetStrFilledWith('=').c_str());
  std::printf("Ran %lu tests from %lu test suites.\n", GetTestNumber(),
              GetTestSuiteNumber());

  internal::ColoredPrintf(
      internal::XTestColor::kGreen, "[%s] ",
      GetStringAlignedTo("PASSED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
                         ALIGN_CENTER)
          .c_str());
  std::printf("%lu %s.\n", GetTestNumber() - GetFailedTestCount(),
              GetTestNumber() == 1 ? "test" : "tests");

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

// Lists the tests with their suite names on the console when called.
static void ListTestsWithSuiteName() {
  for (const XTestUnitTestPair& test_suite :
       XTestRegistryInstance.test_registry_table_) {
    bool printed_test_suite_name = false;
    for (const TestRegistrar* const& test : test_suite.second) {
      if (!printed_test_suite_name) {
        // We only print the suite name once.
        printed_test_suite_name = true;
        std::printf("%s.", test_suite.first);
        std::printf("\n");
      }
      std::printf("  %s", test->test_name_);
      std::printf("\n");
    }
  }
  std::fflush(stdout);
}

// Runs the registered test suite.
//
// This function runs the registered test suite in the
// `xtest::XTestRegistryInstance.test_registry_table_` instance while also
// handling the abort signals raised by `ASSERT_*` assertions.
//
// In case an assertion fails then this function marks that test suite as
// `FAILED` while silently continuing executing rest of the test suites.
static void RunRegisteredTestSuite(const std::list<TestRegistrar*>& tests) {
  void (*SavedSignalHandler)(int);
  SavedSignalHandler = std::signal(SIGABRT, impl::SignalHandler);
  for (TestRegistrar* const& test : tests) {
    if (test->test_func_ == nullptr)
      continue;
    internal::Timer timer;
    // We are setting a jump here to later mark the test result as `FAILED` in
    // case the `test->test_func_` raised an abort signal result of an
    // `ASSERT_*` assertion.  This step is reduntant but it is done to make
    // readers understand that the `abort` signal will be caught here and the
    // test suite will be exited.
    if (setjmp(XTestRegistryInstance.jump_out_of_test_)) {
      test->test_result_ = TestResult::FAILED;
    } else {
      test->test_func_(test);
      if (test->test_result_ == TestResult::UNKNOWN)
        test->test_result_ = TestResult::PASSED;
    }
    test->elapsed_time_ = timer.Elapsed();
  }
}

// Runs all the registered test suites and returns the failure count.
//
// This function runs all the registered test suites in the
// `xtest::XTestRegistryInstance.test_registry_table_` instance while also
// handling the abort signals raised by `ASSERT_*` assertions.
uint64_t RunRegisteredTests() {
  if (XTEST_FLAG_GET_(list_tests)) {
    ListTestsWithSuiteName();
    return XTEST_GLOBAL_INSTANCE_GET_(failure_count);
  }

  PrettyUnitTestResultPrinter::OnTestExecutionStart();
  for (const XTestUnitTestPair& test_suite :
       XTestRegistryInstance.test_registry_table_) {
    PrettyUnitTestResultPrinter::OnTestStart(test_suite);
    RunRegisteredTestSuite(test_suite.second);
    PrettyUnitTestResultPrinter::OnTestEnd(test_suite);
  }
  PrettyUnitTestResultPrinter::OnTestExecutionEnd();
  return XTEST_GLOBAL_INSTANCE_GET_(failure_count);
}

// Checks if the string given is prefixed by valid characters or not.
//
// This function is maily used to validate command line flags given to the
// main executable.
static uint8_t ValidFlagPrefixLength(const char* flag) {
  if (std::strncmp(flag, "--", 2) == 0)
    return 2;
  if (std::strncmp(flag, "-", 1) == 0)
    return 1;
  return 0;
}

// Returns the value of the xtest command line flag.
//
// Parses the value after '=' character over the command line.  If
// `def_optional` is given then the flag value is treated as a boolen true and
// returend.
static const std::string ParseFlagValue(const char* const flag,
                                        const char* flag_name,
                                        bool def_optional) {
  if (flag == nullptr || flag_name == nullptr)
    return std::string();

  // Find the number of '-' used as a prefix in the flag name.
  uint8_t prefix_len = ValidFlagPrefixLength(flag);
  if (prefix_len == 0)
    return std::string();

  // If the flag is not prefixed with '--' or '-' `XTEST_FLAG_PREFIX_` then it
  // is not a valid flag.
  const std::string flag_str_without_prefix =
      std::string(XTEST_FLAG_PREFIX_) + flag_name;
  const std::size_t flag_str_without_prefix_len =
      flag_str_without_prefix.size();

  // Skips the flag name.
  const char* flag_end = flag + (prefix_len + flag_str_without_prefix_len);

  // When def_optional is true, it's OK to not have a "=value" part.
  if (def_optional && *flag_end == '\0')
    return "true";

  // If `def_optional` is true and there are more characters after the flag
  // name, or if `def_optional` is false, there must be a '=' after the flag
  // name.
  if (*flag_end != '=')
    return std::string();

  return ++flag_end;
}

// Parses the given flag i.e., `flag_name` from the command line argument
// string i.e., `flag`.
//
// Parses the given `flag_name` from the string `flag` and sets the value in
// the address `value`.
//
// Note: This function is just an overload for the boolean type command line
// flags like `--help`, `--debug`, etc in this case the address that the
// `value` is holding will have a bool value set at the end of parsing.
static bool ParseFlag(const char* const flag, const char* const flag_name,
                      bool* value) {
  // Gets the value of the flag as a string.
  const std::string value_str = ParseFlagValue(flag, flag_name, true);
  const char* value_cstr = value_str.c_str();

  // Aborts if the parsing failed.
  if (*value_cstr == '\0')
    return false;

  // Sets *value to the value of the flag.
  return (*value = (std::strcmp(value_cstr, "true") == 0));
}

// Parses a string for a string flag, in the form of "--flag=value".
//
// On success, stores the value of the flag in *value, and returns true.  On
// failure, returns false without changing *value.
template <typename String>
static bool ParseFlag(const char* flag, const char* flag_name, String* value) {
  // Gets the value of the flag as a string.
  const std::string value_str = ParseFlagValue(flag, flag_name, false);
  const char* const value_cstr = value_str.c_str();

  // Aborts if the parsing failed.
  if (*value_cstr == '\0')
    return false;

  // Sets *value to the value of the flag.
  *value = value_cstr;
  return true;
}

static const char kColorEncodedHelpMessage[] =
    "This program contains tests written using xtest.  You can use the\n"
    "following command line flags to control its behaviour:\n"
    "\n"
    "Test Selection:\n"
    "  @G--" XTEST_FLAG_PREFIX_
    "list_tests@D\n"
    "     List the names of all tests instead of running them. The name\n"
    "     of TEST(Foo, Bar) is \"Foo.Bar\".\n"
    "\n"
    "Test Execution:\n"
    "   @G--" XTEST_FLAG_PREFIX_
    "shuffle@D\n"
    "     Randomize tests' order. (In development)\n"
    "\n"
    "Test Output:\n"
    "  @G--" XTEST_FLAG_PREFIX_
    "color=@Y(@Gyes@Y|@Gno@Y|@Gauto@Y)@D\n"
    "      Enable/disable colored output. The default is @Gauto@D.\n"
    "\n"
    "Others:\n"
    "   @G--" XTEST_FLAG_PREFIX_
    "help@D\n"
    "      Print this message.\n";

// Parses a single xtest command line flag at a time.
//
// This function parses value of a command line flag and sets the value of the
// global variable that represents that flag with the required type.
static void ParseXTestFlag(const char* const flag) {
#define XTEST_INTERNAL_PARSE_FLAG(flag_name)   \
  do {                                         \
    auto value = XTEST_FLAG_GET_(flag_name);   \
    if (ParseFlag(flag, #flag_name, &value)) { \
      XTEST_FLAG_SET_(flag_name, value);       \
      return;                                  \
    }                                          \
  } while (false)

  // Order is important to fix issue #3
  // (https://github.com/joshiayush/xtest/issues/3).
  XTEST_INTERNAL_PARSE_FLAG(help);
  XTEST_INTERNAL_PARSE_FLAG(color);
  XTEST_INTERNAL_PARSE_FLAG(list_tests);
  XTEST_INTERNAL_PARSE_FLAG(shuffle);
#undef XTEST_INTERNAL_PARSE_FLAG
}

// Parses all the xtest command line flags.
//
// This function should be called only at the initialization step of `xtest`
// library.
void ParseXTestFlags(int32_t* argc, char** argv) {
  for (int32_t i = 1; i < *argc; ++i) {
    const std::string arg_string = internal::StreamableToString(argv[i]);
    ParseXTestFlag(arg_string.c_str());
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
// recognizes. Whenever a Xtest Test flag is seen, it is removed from argv,
// and *argc is decremented.  Calling the function for the second time has no
// user-visible effect.
void InitXTest(int32_t* argc, char** argv) {
  if (XTestIsInitialized())
    return;

  if (*argc <= 0)
    return;

  XTEST_GLOBAL_INSTANCE_GET_(argvs).clear();
  for (int32_t i = 0; i < *argc; ++i)
    XTEST_GLOBAL_INSTANCE_GET_(argvs).push_back(
        internal::StreamableToString(argv[i]));

  ParseXTestFlags(argc, argv);
  PostFlagParsing();
}
}  // namespace xtest
