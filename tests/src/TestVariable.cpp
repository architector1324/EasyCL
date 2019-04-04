#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <EasyCL/EasyCL.hpp>

TEST_CASE("Default Constructor") {
	// TODO
}

TEST_CASE("Overloaded Operators"){
    SECTION("unary"){
        ecl::Variable<int> a = 1;
        bool test = a++ == 2;
        CHECK(test);

		test = a-- == 1;
        CHECK(test);
    }

    SECTION("binary"){
        ecl::Variable<int> a = 1;

		bool test = (a == 1);
		CHECK(test);
		
		test = (a != 2);
		CHECK(test);

		test = (a = 3) == 3;
        CHECK(test);

		test = (a += 4) == 7;
        CHECK(test);

		test = (a -= 3) == 4;
        CHECK(test);

		test = (a *= 2) == 8;
        CHECK(test);

		test = (a /= 4) == 2;
        CHECK(test);
    }

	SECTION("friend") {
		ecl::Variable<int> a = 5;

		bool test = (a + 1) == 6;
		CHECK(test);

		test = (a - 1) == 4;
		CHECK(test);

		test = (a * 3) == 15;
		CHECK(test);

		test = (a / 3) == 1;
		CHECK(test);
	}

	SECTION("cast") {
		ecl::Variable<int> a = 5;
		ecl::Variable<int> b = 4;

		auto f = [](const int& v) {
			return v + 1;
		};

		a += b;
		bool test = (a == 9);
		CHECK(test);

		test = (a - b) == 5;
		CHECK(test);

		test = f(a) == 10;
		CHECK(test);
	}
}