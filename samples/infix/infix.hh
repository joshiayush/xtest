// Copyright 2022, The Infix authors.
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
//     * Neither the name of The Infix authors. nor the names of
// its contributors may be used to endorse or promote products derived from
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

#ifndef XTEST_SAMPLES_INFIX_INFIX_HH_
#define XTEST_SAMPLES_INFIX_INFIX_HH_

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#define INFIX_TO_POSTFIX_POWER_SYMBOL_ '^'
#define INFIX_TO_POSTFIX_MULTIPLY_SYMBOL_ '*'
#define INFIX_TO_POSTFIX_DIVIDE_SYMBOL_ '/'
#define INFIX_TO_POSTFIX_ADD_SYMBOL_ '+'
#define INFIX_TO_POSTFIX_SUBTRACT_SYMBOL_ '-'

class Infix {
 private:
  // Pushes the given `item` onto the `M_infix_to_postfix_stack` at `++M_top`.
  // Prints an error message `Stack overflow\n` when `M_top` is equals or
  // greater than `M_infix_to_postfix_stack_size`.
  void PushInInfixToPostfixStack(const char& item);

  // Pops the top item from the `M_infix_to_postfix_stack` at `M_top` and brings
  // back `M_top`'s value by one.  Returns the item that got popped.
  char PopFromInfixToPostfixStack();

  // Returns a bool indicating whether the given `symbol` is a valid Infix
  // expression operator or not.
  static bool IsOperator(const char& symbol);

  // Returns a bool indicating whether the given `symbol` is a valid Infix
  // expression operand or not.
  //
  // Assumes that single letters or digits are used as operands.
  //
  // @TODO(joshiayush): Add support for more complex operands where multiple
  // letters are used as a single operand.
  static bool IsOperand(const char& symbol);

  // Returns an `unsigned int` indicating the precedence value of the given
  // `symbol` (ideally operator).  As high as the return value is, as high the
  // precedence of the given `symbol` is.
  static uint32_t GetOperatorPrecedence(const char& symbol);

 public:
  // Default constructor for no explicit value.  Assigns `nullptr` to
  // `M_infix_to_postfix_stack` when an instance is instantiated through default
  // constructor.  `M_top` is always initialized to `-1`.
  Infix();

  // Constructor for explicit value.  Assigns `expr` to `M_infix_expr` and
  // allocates `M_infix_to_postfix_stack_size + 1` bytes to
  // `M_infix_to_postfix_stack`.  `M_top` is always initialized to `-1`.
  explicit Infix(const char* expr);

  // Constructor for explicit value.  Assigns `expr` to `M_infix_expr` and
  // allocates `M_infix_to_postfix_stack_size + 1` bytes to
  // `M_infix_to_postfix_stack`.  `M_top` is always initialized to `-1`.
  //
  // This overload takes care of situation where `std::string` is used for
  // constructing an instance of `Infix`.
  explicit Infix(const std::string& expr);

  // Deallocates `M_infix_to_postfix_stack` when an instance is destroyed.
  ~Infix();

  // Algorithm to convert an Infix expression to a Postfix expression.  This
  // doesn't take in account cases where complex operands are used as an Infix
  // expression operand (multiple letters).
  std::string toPostfix();

 private:
  std::string M_infix_expr;

  size_t M_infix_to_postfix_stack_size;
  ssize_t M_top;

  char* M_infix_to_postfix_stack;
};

#endif  // XTEST_SAMPLES_INFIX_INFIX_HH_
