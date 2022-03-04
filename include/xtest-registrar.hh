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

#include <cstdint>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "internal/xtest-port.hh"

namespace xtest {
struct TestRegistrar;

using XTestUnitTest = std::map<const char*, std::vector<TestRegistrar*>>;
using XTestUnitTestPair = std::pair<const char*, std::vector<TestRegistrar*>>;

// Creates a test suite and register it using TestRegistrar.
//
// This macro expands to a function declaration and definition.
// Declaration for registering your test suite for automatic test execution and
// definition that contains your test suite body.
//
// For the best usage of this macro avoid giving same suiteName and testName to
// different test suites, otherwise compiler will complain regarding the
// multiple definitions and declarations of the same function.
#define TEST(suiteName, testName)                                              \
  void TESTFUNCTION__##suiteName##testName(xtest::TestRegistrar* currentTest); \
  namespace {                                                                  \
  xtest::TestRegistrar TESTREGISTRAR__##suiteName##testName(                   \
      #suiteName, #testName, TESTFUNCTION__##suiteName##testName);             \
  }                                                                            \
  void TESTFUNCTION__##suiteName##testName(xtest::TestRegistrar* currentTest)

typedef internal::TimeInMillis TimeInMillis;

// Pointer to a void function type.
//
// This typedef is required to pass the test suite to TestRegistrar constructor
// to register as a test suite entry for automatic test execution.
typedef void (*TestFunction)(TestRegistrar* currentTest);

enum class TestResult { UNKNOWN, PASSED, FAILED };

class TestRegistrar {
 public:
  // Constructs a new TestRegistrar instance.  Also links test functions from
  // similar test suites together.
  TestRegistrar(const char* suiteName, const char* testName,
                TestFunction testFunc);

 public:
  const char* M_testName;   // Test name.
  const char* M_suiteName;  // Test suite name.

  TestFunction M_testFunc;     // Test function to execute.
  TestResult M_testResult;     // Result of the test suite.
  TimeInMillis M_elapsedTime;  // Elapsed time in milliseconds.
};

// `XTestUnitTest` instance that links nodes of different test suites.
extern XTestUnitTest GTestRegistryTable;
}  // namespace xtest

#endif  // XTEST_INCLUDE_XTEST_REGISTRAR_HH_
