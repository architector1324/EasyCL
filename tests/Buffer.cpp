#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <EasyCL/EasyCL.hpp>

TEST_CASE("Constructor") {
	ecl::Buffer buffer(nullptr, 0, ecl::ACCESS::READ);
	REQUIRE(buffer.getPtr() == nullptr);
	REQUIRE(buffer.getSize() == 0);
	REQUIRE(buffer.getAccess() == ecl::ACCESS::READ);
	// TODO
}

TEST_CASE("Copy Constructor") {
	// TODO
}

TEST_CASE("Copy Assignment Operator") {
	// TODO
}

TEST_CASE("Move Constructor") {
	// TODO
}

TEST_CASE("Move Assignment Operator") {
	// TODO
}

// TODO