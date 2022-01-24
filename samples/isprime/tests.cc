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

#include <cstdint>
#include <vector>

#include "isprime/isprime.hh"
#include "xtest.hh"

TEST(TestWithPositiveLowPrimes, TestIsPrime) {
  std::vector<std::int64_t> positive_low_primes = {
      2,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,
      47,  53,  59,  61,  67,  71,  73,  79,  83,  89,  97,  101, 103, 107,
      109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
      191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263,
      269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
      353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433,
      439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521,
      523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613,
      617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701,
      709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809,
      811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887,
      907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997};
  for (const std::int64_t& prime : positive_low_primes)
    EXPECT_TRUE(IsPrime(prime));
}

TEST(TestWithPositiveBalancedPrimes, TestIsPrime) {
  std::vector<std::int64_t> positive_balanced_primes = {
      5,    53,   157,  173,  211,  257,  263,  373,  563,  593,  607,  653,
      733,  947,  977,  1103, 1123, 1187, 1223, 1367, 1511, 1747, 1753, 1907,
      2287, 2417, 2677, 2903, 2963, 3307, 3313, 3637, 3733, 4013, 4409, 4457,
      4597, 4657, 4691, 4993, 5107, 5113, 5303, 5387, 5393};
  for (const std::int64_t& prime : positive_balanced_primes)
    EXPECT_TRUE(IsPrime(prime));
}

TEST(DoomedToBeFailedTests, TestIsPrime) {
  std::vector<std::int64_t> positive_bell_primes = {
      2,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  47,  53,
      59,  67,  71,  83,  89,  101, 107, 109, 113, 127, 131, 137, 139, 149, 157,
      167, 179, 181, 191, 197, 199, 211, 227, 233, 239, 251, 257, 263, 269, 281,
      293, 307, 311, 317, 337, 347, 353, 359, 379, 389, 401, 409};
  for (const std::int64_t& prime : positive_bell_primes)
    EXPECT_FALSE(IsPrime(prime));
}

int32_t main(int32_t argc, char** argv) {
  xtest::InitXTest(&argc, argv);
  return RUN_ALL_TESTS();
}
