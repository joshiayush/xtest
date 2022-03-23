// Copyright 2022, The xtest authors.
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

#ifndef XTEST_TESTS_XTEST_PORT_TEST_HH_
#define XTEST_TESTS_XTEST_PORT_TEST_HH_

#include "internal/xtest-port.hh"
#include "xtest.hh"

TEST(GlobalCountersTest, CanBeAccessInCodeOnceXTestPortIsIncluded) {
  uint64_t dummy = XTEST_GLOBAL_INSTANCE_GET_(failure_count) ||
                   XTEST_GLOBAL_INSTANCE_GET_(test_count) ||
                   XTEST_GLOBAL_INSTANCE_GET_(test_suite_count) ||
                   XTEST_GLOBAL_INSTANCE_GET_(failed_test_count);
  EXPECT_TRUE(dummy || !dummy);  // Suppresses warning that dummy is unused.
}

TEST(GetStrFilledWithTest, WithoutPassingWidthExplicitly) {
  const std::string actual = xtest::GetStrFilledWith(
      '-');  // Default width passed should be
             // `XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_`.
  const std::string expected("----------");
  EXPECT_EQ(actual, expected);
}

TEST(GetStrFilledWithTest, WhenPassedDefaultSummaryStatusStrWidthExplicitly) {
  const std::string actual =
      xtest::GetStrFilledWith('-', XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_);
  const std::string expected("----------");
  EXPECT_EQ(actual, expected);
}

TEST(GetStrFilledWithTest, WhenPassedAnArbitraryNumber) {
  const std::string actual = xtest::GetStrFilledWith('-', 20);
  const std::string expected("--------------------");
  EXPECT_EQ(actual, expected);
}

TEST(GetStringAlignedToTest, AlignCenterWithDefaultSummaryStatusWidth) {
  const std::string actual = xtest::GetStringAlignedTo(
      "FAILED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
      xtest::StringAlignValues::ALIGN_CENTER);
  const std::string expected("  FAILED  ");
  EXPECT_EQ(actual, expected);
}

TEST(GetStringAlignedToTest, AlignLeftWithDefaultSummaryStatusWidth) {
  const std::string actual = xtest::GetStringAlignedTo(
      "FAILED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
      xtest::StringAlignValues::ALIGN_LEFT);
  const std::string expected("FAILED    ");
  EXPECT_EQ(actual, expected);
}

TEST(GetStringAlignedToTest, AlignRightWithDefaultSummaryStatusWidth) {
  const std::string actual = xtest::GetStringAlignedTo(
      "FAILED", XTEST_DEFAULT_SUMMARY_STATUS_STR_WIDTH_,
      xtest::StringAlignValues::ALIGN_RIGHT);
  const std::string expected("    FAILED");
  EXPECT_EQ(actual, expected);
}

#endif  // XTEST_TESTS_XTEST_PORT_TEST_HH_
