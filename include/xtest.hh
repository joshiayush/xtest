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

#ifndef _XTEST_HH_
#define _XTEST_HH_

#include <cinttypes>

#include "assertions.hh"

namespace xtest {
// @brief Global counter for non-fatal test failures.
//
// This global counter is defined in the object file xtest.cc and incremented
// every time a non-fatal test assertion fails.
extern uint64_t G_n_testFailures;
}  // namespace xtest

#endif