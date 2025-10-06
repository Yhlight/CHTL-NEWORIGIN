#ifndef CHTLJS_GENERATOR_H
#define CHTLJS_GENERATOR_H

#include "../../Common.h"
#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../../SharedCore/SaltBridge.h"

namespace CHTL {
namespace JS {

// CHTL JS生成器配置
struct JSGeneratorConfig {
    bool prettyPrint = true;
    bool useStrict = false;
    bool wrapIIFE = true;  // 是否用IIFE包装
};

// CHTL JS代码生成器
class CHTLJSGenerator {
public:
    explicit CHTLJSGenerator(const JSGeneratorConfig& config = JSGeneratorConfig());
    
    // 生成JavaScript代码
    String generate(const String& chtljsCode);
    
    // 处理增强选择器
    String processEnhancedSelector(const String& selector);
    
    // 处理Listen语法
    String processListen(const String& target, const String& events);
    
    // 处理事件绑定操作符
    String processEventBind(const String& target, const String& event, const String& handler);
    
    // 处理Delegate语法
    String processDelegate(const String& parent, const String& config);
    
    // 处理Animate语法
    String processAnimate(const String& config);
    
    // 处理Router语法
    String processRouter(const String& config);
    
private:
    JSGeneratorConfig config_;
    Bridge::SaltBridge& bridge_;
    
    String wrapWithIIFE(const String& code);
    String processListenBlocks(const String& code);
    String processEventBindOperators(const String& code);
    String processDelegateBlocks(const String& code);
    String processAnimateBlocks(const String& code);
    String processRouterBlocks(const String& code);
    String processVirDeclarations(const String& code);
};

} // namespace JS
} // namespace CHTL

#endif // CHTLJS_GENERATOR_H
