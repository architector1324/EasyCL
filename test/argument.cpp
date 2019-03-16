#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "EasyCL.hpp"

TEST_CASE("Testing variable operators"){
    SECTION("Unary"){
        ecl::Variable<int> a(1);
        REQUIRE(a++ == 2);
        REQUIRE(a-- == 1);
    }

    SECTION("Binary"){
        ecl::Variable<int> a(1);
        REQUIRE((a = 3) == 3);
        REQUIRE((a += 4) == 7);
        REQUIRE((a -= 3) == 4);
        REQUIRE((a *= 2) == 8);
        REQUIRE((a /= 4) == 2);
    }
}

TEST_CASE("Testing array operators"){
    ecl::Array<int> a(new int[5]{1, 2, 3, 4, 5}, 5, ecl::CONTROL::FREE);
    for(size_t i = 0; i < 5; i++)
        REQUIRE(a[i] == i + 1);
}