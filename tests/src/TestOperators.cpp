#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <EasyCL/EasyCL.hpp>

TEST_CASE("Testing variable operators"){
    SECTION("Unary"){
        ecl::Variable<int> a = 1;
        bool first = a++ == 2;
        CHECK(first);
        // CHECK(a-- == 1);
    }

    SECTION("Binary"){
        ecl::Variable<int> a = 1;
        // CHECK((a = 3) == 3);
        // CHECK((a += 4) == 7);
        // CHECK((a -= 3) == 4);
        // CHECK((a *= 2) == 8);
        // CHECK((a /= 4) == 2);
    }
}

TEST_CASE("Testing array operators"){
    ecl::Array<int> a(new int[5]{1, 2, 3, 4, 5}, 5, ecl::CONTROL::BIND);
    for(size_t i = 0; i < 5; i++)
        CHECK(a[i] == i + 1);
}