# How to become a contributor and submit your own code

## Contributing A Patch

1. Submit an issue describing your proposed change to the [issue tracker](https://github.com/joshiayush/xtest/issues).
2. Please don't mix more than one logical change per submittal, because it makes the history hard to follow. If you want to make a change that doesn't have a corresponding issue in the issue tracker, please create one.
3. Also, coordinate with team members that are listed on the issue in question. This ensures that work isn't being duplicated and communicating your plan early also generally leads to better patches.
4. Fork the repo, develop and test your code changes.
5. Ensure that your code adheres to the existing style. See [.clang-format](https://github.com/joshiayush/xtest/blob/master/.clang-format) in the root directory.
6. Ensure that your code has an appropriate set of unit tests which all pass.
7. Submit a pull request.

## Style

To keep the source consistent, readable, diffable and easy to merge, we use a fairly rigid coding style, as defined by the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

## Requirements for Contributors

If you plan to contribute a patch, you need:

- `gcc 9.3.0` or similar.
- `g++ 9.3.0` or similar.

## Commit

Before commiting please make sure that you have read the [`DEVELOPERS.md`](https://github.com/joshiayush/xtest/blob/master/DEVELOPERS.md) file.

## License

By contributing, you agree that your contributions will be licensed under its BSD 3-Clause License. Include the following license at the beginning of every file.

```C++
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
```

<!-- Definitions -->

[_unittest]: https://docs.python.org/3/library/unittest.html
