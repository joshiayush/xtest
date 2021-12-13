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

#include "registrar.hh"

#include <csetjmp>
#include <csignal>
#include <iostream>

namespace xtest {
// TestRegistry instance that links nodes of different test suites.
TestRegistry GTestRegistryInst = {0};

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
void SignalHandler(int param) {
  std::longjmp(GTestRegistryInst.M_jumpOutOfTest, 1);
}
}  // namespace impl

// @brief Converts a TestResult instance to its string representation form.
//
// This function takes in a TestResult instance and returns a string that is
// equal either of: UNKNOWN PASSED FAILED
//
// @param result TestResult instance to convert to its string representation
// form.
// @return const char* to the string representing test result.
const char* GetTestResultStr(TestResult result) {
  switch (result) {
    case TestResult::UNKNOWN:
      return "UNKNOWN";
    case TestResult::PASSED:
      return "PASSED";
    case TestResult::FAILED:
      return "FAILED";
  }
}

// @brief Construct a new TestRegistrar object.
//
// This constructor appends a new entry of test suite to GTestRegistryInst list.
// We link the comming test suite to the TestRegistrar* type variable
// M_head linked list declared inside of the struct TestRegistry to
// later traverse through this list to execute each test suite.
//
// @param[in] suiteName Test suite name.
// @param[in] testName Test name.
// @param[in] testFunc Test function to execute.
TestRegistrar::TestRegistrar(const char* suiteName, const char* testName,
                             TestFunction testFunc)
    : M_suiteName{suiteName},
      M_testFunc{testFunc},
      M_testName{testName},
      M_nextTestSuite{nullptr},
      M_testResult{TestResult::UNKNOWN} {
  TestRegistrar** link = &GTestRegistryInst.M_head;
  while (*link)
    link = &((*link)->M_nextTestSuite);
  *link = this;
}

// @brief Destroys a TestRegistrar object.
//
// For now there is no certain code to destroy a TestRegistrar object.
//
// @note Not marking as virtual because not sure if needed a v-table for
// destructor.
TestRegistrar::~TestRegistrar() {
  /**
   * @brief No ideas for now.
   */
}

// @brief Prints out debugging information of the registered test suites.
//
// This function reads the TestRegistry::m_firt_test linked list and writes that
// information down to the given file stream.
//
// @param[in] file File to redirect the debugging message.
void _DebugListRegisteredTests(std::ostream& stream) {
  TestRegistrar* node = GTestRegistryInst.M_head;
  while (node) {
    stream << "test " << node->M_suiteName << '.' << node->M_testName << "-> "
           << node->M_testFunc << ": " << GetTestResultStr(node->M_testResult)
           << std::endl;
    node = node->M_nextTestSuite;
  }
}

// @brief Runs all the registered test suites.
//
// This function runs all the registered test suites in the
// xtest::GTestRegistryInst.M_head instance while also handling the abort
// signals raised by ASSERT_* assertions.
//
// In case an assertion fails then this function marks that test suite as
// FAILED while silently continuing executing rest of the test suites.
void RunRegisteredTests() {
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
}
}  // namespace xtest
