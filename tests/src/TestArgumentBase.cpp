#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <EasyCL/EasyCL.hpp>

TEST_CASE("Default Constructor") {
	ecl::ArgumentBase argument_base;
	REQUIRE(argument_base.getDataPtr() == nullptr);
	REQUIRE(argument_base.getDataSize() == 0);
	REQUIRE(argument_base.getMemoryType() == 0);
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