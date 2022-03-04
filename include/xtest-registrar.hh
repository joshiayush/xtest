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

#ifndef XTEST_INCLUDE_XTEST_REGISTRAR_HH_
#define XTEST_INCLUDE_XTEST_REGISTRAR_HH_

#include <csetjmp>
#include <cstdint>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "internal/xtest-port.hh"

// Creates a test suite and register it using TestRegistrar.
//
// This macro expands to a function declaration and definition.
// Declaration for registering your test suite for automatic test execution and
// definition that contains your test suite body.
//
// Note: For the best usage of this macro avoid giving same suiteName and
// testName to different test suites, otherwise compiler will complain
// regarding the multiple definitions and declarations of the same function.
//
// Should we encapsulate `testRegistry` and `currentTest` into a test context
// struct?
#define TEST(suiteName, testName)                                              \
  void TESTFUNCTION__##suiteName##testName(xtest::TestRegistry* testRegistry,  \
                                           xtest::TestRegistrar* currentTest); \
  namespace {                                                                  \
  xtest::TestRegistrar TESTREGISTRAR__##suiteName##testName(                   \
      #suiteName, #testName, TESTFUNCTION__##suiteName##testName);             \
  }                                                                            \
  void TESTFUNCTION__##suiteName##testName(xtest::TestRegistry* testRegistry,  \
                                           xtest::TestRegistrar* currentTest)

namespace xtest {
struct TestRegistrar;
struct TestRegistry;

typedef internal::TimeInMillis TimeInMillis;

// Pointer to a void function type.
//
// This typedef is required to pass the test suite to TestRegistrar constructor
// to register as a test suite entry for automatic test execution.
//
// This function also takes in a pointer to the TestRegistry structure which is
// a container for the pointer that points to the head of the test suites linked
// list and a pointer to the current test i.e., TestRegistrar instance.
typedef void (*TestFunction)(TestRegistry* testRegistry,
                             TestRegistrar* currentTest);

enum class TestResult { UNKNOWN, PASSED, FAILED };

class TestRegistrar {
 public:
  // Construct a new TestRegistrar object.
  //
  // This constructor appends a new entry of test suite to testRegistry list.
  // We link the comming test suite to the TestRegistrar* type variable
  // M_head linked list declared inside of the struct TestRegistry to
  // later traverse through this list to execute each test suite.
  TestRegistrar(const char* suiteName, const char* testName,
                TestFunction testFunc);

  // Destroys a TestRegistrar object.
  //
  // For now there is no certain code to destroy a TestRegistrar object.
  //
  // Note: Not marking as virtual because not sure if needed a v-table for
  // destructor.
  ~TestRegistrar() {}

 public:
  const char* M_testName;   // Test name
  const char* M_suiteName;  // Test suite name

  TestFunction M_testFunc;     // Test function to execute.
  TestResult M_testResult;     // Result of the test suite.
  TimeInMillis M_elapsedTime;  // Elapsed time in milliseconds.
};

// Constructs a 'map' object that links test suites to their test cases.
//
// This structure contains a 'map' instance that links test suites with their
// test cases to traverse through all of the test cases and run them to later
// group the result of test cases with their test suites.
//
// This structure also contains an instance of std::jmp_buf that stores the
// environment information for the function xtest::run_registered_tests().
struct TestRegistry {
 public:
  std::map<const char*, std::vector<TestRegistrar*>> M_testRegistryTable;

  // M_jumpOutOfTest instance stores the environment information for function
  // run_registered_tests() to later make a long jump using function
  // std::longjmp to register the test result as TestResult::FAILED.
  std::jmp_buf M_jumpOutOfTest;
};

using UnitTest = std::map<const char*, std::vector<TestRegistrar*>>;
using UnitTestPair = std::pair<const char*, std::vector<TestRegistrar*>>;

// TestRegistry instance that links nodes of different test suites.
extern TestRegistry GTestRegistryInst;
}  // namespace xtest

#endif  // XTEST_INCLUDE_XTEST_REGISTRAR_HH_
