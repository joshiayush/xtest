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

#include <cstdio>

/**
 * @brief Creates a test suite and register it using TestRegistrar.
 *
 * This macro expands to a function declaration and definition.
 * Declaration for registering your test suite for automatic test execution and
 * definition that contains your test suite body.
 *
 * Note: For the best usage of this macro avoid giving same suite_name and
 * test_name to different test suites, otherwise compiler will complain
 * regarding the multiple definitions and declarations of the same function.
 *
 * @param suite_name Test suite name.
 * @param test_name Test name.
 *
 * @example
 * @code {.c++}
 * TEST(MyTestSuite, testingIfTrueEvaluatesToTrue) {
 *  EXPECT_TRUE(true);
 * }
 * @endcode
 */
#define TEST(suite_name, test_name)                                    \
  void TESTFUNCTION__##suite_name##test_name();                        \
  namespace {                                                          \
  ::xtest::TestRegistrar TESTREGISTRAR__##suite_name##test_name(       \
      #suite_name, #test_name, TESTFUNCTION__##suite_name##test_name); \
  }                                                                    \
  void TESTFUNCTION__##suite_name##test_name()

namespace xtest {
/**
 * @brief Pointer to a void function type that takes in no parameters.
 *
 * This typedef is required to pass the test suite to TestRegistrar constructor
 * to register as a test suite entry for automatic test execution.
 */
typedef void (*TestFunction)();

struct TestRegistrar {
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
  TestRegistrar(const char* suite_name, const char* test_name,
                TestFunction test_fn);

  /**
   * @brief Destroys a TestRegistrar object.
   *
   * For now there is no certain code to destroy a TestRegistrar object.
   *
   * @note Not marking as virtual because not sure if needed a v-table for
   * destructor.
   */
  ~TestRegistrar();

  /** @brief Test suite name. */
  const char* m_suite_name;
  /** @brief Test name. */
  const char* m_test_name;
  /** @brief Test function to execute. */
  TestFunction m_test_fn;
  /** @brief Pointer to the next test suite or TestRegistrar object. */
  TestRegistrar* m_next;
};

/**
 * @brief Defines a pointer that links nodes of different test suites.
 *
 * This structure contains a pointer to the test suites linked list to later
 * traverse the entire linked list to execute each test suite.
 */
struct TestRegistry {
  /** @brief Pointer to the head of the test suite linked list. */
  TestRegistrar* m_first_test;
};

/**
 * @brief Prints out debugging information of the registered test suites.
 *
 * This function reads the TestRegistry::m_firt_test linked list and writes that
 * information down to the given file stream.
 *
 * @param[in] indent Indent to add before debugging message.
 * @param[in] file File to redirect the debugging message.
 */
void debug_list_registered_tests(const char* indent, FILE* file);

/** @brief TestRegistry instance that links nodes of different test suites. */
extern TestRegistry test_registry;
}  // namespace xtest

#endif