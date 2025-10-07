#include "INeverAway.h"
#include "../../../../../src/Util/StringUtil/StringUtil.h"
#include <sstream>
#include <regex>

using namespace CHTL;
using namespace CHTL::CJMOD;
using namespace CHTL::Util;

namespace Chtholly {
namespace CJMOD {

void INeverAwayProcessor::registerSyntax() {
    CJMODRegistry::SyntaxInfo info;
    info.pattern = "Vir $_ = INeverAway { ... }";
    info.description = "创建一组标记函数，支持状态区分的同名重载";
    info.examples = {
        "Vir Test = INeverAway { Void<A>: function(int, int) { ... }, Void<B>: function(int, int) { ... } };",
        "Vir Utils = INeverAway { log: function(msg) { console.log(msg); } };",
        "Test->Void<A>(1, 2);"
    };
    
    CJMODRegistry::registerSyntax("INeverAway", info);
}

Arg INeverAwayProcessor::process(const String& code) {
    Arg result;
    
    // 查找 Vir 声明
    size_t virPos = code.find("Vir ");
    if (virPos == String::npos) {
        return result;
    }
    
    // 提取虚对象名称
    size_t nameStart = virPos + 4;
    size_t nameEnd = code.find("=", nameStart);
    if (nameEnd == String::npos) {
        return result;
    }
    
    String virName = StringUtil::trim(code.substr(nameStart, nameEnd - nameStart));
    
    // 查找 INeverAway 块
    size_t ineverPos = code.find("INeverAway", nameEnd);
    if (ineverPos == String::npos) {
        return result;
    }
    
    size_t blockStart = code.find("{", ineverPos);
    if (blockStart == String::npos) {
        return result;
    }
    
    // 找到匹配的右大括号
    int depth = 1;
    size_t blockEnd = blockStart + 1;
    while (blockEnd < code.length() && depth > 0) {
        if (code[blockEnd] == '{') depth++;
        else if (code[blockEnd] == '}') depth--;
        blockEnd++;
    }
    
    String blockContent = code.substr(blockStart + 1, blockEnd - blockStart - 2);
    
    // 解析函数定义
    auto functions = parseFunctions(blockContent);
    
    // 生成 JavaScript 代码
    String jsCode = generateJavaScript(functions, virName);
    
    result.transform(jsCode);
    
    // 绑定虚对象
    CHTLJSFunction::bindVirtualObject(virName);
    
    return result;
}

Vector<INeverAwayProcessor::FunctionDef> INeverAwayProcessor::parseFunctions(const String& code) {
    Vector<FunctionDef> functions;
    
    // 简单解析：按逗号分割（需要考虑嵌套）
    size_t pos = 0;
    int depth = 0;
    size_t lastSplit = 0;
    
    while (pos < code.length()) {
        char c = code[pos];
        
        if (c == '{' || c == '(' || c == '[') {
            depth++;
        } else if (c == '}' || c == ')' || c == ']') {
            depth--;
        } else if (c == ',' && depth == 0) {
            // 提取函数定义
            String funcDef = StringUtil::trim(code.substr(lastSplit, pos - lastSplit));
            
            if (!funcDef.empty()) {
                FunctionDef def;
                
                // 解析函数定义：Name<State>: type { body }
                size_t colonPos = funcDef.find(':');
                if (colonPos != String::npos) {
                    String namePart = StringUtil::trim(funcDef.substr(0, colonPos));
                    String valuePart = StringUtil::trim(funcDef.substr(colonPos + 1));
                    
                    // 提取状态标记
                    size_t stateStart = namePart.find('<');
                    size_t stateEnd = namePart.find('>');
                    if (stateStart != String::npos && stateEnd != String::npos) {
                        def.name = StringUtil::trim(namePart.substr(0, stateStart));
                        def.state = namePart.substr(stateStart + 1, stateEnd - stateStart - 1);
                    } else {
                        def.name = namePart;
                        def.state = "";
                    }
                    
                    // 解析函数体
                    if (StringUtil::contains(valuePart, "function")) {
                        def.body = valuePart;
                    } else {
                        def.body = valuePart;
                    }
                    
                    functions.push_back(def);
                }
            }
            
            lastSplit = pos + 1;
        }
        
        pos++;
    }
    
    // 处理最后一个函数
    if (lastSplit < code.length()) {
        String funcDef = StringUtil::trim(code.substr(lastSplit));
        
        if (!funcDef.empty()) {
            FunctionDef def;
            
            size_t colonPos = funcDef.find(':');
            if (colonPos != String::npos) {
                String namePart = StringUtil::trim(funcDef.substr(0, colonPos));
                String valuePart = StringUtil::trim(funcDef.substr(colonPos + 1));
                
                size_t stateStart = namePart.find('<');
                size_t stateEnd = namePart.find('>');
                if (stateStart != String::npos && stateEnd != String::npos) {
                    def.name = StringUtil::trim(namePart.substr(0, stateStart));
                    def.state = namePart.substr(stateStart + 1, stateEnd - stateStart - 1);
                } else {
                    def.name = namePart;
                    def.state = "";
                }
                
                def.body = valuePart;
                functions.push_back(def);
            }
        }
    }
    
    return functions;
}

String INeverAwayProcessor::generateJavaScript(const Vector<FunctionDef>& functions, const String& virName) {
    std::stringstream ss;
    
    ss << "(function() {\n";
    ss << "  // INeverAway generated functions for " << virName << "\n";
    ss << "  const __" << virName << "_funcs = {};\n\n";
    
    // 为每个函数生成 JavaScript
    for (const auto& func : functions) {
        String funcName = "__" + virName + "_" + func.name;
        if (!func.state.empty()) {
            funcName += "_" + func.state;
        }
        
        ss << "  // " << func.name;
        if (!func.state.empty()) {
            ss << "<" << func.state << ">";
        }
        ss << "\n";
        
        ss << "  const " << funcName << " = " << func.body << ";\n";
        ss << "  __" << virName << "_funcs['" << func.name;
        if (!func.state.empty()) {
            ss << "<" << func.state << ">";
        }
        ss << "'] = " << funcName << ";\n\n";
    }
    
    // 创建虚对象访问器
    ss << "  // Virtual object accessor\n";
    ss << "  window." << virName << " = new Proxy(__" << virName << "_funcs, {\n";
    ss << "    get(target, prop) {\n";
    ss << "      if (prop in target) {\n";
    ss << "        return target[prop];\n";
    ss << "      }\n";
    ss << "      // 尝试不带状态的访问\n";
    ss << "      for (const key in target) {\n";
    ss << "        if (key.startsWith(prop + '<') || key === prop) {\n";
    ss << "          return target[key];\n";
    ss << "        }\n";
    ss << "      }\n";
    ss << "      return undefined;\n";
    ss << "    }\n";
    ss << "  });\n";
    ss << "})();\n";
    
    return ss.str();
}

} // namespace CJMOD
} // namespace Chtholly
