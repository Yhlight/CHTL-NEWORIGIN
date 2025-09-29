#include "CJMOD/Syntax.h"
#include "CJMOD/Arg.h"
#include <string>
#include <sstream>

// This is a placeholder for a real API that would be passed by the compiler.
struct CJMOD_API {
    void (*register_syntax)(const std::string& name, std::function<std::string(Arg&)> transform_func);
};

// The function that the CHTL compiler will look for when loading this module.
extern "C" void register_cjmod(CJMOD_API* api) {
    if (!api) {
        return;
    }

    // Register the "printMylove" function.
    api->register_syntax("printMylove", [](Arg& args) -> std::string {
        // In a real implementation, 'args' would be populated by the compiler
        // with the values from the CHTL JS call, e.g., args.getValue("url").
        // For this proof-of-concept, we'll hardcode some values.
        std::string url = "https://img.moegirl.org.cn/common/thumb/f/f5/珂朵莉·诺塔.png/260px-珂朵莉·诺塔.png";
        std::string mode = "ASCII";
        int width = 100;

        std::stringstream ss;
        // This is the standard JavaScript that will be generated and executed in the browser.
        ss << "(function() {\n";
        ss << "    const imageUrl = '" << url << "';\n";
        ss << "    const asciiWidth = " << width << ";\n";
        ss << "    const asciiChars = '`.-_':,;^=+/\"|)\\<>)iv%xclrs}1]*?sL7jCf)tI{3Sc?]2(5Z0wPQKASU$X#bH&d46gE_R@N';\n";
        ss << "    const img = new Image();\n";
        ss << "    img.crossOrigin = 'Anonymous';\n";
        ss << "    img.onload = () => {\n";
        ss << "        const aspectRatio = img.height / img.width;\n";
        ss << "        const asciiHeight = Math.floor(aspectRatio * asciiWidth * 0.5);\n";
        ss << "        const canvas = document.createElement('canvas');\n";
        ss << "        canvas.width = asciiWidth;\n";
        ss << "        canvas.height = asciiHeight;\n";
        ss << "        const ctx = canvas.getContext('2d');\n";
        ss << "        ctx.drawImage(img, 0, 0, asciiWidth, asciiHeight);\n";
        ss << "        const imageData = ctx.getImageData(0, 0, asciiWidth, asciiHeight);\n";
        ss << "        let asciiArt = '';\n";
        ss << "        for (let i = 0; i < imageData.data.length; i += 4) {\n";
        ss << "            const gray = (imageData.data[i] * 0.3 + imageData.data[i+1] * 0.59 + imageData.data[i+2] * 0.11);\n";
        ss << "            const charIndex = Math.floor((gray / 255) * (asciiChars.length - 1));\n";
        ss << "            asciiArt += asciiChars[charIndex];\n";
        ss << "            if ((i / 4 + 1) % asciiWidth === 0) asciiArt += '\\n';\n";
        ss << "        }\n";
        ss << "        console.log(`%c${asciiArt}`, 'font-family: monospace; font-size: 8px; line-height: 6px;');\n";
        ss << "    };\n";
        ss << "    img.onerror = () => console.error('Failed to load image for printMylove.');\n";
        ss << "    img.src = imageUrl;\n";
        ss << "})();\n";

        return ss.str();
    });
}