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

#include <cstdio>

namespace xtest {
/** @brief TestRegistry instance that links nodes of different test suites. */
TestRegistry test_registry = {0};

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
    : m_suite_name{suite_name},
      m_test_name{test_name},
      m_test_fn{test_fn},
      m_next{nullptr} {
  fprintf(stdout, "Test being registered: %s.%s %p\n", suite_name, test_name,
          test_fn);
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
  fprintf(stdout, "Test being un-registered: %s.%s %p\n", m_suite_name,
          m_test_name, m_test_fn);
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
void debug_list_registered_tests(const char* indent, FILE* file) {
  TestRegistrar* node = test_registry.m_first_test;
  while (node) {
    fprintf(file, "%stest %s.%s -> %p\n", indent, node->m_suite_name,
            node->m_test_name, node->m_test_fn);
    node = node->m_next;
  }
}
}  // namespace xtest