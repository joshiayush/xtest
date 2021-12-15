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

#ifndef XTEST_INCLUDE_XTEST_MESSAGE_HH_
#define XTEST_INCLUDE_XTEST_MESSAGE_HH_

#include <memory>
#include <sstream>
#include <string>

namespace xtest {
// The Message class works like an ostream repeater.
//
// Typical usage:
//
//   1. You stream a bunch of values to a Message object.  It will remember the
//        text in a stringstream.
//   2. Then you stream the Message object to an ostream.  This causes the text
//        in the Message to be streamed to the ostream.
//
// For example;
//
//   xtest::internal::Message foo;
//   foo << 1 << " != " << 2;
//   std::cout << foo;
//
// will print "1 != 2".
//
// Message is not intended to be inherited from.  In particular, its destructor
// is not virtual.
//
// Note that stringstream behaves differently in gcc and in MSVC.  You can
// stream a NULL char pointer to it in the former, but not in the latter (it
// causes an access violation if you do).  The Message class hides this
// difference by treating a NULL char pointer as "(null)".
class Message {
 private:
  // The type of basic IO manipulators (endl, ends, and flush) for narrow
  // streams.
  typedef std::ostream& (*BasicNarrowIoManip)(std::ostream&);

 public:
  // Constructs an empty Message.
  Message();

  // Copy constructor.
  Message(const Message& msg) : _M_sstream(new ::std::stringstream) {  // NOLINT
    *_M_sstream << msg.GetString();
  }

  // Constructs a Message from a C-string.
  explicit Message(const char* str) : _M_sstream(new ::std::stringstream) {
    *_M_sstream << str;
  }

  // Streams a non-pointer value to this object.
  template <typename T>
  inline Message& operator<<(const T& val) {
    *_M_sstream << val;
    return *this;
  }

  // Streams a pointer value to this object.
  //
  // This function is an overload of the previous one.  When you stream a
  // pointer to a Message, this definition will be used as it is more
  // specialized.  (The C++ Standard, section [temp.func.order].)  If you stream
  // a non-pointer, then the previous definition will be used.
  //
  // The reason for this overload is that streaming a NULL pointer to ostream is
  // undefined behavior.  Depending on the compiler, you may get "0", "(nil)",
  // "(null)", or an access violation.  To ensure consistent result across
  // compilers, we always treat NULL as "(null)".
  template <typename T>
  inline Message& operator<<(T* const& pointer) {  // NOLINT
    if (pointer == nullptr)
      *_M_sstream << "(null)";
    else
      *_M_sstream << pointer;
    return *this;
  }

  // Since the basic IO manipulators are overloaded for both narrow and wide
  // streams, we have to provide this specialized definition of operator <<,
  // even though its body is the same as the templatized version above.  Without
  // this definition, streaming endl or other basic IO manipulators to Message
  // will confuse the compiler.
  Message& operator<<(BasicNarrowIoManip val) {
    *_M_sstream << val;
    return *this;
  }

  // Instead of 1/0, we want to see true/false for bool values.
  Message& operator<<(bool b) { return *this << (b ? "true" : "false"); }

  // Gets the text streamed to this object so far as an std::string.  Each '\0'
  // character in the buffer is replaced with "\\0".
  //
  // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
  ::std::string GetString() const;

 private:
  // We'll hold the text streamed to this object here.
  const std::unique_ptr<::std::stringstream> _M_sstream;

  // We declare (but don't implement) this to prevent the compiler from
  // implementing the assignment operator.
  void operator=(const Message&);
};

// Streams a Message to an ostream.
inline std::ostream& operator<<(std::ostream& os, const Message& streamable) {
  return os << streamable.GetString();
}

namespace internal {
// Converts a streamable value to an std::string.  A NULL pointer is converted
// to "(null)".  When the input value is a ::string, ::std::string, ::wstring,
// or ::std::wstring object, each NULL character in it is replaced with "\\0".
template <typename T>
::std::string StreamableToString(const T& streamable) {
  return (Message() << streamable).GetString();
}
}  // namespace internal
}  // namespace xtest

#endif  // XTEST_INCLUDE_XTEST_MESSAGE_HH_
