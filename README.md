# xtest

**xtest** is a C++ testing framework inspired by [`gtest`][_gtest].

## Minimum Viable Requirements

    EXPECT_*, ASSERT_* macro, on error: print values (and message if given)
        bonus: typesafe
    catching abort(), assertion failures, access violations, other HW exceptions
    test seed logging, forcing
    automatic test registration

<!-- Definitions -->

[_gtest]: https://github.com/google/googletest
