<div id="top"></div>

<div align="center">

[![Contributors][xtest_contributors]][xtest_contributors_url]
[![Forks][xtest_forks]][xtest_forks_url]
[![Stargazers][xtest_stars]][xtest_stars_url]
[![Issues][xtest_issues]][xtest_issues_url]
[![LICENSE][xtest_license]][xtest_license_url]
[![LinkedIn][developer_linkedin]][developer_linkedin_url]

</div>

<br/>

<div align="center">
  <a href="https://github.com/joshiayush/xtest">
    <img src="./images/testing.png" alt="Logo" width="80" height="80">
  </a>
  <h3 align="center">xtest</h3>
  <p align="center">
    C++ testing framework inspired by <a href="https://github.com/google/googletest"><strong>googletest</strong></a>
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

<details>
  <summary>Contents</summary>
  <ol>
    <li>
        <a href="#xtest">xtest</a>
        <ul>
            <a href="#commence">Commence</a>
            <ul>
                <li>
                  <a href="#prerequisites">Prerequisites</a>
                  <ul>
                    <li><a href="#ubuntu">Ubuntu</a></li>
                  </ul>
                </li>
                <li><a href="#installation">Installation</a></li>
            </ul>
        </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contribution">Contribution</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#developers">Developers</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

# xtest

**xtest** is a C++ testing framework inspired by [`gtest`][_gtest]. This is just a minimal implementation of a C++ unit testing framework which gives all the required facilities for unit testing a software without going template crazy and causing compilation issues.

<div align="right">
  <a href="#top">
  
  ![Back to top][back_to_top]
  
  </a>
</div>

# Commence

To download and install library [__xtest__][_xtest] in your system follow the following steps.

## Prerequisites

You must have a `g++ 9.3.0` or greater version with a `cmake` version `3.16.3` or _higher_ installed in your system.

### Ubuntu

__Step 1: Install gcc__

```shell
sudo apt update
sudo apt install build-essential
```

If this is the first time you are installing `g++` then you may also want to install the manual pages about using GNU/Linux for development.

```shell
sudo apt install manpages-dev
```

Verify that the compiler is installed.

```shell
g++ --version
```

and it should print,

```shell
g++ (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

In the above output __ubuntu1~20.04__ is my version, yours may be different.

__Step 2: Install cmake__

1. Uninstall the default `cmake` version provided by Ubuntu's package manager and configuration by using:

    ```shell
    sudo apt remove --purge --auto-remove cmake
    ```
  
   OR:
  
    ```shell
    sudo apt purge --auto-remove cmake
    ```

   This step is crucial if you have a old version of `cmake` installed in your system.

2. Prepare for installation.

    ```shell
    sudo apt update
    sudo apt install -y software-properties-common lsb-release
    sudo apt clean all
    ```
  
3. Obtain a copy of kitware's signing key.

    ```shell
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
    ```
  
4. Add kitware's repository to your sources list for `Ubuntu Focal Fossa (20.04)`, `Ubuntu Bionic Beaver (18.04)` and `Ubuntu Xenial Xerus (16.04)`.

    ```shell
    sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"
    ```
  
5. As an optional step it is recommended that we also install the kitware-archive-keyring package to ensure that Kitware's keyring stays up to date as they rotate their keys.

    ```shell
    sudo apt update
    sudo apt install kitware-archive-keyring
    sudo rm /etc/apt/trusted.gpg.d/kitware.gpg
    ```
  
6. Finally we can update and install the cmake package.

    ```shell
    sudo apt update
    sudo apt install cmake
    ```

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

__Build xtest__

Next step is to build `xtest` as a shared library using our build system `cmake`.

```shell
cmake -B build/ -S .
cd build
make install
```

This will build `xtest` library as `libxtest.so` file inside the `build` directory. 

__Build samples__

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

# Usage

Currently the project in its development state so you can't use it at the moment. Your contributions are what makes this project grow faster you can consider contributing to [**xtest**][_xtest].

You can start by looking into the docstrings, there's enough information in that to get you started.

<div align="right">
  <a href="#top">
  
  ![Back to top][back_to_top]
  
  </a>
</div>

# Contribution

Contributions are what makes the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement", "bug", or "documentation". Don't forget to give the project a star! Thanks again!

Project [**xtest**][_xtest] is hosted on [GitHub][_github]. If you want to contribute changes please make sure to read the [`CONTRIBUTING.md`][_xtest_contrib_f] file. You can also contribute changes to the [`CONTRIBUTING.md`][_xtest_contrib_f] file itself.

<div align="right">
  <a href="#top">
  
  ![Back to top][back_to_top]
  
  </a>
</div>

# License

Distributed under the BSD 3-Clause License. See [`LICENSE`][_xtest_license_f] for more information. Please do not use project [**xtest**][_xtest] if you have any issue with the BSD 3-Clause License.

<div align="right">
  <a href="#top">
  
  ![Back to top][back_to_top]
  
  </a>
</div>

# Developers

- [**joshiayush**](https://www.github.com/joshiayush)

  <a href="https://github.com/joshiayush">
  <img src="./images/github.png" alt="Logo" width="40" height="40">
  </a>
  <a href="https://www.linkedin.com/in/ayush-joshi-3600a01b7/">
  <img src="./images/linkedin.png" alt="Logo" width="40" height="40">
  </a>
  <a href="http://twitter.com/joshiayushjoshi">
  <img src="./images/twitter.png" alt="Logo" width="40" height="40">
  </a>
  <a href="https://stackoverflow.com/users/13910122/ayush">
  <img src="./images/stack-overflow.png" alt="Logo" width="40" height="40">
  </a>

  <!-- Contibutors who have contributed non-trivial changes are encouraged to add their details here. -->

  <div align="right">
    <a href="#top">
    
    ![Back to top][back_to_top]
    
    </a>
  </div>

# Acknowledgments

A big thanks goes to the following resources who have helped in the development of project [**xtest**][_xtest].

- [Flaticon](https://www.flaticon.com/)
- [Shields.io](https://shields.io/)
- [Best-README-Template](https://github.com/othneildrew/Best-README-Template)
- [Stack Overflow](https://www.stackoverflow.com)

<div align="right">
  <a href="#top">
  
  ![Back to top][back_to_top]
  
  </a>
</div>

<!-- Definitions -->

[_gtest]: https://www.github.com/google/googletest
[_xtest]: https://www.github.com/joshiayush/xtest
[_github]: https://www.github.com

<!-- Shields and attached links -->

[xtest_contributors]: https://img.shields.io/github/contributors/joshiayush/xtest?logo=GitHub&style=for-the-badge
[xtest_contributors_url]: https://github.com/joshiayush/xtest/graphs/contributors
[xtest_forks]: https://img.shields.io/github/forks/joshiayush/xtest?logo=GitHub&style=for-the-badge
[xtest_forks_url]: https://github.com/joshiayush/xtest/network/members
[xtest_stars]: https://img.shields.io/github/stars/joshiayush/xtest?logo=GitHub&style=for-the-badge
[xtest_stars_url]: https://github.com/joshiayush/xtest/stargazers
[xtest_issues]: https://img.shields.io/github/issues/joshiayush/xtest?logo=GitHub&style=for-the-badge
[xtest_issues_url]: https://github.com/joshiayush/xtest/issues
[xtest_license]: https://img.shields.io/github/license/joshiayush/xtest?logo=GitHub&style=for-the-badge
[xtest_license_url]: https://github.com/joshiayush/xtest/blob/master/LICENSE
[developer_linkedin]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[developer_linkedin_url]: https://www.linkedin.com/in/ayush-joshi-3600a01b7/
[back_to_top]: https://img.shields.io/badge/-Back%20to%20top-lightgrey

<!-- Files -->

[_xtest_contrib_f]: https://www.github.com/joshiayush/xtest/blob/master/CONTRIBUTING.md
[_xtest_license_f]: https://www.github.com/joshiayush/xtest/blob/master/LICENSE
