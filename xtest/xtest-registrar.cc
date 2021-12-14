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

#include "xtest-registrar.hh"

#include <csetjmp>
#include <csignal>
#include <iostream>

namespace xtest {
// TestRegistry instance that links nodes of different test suites.
TestRegistry GTestRegistryInst = {0};

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

// Converts a TestResult instance to its string representation form.
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

// Construct a new TestRegistrar object.
//
// This constructor appends a new entry of test suite to GTestRegistryInst list.
// We link the comming test suite to the TestRegistrar* type variable
// M_head linked list declared inside of the struct TestRegistry to
// later traverse through this list to execute each test suite.
//
// @param suiteName Test suite name.
// @param testName Test name.
// @param testFunc Test function to execute.
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

// Prints out debugging information of the registered test suites.
//
// This function reads the TestRegistry::m_firt_test linked list and writes that
// information down to the given file stream.
//
// @param file File to redirect the debugging message.
void _DebugListRegisteredTests(std::ostream& stream) {
  TestRegistrar* node = GTestRegistryInst.M_head;
  while (node) {
    stream << "test " << node->M_suiteName << '.' << node->M_testName << "-> "
           << node->M_testFunc << ": " << GetTestResultStr(node->M_testResult)
           << std::endl;
    node = node->M_nextTestSuite;
  }
}

// Runs all the registered test suites.
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
