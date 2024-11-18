/*
  Copyright (C) 2018 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/

// This is an unit test for the FSize class

#define BOOST_TEST_MODULE FSize_tests
#include <boost/test/unit_test.hpp>
#include <boost/version.hpp>

#include <limits>
#include <sstream>

#include "FSize.h"

using boost::multiprecision::cpp_int;

// decrease the log level to warnings
struct LogWarnings {
  // global initialization before running any test
  void setup() {
      boost::unit_test::unit_test_log.set_threshold_level( boost::unit_test::log_warnings );
  }
  // cleanup after all tests are finished
  void teardown() { }
};

BOOST_TEST_GLOBAL_FIXTURE( LogWarnings );

BOOST_AUTO_TEST_CASE( constructor )
{
    // check the default constructor
    FSize fsize;
    BOOST_CHECK_EQUAL( fsize, 0 );

    // check an int parameter
    FSize fsize_int(42);
    BOOST_CHECK_EQUAL( fsize_int, 42 );

    // check a long long parameter
    FSize fsize_long(1LL << 40);
    BOOST_CHECK_EQUAL( fsize_long, 1LL << 40);

    // check a double parameter
    FSize fsize_double(42.0);
    BOOST_CHECK_EQUAL( fsize_double, 42.0);
    // the decimal part is ignored
    BOOST_CHECK_EQUAL( fsize_double, FSize(42.1));
    BOOST_CHECK_EQUAL( fsize_double, FSize(42.5));
    BOOST_CHECK_EQUAL( fsize_double, FSize(42.9));

    // check negative number
    FSize fsize_int_neg(-42);
    BOOST_CHECK_EQUAL( fsize_int_neg, -42 );

    // create with units
    long long u = 1;
    BOOST_CHECK_EQUAL( FSize(1, FSize::Unit::B), 1);
    BOOST_CHECK_EQUAL( FSize(1, FSize::Unit::K), u *= 1024);
    BOOST_CHECK_EQUAL( FSize(1, FSize::Unit::M), u *= 1024);
    BOOST_CHECK_EQUAL( FSize(1, FSize::Unit::G), u *= 1024);
    BOOST_CHECK_EQUAL( FSize(1, FSize::Unit::T), u *= 1024);
    BOOST_CHECK_EQUAL( FSize(1, FSize::Unit::P), u *= 1024);
    BOOST_CHECK_EQUAL( FSize(1, FSize::Unit::E), u *= 1024);
    // too much for a long long...
    BOOST_CHECK_EQUAL( FSize(1, FSize::Unit::Z), cpp_int(1) << 70);
    BOOST_CHECK_EQUAL( FSize(1, FSize::Unit::Y), cpp_int(1) << 80);

    BOOST_CHECK_EQUAL( FSize(128, FSize::Unit::K), 128 * 1024);
    BOOST_CHECK_EQUAL( FSize(-128, FSize::Unit::K), -(128 * 1024));

    // from string
    BOOST_CHECK_EQUAL( FSize("128", FSize::Unit::K), 128 * 1024);
    BOOST_CHECK_EQUAL( FSize("-128", FSize::Unit::K), -128 * 1024);
    BOOST_CHECK_EQUAL( FSize("856", FSize::Unit::M), 856 * 1024 * 1024);

    // throws std::runtime_error exception for invalid data!!
    BOOST_CHECK_THROW( FSize("72.89", FSize::Unit::K), std::runtime_error);
    BOOST_CHECK_THROW( FSize(" 72  ", FSize::Unit::K), std::runtime_error);
    BOOST_CHECK_THROW( FSize("72asdf", FSize::Unit::K), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( comparing_positive )
{
    FSize fsize(42);
    BOOST_CHECK_EQUAL( fsize == 42, true );
    BOOST_CHECK_EQUAL( fsize != 42, false );

    BOOST_CHECK_EQUAL( fsize > 0, true );
    BOOST_CHECK_EQUAL( fsize > 10, true );
    BOOST_CHECK_EQUAL( fsize > 42, false );
    BOOST_CHECK_EQUAL( fsize > -42, true );

    BOOST_CHECK_EQUAL( fsize >= 0, true );
    BOOST_CHECK_EQUAL( fsize >= 10, true );
    BOOST_CHECK_EQUAL( fsize >= 42, true );
    BOOST_CHECK_EQUAL( fsize >= -42, true );

    BOOST_CHECK_EQUAL( fsize < 0, false );
    BOOST_CHECK_EQUAL( fsize < 10, false );
    BOOST_CHECK_EQUAL( fsize < 42, false );
    BOOST_CHECK_EQUAL( fsize < -42, false );

    BOOST_CHECK_EQUAL( fsize < 0, false );
    BOOST_CHECK_EQUAL( fsize < 10, false );
    BOOST_CHECK_EQUAL( fsize <= 42, true );
    BOOST_CHECK_EQUAL( fsize <= -42, false );
}

BOOST_AUTO_TEST_CASE( comparing_negative )
{
    FSize fsize(-42);
    BOOST_CHECK_EQUAL( fsize == -42, true );
    BOOST_CHECK_EQUAL( fsize != -42, false );

    BOOST_CHECK_EQUAL( fsize > 0, false );
    BOOST_CHECK_EQUAL( fsize > 10, false );
    BOOST_CHECK_EQUAL( fsize > -42, false );
    BOOST_CHECK_EQUAL( fsize > -52, true );

    BOOST_CHECK_EQUAL( fsize >= 0, false );
    BOOST_CHECK_EQUAL( fsize >= 10, false );
    BOOST_CHECK_EQUAL( fsize >= -42, true );
    BOOST_CHECK_EQUAL( fsize >= -52, true );

    BOOST_CHECK_EQUAL( fsize < 0, true );
    BOOST_CHECK_EQUAL( fsize < 10, true );
    BOOST_CHECK_EQUAL( fsize < 42, true );
    BOOST_CHECK_EQUAL( fsize < -42, false );
    BOOST_CHECK_EQUAL( fsize < -52, false );

    BOOST_CHECK_EQUAL( fsize < 0, true );
    BOOST_CHECK_EQUAL( fsize < 10, true );
    BOOST_CHECK_EQUAL( fsize <= 42, true );
    BOOST_CHECK_EQUAL( fsize <= -42, true );
    BOOST_CHECK_EQUAL( fsize <= -52, false );
}

BOOST_AUTO_TEST_CASE( comparing_fsizes )
{
    FSize fsize(100);
    FSize fsize_copy(fsize);
    FSize fsize_double(100.0);
    FSize fsize_negative(-100);
    FSize fsize_different(200);

    BOOST_CHECK_EQUAL( fsize, fsize_copy );
    BOOST_CHECK_EQUAL( fsize, fsize_double );
    BOOST_CHECK_NE( fsize, fsize_different );
    BOOST_CHECK_NE( fsize, fsize_negative );

    BOOST_CHECK_EQUAL( fsize <  fsize_copy, false );
    BOOST_CHECK_EQUAL( fsize >  fsize_copy, false );
    BOOST_CHECK_EQUAL( fsize <= fsize_copy, true );
    BOOST_CHECK_EQUAL( fsize >= fsize_copy, true );
    BOOST_CHECK_EQUAL( fsize <  fsize_different, true );
    BOOST_CHECK_EQUAL( fsize >  fsize_different, false );
    BOOST_CHECK_EQUAL( fsize <= fsize_different, true );
    BOOST_CHECK_EQUAL( fsize >= fsize_different, false );
}

BOOST_AUTO_TEST_CASE( comparing_with_limits )
{
    // 2^1024
    FSize fsize(cpp_int(1) << 1024);

    // even bigger than the max long long
    BOOST_CHECK(fsize > std::numeric_limits<long long>::max());
    // even bigger than the max unsigned long long
    BOOST_CHECK(fsize > std::numeric_limits<unsigned long long>::max());
    // even bigger than the max double
    BOOST_CHECK(fsize > std::numeric_limits<double>::max());

    // compare with the negative limits
    fsize *= -1;
    // even lower than the min long long
    BOOST_CHECK(fsize < std::numeric_limits<long long>::min());
    // even lower than the min unsigned long long
    BOOST_CHECK(fsize < std::numeric_limits<unsigned long long>::min());
    // even lower than the min double
    BOOST_CHECK(fsize < std::numeric_limits<double>::min());
}

BOOST_AUTO_TEST_CASE( operators_on_self )
{
    FSize fsize(42);

    fsize += 42;
    BOOST_CHECK(fsize == 84);
    fsize -= 42;
    BOOST_CHECK(fsize == 42);
    fsize -= 42;
    BOOST_CHECK(fsize == 0);
    fsize -= 42;
    BOOST_CHECK(fsize == -42);
    fsize *= -1;
    BOOST_CHECK(fsize == 42);
    fsize *= 8;
    BOOST_CHECK(fsize == 336);
    fsize /= 6;
    BOOST_CHECK(fsize == 56);

    // check some bigger values, 10TiB
    FSize fsize2(10, FSize::Unit::T);
    // 10TiB / 10 => 1TiB
    fsize2 /= 10;
    BOOST_CHECK(fsize2 == FSize::TB);
}

BOOST_AUTO_TEST_CASE( operators )
{
    BOOST_CHECK_EQUAL(FSize(20) + FSize(22), 42);
    BOOST_CHECK_EQUAL(FSize(64) - FSize(22), 42);
    BOOST_CHECK_EQUAL(FSize(64) * FSize(4), 256);
    BOOST_CHECK_EQUAL(FSize(1024) / FSize(8), 128);

    BOOST_CHECK_EQUAL(20 + FSize(22), 42);
    BOOST_CHECK_EQUAL(64 - FSize(22), 42);
    BOOST_CHECK_EQUAL(64 * FSize(4), 256);
    BOOST_CHECK_EQUAL(1024 / FSize(8), 128);

    BOOST_CHECK_EQUAL(FSize(20) + 22, 42);
    BOOST_CHECK_EQUAL(FSize(64) - 22, 42);
    BOOST_CHECK_EQUAL(FSize(64) * 4, 256);
    BOOST_CHECK_EQUAL(FSize(1024) / 8, 128);

    // unary minus
    BOOST_CHECK_EQUAL(-FSize(120), -120);

    // increment/decrement
    FSize test(120);
    // post increment
    BOOST_CHECK_EQUAL(test++, 120);
    // changed
    BOOST_CHECK_EQUAL(test, 121);
    // pre increment
    BOOST_CHECK_EQUAL(++test, 122);
    // not changed
    BOOST_CHECK_EQUAL(test, 122);

    // post decrement
    BOOST_CHECK_EQUAL(test--, 122);
    // changed
    BOOST_CHECK_EQUAL(test, 121);
    // pre decrement
    BOOST_CHECK_EQUAL(--test, 120);
    // not changed
    BOOST_CHECK_EQUAL(test, 120);
}

BOOST_AUTO_TEST_CASE( fillBlock )
{
    BOOST_CHECK_EQUAL(FSize(0).fillBlock(), 0);
    BOOST_CHECK_EQUAL(FSize(1).fillBlock(), 1024);
    BOOST_CHECK_EQUAL(FSize(4000).fillBlock(), 4096);
    BOOST_CHECK_EQUAL(FSize(42 * 1024).fillBlock(), 42 * 1024);
    BOOST_CHECK_EQUAL(FSize(42 * 1024).fillBlock(32 * 1024), 65536);

    // fillBlock() changes the object
    int value = 42 * 1024; // 42KiB
    FSize fsize(value);
    BOOST_CHECK_EQUAL(fsize.fillBlock(32 * 1024), 65536);
    // not equal to the original size
    BOOST_CHECK_NE(fsize, value);
}

BOOST_AUTO_TEST_CASE( fullBlock )
{
    BOOST_CHECK_EQUAL(FSize(0).fullBlock(), 0);
    BOOST_CHECK_EQUAL(FSize(1).fullBlock(), 1024);
    BOOST_CHECK_EQUAL(FSize(4000).fullBlock(), 4096);
    BOOST_CHECK_EQUAL(FSize(42 * 1024).fullBlock(), 42 * 1024);
    BOOST_CHECK_EQUAL(FSize(42 * 1024).fullBlock(32 * 1024), 65536);

    // fullBlock() creates a new object
    int value = 42 * 1024; // 42KiB
    FSize fsize(value);
    BOOST_CHECK_EQUAL(fsize.fullBlock(32 * 1024), 65536);
    // not changed
    BOOST_CHECK_EQUAL(fsize, value);
}

BOOST_AUTO_TEST_CASE( bestUnit )
{
    BOOST_CHECK_EQUAL(FSize(0).bestUnit(), FSize::Unit::B);
    BOOST_CHECK_EQUAL(FSize(999).bestUnit(), FSize::Unit::B);
    BOOST_CHECK_EQUAL(FSize(1023).bestUnit(), FSize::Unit::B);
    BOOST_CHECK_EQUAL(FSize(1024).bestUnit(), FSize::Unit::K);

    BOOST_CHECK_EQUAL(FSize(1LL << 20).bestUnit(), FSize::Unit::M);
    BOOST_CHECK_EQUAL(FSize(1LL << 30).bestUnit(), FSize::Unit::G);
    BOOST_CHECK_EQUAL(FSize(1LL << 40).bestUnit(), FSize::Unit::T);
    BOOST_CHECK_EQUAL(FSize(1LL << 50).bestUnit(), FSize::Unit::P);
    BOOST_CHECK_EQUAL(FSize(1LL << 60).bestUnit(), FSize::Unit::E);
    BOOST_CHECK_EQUAL(FSize(cpp_int(1) << 70).bestUnit(), FSize::Unit::Z);
    BOOST_CHECK_EQUAL(FSize(cpp_int(1) << 80).bestUnit(), FSize::Unit::Y);
}

BOOST_AUTO_TEST_CASE( unit )
{
    BOOST_CHECK_EQUAL(FSize::unit(FSize::Unit::B), "B");
    BOOST_CHECK_EQUAL(FSize::unit(FSize::Unit::K), "KiB");
    BOOST_CHECK_EQUAL(FSize::unit(FSize::Unit::M), "MiB");
    BOOST_CHECK_EQUAL(FSize::unit(FSize::Unit::G), "GiB");
    BOOST_CHECK_EQUAL(FSize::unit(FSize::Unit::T), "TiB");
    BOOST_CHECK_EQUAL(FSize::unit(FSize::Unit::P), "PiB");
    BOOST_CHECK_EQUAL(FSize::unit(FSize::Unit::E), "EiB");
    BOOST_CHECK_EQUAL(FSize::unit(FSize::Unit::Z), "ZiB");
    BOOST_CHECK_EQUAL(FSize::unit(FSize::Unit::Y), "YiB");
}

BOOST_AUTO_TEST_CASE( form )
{
    BOOST_CHECK_EQUAL(FSize(0).form(), "0 B");
    BOOST_CHECK_EQUAL(FSize(1).form(), "1 B");
    BOOST_CHECK_EQUAL(FSize(1024).form(), "1.0 KiB");
    BOOST_CHECK_EQUAL(FSize(-1024).form(), "-1.0 KiB");
    BOOST_CHECK_EQUAL(FSize(FSize::MB).form(), "1.0 MiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(), "117.7 MiB");
    // requested unit
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::K), "120563.3 KiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::M), "117.7 MiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::G), "0.11 GiB");
    // too small for the required unit
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::T), "0.000 TiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::P), "0.000 PiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::E), "0.000 EiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::Z), "0.000 ZiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::Y), "0.000 YiB");
    
    // field width
    BOOST_CHECK_EQUAL(FSize(123456789).form(10), "     117.7 MiB");
    // the width parameter does NOT include the units!
    BOOST_CHECK_EQUAL(FSize(123456789).form(10).length(), 14);
    // does not cut numbers for longer strings
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::K, 5), "120563.3 KiB");
    
    // precision
    BOOST_CHECK_EQUAL(FSize(123456789).form(0, 3), "117.738 MiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(0, 0), "118 MiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::K, 0, 3), "120563.271 KiB");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::K, 0, 0), "120563 KiB");
    // precision for bytes is ignored
    BOOST_CHECK_EQUAL(FSize(1234).form(FSize::Unit::B, 0, 3), "1234 B");
    
    // without units
    BOOST_CHECK_EQUAL(FSize(123456789).form(0, 3, false), "117.738");
    BOOST_CHECK_EQUAL(FSize(123456789).form(0, 0, false), "118");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::K, 0, 3, false), "120563.271");
    BOOST_CHECK_EQUAL(FSize(123456789).form(FSize::Unit::K, 0, 0, false), "120563");
}

BOOST_AUTO_TEST_CASE( as_string )
{
    // the same behavior like form(), test just few examples
    BOOST_CHECK_EQUAL(FSize(0).asString(), "0 B");
    BOOST_CHECK_EQUAL(FSize(1).asString(), "1 B");
    BOOST_CHECK_EQUAL(FSize(1024).asString(), "1.0 KiB");
    BOOST_CHECK_EQUAL(FSize(-1024).asString(), "-1.0 KiB");
    BOOST_CHECK_EQUAL(FSize(123456789).asString(), "117.7 MiB");
}

template<class T> std::string to_stream(const T& object)
{
    std::ostringstream str;
    
    str << object;
    return str.str();
}

BOOST_AUTO_TEST_CASE( output_operator )
{
    BOOST_CHECK_EQUAL(to_stream(FSize(0)), "0 B");
    BOOST_CHECK_EQUAL(to_stream(FSize(1)), "1 B");
    BOOST_CHECK_EQUAL(to_stream(FSize(1024)), "1.0 KiB");
    BOOST_CHECK_EQUAL(to_stream(FSize(1024)), "1.0 KiB");
    BOOST_CHECK_EQUAL(to_stream(FSize(123456789)), "117.7 MiB");

    BOOST_CHECK_EQUAL(to_stream(FSize::Unit::B), "B");
    BOOST_CHECK_EQUAL(to_stream(FSize::Unit::K), "KiB");
    BOOST_CHECK_EQUAL(to_stream(FSize::Unit::M), "MiB");
    BOOST_CHECK_EQUAL(to_stream(FSize::Unit::G), "GiB");
    BOOST_CHECK_EQUAL(to_stream(FSize::Unit::T), "TiB");
    BOOST_CHECK_EQUAL(to_stream(FSize::Unit::P), "PiB");
    BOOST_CHECK_EQUAL(to_stream(FSize::Unit::E), "EiB");
    BOOST_CHECK_EQUAL(to_stream(FSize::Unit::Z), "ZiB");
    BOOST_CHECK_EQUAL(to_stream(FSize::Unit::Y), "YiB");
}

BOOST_AUTO_TEST_CASE( long_long_conversion )
{
    if (BOOST_VERSION >= 106700)
    {
        // Boost 1.67 returns min/max values for the values out of range
        // https://www.boost.org/users/history/version_1_67_0.html
        BOOST_CHECK_EQUAL((long long)(FSize::ZB), std::numeric_limits<long long>::max());
        BOOST_CHECK_EQUAL((long long)(-FSize::YB), std::numeric_limits<long long>::min());
    }
    else
    {
        // the cpp_int convertion is buggy in older boost
        // the older Boost (at least the 1.66 version) returns just the lower bits
        // in that case just print a warning and continue
        BOOST_WARN_EQUAL((long long)(FSize::ZB), std::numeric_limits<long long>::max());
        BOOST_WARN_EQUAL((long long)(-FSize::YB), std::numeric_limits<long long>::min());
    }
}
