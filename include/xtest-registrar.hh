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
#include <list>
#include <map>
#include <utility>

#include "internal/xtest-internal.hh"
#include "internal/xtest-port.hh"

namespace xtest {
struct TestRegistrar;

using XTestUnitTest = std::map<const char*, std::list<TestRegistrar*>>;
using XTestUnitTestPair = std::pair<const char*, std::list<TestRegistrar*>>;

// Creates a test suite and register it using TestRegistrar.
//
// This macro expands to a function declaration and definition.
// Declaration for registering your test suite for automatic test execution and
// definition that contains your test suite body.
//
// For the best usage of this macro avoid giving same suite_name and test_name
// to different test suites, otherwise compiler will complain regarding the
// multiple definitions and declarations of the same function.
#define TEST(suite_name, test_name)                                    \
  static_assert(sizeof(XTEST_STRINGIFY_(suite_name)) > 1,              \
                "suite_name must not be empty!");                      \
  static_assert(sizeof(XTEST_STRINGIFY_(test_name)) > 1,               \
                "test_name must not be empty!");                       \
  void TESTFUNCTION__##suite_name##test_name(                          \
      xtest::TestRegistrar* current_test);                             \
  namespace {                                                          \
  xtest::TestRegistrar TESTREGISTRAR__##suite_name##test_name(         \
      #suite_name, #test_name, TESTFUNCTION__##suite_name##test_name); \
  }                                                                    \
  void TESTFUNCTION__##suite_name##test_name(xtest::TestRegistrar* current_test)

typedef internal::TimeInMillis TimeInMillis;

// Pointer to a void function type.
//
// This typedef is required to pass the test suite to TestRegistrar constructor
// to register as a test suite entry for automatic test execution.
typedef void (*TestFunction)(TestRegistrar* current_test);

enum class TestResult { UNKNOWN, PASSED, FAILED };

class TestRegistrar {
 public:
  // Constructs a new TestRegistrar instance.  Also links test functions from
  // similar test suites together.
  TestRegistrar(const char* suite_name, const char* test_name,
                TestFunction test_func);

 public:
  const char* test_name_;   // Test name.
  const char* suite_name_;  // Test suite name.

  TestFunction test_func_;     // Test function to execute.
  TestResult test_result_;     // Result of the test suite.
  TimeInMillis elapsed_time_;  // Elapsed time in milliseconds.
};

// Constructs a `map` object that links test suites to their test cases.
//
// This structure contains a `map` instance that links test suites with their
// test cases to traverse through all of the test cases and run them to later
// group the result of test cases with their test suites.
//
// This structure also contains an instance of `std::jmp_buf` that stores the
// environment information for the function `xtest::RunRegisteredTests()`.
struct TestRegistry {
 public:
  XTestUnitTest test_registry_table_;

  // jump_out_of_test_ instance stores the environment information for function
  // run_registered_tests() to later make a long jump using function
  // std::longjmp to register the test result as TestResult::FAILED.
  std::jmp_buf jump_out_of_test_;
};

// `XTestUnitTest` instance that links nodes of different test suites.
extern TestRegistry XTestRegistryInstance;
}  // namespace xtest

#endif  // XTEST_INCLUDE_XTEST_REGISTRAR_HH_
