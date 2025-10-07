#include "../../../src/CHTL-JS/CJMODSystem/CJMODApi.h"
#include <string>
#include <sstream>

using namespace CHTL::CJMOD;

// util...end表达式 CJMOD实现
// util 表达式 -> change { 条件变化时 } -> end { 条件完成时 }

extern "C" {

// 处理util...end表达式
std::string ProcessUtilEnd(const std::string& code) {
    // util...end格式:
    // util a > b -> change print("发生变化") -> end print("a > b");
    // util a < b -> change {print("发生变化");} -> end {print("a < b");}
    
    std::ostringstream output;
    
    // 查找util关键字
    size_t utilPos = code.find("util");
    if (utilPos == std::string::npos) {
        return code;
    }
    
    // 查找->符号
    size_t changePos = code.find("-> change", utilPos);
    size_t endPos = code.find("-> end", changePos);
    
    if (changePos == std::string::npos || endPos == std::string::npos) {
        return code;
    }
    
    // 提取条件表达式
    std::string condition = code.substr(utilPos + 4, changePos - utilPos - 4);
    condition.erase(0, condition.find_first_not_of(" \t\n"));
    condition.erase(condition.find_last_not_of(" \t\n") + 1);
    
    // 提取change块
    size_t changeStart = code.find("change", changePos) + 6;
    std::string changeBlock = code.substr(changeStart, endPos - changeStart);
    changeBlock.erase(0, changeBlock.find_first_not_of(" \t\n"));
    changeBlock.erase(changeBlock.find_last_not_of(" \t\n") + 1);
    if (changeBlock.starts_with("->")) {
        changeBlock = changeBlock.substr(2);
        changeBlock.erase(0, changeBlock.find_first_not_of(" \t\n"));
    }
    
    // 提取end块
    size_t endStart = code.find("end", endPos) + 3;
    std::string endBlock = code.substr(endStart);
    endBlock.erase(0, endBlock.find_first_not_of(" \t\n"));
    endBlock.erase(endBlock.find_last_not_of(" \t\n;") + 1);
    
    // 生成JS代码
    output << "(function() {\n";
    output << "    // util...end表达式\n";
    output << "    let __util_prev_value = null;\n";
    output << "    let __util_condition_met = false;\n";
    output << "    \n";
    output << "    const checkCondition = () => {\n";
    output << "        const currentValue = " << condition << ";\n";
    output << "        \n";
    output << "        if (currentValue !== __util_prev_value) {\n";
    output << "            // 条件变化\n";
    output << "            " << changeBlock << "\n";
    output << "            __util_prev_value = currentValue;\n";
    output << "        }\n";
    output << "        \n";
    output << "        if (currentValue && !__util_condition_met) {\n";
    output << "            // 条件完成\n";
    output << "            " << endBlock << "\n";
    output << "            __util_condition_met = true;\n";
    output << "        }\n";
    output << "    };\n";
    output << "    \n";
    output << "    // 使用MutationObserver监听变化\n";
    output << "    const observer = new MutationObserver(checkCondition);\n";
    output << "    observer.observe(document.body, { \n";
    output << "        attributes: true, \n";
    output << "        childList: true, \n";
    output << "        subtree: true \n";
    output << "    });\n";
    output << "    \n";
    output << "    // 初始检查\n";
    output << "    checkCondition();\n";
    output << "})();\n";
    
    return output.str();
}

} // extern "C"
