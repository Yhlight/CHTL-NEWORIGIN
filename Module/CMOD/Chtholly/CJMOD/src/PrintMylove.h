#ifndef CJMOD_PRINTMYLOVE_H
#define CJMOD_PRINTMYLOVE_H

#include "../../../../../src/Common.h"
#include "../../../../../src/CHTL-JS/CJMODSystem/CJMODApi.h"

// PrintMylove CJMOD模块
// 提供图片转字符画功能

namespace Chtholly {
namespace CJMOD {

// PrintMylove配置
struct PrintMyloveConfig {
    String url;           // 图片URL（必需）
    String mode;          // 模式：ASCII 或 Pixel（可选，默认Pixel）
    String width;         // 宽度（可选）
    String height;        // 高度（可选）
    double scale;         // 缩放倍数（可选，默认1.0）
    
    PrintMyloveConfig() : mode("Pixel"), scale(1.0) {}
};

// PrintMylove处理器
class PrintMyloveProcessor {
public:
    static CHTL::CJMOD::Arg process(const String& code);
    static void registerSyntax();
    
private:
    static CHTL::CJMOD::Arg buildJavaScriptCode(const PrintMyloveConfig& config);
};

} // namespace CJMOD
} // namespace Chtholly

#endif // CJMOD_PRINTMYLOVE_H
