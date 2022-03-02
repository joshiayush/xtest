// Copyright 2021, The xtest authors.
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
//     * Neither the name of The xtest authors. nor the names of its
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

#ifndef XTEST_TESTS_XTEST_ASSERTIONS_TEST_HH_
#define XTEST_TESTS_XTEST_ASSERTIONS_TEST_HH_

#include "xtest-assertions.hh"
#include "xtest.hh"

TEST(AssertionContextTest, ReturnsFileName) {
  const char* file = "tests/xtest-assertions-test.hh";
  xtest::internal::AssertionContext context(file, 0, nullptr);
  EXPECT_EQ(file, context.file());
}

TEST(AssertionContextTest, ReturnsFileNameAndLineNumber) {
  const char* file = "tests/xtest-assertions-test.hh";
  const uint64_t line = 42;
  xtest::internal::AssertionContext context(file, line, nullptr);
  EXPECT_EQ(file, context.file());
  EXPECT_EQ(line, context.line());
}

TEST(AssertionContextTest,
     ReturnsFileNameAndLineNumberAndPointerToCurrentTest) {
  const char* file = "tests/xtest-assertions-test.hh";
  const uint64_t line = 42;
  xtest::internal::AssertionContext context(
      file, line,
      currentTest);  // `currentTest` is in the scope of the test function.
  EXPECT_EQ(file, context.file());
  EXPECT_EQ(line, context.line());
  EXPECT_EQ(currentTest, context.current_test());
}

#endif  // XTEST_TESTS_XTEST_ASSERTIONS_TEST_HH_