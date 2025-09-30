#include "catch.hpp"
#include "sass/sass.h" // Include the libsass header

TEST_CASE("LibSass API Integration", "[integration][libsass]") {
    SECTION("libsass_version() should return a valid version string") {
        const char* version = libsass_version();
        INFO("LibSass version reported: " << (version ? version : "null"));

        // Check that the returned version string is not null or empty.
        // This confirms that we can successfully call a function from the linked library.
        REQUIRE(version != nullptr);
        REQUIRE(std::string(version).length() > 0);
    }
}