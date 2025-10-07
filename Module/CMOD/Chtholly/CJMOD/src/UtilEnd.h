#ifndef CJMOD_UTILEND_H
#define CJMOD_UTILEND_H

#include "../../../../../src/Common.h"
#include "../../../../../src/CHTL-JS/CJMODSystem/CJMODApi.h"

// util...end 表达式 CJMOD模块
// 提供条件监视和变化响应功能

namespace Chtholly {
namespace CJMOD {

// util...end 处理器
class UtilEndProcessor {
public:
    // 处理 util...end 语法
    static CHTL::CJMOD::Arg process(const String& code);
    
    // 注册语法
    static void registerSyntax();
    
private:
    struct UtilEndExpr {
        String condition;     // 条件表达式
        String changeBody;    // change 块内容
        String endBody;       // end 块内容
        bool singleLine;      // 是否是单行
    };
    
    static UtilEndExpr parseExpression(const String& code);
    static String generateJavaScript(const UtilEndExpr& expr);
};

} // namespace CJMOD
} // namespace Chtholly

#endif // CJMOD_UTILEND_H
