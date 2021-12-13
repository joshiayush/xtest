// Copyright (C) 2021  Ayush Joshi
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program. If not, see <https://www.gnu.org/licenses/>.

#ifndef _REGISTRAR_HH_
#define _REGISTRAR_HH_

#include <csetjmp>

// @brief Creates a test suite and register it using TestRegistrar.
//
// This macro expands to a function declaration and definition.
// Declaration for registering your test suite for automatic test execution and
// definition that contains your test suite body.
//
// Note: For the best usage of this macro avoid giving same suitName and
// testName to different test suites, otherwise compiler will complain
// regarding the multiple definitions and declarations of the same function.
//
// @param suitName Test suite name.
// @param testName Test name.
#define TEST(suitName, testName)                                               \
  void TESTFUNCTION__##suitName##testName(                                     \
      ::xtest::TestRegistry* testRegistry,                                     \
      ::xtest::TestRegistrar* currentTest);                                    \
  namespace {                                                                  \
  ::xtest::TestRegistrar TESTREGISTRAR__##suitName##testName(                  \
      #suitName, #testName, TESTFUNCTION__##suitName##testName);               \
  }                                                                            \
  void TESTFUNCTION__##suitName##testName(::xtest::TestRegistry* testRegistry, \
                                          ::xtest::TestRegistrar* currentTest)

namespace xtest {
namespace impl {
// @brief Calls std::longjmp() with M_jumpOutOfTest instance as its first
// argument.
//
// This function calls the std::longjmp() function with the std::jmp_buf
// instance M_jumpOutOfTest as its first argument when the SIGABRT is raised
// inside of the function run_registered_test() that runs the registered test
// suites.
//
// @todo Give a description to param.
//
// @param param Have no idea what this is for :D.
void SignalHandler(int param);
}  // namespace impl

struct TestRegistrar;
struct TestRegistry;

// @brief Pointer to a void function type.
//
// This typedef is required to pass the test suite to TestRegistrar constructor
// to register as a test suite entry for automatic test execution.
//
// This function also takes in a pointer to the TestRegistry structure which is
// a container for the pointer that points to the head of the test suites linked
// list and a pointer to the current test i.e., TestRegistrar instance.
//
// @param testRegistry Pointer to the structure containing test suites.
// @param currentTest The current test.
typedef void (*TestFunction)(TestRegistry* testRegistry,
                             TestRegistrar* currentTest);

enum class TestResult { UNKNOWN, PASSED, FAILED };

// @brief Converts a TestResult instance to its string representation form.
//
// This function takes in a TestResult instance and returns a string that is
// equal either of: UNKNOWN PASSED FAILED
//
// @param result TestResult instance to convert to its string representation
// form.
// @return const char* to the string representing test result.
const char* GetTestResultStr(TestResult result);

struct TestRegistrar {
 public:
  // @brief Construct a new TestRegistrar object.
  //
  // This constructor appends a new entry of test suite to testRegistry list.
  // We link the comming test suite to the TestRegistrar* type variable
  // M_head linked list declared inside of the struct TestRegistry to
  // later traverse through this list to execute each test suite.
  //
  // @param[in] suitName Test suite name.
  // @param[in] testName Test name.
  // @param[in] testFunc Test function to execute.
  TestRegistrar(const char* suiteName, const char* testName,
                TestFunction testFunc);

  // @brief Destroys a TestRegistrar object.
  //
  // For now there is no certain code to destroy a TestRegistrar object.
  //
  // @note Not marking as virtual because not sure if needed a v-table for
  // destructor.
  ~TestRegistrar();

 public:
  const char* M_testName;   // Test name
  const char* M_suiteName;  // Test suite name

  TestFunction M_testFunc;         // Test function to execute
  TestRegistrar* M_nextTestSuite;  // Pointer to the next TestRegistrar object
                                   // i.e., to the next test suite
  TestResult M_testResult;         // Result of the test suite
};

// @brief Defines a pointer that links nodes of different test suites.
//
// This structure contains a pointer to the test suites linked list to later
// traverse the entire linked list to execute each test suite.
//
// This structure also contains an instance of std::jmp_buf that stores the
// environment information for the function xtest::run_registered_tests().
struct TestRegistry {
 public:
  // Pointer to the head of the test suite linked list.
  TestRegistrar* M_head;

  // M_jumpOutOfTest instance stores the environment information for function
  // run_registered_tests() to later make a long jump using function
  // std::longjmp to register the test result as TestResult::FAILED.
  std::jmp_buf M_jumpOutOfTest;
};

// @brief Prints out debugging information of the registered test suites.
//
// This function reads the TestRegistry::m_firt_test linked list and writes that
// information down to the given file stream.
//
// @param[in] indent Indent to add before debugging message.
// @param[in] file File to redirect the debugging message.
void _DebugListRegisteredTests(std::ostream& stream);

// @brief Runs all the registered test suites.
//
// This function runs all the registered test suites in the
// xtest::testRegistry.M_head instance while also handling the abort
// signals raised by ASSERT_* assertions.
//
// In case an assertion fails then this function marks that test suite as
// FAILED while silently continuing executing rest of the test suites.
void RunRegisteredTests();

// TestRegistry instance that links nodes of different test suites.
extern TestRegistry GTestRegistryInst;
}  // namespace xtest

#endif