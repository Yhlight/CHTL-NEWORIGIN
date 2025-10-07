#ifndef CJMOD_INEVERAWAY_H
#define CJMOD_INEVERAWAY_H

#include "../../../../../src/Common.h"
#include "../../../../../src/CHTL-JS/CJMODSystem/CJMODApi.h"

// INeverAway CJMOD模块
// 提供标记函数和状态区分的同名重载功能

namespace Chtholly {
namespace CJMOD {

// INeverAway 处理器
class INeverAwayProcessor {
public:
    // 处理 INeverAway 语法
    static CHTL::CJMOD::Arg process(const String& code);
    
    // 注册语法
    static void registerSyntax();
    
private:
    // 解析函数定义
    struct FunctionDef {
        String name;          // 函数名（可能带状态标记 <A>）
        String state;         // 状态标记（A, B, C等）
        String returnType;    // 返回类型
        String params;        // 参数列表
        String body;          // 函数体
    };
    
    static Vector<FunctionDef> parseFunctions(const String& code);
    static String generateJavaScript(const Vector<FunctionDef>& functions, const String& virName);
};

} // namespace CJMOD
} // namespace Chtholly

#endif // CJMOD_INEVERAWAY_H
