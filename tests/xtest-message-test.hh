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

#ifndef XTEST_TESTS_XTEST_MESSAGE_TEST_HH_
#define XTEST_TESTS_XTEST_MESSAGE_TEST_HH_

#include "xtest-message.hh"
#include "xtest.hh"

TEST(MessageTest, WhenConstructedAEmptyMessage) {
  xtest::Message msg;
  EXPECT_EQ("", msg.GetString());
  msg << "foo";
  EXPECT_EQ("foo", msg.GetString());
}

TEST(MessageTest, TestFloatingPointPrecisionWhenConstructedAEmptyMessage) {
  xtest::Message msg;
  msg << 1.0000000000000004;
  EXPECT_EQ("1.0000000000000004", msg.GetString());
}

TEST(MessageTest, WhenConstructedWithAConstCharPointer) {
  const char* const_char_ptr = "foo";
  xtest::Message msg(const_char_ptr);
  EXPECT_EQ("foo", msg.GetString());
}

TEST(MessageTest, WhenStreamedMultipleNonPointerValues) {
  xtest::Message msg;
  msg << 1 << " " << 2.6 << " " << 3 << " and " << 4 << '\n';
  EXPECT_EQ("1 2.6000000000000001 3 and 4\n", msg.GetString());
}

TEST(MessageTest, WhenStreamedANullPointer) {
  xtest::Message msg;
  msg << static_cast<void*>(nullptr);
  EXPECT_EQ("(null)", msg.GetString());
}

TEST(MessageTest, WhenStreamedBasicNarrowIoManip) {
  xtest::Message msg;
  msg << std::endl;
  EXPECT_EQ("\n", msg.GetString());
}

TEST(MessageTest, WhenStreamedBooleans) {
  xtest::Message msg;
  msg << true << " and " << false;
  EXPECT_EQ("true and false", msg.GetString());
}

TEST(MessageTest, WhenStreamedMultipleNullCharacters) {
  xtest::Message msg;
  msg << '\0' << '\0' << '\0' << '\0';
  EXPECT_EQ(msg.GetString(), "\\0\\0\\0\\0");
}

#endif  // XTEST_TESTS_XTEST_MESSAGE_TEST_HH_
