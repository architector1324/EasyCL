#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <EasyCL/EasyCL.hpp>

TEST_CASE("Default Constructor") {
	ecl::Buffer buffer(nullptr, 0, ecl::ACCESS::READ);
	REQUIRE(buffer.getDataPtr() == nullptr);
	REQUIRE(buffer.getDataSize() == 0);
	REQUIRE(buffer.getMemoryType() == 0);
	// TODO
}

TEST_CASE("Overloaded Constructor 1") {
	// TODO
}

TEST_CASE("Overloaded Constructor 2") {
	// TODO
}

TEST_CASE("Move Constructor") {
	// TODO
}

TEST_CASE("Move Assignment Operator") {
	// TODO
}

// TODO