
# Unit Tests

This directory contains unit tests.

They are always compiled, unlike with `legacy-buildtools`.

## Writing Tests

- The test files should end with `_test.cc` suffix.
- Use the boost test framework, see the [documentation](
https://www.boost.org/doc/libs/release/libs/test/doc/html/index.html).


## Running the Tests

Run `make test`. If some some test fails and you need to get more details
then directly run the test binary from `build/tests` directory, it will print
the details on the console.

There are two levels of verbosity options:

**CMake/CTest**: `make test ARGS=-V` will run `ctest -V` which will display

1. test harness internals (less useful)
2. the full output of the test programs (useful)

**Boost.Test**: `export BOOST_TEST_COLOR_OUTPUT=1 BOOST_TEST_LOG_LEVEL=all`
will make the test programs report progress on the individual test assertions.
Normally you don't need it, failures are always reported.

For the full list of Boost.Test environment variables and corresponding CLI
options, run a `*_test` program with `--help`.

## Code Coverage

FIXME: This has not been ported from `legacy-buildtools` yet. See
<https://github.com/libyui/libyui-old/pull/129>

The code coverage reporting can be enabled using the `-DENABLE_CODE_COVERAGE=ON`
cmake option or using the `make -f Makefile.cvs COVERAGE=1` initial call.
The `lcov` package needs to be installed.

To print the code coverage run `make`, `make test` and `make coverage`.
There is an additional all-in-one target which build the sources, runs the tests
and prints the code coverage - simply run `make tests`.

The code coverage summary will be displayed on the console. If you need more
details (e.g. which lines are covered) then open the `coverage/index.html`
file in a web browser.
