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

#ifndef XTEST_INCLUDE_XTEST_HH_
#define XTEST_INCLUDE_XTEST_HH_

#include <cinttypes>
#include <string>

#include "xtest-assertions.hh"
#include "xtest-registrar.hh"

namespace xtest {
namespace impl {
// Calls std::longjmp() with M_jumpOutOfTest instance as its first
// argument.
//
// This function calls the std::longjmp() function with the std::jmp_buf
// instance M_jumpOutOfTest as its first argument when the SIGABRT is raised
// inside of the function run_registered_test() that runs the registered test
// suites.
void SignalHandler(int param);
}  // namespace impl

// Global counter for non-fatal test failures.
//
// This global counter is defined in the object file xtest.cc and incremented
// every time a non-fatal test assertion fails.
extern uint64_t G_n_testFailures;

// New string width for the aligned string returned by the function
// `GetStringAlignedTo()`.
#define XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_ 10

// Returns a string of length `width` all filled with the character `chr`.
//
// This function is mainly used to decorate the box used in the test summary
// like show below where we are filling the boxes with either `-` or `=`
// character,
//
// ```shell
// [‑‑‑‑‑‑‑‑‑‑] Global test environment tear‑down
// [==========] 2 tests from 1 test case ran. (10 ms total)
// [  PASSED  ] 1 test.
// [  FAILED  ] 1 test, listed below:
// [  FAILED  ] SquareRootTest.PositiveNos
//
//  1 FAILED TEST
// ```
std::string GetStrFilledWith(
    const char& chr,
    ::std::size_t width = XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_);

enum StringAlignValues { ALIGN_RIGHT, ALIGN_LEFT, ALIGN_CENTER };

// Align the original string to the given alignment inside of the new sized
// buffer.
//
// This function is mainly used to print out nice visual representation of the
// test results like the following,
//
// ```shell
// [  FAILED  ] SquareRootTest.PositiveNos
// ```
//
// In order to align the letter `FAILED` to the center we use this function with
// a 'alignSide' of 'ALIGN_CENTER'.
//
// If not given this function will use the default values for `newStrWidth` and
// `alignSide` i.e., `XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_` and
// `ALIGN_CENTER` respectively.
std::string GetStringAlignedTo(
    const ::std::string& str,
    const ::std::size_t& newStrWidth = XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
    const StringAlignValues& alignSide = ALIGN_CENTER);

// Summarize test results.
//
// This function produces a nice colored output on the screen like the
// following starting from "Global test environment tear‑down",
//
// ```shell
// Running main() from user_main.cpp
// [==========] Running 2 tests from 1 test case.
// [‑‑‑‑‑‑‑‑‑‑] Global test environment set‑up.
// [‑‑‑‑‑‑‑‑‑‑] 2 tests from SquareRootTest
// [ RUN      ] SquareRootTest.PositiveNos
// /C/project/test/user_sqrt.cpp(6862): error: Value of: sqrt (2533.310224)
//   Actual: 50.332
// Expected: 50.3321
// [  FAILED  ] SquareRootTest.PositiveNos (9 ms)
// [ RUN      ] SquareRootTest.ZeroAndNegativeNos
// [       OK ] SquareRootTest.ZeroAndNegativeNos (0 ms)
// [‑‑‑‑‑‑‑‑‑‑] 2 tests from SquareRootTest (0 ms total)
//
// [‑‑‑‑‑‑‑‑‑‑] Global test environment tear‑down
// [==========] 2 tests from 1 test case ran. (10 ms total)
// [  PASSED  ] 1 test.
// [  FAILED  ] 1 test, listed below:
// [  FAILED  ] SquareRootTest.PositiveNos
//
//  1 FAILED TEST
// ```
void SummarizeTestResults();

// Runs all the registered test suites and returns the failure count.
//
// This function runs all the registered test suites in the
// xtest::GTestRegistryInst.M_head instance while also handling the abort
// signals raised by ASSERT_* assertions.
//
// In case an assertion fails then this function marks that test suite as
// FAILED while silently continuing executing rest of the test suites.
uint64_t RunRegisteredTests();

#define RUN_ALL_TESTS() ::xtest::RunRegisteredTests()

void InitXtest();
}  // namespace xtest

#endif  // XTEST_INCLUDE_XTEST_HH_
