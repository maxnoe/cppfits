#include <catch2/catch.hpp>
#include "fits/string_utils.h"

TEST_CASE("left strip") {
    using fits::left_strip;

    REQUIRE(left_strip("foo") == "foo");
    REQUIRE(left_strip(" foo") == "foo");
    REQUIRE(left_strip("  foo") == "foo");
    REQUIRE(left_strip("  foo  ") == "foo  ");
    REQUIRE(left_strip(" foo  ") == "foo  ");
    REQUIRE(left_strip("foo  ") == "foo  ");
    REQUIRE(left_strip("foo bar  ") == "foo bar  ");
    REQUIRE(left_strip("  foo  bar  ") == "foo  bar  ");
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

