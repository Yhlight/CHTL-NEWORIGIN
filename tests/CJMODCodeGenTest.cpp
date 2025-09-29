#include "../catch.hpp"
#include "Dispatcher/CompilerDispatcher.h"
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE("CJMOD Code Generation End-to-End Test", "[cjmod_codegen]") {
    // This path assumes the test is run from the build directory,
    // and the cjmod is in a 'cjmods' subdirectory, as configured in CMakeLists.txt.
    fs::path cjmod_path = fs::current_path() / "cjmods";

    // Construct the platform-specific shared library name
#ifdef _WIN32
    cjmod_path /= "printMylove_cjmod.dll";
#elif __APPLE__
    cjmod_path /= "libprintMylove_cjmod.dylib";
#else
    cjmod_path /= "libprintMylove_cjmod.so";
#endif

    REQUIRE(fs::exists(cjmod_path));

    std::string source = R"(
        [Import] @CJmod from ")";
    source += cjmod_path.string();
    source += R"(";

        script {
            printMylove {}
        }
    )";

    // Compile the source using the main dispatcher
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", false, false, false, "", "");

    // Check that the generated JS contains the code from our CJMOD
    REQUIRE(result.js_content.find("const asciiChars = '`.-_':,;^=+/\"|)\\<>)iv%xclrs}1]*?sL7jCf)tI{3Sc?]2(5Z0wPQKASU$X#bH&d46gE_R@N';") != std::string::npos);
    REQUIRE(result.js_content.find("img.onload = () => {") != std::string::npos);
    REQUIRE(result.js_content.find("console.log(`%c${asciiArt}`") != std::string::npos);
}