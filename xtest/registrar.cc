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
/* TestRegistry instance that links nodes of different test suites. */
TestRegistry test_registry = {0};

namespace impl {
/**
 * @brief Calls std::longjmp() with m_jump_out_of_test instance as its first
 * argument.
 *
 * This function calls the std::longjmp() function with the std::jmp_buf
 * instance m_jump_out_of_test as its first argument when the SIGABRT is raised
 * inside of the function run_registered_test() that runs the registered test
 * suites.
 *
 * @todo Give a description to param.
 *
 * @param param Have no idea what this is for :D.
 */
void signal_handler(int param) {
  std::longjmp(test_registry.m_jump_out_of_test, 1);
}
}  // namespace impl

/**
 * @brief Converts a TestResult instance to its string representation form.
 *
 * This function takes in a TestResult instance and returns a string that is
 * equal either of: UNKNOWN PASSED FAILED
 *
 * @param result TestResult instance to convert to its string representation
 * form.
 * @return const char* to the string representing test result.
 */
const char* test_result_str(TestResult result) {
  switch (result) {
    case TestResult::UNKNOWN:
      return "UNKNOWN";
    case TestResult::PASSED:
      return "PASSED";
    case TestResult::FAILED:
      return "FAILED";
  }
}

/**
 * @brief Construct a new TestRegistrar object.
 *
 * This constructor appends a new entry of test suite to test_registry list.
 * We link the comming test suite to the TestRegistrar* type variable
 * m_first_test linked list declared inside of the struct TestRegistry to
 * later traverse through this list to execute each test suite.
 *
 * @param[in] suite_name Test suite name.
 * @param[in] test_name Test name.
 * @param[in] test_fn Test function to execute.
 *
 * @example
 * @code {.c++}
 *  TestRegistrar TESTREGISTRAR__MyTestSuitetestingIfTrueEvaluatesToTrue(
 *    "MyTestSuite",
 *    "testingIfTrueEvaluatesToTrue",
 *    TESTFUNCTION__MyTestSuitetestingIfTrueEvaluatesToTrue);
 * @endcode
 */
TestRegistrar::TestRegistrar(const char* suite_name, const char* test_name,
                             TestFunction test_fn)
    : m_suite_name{suite_name}, m_test_name{test_name}, m_test_fn{test_fn},
      m_next{nullptr}, m_result{TestResult::UNKNOWN} {
  TestRegistrar** link = &test_registry.m_first_test;
  while (*link)
    link = &((*link)->m_next);
  *link = this;
}

/**
 * @brief Destroys a TestRegistrar object.
 *
 * For now there is no certain code to destroy a TestRegistrar object.
 *
 * @note Not marking as virtual because not sure if needed a v-table for
 * destructor.
 */
TestRegistrar::~TestRegistrar() {
  /**
   * @brief No ideas for now.
   */
}

/**
 * @brief Prints out debugging information of the registered test suites.
 *
 * This function reads the TestRegistry::m_firt_test linked list and writes that
 * information down to the given file stream.
 *
 * @param[in] indent Indent to add before debugging message.
 * @param[in] file File to redirect the debugging message.
 */
void debug_list_registered_tests(const char* indent, std::ostream& stream) {
  TestRegistrar* node = test_registry.m_first_test;
  while (node) {
    stream << indent << "test " << node->m_suite_name << '.'
           << node->m_test_name << "-> " << node->m_test_fn << ": "
           << test_result_str(node->m_result) << std::endl;
    node = node->m_next;
  }
}

/**
 * @brief Runs all the registered test suites.
 *
 * This function runs all the registered test suites in the
 * xtest::test_registry.m_first_test instance while also handling the abort
 * signals raised by ASSERT_* assertions.
 *
 * In case an assertion fails then this function marks that test suite as
 * FAILED while silently continuing executing rest of the test suites.
 */
void run_registered_tests() {
  TestRegistrar* node = test_registry.m_first_test;

  void (*saved_signal_handler)(int);
  saved_signal_handler = std::signal(SIGABRT, impl::signal_handler);

  while (node) {
    if (node->m_test_fn) {
      /* We are setting a jump here to later mark the test result as FAILED in
       * case the node->m_test_fn raised an abort signal result of an ASSERT_*
       * assertion.
       */
      if (setjmp(test_registry.m_jump_out_of_test)) {
        node->m_result = TestResult::FAILED;
      } else {
        node->m_test_fn(&test_registry, node);
        if (node->m_result == TestResult::UNKNOWN)
          node->m_result = TestResult::PASSED;
      }
    }
    node = node->m_next;
  }

  std::signal(SIGABRT, saved_signal_handler);
}
}  // namespace xtest