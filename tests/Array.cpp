#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <EasyCL/EasyCL.hpp>

TEST_CASE("Default Constructor") {
	ecl::array<int> array;
	REQUIRE(array.getArray() == nullptr);
	SECTION("Set array") {
		// TODO
	}
}

TEST_CASE("Overloaded Constructor 1") {
	ecl::array<int> array(5);
	REQUIRE(array.getArray() != nullptr);
	SECTION("Set array") {
		// TODO
	}
}

TEST_CASE("Overloaded Constructor 2") {
	int A[] = { 0, 1, 2, 3, 4 };
	int B[] = { 0, 2, 4, 6, 8 };
	SECTION("Read Access") {
		ecl::array<int> array(5, ecl::ACCESS::READ);
		REQUIRE(array.getArray() != nullptr);
		SECTION("Set array") {
			// TODO
		}
	}
	SECTION("Write Access") {
		ecl::array<int> array(5, ecl::ACCESS::WRITE);
		REQUIRE(array.getArray() != nullptr);
		SECTION("Set array") {
			// TODO
		}
	}
	SECTION("Read + Write Access") {
		ecl::array<int> array(5, ecl::ACCESS::READ_WRITE);
		REQUIRE(array.getArray() != nullptr);
		SECTION("Set array") {
			// TODO
		}
	}
}

TEST_CASE("Overloaded Constructor 3") {
	SECTION("Set array") {
		// TODO
	}
}

TEST_CASE("Overloaded Constructor 4") {
	int A[] = { 0, 1, 2, 3, 4 };
	ecl::array<int> array(A, 5);
	REQUIRE(array.getArray() != nullptr);
	for (std::size_t i = 0; i < 5; i++) {
		CHECK(array[i] == A[i]);
	}
}

TEST_CASE("Copy Constructor") {
	int A[] = { 0, 1, 2, 3, 4 };
	ecl::array<int> array1(A, 5);
	ecl::array<int> array2 = array1;
	REQUIRE(array1.getArray() != nullptr);
	REQUIRE(array2.getArray() != nullptr);
	for (std::size_t i = 0; i < 5; i++) {
		CHECK(array1[i] == A[i]);
		CHECK(array2[i] == A[i]);
	}
}

TEST_CASE("Move Constructor") {
	int A[] = { 0, 1, 2, 3, 4 };
	ecl::array<int> array1(A, 5);
	ecl::array<int> array2 = std::move(array1);
	REQUIRE(array1.getArray() == nullptr);
	REQUIRE(array2.getArray() != nullptr);
	for (std::size_t i = 0; i < 5; i++) {
		CHECK(array2[i] == A[i]);
	}
}

TEST_CASE("Copy Assign Operator"){
	int A[] = { 0, 1, 2, 3, 4 };
	ecl::array<int> array1(A, 5);
	ecl::array<int> array2(5);
	array2 = array1;
	REQUIRE(array1.getArray() != nullptr);
	REQUIRE(array2.getArray() != nullptr);
	for (std::size_t i = 0; i < 5; i++) {
		CHECK(array2[i] == array1[i]);
	}
}

TEST_CASE("Move Assignment Operator") {
	int A[] = { 0, 1, 2, 3, 4 };
	ecl::array<int> array1(A, 5);
	ecl::array<int> array2(5);
	array2 = std::move(array1);
	REQUIRE(array1.getArray() == nullptr);
	REQUIRE(array2.getArray() != nullptr);
	for (std::size_t i = 0; i < 5; i++) {
		CHECK(array2[i] == A[i]);
	}
}

TEST_CASE("Overloaded Operators"){
	int A[] = { 0, 1, 2, 3, 4 };
    ecl::array<int> array(A, 5);
	SECTION("Subscript Operator") {
		for (std::size_t i = 0; i < 5; i++) {
			CHECK(array[i] == A[i]);
		}
	}
	SECTION("Implicit User Defined Conversion") {
		CHECK(array == A);
	}
}