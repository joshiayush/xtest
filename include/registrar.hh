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
  void TESTFUNCTION__##suite_name##test_name(                          \
      ::xtest::TestRegistry* test_registry,                            \
      ::xtest::TestRegistrar* current_test);                           \
  namespace {                                                          \
  ::xtest::TestRegistrar TESTREGISTRAR__##suite_name##test_name(       \
      #suite_name, #test_name, TESTFUNCTION__##suite_name##test_name); \
  }                                                                    \
  void TESTFUNCTION__##suite_name##test_name(                          \
      ::xtest::TestRegistry* test_registry,                            \
      ::xtest::TestRegistrar* current_test)

namespace xtest {
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
void signal_handler(int param);
}  // namespace impl

struct TestRegistrar;
struct TestRegistry;

/**
 * @brief Pointer to a void function type.
 *
 * This typedef is required to pass the test suite to TestRegistrar constructor
 * to register as a test suite entry for automatic test execution.
 *
 * This function also takes in a pointer to the TestRegistry structure which is
 * a container for the pointer that points to the head of the test suites linked
 * list and a pointer to the current test i.e., TestRegistrar instance.
 *
 * @param test_registry Pointer to the structure containing test suites.
 * @param current_test The current test.
 */
typedef void (*TestFunction)(TestRegistry* test_registry,
                             TestRegistrar* current_test);

enum class TestResult { UNKNOWN, PASSED, FAILED };

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
const char* test_result_str(TestResult result);

struct TestRegistrar {
 public:
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

 public:
  const char* m_suite_name;  // Test suite name
  const char* m_test_name;   // Test name
  TestFunction m_test_fn;    // Test function to execute
  TestRegistrar* m_next;  // Pointer to the next TestRegistrar object i.e., to
                          // the next test suite
  TestResult m_result;    // Result of the test suite
};

/**
 * @brief Defines a pointer that links nodes of different test suites.
 *
 * This structure contains a pointer to the test suites linked list to later
 * traverse the entire linked list to execute each test suite.
 *
 * This structure also contains an instance of std::jmp_buf that stores the
 * environment information for the function xtest::run_registered_tests().
 */
struct TestRegistry {
 public:
  /* Pointer to the head of the test suite linked list. */
  TestRegistrar* m_first_test;

  /* m_jump_out_of_test instance stores the environment information for function
   * run_registered_tests() to later make a long jump using function
   * std::longjmp to register the test result as TestResult::FAILED.
   */
  std::jmp_buf m_jump_out_of_test;
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
void run_registered_tests();

/* TestRegistry instance that links nodes of different test suites. */
extern TestRegistry test_registry;
}  // namespace xtest

#endif