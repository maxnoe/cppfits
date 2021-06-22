#define CATCH_CONFIG_MAIN  
#include <catch2/catch.hpp>
#include "fits/header.h"

TEST_CASE("left strip") {
    using fits::left_strip;

    REQUIRE(left_strip("foo") == "foo");
    REQUIRE(left_strip(" foo") == "foo");
    REQUIRE(left_strip("  foo") == "foo");
    REQUIRE(left_strip("  foo  ") == "foo  ");
    REQUIRE(left_strip(" foo  ") == "foo  ");
    REQUIRE(left_strip("foo  ") == "foo  ");
}

TEST_CASE("right strip") {
    using fits::right_strip;

    REQUIRE(right_strip("foo") == "foo");
    REQUIRE(right_strip(" foo") == " foo");
    REQUIRE(right_strip("  foo") == "  foo");
    REQUIRE(right_strip("  foo  ") == "  foo");
    REQUIRE(right_strip(" foo  ") == " foo");
    REQUIRE(right_strip("foo  ") == "foo");
}

TEST_CASE("strip") {
    using fits::strip;
    REQUIRE(strip("foo") == "foo");
    REQUIRE(strip(" foo") == "foo");
    REQUIRE(strip("  foo") == "foo");
    REQUIRE(strip("  foo  ") == "foo");
    REQUIRE(strip(" foo  ") == "foo");
    REQUIRE(strip("foo  ") == "foo");
    REQUIRE(strip(" foo bar ") == "foo bar");
}



TEST_CASE("HeaderEntry::parse") {
    using fits::HeaderEntry;
    HeaderEntry e;

    e = HeaderEntry::parse("SIMPLE  =                    T / conforms to FITS standard                      ");
    REQUIRE(e.key == "SIMPLE");
    REQUIRE(std::get<bool>(e.value));
    REQUIRE(e.comment == "conforms to FITS standard");

    e = HeaderEntry::parse("DATE-OBS= '2020-01-01T20:00:00'                                                 ");
    REQUIRE(e.key == "DATE-OBS");
    REQUIRE(std::get<std::string>(e.value) == "2020-01-01T20:00:00");

    e = HeaderEntry::parse("DATE-OBS= 'Foo'                                                                 ");
    REQUIRE(e.key == "DATE-OBS");
    REQUIRE(std::get<std::string>(e.value) == "Foo");

    e = HeaderEntry::parse("DATE-OBS= 'Foo''s bar'                                                          ");
    REQUIRE(e.key == "DATE-OBS");
    REQUIRE(std::get<std::string>(e.value) == "Foo's bar");

    e = HeaderEntry::parse("NAXIS1  = 12345                                                                 ");
    REQUIRE(e.key == "NAXIS1");
    REQUIRE(std::get<int64_t>(e.value) == 12345);

    e = HeaderEntry::parse("FOOBAR  = -12345                                                                ");
    REQUIRE(std::get<int64_t>(e.value) == -12345);

    e = HeaderEntry::parse("FOOBAR  = +12345                                                                ");
    REQUIRE(std::get<int64_t>(e.value) == 12345);

    e = HeaderEntry::parse("FOOBAR  = +12.345                                                               ");
    REQUIRE(std::get<double>(e.value) == 12.345);

    e = HeaderEntry::parse("FOOBAR  = +12.345e6                                                             ");
    REQUIRE(std::get<double>(e.value) == 12.345e6);

    e = HeaderEntry::parse("COMMENT Hello this is a comment                                                 ");
    REQUIRE(e.key == "COMMENT");
    REQUIRE(e.comment == "Hello this is a comment");

    e = HeaderEntry::parse("HISTORY Hello this is a history                                                 ");
    REQUIRE(e.key == "HISTORY");
    REQUIRE(e.comment == "Hello this is a history");
}
