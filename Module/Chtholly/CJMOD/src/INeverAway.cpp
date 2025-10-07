#include "../../../src/CHTL-JS/CJMODSystem/CJMODApi.h"
#include <string>
<parameter name="sstream">
#include <map>

using namespace CHTL::CJMOD;

// INeverAway CJMOD实现
// 创建标记函数，支持状态重载和虚对象

extern "C" {

// 存储生成的函数名映射
static std::map<std::string, int> functionNameMap;
static int functionCounter = 0;

// 初始化INeverAway函数
void InitINeverAway() {
    CHTLJSFunction::bindVirtualObject("INeverAway");
}

// 处理INeverAway函数调用
std::string ProcessINeverAway(const std::string& code) {
    // INeverAway函数格式:
    // Vir Test = INeverAway {
    //     Void<A>: function(int, int) { ... },
    //     Void<B>: function(int, int) { ... },
    //     Ax: { ... }
    // };
    
    std::ostringstream output;
    output << "(function() {\n";
    output << "    // INeverAway - 标记函数系统\n";
    output << "    const __INeverAway_functions = {};\n";
    output << "    \n";
    
    // 解析函数体
    size_t braceStart = code.find('{');
    size_t braceEnd = code.rfind('}');
    
    if (braceStart != std::string::npos && braceEnd != std::string::npos) {
        std::string body = code.substr(braceStart + 1, braceEnd - braceStart - 1);
        
        // 简化实现：直接返回函数对象
        output << "    const functionsObject = {\n";
        output << body;  // 保留原始函数定义
        output << "\n    };\n";
        output << "    \n";
        output << "    // 创建代理对象支持函数访问\n";
        output << "    return new Proxy(functionsObject, {\n";
        output << "        get(target, prop) {\n";
        output << "            if (typeof prop === 'string' && prop.includes('<')) {\n";
        output << "                // 处理状态标记\n";
        output << "                const baseName = prop.split('<')[0];\n";
        output << "                const state = prop.match(/<(.+)>/)?.[1];\n";
        output << "                const key = prop;\n";
        output << "                return target[key] || target[baseName];\n";
        output << "            }\n";
        output << "            return target[prop];\n";
        output << "        }\n";
        output << "    });\n";
    }
    
    output << "})()";
    
    return output.str();
}

} // extern "C"
