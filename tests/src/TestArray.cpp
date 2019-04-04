#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <EasyCL/EasyCL.hpp>

TEST_CASE("Default Constructor") {
	ecl::Array<int> array;
	SECTION("Set Array") {
		// TODO
	}
	SECTION("Get Array") {
		REQUIRE(array.getArray() == nullptr);
	}
}

TEST_CASE("Overloaded Constructor 1") {
	ecl::Array<int> array(5);
	SECTION("Set Array") {
		// TODO
	}
	SECTION("Get Array") {
		REQUIRE(array.getArray() != nullptr);
	}
}

TEST_CASE("Overloaded Constructor 2") {
	int A[] = { 0, 1, 2, 3, 4 };
	int B[] = { 0, 2, 4, 6, 8 };
	SECTION("Read Access") {
		ecl::Array<int> array(5, ecl::ACCESS::READ);
		SECTION("Set Array") {
			// TODO
		}
		SECTION("Get Array") {
			// TODO
		}
	}
	SECTION("Write Access") {
		ecl::Array<int> array(5, ecl::ACCESS::WRITE);
		SECTION("Set Array") {
			// TODO
		}
		SECTION("Get Array") {
			// TODO
		}
	}
	SECTION("Read + Write Access") {
		ecl::Array<int> array(5, ecl::ACCESS::READ_WRITE);
		SECTION("Set Array") {
			// TODO
		}
		SECTION("Get Array") {
			// TODO
		}
	}
}

TEST_CASE("Overloaded Constructor 3") {
	SECTION("Set Array") {
		// TODO
	}
	SECTION("Get Array") {
		// TODO
	}
}

TEST_CASE("Overloaded Constructor 4") {
	int A[] = { 0, 1, 2, 3, 4 };
	ecl::Array<int> array(A, 5);
	SECTION("Set Array") {
		// TODO
	}
	SECTION("Get Array") {
		REQUIRE(array.getArray() != nullptr);
		for (std::size_t i = 0; i < 5; i++) {
			REQUIRE(array[i] == A[i]);
		}
	}
}

TEST_CASE("Copy Constructor") {
	SECTION("Set Array") {
		// TODO
	}
	SECTION("Get Array") {
		// TODO
	}
}

TEST_CASE("Move Constructor") {
	SECTION("Set Array") {
		// TODO
	}
	SECTION("Get Array") {
		// TODO
	}
}

TEST_CASE("Overloaded Operators"){
	int A[] = { 0, 1, 2, 3, 4 };
    ecl::Array<int> array(A, 5);
	SECTION("Subscript Operator") {
		for (std::size_t i = 0; i < 5; i++) {
			CHECK(array[i] == A[i]);
		}
	}
	SECTION("Implicit User Defined Conversion") {
		bool test = (array == A);
		CHECK(test);
	}
}