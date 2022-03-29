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

#include "infix.hh"

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

// Pushes the given `item` onto the `M_infix_to_postfix_stack` at `++M_top`.
// Prints an error message `Stack overflow\n` when `M_top` is equals or
// greater than `M_infix_to_postfix_stack_size`.
void Infix::PushInInfixToPostfixStack(const char& item) {
  if (M_top >= static_cast<ssize_t>(M_infix_to_postfix_stack_size - 1))
    std::fprintf(stderr, "Stack overflow\n");
  else
    M_infix_to_postfix_stack[++M_top] = item;
}

// Pops the top item from the `M_infix_to_postfix_stack` at `M_top` and brings
// back `M_top`'s value by one.  Returns the item that got popped.
char Infix::PopFromInfixToPostfixStack() {
  if (M_top < 0) {
    std::fprintf(stderr, "Stack underflow\n");
    std::exit(EXIT_FAILURE);
  }
  char item;
  item = M_infix_to_postfix_stack[M_top--];
  return item;
}

// Returns a bool indicating whether the given `symbol` is a valid Infix
// expression operator or not.
bool Infix::IsOperator(const char& symbol) {
  switch (symbol) {
    case INFIX_TO_POSTFIX_POWER_SYMBOL_:
    case INFIX_TO_POSTFIX_MULTIPLY_SYMBOL_:
    case INFIX_TO_POSTFIX_DIVIDE_SYMBOL_:
    case INFIX_TO_POSTFIX_ADD_SYMBOL_:
    case INFIX_TO_POSTFIX_SUBTRACT_SYMBOL_:
      return true;
    default:
      return false;
  }
}

// Returns a bool indicating whether the given `symbol` is a valid Infix
// expression operand or not.
//
// Assumes that single letters or digits are used as operands.
//
// @TODO(joshiayush): Add support for more complex operands where multiple
// letters are used as a single operand.
bool Infix::IsOperand(const char& symbol) {
  return std::isdigit(symbol) || std::isalpha(symbol);
}

// Returns an `unsigned int` indicating the precedence value of the given
// `symbol` (ideally operator).  As high as the return value is, as high the
// precedence of the given `symbol` is.
uint32_t Infix::GetOperatorPrecedence(const char& symbol) {
  switch (symbol) {
    case INFIX_TO_POSTFIX_POWER_SYMBOL_:
      return 3;
    case INFIX_TO_POSTFIX_MULTIPLY_SYMBOL_:
    case INFIX_TO_POSTFIX_DIVIDE_SYMBOL_:
      return 2;
    case INFIX_TO_POSTFIX_ADD_SYMBOL_:
    case INFIX_TO_POSTFIX_SUBTRACT_SYMBOL_:
      return 1;
    default:
      return 0;
  }
}

// Default constructor for no explicit value.  Assigns `nullptr` to
// `M_infix_to_postfix_stack` when an instance is instantiated through default
// constructor.  `M_top` is always initialized to `-1`.
Infix::Infix()
    : M_infix_expr(""),
      M_top(-1),
      M_infix_to_postfix_stack(nullptr),
      M_infix_to_postfix_stack_size(0) {}

// Constructor for explicit value.  Assigns `expr` to `M_infix_expr` and
// allocates `M_infix_to_postfix_stack_size + 1` bytes to
// `M_infix_to_postfix_stack`.  `M_top` is always initialized to `-1`.
Infix::Infix(const char* expr)
    : M_infix_expr(expr),
      M_top(-1),
      M_infix_to_postfix_stack_size(std::strlen(expr) + 1) {
  M_infix_to_postfix_stack = new char[M_infix_to_postfix_stack_size];
}

// Constructor for explicit value.  Assigns `expr` to `M_infix_expr` and
// allocates `M_infix_to_postfix_stack_size + 1` bytes to
// `M_infix_to_postfix_stack`.  `M_top` is always initialized to `-1`.
//
// This overload takes care of situation where `std::string` is used for
// constructing an instance of `Infix`.
Infix::Infix(const std::string& expr)
    : M_infix_expr(expr),
      M_top(-1),
      M_infix_to_postfix_stack_size(expr.length() + 1) {
  M_infix_to_postfix_stack = new char[M_infix_to_postfix_stack_size];
}

// Deallocates `M_infix_to_postfix_stack` when an instance is destroyed.
Infix::~Infix() { delete[] M_infix_to_postfix_stack; }

// Algorithm to convert an Infix expression to a Postfix expression.  This
// doesn't take in account cases where complex operands are used as an Infix
// expression operand (multiple letters).
std::string Infix::toPostfix() {
  std::string postfix_expr;
  for (const char* p = M_infix_expr.c_str(); *p != '\0'; ++p) {
    if (IsOperand(*p)) {
      // In case of an operand, simply push it onto the stack.
      postfix_expr += *p;
    } else if (IsOperator(*p)) {
      // In case of an operator, check if there are other operators on the
      // stack that have greater precedence than the current operator, if so
      // pop them off the stack and push them onto the output string.
      while (M_top >= 0 &&
             GetOperatorPrecedence(M_infix_to_postfix_stack[M_top]) >=
                 GetOperatorPrecedence(*p)) {
        postfix_expr += PopFromInfixToPostfixStack();
      }
      // Push current operator onto the stack.
      PushInInfixToPostfixStack(*p);
    } else if (*p == '(') {
      // Push left bracket onto the stack.
      PushInInfixToPostfixStack(*p);
    } else if (*p == ')') {
      // Pop all operators off the stack until a left bracket is found and
      // push them onto the output string.
      while (M_infix_to_postfix_stack[M_top] != '(') {
        postfix_expr += PopFromInfixToPostfixStack();
      }
      // Pop the left bracket off the stack.
      PopFromInfixToPostfixStack();
    }
  }
  // Pop all operators off the stack and push them onto the output string in
  // case there are any.
  while (M_top >= 0) {
    postfix_expr += PopFromInfixToPostfixStack();
  }
  return postfix_expr;
}
