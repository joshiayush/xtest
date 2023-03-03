<div id="top"></div>

<br/>

<div align="center">
  <a href="https://github.com/joshiayush/xtest">
    <img src="./images/testing.png" alt="Logo" width="80" height="80">
  </a>
  <h3 align="center">xtest</h3>
  <p align="center">
    C++ testing framework inspired by <a href="https://github.com/google/googletest"><strong>googletest</strong></a>!
    <br />
    <a href="https://github.com/joshiayush/xtest/tree/master/docs"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/joshiayush/xtest/wiki">Wiki</a>
    ·
    <a href="https://github.com/joshiayush/xtest/issues">Report Bug</a>
    ·
    <a href="https://github.com/joshiayush/xtest/issues">Request Feature</a>
  </p>
</div>

**xtest** is a C++ testing framework inspired by [`gtest`][_gtest]. This is just a minimal implementation of a C++ unit testing framework which gives all the required facilities for unit testing a software without going template crazy and causing compilation issues.

A better and more robust version of this library is [`GoogleTest`][_gtest] which is developed and maintained by Google. It is a official Google product and is used by many notable projects.

<div align="right">
  <a href="#top">
  
  ![Back to top][back_to_top]
  
  </a>
</div>

## Installation

Clone the repository.

```shell
git clone https://github.com/joshiayush/xtest.git
```

**Install `libxtest`**

```shell
sudo chmod +x ./tools/install_libxtest.sh
./tools/install_libxtest.sh
```

**Build samples**

To build samples,

```shell
cmake -B build/ -S . -DBUILD_SAMPLES:BOOL=ON
cd build
make install
```

Now run the `samples`,

```shell
./samples/factorial/factorial  # Runs the factorial unit test samples binary
./samples/isprime/isprime      # Runs the isprime unit test samples binary
```

<div align="right">
  <a href="#top">
  
  ![Back to top][back_to_top]
  
  </a>
</div>

## Usage

Testing a custom function that returns the sqaure of a given number.

```C++
#include <xtest/xtest.hh>

double square(int n) {
  return n*n;
}

TEST(TestSquare, TestWithPositiveNumber) {
  EXPECT_EQ(square(10), 100);
}

int32_t main(int32_t argc, char** argv) {
  ::xtest::InitXTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```

Now compile the file and link the `libxtest.so` file with it:

```shell
g++ -lxtest square_test.cc -o square_test
```

<div align="right">
  <a href="#top">
  
  ![Back to top][back_to_top]
  
  </a>
</div>

## Contribution

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement", "bug", or "documentation". Don't forget to give the project a star! Thanks again!

Project [**xtest**][_xtest] is hosted on [GitHub][_github]. If you want to contribute changes please make sure to read the [`CONTRIBUTING.md`][_xtest_contrib_f] file. You can also contribute changes to the [`CONTRIBUTING.md`][_xtest_contrib_f] file itself.

<div align="right">
  <a href="#top">
  
  ![Back to top][back_to_top]
  
  </a>
</div>

<!-- Definitions -->

[_gtest]: https://www.github.com/google/googletest
[_xtest]: https://www.github.com/joshiayush/xtest
[_github]: https://www.github.com

<!-- Attached links -->

[back_to_top]: https://img.shields.io/badge/-Back%20to%20top-lightgrey

<!-- Files -->

[_xtest_contrib_f]: https://www.github.com/joshiayush/xtest/blob/master/CONTRIBUTING.md
[_xtest_license_f]: https://www.github.com/joshiayush/xtest/blob/master/LICENSE
