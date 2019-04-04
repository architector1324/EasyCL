#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <EasyCL/EasyCL.hpp>

TEST_CASE("Default Constructor") {
	// TODO
}

TEST_CASE("Overloaded Operators"){
    SECTION("Unary"){
        ecl::Variable<int> a = 1;
        bool test = a++ == 2;
        CHECK(test);

		test = a-- == 1;
        CHECK(test);
    }

    SECTION("Binary"){
        ecl::Variable<int> a = 1;
		bool test = false;

		SECTION("Comparison") {
			test = (a == 1);
			CHECK(test);

			test = (a != 3);
			CHECK(test);
		}

		SECTION("Arithmetic") {
			test = (a + 3) == 4;
			CHECK(test);

			test = (a - 2) == -1;
			CHECK(test);

			test = (a * 3) == 3;
			CHECK(test);

			test = (a / 2) == 0;
			CHECK(test);
		}

		SECTION("Assignment") {
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
    }

	SECTION("Implicit User Defined Conversion") {
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