#include "UtilEnd.h"
#include "../../../../../src/Util/StringUtil/StringUtil.h"
#include <sstream>

using namespace CHTL;
using namespace CHTL::CJMOD;
using namespace CHTL::Util;

namespace Chtholly {
namespace CJMOD {

void UtilEndProcessor::registerSyntax() {
    CJMODRegistry::SyntaxInfo info;
    info.pattern = "util $_ -> change { ... } -> end { ... }";
    info.description = "条件监视表达式，当条件变化时触发change，条件满足时触发end";
    info.examples = {
        "util a > b -> change { print('changed'); } -> end { print('a > b'); }",
        "util a > b -> change print('changed') -> end print('done');",
        "util x < 100 -> change { updateUI(); } -> end { complete(); }"
    };
    
    CJMODRegistry::registerSyntax("util...end", info);
}

Arg UtilEndProcessor::process(const String& code) {
    Arg result;
    
    auto expr = parseExpression(code);
    String jsCode = generateJavaScript(expr);
    
    result.transform(jsCode);
    return result;
}

UtilEndProcessor::UtilEndExpr UtilEndProcessor::parseExpression(const String& code) {
    UtilEndExpr expr;
    expr.singleLine = false;
    
    // 查找 util 关键字
    size_t utilPos = code.find("util ");
    if (utilPos == String::npos) {
        return expr;
    }
    
    // 提取条件表达式（util 和第一个 -> 之间）
    size_t firstArrow = code.find("->", utilPos);
    if (firstArrow == String::npos) {
        return expr;
    }
    
    expr.condition = StringUtil::trim(code.substr(utilPos + 5, firstArrow - utilPos - 5));
    
    // 查找 change 关键字
    size_t changePos = code.find("change", firstArrow);
    if (changePos == String::npos) {
        return expr;
    }
    
    // 提取 change 内容
    size_t changeStart = changePos + 6;
    changeStart = code.find_first_not_of(" \t\n", changeStart);
    
    if (changeStart != String::npos && code[changeStart] == '{') {
        // 多行模式
        int depth = 1;
        size_t changeEnd = changeStart + 1;
        while (changeEnd < code.length() && depth > 0) {
            if (code[changeEnd] == '{') depth++;
            else if (code[changeEnd] == '}') depth--;
            changeEnd++;
        }
        expr.changeBody = code.substr(changeStart + 1, changeEnd - changeStart - 2);
    } else {
        // 单行模式
        expr.singleLine = true;
        size_t secondArrow = code.find("->", changeStart);
        if (secondArrow != String::npos) {
            expr.changeBody = StringUtil::trim(code.substr(changeStart, secondArrow - changeStart));
        }
    }
    
    // 查找 end 关键字
    size_t endPos = code.find("end", changeStart);
    if (endPos == String::npos) {
        return expr;
    }
    
    // 提取 end 内容
    size_t endStart = endPos + 3;
    endStart = code.find_first_not_of(" \t\n", endStart);
    
    if (endStart != String::npos && code[endStart] == '{') {
        // 多行模式
        int depth = 1;
        size_t endEnd = endStart + 1;
        while (endEnd < code.length() && depth > 0) {
            if (code[endEnd] == '{') depth++;
            else if (code[endEnd] == '}') depth--;
            endEnd++;
        }
        expr.endBody = code.substr(endStart + 1, endEnd - endStart - 2);
    } else {
        // 单行模式 - 查找分号或行尾
        size_t endEnd = code.find(";", endStart);
        if (endEnd == String::npos) {
            endEnd = code.length();
        }
        expr.endBody = StringUtil::trim(code.substr(endStart, endEnd - endStart));
    }
    
    return expr;
}

String UtilEndProcessor::generateJavaScript(const UtilEndExpr& expr) {
    std::stringstream ss;
    
    ss << "(function() {\n";
    ss << "  // util...end expression watcher\n";
    ss << "  let __prevCondition = false;\n";
    ss << "  let __conditionMet = false;\n\n";
    
    ss << "  const __checkCondition = () => {\n";
    ss << "    const currentCondition = (" << expr.condition << ");\n\n";
    
    ss << "    // 条件发生变化时\n";
    ss << "    if (currentCondition !== __prevCondition) {\n";
    ss << "      __prevCondition = currentCondition;\n";
    ss << "      // change 块\n";
    ss << "      " << expr.changeBody << "\n";
    ss << "    }\n\n";
    
    ss << "    // 条件满足时\n";
    ss << "    if (currentCondition && !__conditionMet) {\n";
    ss << "      __conditionMet = true;\n";
    ss << "      // end 块\n";
    ss << "      " << expr.endBody << "\n";
    ss << "    }\n";
    ss << "  };\n\n";
    
    ss << "  // 使用 requestAnimationFrame 持续检查\n";
    ss << "  const __watch = () => {\n";
    ss << "    __checkCondition();\n";
    ss << "    if (!__conditionMet) {\n";
    ss << "      requestAnimationFrame(__watch);\n";
    ss << "    }\n";
    ss << "  };\n\n";
    
    ss << "  // 开始监视\n";
    ss << "  __watch();\n";
    ss << "})();\n";
    
    return ss.str();
}

} // namespace CJMOD
} // namespace Chtholly
