#include "../catch.hpp"
#include "quickjs/quickjs.h" // Include the QuickJS header

TEST_CASE("QuickJS API Integration", "[integration][quickjs]") {
    SECTION("JS_NewRuntime and JS_FreeRuntime should execute without errors") {
        // Create a new QuickJS runtime.
        JSRuntime* rt = JS_NewRuntime();

        // Check that the runtime was created successfully.
        REQUIRE(rt != nullptr);

        // Create a new QuickJS context.
        JSContext* ctx = JS_NewContext(rt);
        REQUIRE(ctx != nullptr);

        // Free the context and runtime to confirm cleanup functions can be called.
        JS_FreeContext(ctx);
        JS_FreeRuntime(rt);
    }
}