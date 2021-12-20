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

#include "xtest-registrar.hh"

#include <cstdint>

#include "internal/xtest-port.hh"
#include "xtest-message.hh"

namespace xtest {
// We initialize 'TestRegistry' instance here which then later gets served to
// each file that include 'xtest-registrar.hh'.
TestRegistry GTestRegistryInst = {{}, 0};

// Construct a new TestRegistrar object.
//
// This constructor appends a new entry of test suite to GTestRegistryInst list.
// We link the comming test suite to the TestRegistrar* type variable M_head
// linked list declared inside of the struct TestRegistry to later traverse
// through this list to execute each test suite.
TestRegistrar::TestRegistrar(const char* suiteName, const char* testName,
                             TestFunction testFunc)
    : M_suiteName(suiteName),
      M_testFunc(testFunc),
      M_testName(testName),
      M_nextTestSuite(nullptr),
      M_testResult(TestResult::UNKNOWN) {
  GTestRegistryInst.M_testRegistryTable[M_suiteName].push_back(this);
}
}  // namespace xtest
