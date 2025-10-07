#include "../../../src/CHTL-JS/CJMODSystem/CJMODApi.h"
#include <string>
#include <sstream>

using namespace CHTL::CJMOD;

// PrintMylove CJMOD实现
// 将图片转换为字符像素块或ASCII艺术

extern "C" {

// 初始化PrintMylove函数
void InitPrintMylove() {
    // 创建PrintMylove函数签名
    // PrintMylove { url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_ }
    Arg signature = Syntax::analyze("PrintMylove { url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_ }");
    
    // 绑定为CHTL JS函数
    CHTLJSFunction::bindVirtualObject("PrintMylove");
}

// 处理PrintMylove函数调用
std::string ProcessPrintMylove(const std::string& code) {
    // 创建模式
    Arg pattern = Syntax::analyze("PrintMylove { url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_ }");
    
    // 绑定参数处理函数
    pattern.bind("url", [](const std::string& value) -> std::string {
        return value;  // URL直接返回
    });
    
    pattern.bind("mode", [](const std::string& value) -> std::string {
        if (value.empty()) return "\"Pixel\"";  // 默认值
        return value;
    });
    
    pattern.bind("width", [](const std::string& value) -> std::string {
        if (value.empty()) return "100";
        return value;
    });
    
    pattern.bind("height", [](const std::string& value) -> std::string {
        if (value.empty()) return "100";
        return value;
    });
    
    pattern.bind("scale", [](const std::string& value) -> std::string {
        if (value.empty()) return "1.0";
        return value;
    });
    
    // 扫描代码
    CJMODScanner::setSourceCode(code);
    Arg scanned = CJMODScanner::scan(pattern, "PrintMylove");
    
    // 填充值
    pattern.fillValue(scanned);
    
    // 生成JS代码
    std::ostringstream output;
    output << "(function() {\n";
    output << "    // PrintMylove - 图片转字符像素块\n";
    output << "    const printMyloveConfig = {\n";
    output << "        url: " << pattern[0].value << ",\n";
    output << "        mode: " << pattern[1].value << ",\n";
    output << "        width: " << pattern[2].value << ",\n";
    output << "        height: " << pattern[3].value << ",\n";
    output << "        scale: " << pattern[4].value << "\n";
    output << "    };\n";
    output << "    \n";
    output << "    return (function printMylove(config) {\n";
    output << "        const img = new Image();\n";
    output << "        img.crossOrigin = 'Anonymous';\n";
    output << "        img.src = config.url;\n";
    output << "        \n";
    output << "        return new Promise((resolve) => {\n";
    output << "            img.onload = function() {\n";
    output << "                const canvas = document.createElement('canvas');\n";
    output << "                const ctx = canvas.getContext('2d');\n";
    output << "                \n";
    output << "                const w = parseInt(config.width) || 100;\n";
    output << "                const h = parseInt(config.height) || 100;\n";
    output << "                canvas.width = w;\n";
    output << "                canvas.height = h;\n";
    output << "                \n";
    output << "                ctx.drawImage(img, 0, 0, w, h);\n";
    output << "                const imageData = ctx.getImageData(0, 0, w, h).data;\n";
    output << "                \n";
    output << "                let result = '';\n";
    output << "                const chars = config.mode === 'ASCII' ? \n";
    output << "                    ' .:-=+*#%@' : '░▒▓█';\n";
    output << "                \n";
    output << "                for (let i = 0; i < h; i++) {\n";
    output << "                    for (let j = 0; j < w; j++) {\n";
    output << "                        const idx = (i * w + j) * 4;\n";
    output << "                        const r = imageData[idx];\n";
    output << "                        const g = imageData[idx + 1];\n";
    output << "                        const b = imageData[idx + 2];\n";
    output << "                        const brightness = (r + g + b) / 3;\n";
    output << "                        const charIdx = Math.floor(brightness / 255 * (chars.length - 1));\n";
    output << "                        result += chars[charIdx];\n";
    output << "                    }\n";
    output << "                    result += '\\n';\n";
    output << "                }\n";
    output << "                \n";
    output << "                resolve(result);\n";
    output << "            };\n";
    output << "        });\n";
    output << "    })(printMyloveConfig);\n";
    output << "})()";
    
    pattern.transform(output.str());
    CJMODGenerator::exportResult(pattern);
    
    return pattern.getResult();
}

} // extern "C"
