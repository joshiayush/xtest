// Copyright 2022, The xtest authors Inc.
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
//     * Neither the name of The xtest authors Inc. nor the names of its
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

#ifndef XTEST_TESTS_XTEST_STRING_TEST_HH_
#define XTEST_TESTS_XTEST_STRING_TEST_HH_

#include "internal/xtest-string.hh"
#include "xtest.hh"

TEST(ReprTest, Empty) { EXPECT_EQ(xtest::String::Repr(""), ""); }

TEST(ReprTest, TestWithCommonControlCharacters) {
  EXPECT_EQ(xtest::String::Repr("\a"), "\\a");
  EXPECT_EQ(xtest::String::Repr("\b"), "\\b");
  EXPECT_EQ(xtest::String::Repr("\f"), "\\f");
  EXPECT_EQ(xtest::String::Repr("\n"), "\\n");
  EXPECT_EQ(xtest::String::Repr("\r"), "\\r");
  EXPECT_EQ(xtest::String::Repr("\t"), "\\t");
  EXPECT_EQ(xtest::String::Repr("\v"), "\\v");
  EXPECT_EQ(xtest::String::Repr("\\"), "\\\\");
  EXPECT_EQ(xtest::String::Repr("\'"), "\\'");
  EXPECT_EQ(xtest::String::Repr("\""), "\\\"");
}

TEST(ReprTest, TestWithNumbers) {
  EXPECT_EQ(xtest::String::Repr("\001"), "\\0x01");
  EXPECT_EQ(xtest::String::Repr("\x01"), "\\0x01");
}

#endif  // XTEST_TESTS_XTEST_STRING_TEST_HH_
