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

#include "isprime/isprime.hh"

#include <cstdint>

// Returns the modular exponentiation.
//
// This function returns the result of,
//  (base^num) % mod
int64_t Pow(int64_t base, int64_t num, int64_t mod) {
  int64_t pow = base;
  int64_t result = 1;

  while (num) {
    if (num & 1)
      result = (result * pow) % mod;
    pow = (pow * pow) % mod;
    num >>= 1;
  }

  return result;
}

// Checks if the given num is a prime number using a probabilistic method called
// Miller - Rabin method.
//
// This function takes in a prime number and a number that is big enough for
// primality test.
bool RabinMiller(int64_t var, int64_t num) {
  int64_t t, u, i;
  int64_t prev, curr;

  u = num / 2;
  t = 1;
  while (!(u & 1)) {
    u /= 2;
    ++t;
  }

  prev = Pow(var, u, num);
  for (i = 1; i <= t; ++i) {
    curr = (prev * prev) % num;
    if ((curr == 1) && (prev != 1) && (prev != num - 1))
      return true;
    prev = curr;
  }

  if (curr != 1)
    return true;
  return false;
}

// Returns true if and only if num is a prime number.
//
// This function is written and based on the deterministic variant of the famous
// Rabin-Miller algorithm, combined with optimized step brute forcing, giving
// you one of the fastest prime testing functions out there.
//
// The main caveat here is that the algorithm is deterministic only for the
// numbers that are less than 4,759,123,141.
bool IsPrime(int64_t num) {
  if (((!(num & 1)) && num != 2) || (num < 2) || (num % 3 == 0 && num != 3))
    return false;

  if (num < 1373653) {
    for (int64_t k = 1; 36 * k * k - 12 * k < num; ++k)
      if ((num % (6 * k + 1) == 0) || (num % (6 * k - 1) == 0))
        return (false);
    return true;
  }

  if (num < 9080191) {
    if (RabinMiller(31, num))
      return false;
    if (RabinMiller(73, num))
      return false;
    return true;
  }

  if (RabinMiller(2, num))
    return false;
  if (RabinMiller(7, num))
    return false;
  if (RabinMiller(61, num))
    return false;

  return true;
}
