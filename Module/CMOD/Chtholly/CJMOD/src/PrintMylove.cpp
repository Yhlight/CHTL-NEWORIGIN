#include "../../../../../src/CHTL-JS/CJMODSystem/CJMODApi.h"
#include <iostream>

using namespace CHTL;
using namespace CHTL::CJMOD;

// ========================================
// PrintMylove - 图片转字符画功能
// ========================================

namespace {

// 注册PrintMylove语法
void registerPrintMyloveSyntax() {
    CJMODRegistry::SyntaxInfo info;
    info.pattern = "PrintMylove { url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_ }";
    info.description = "将图片转换为字符像素块或ASCII形式并输出到控制台";
    info.examples = {
        "const str = PrintMylove { url: \"image.jpg\", mode: \"ASCII\" };",
        "const str = PrintMylove { url: \"photo.png\", mode: \"Pixel\", scale: 0.5 };",
        "PrintMylove { url: \"avatar.jpg\", width: \"100px\", height: \"100px\" };"
    };
    
    CJMODRegistry::registerSyntax("PrintMylove", info);
}

// PrintMylove转换函数
Arg processPrintMylove(const String& code) {
    // 分析语法
    Arg pattern = Syntax::analyze("PrintMylove { url: $!_, mode: $?_, width: $?_, height: $?_, scale: $?_ }");
    
    // 绑定参数处理函数
    pattern.bind("$!_", [](const String& value) {
        // 处理url参数
        return value;
    });
    
    pattern.bind("$?_", [](const String& value) {
        // 处理可选参数
        if (value.empty()) {
            return String("undefined");
        }
        return value;
    });
    
    // 扫描代码
    Arg result = CJMODScanner::scan(pattern, "PrintMylove", code);
    
    // 转换为JS代码
    std::string jsCode = R"(
(function() {
    const PrintMylove_impl = function(config) {
        const { url, mode = 'Pixel', width, height, scale = 1.0 } = config;
        
        // 创建临时图片元素
        const img = new Image();
        img.crossOrigin = 'Anonymous';
        
        return new Promise((resolve, reject) => {
            img.onload = function() {
                try {
                    const canvas = document.createElement('canvas');
                    const ctx = canvas.getContext('2d');
                    
                    // 计算尺寸
                    let w = width || img.width;
                    let h = height || img.height;
                    
                    if (scale !== 1.0) {
                        w = Math.floor(w * scale);
                        h = Math.floor(h * scale);
                    }
                    
                    canvas.width = w;
                    canvas.height = h;
                    
                    // 绘制图片
                    ctx.drawImage(img, 0, 0, w, h);
                    
                    // 获取像素数据
                    const imageData = ctx.getImageData(0, 0, w, h);
                    const pixels = imageData.data;
                    
                    let result = '';
                    
                    if (mode === 'ASCII') {
                        // ASCII模式
                        const chars = ' .:-=+*#%@';
                        for (let y = 0; y < h; y += 2) {
                            for (let x = 0; x < w; x++) {
                                const i = (y * w + x) * 4;
                                const r = pixels[i];
                                const g = pixels[i + 1];
                                const b = pixels[i + 2];
                                const brightness = (r + g + b) / 3;
                                const charIndex = Math.floor(brightness / 255 * (chars.length - 1));
                                result += chars[charIndex];
                            }
                            result += '\n';
                        }
                    } else {
                        // Pixel模式 - 使用彩色字符
                        for (let y = 0; y < h; y += 2) {
                            for (let x = 0; x < w; x++) {
                                const i = (y * w + x) * 4;
                                const r = pixels[i];
                                const g = pixels[i + 1];
                                const b = pixels[i + 2];
                                result += `\x1b[38;2;${r};${g};${b}m█\x1b[0m`;
                            }
                            result += '\n';
                        }
                    }
                    
                    resolve(result);
                } catch (error) {
                    reject(error);
                }
            };
            
            img.onerror = reject;
            img.src = url;
        });
    };
    
    // 支持虚对象Vir
    window.PrintMylove = PrintMylove_impl;
    return PrintMylove_impl;
})()
)";
    
    result.transform(jsCode);
    return result;
}

// 自动注册（模块加载时调用）
class PrintMyloveRegistrar {
public:
    PrintMyloveRegistrar() {
        registerPrintMyloveSyntax();
    }
};

static PrintMyloveRegistrar registrar;

} // anonymous namespace

// 导出函数供CHTL编译器调用
extern "C" {
    Arg CJMOD_PrintMylove_Process(const String& code) {
        return processPrintMylove(code);
    }
}
