#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <EasyCL/EasyCL.hpp>

TEST_CASE("Default Constructor") {
	ecl::var<int> variable;
	variable.setValue(3);
	CHECK(variable.getValue() == 3);
}

TEST_CASE("Overloaded Operators"){
	SECTION("Unary"){
		ecl::var<int> variable = 1;
		REQUIRE(variable == 1);
		CHECK(variable++ == 2);
        CHECK(variable-- == 1);
    }
    SECTION("Binary"){
		ecl::var<int> variable = 1;
		REQUIRE(variable == 1);
		SECTION("Comparison") {
			CHECK(variable == 1);
			CHECK(variable != 3);
		}
		SECTION("Arithmetic") {
			CHECK((variable + 3) == 4);
			CHECK((variable - 2) == -1);
			CHECK((variable * 3) == 3);
			CHECK((variable / 2) == 0);
		}
		SECTION("Assignment") {
			CHECK((variable = 3) == 3);
			CHECK((variable += 4) == 7);
			CHECK((variable -= 3) == 4);
			CHECK((variable *= 2) == 8);
			CHECK((variable /= 4) == 2);
		}
    }
	SECTION("Implicit User Defined Conversion") {
		ecl::var<int> a = 5;
		ecl::var<int> b = 4;
		REQUIRE(a == 5);
		REQUIRE(b == 4);
		CHECK((a += b) == 9);
		CHECK((a -= b) == 5);
	}
}