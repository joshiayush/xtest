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

#include <limits>

#include "factorial/factorial.hh"
#include "xtest.hh"

TEST(TestWithNegativeNumbers, TestFactorial) {
  EXPECT_EQ(Factorial(std::numeric_limits<int64_t>::lowest()), 1);
}

TEST(TestWithNegativePrimeNumbers, TestFactorial) {
  EXPECT_EQ(Factorial(-2), 1);
  EXPECT_EQ(Factorial(-5), 1);
  EXPECT_EQ(Factorial(-7), 1);
  EXPECT_EQ(Factorial(-13), 1);
}

TEST(TestWithNegativeEvenCompositeNumbers, TestFactorial) {
  EXPECT_EQ(Factorial(-4), 1);
  EXPECT_EQ(Factorial(-6), 1);
  EXPECT_EQ(Factorial(-8), 1);
  EXPECT_EQ(Factorial(-10), 1);
}

TEST(TestWithNegativeOddCompositeNumbers, TestFactorial) {
  EXPECT_EQ(Factorial(-4), 1);
  EXPECT_EQ(Factorial(-6), 1);
  EXPECT_EQ(Factorial(-8), 1);
  EXPECT_EQ(Factorial(-10), 1);
}

TEST(DoomedToBeFailedTestsPrimary, TestFactorial) {
  EXPECT_EQ(Factorial(2), 1);
  EXPECT_EQ(Factorial(5), 1);
  EXPECT_EQ(Factorial(7), 1);
  EXPECT_EQ(Factorial(13), 1);
  EXPECT_EQ(Factorial(-4), 2);
  EXPECT_EQ(Factorial(-6), 120);
  EXPECT_EQ(Factorial(-8), 5040);
  EXPECT_EQ(Factorial(-10), 6227020800);
}

TEST(DoomedToBeFailedTestsSecondary, TestFactorial) {
  EXPECT_EQ(Factorial(2), 21);
  EXPECT_EQ(Factorial(5), 21);
  EXPECT_EQ(Factorial(7), 21);
  EXPECT_EQ(Factorial(13), 21);
  EXPECT_EQ(Factorial(-4), 22);
  EXPECT_EQ(Factorial(-6), 2120);
  EXPECT_EQ(Factorial(-8), 25040);
  EXPECT_EQ(Factorial(-10), 26227020800);
}

TEST(TestWithPrimeNumbers, TestFactorial) {
  EXPECT_EQ(Factorial(2), 2);
  EXPECT_EQ(Factorial(5), 120);
  EXPECT_EQ(Factorial(7), 5040);
  EXPECT_EQ(Factorial(13), 6227020800);
}

TEST(TestWithEvenCompositeNumbers, TestFactorial) {
  EXPECT_EQ(Factorial(4), 24);
  EXPECT_EQ(Factorial(6), 720);
  EXPECT_EQ(Factorial(8), 40320);
  EXPECT_EQ(Factorial(10), 3628800);
}

TEST(TestWithOddCompositeNumbers, TestFactorial) {
  EXPECT_EQ(Factorial(9), 362880);
  EXPECT_EQ(Factorial(15), 1307674368000);
}

int32_t main(int32_t argc, char** argv) {
  xtest::InitXTest(&argc, argv);
  return RUN_ALL_TESTS();
}
