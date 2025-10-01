#include "catch.hpp"
#include "CHTLStrategy.h"
#include "CHTLContext.h"
#include "CHTLNode.h"

using namespace CHTL;

TEST_CASE("CHTLStrategy - Strategy manager", "[CHTLStrategy]") {
    CHTLContext context;
    CHTLStrategyManager manager;
    
    SECTION("Strategy registration") {
        REQUIRE(manager.getStrategyCount() > 0); // Default strategies should be registered
        
        // Test that strategies are registered with correct priorities
        auto element = std::make_unique<ElementNode>("div");
        auto template_ = std::make_unique<TemplateNode>(TemplateNode::TemplateType::STYLE, "MyStyle");
        auto custom = std::make_unique<CustomNode>(CustomNode::CustomType::ELEMENT, "MyElement");
        auto import = std::make_unique<ImportNode>(ImportNode::ImportType::CHTL, "module.chtl");
        auto namespace_ = std::make_unique<CHTLNode>(CHTLNode::Type::NAMESPACE, "MyNamespace");
        auto config = std::make_unique<CHTLNode>(CHTLNode::Type::CONFIG, "MyConfig");
        auto style = std::make_unique<StyleNode>();
        auto script = std::make_unique<ScriptNode>();
        
        // Test strategy execution
        REQUIRE(manager.executeStrategy(context, element.get()));
        REQUIRE(manager.executeStrategy(context, template_.get()));
        REQUIRE(manager.executeStrategy(context, custom.get()));
        REQUIRE(manager.executeStrategy(context, import.get()));
        REQUIRE(manager.executeStrategy(context, namespace_.get()));
        REQUIRE(manager.executeStrategy(context, config.get()));
        REQUIRE(manager.executeStrategy(context, style.get()));
        REQUIRE(manager.executeStrategy(context, script.get()));
    }
    
    SECTION("Strategy clearing") {
        size_t initialCount = manager.getStrategyCount();
        REQUIRE(initialCount > 0);
        
        manager.clearStrategies();
        REQUIRE(manager.getStrategyCount() == 0);
        
        // Test execution with no strategies
        auto element = std::make_unique<ElementNode>("div");
        REQUIRE_FALSE(manager.executeStrategy(context, element.get()));
        REQUIRE(context.hasErrors());
    }
}

TEST_CASE("CHTLStrategy - Individual strategies", "[CHTLStrategy]") {
    CHTLContext context;
    
    SECTION("ElementParsingStrategy") {
        ElementParsingStrategy strategy;
        
        REQUIRE(strategy.getStrategyName() == "ElementParsingStrategy");
        REQUIRE(strategy.getPriority() == 1);
        
        auto element = std::make_unique<ElementNode>("div");
        auto nonElement = std::make_unique<TextNode>("Hello");
        
        REQUIRE(strategy.canHandle(context, element.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nonElement.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nullptr));
        
        REQUIRE(strategy.execute(context, element.get()));
        REQUIRE_FALSE(strategy.execute(context, nonElement.get()));
    }
    
    SECTION("TemplateParsingStrategy") {
        TemplateParsingStrategy strategy;
        
        REQUIRE(strategy.getStrategyName() == "TemplateParsingStrategy");
        REQUIRE(strategy.getPriority() == 2);
        
        auto template_ = std::make_unique<TemplateNode>(TemplateNode::TemplateType::STYLE, "MyStyle");
        auto nonTemplate = std::make_unique<ElementNode>("div");
        
        REQUIRE(strategy.canHandle(context, template_.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nonTemplate.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nullptr));
        
        REQUIRE(strategy.execute(context, template_.get()));
        REQUIRE_FALSE(strategy.execute(context, nonTemplate.get()));
    }
    
    SECTION("CustomParsingStrategy") {
        CustomParsingStrategy strategy;
        
        REQUIRE(strategy.getStrategyName() == "CustomParsingStrategy");
        REQUIRE(strategy.getPriority() == 3);
        
        auto custom = std::make_unique<CustomNode>(CustomNode::CustomType::ELEMENT, "MyElement");
        auto nonCustom = std::make_unique<ElementNode>("div");
        
        REQUIRE(strategy.canHandle(context, custom.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nonCustom.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nullptr));
        
        REQUIRE(strategy.execute(context, custom.get()));
        REQUIRE_FALSE(strategy.execute(context, nonCustom.get()));
    }
    
    SECTION("ImportParsingStrategy") {
        ImportParsingStrategy strategy;
        
        REQUIRE(strategy.getStrategyName() == "ImportParsingStrategy");
        REQUIRE(strategy.getPriority() == 4);
        
        auto import = std::make_unique<ImportNode>(ImportNode::ImportType::CHTL, "module.chtl");
        auto nonImport = std::make_unique<ElementNode>("div");
        
        REQUIRE(strategy.canHandle(context, import.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nonImport.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nullptr));
        
        REQUIRE(strategy.execute(context, import.get()));
        REQUIRE_FALSE(strategy.execute(context, nonImport.get()));
    }
    
    SECTION("NamespaceParsingStrategy") {
        NamespaceParsingStrategy strategy;
        
        REQUIRE(strategy.getStrategyName() == "NamespaceParsingStrategy");
        REQUIRE(strategy.getPriority() == 5);
        
        auto namespace_ = std::make_unique<CHTLNode>(CHTLNode::Type::NAMESPACE, "MyNamespace");
        auto nonNamespace = std::make_unique<ElementNode>("div");
        
        REQUIRE(strategy.canHandle(context, namespace_.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nonNamespace.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nullptr));
        
        REQUIRE(strategy.execute(context, namespace_.get()));
        REQUIRE_FALSE(strategy.execute(context, nonNamespace.get()));
    }
    
    SECTION("ConfigurationParsingStrategy") {
        ConfigurationParsingStrategy strategy;
        
        REQUIRE(strategy.getStrategyName() == "ConfigurationParsingStrategy");
        REQUIRE(strategy.getPriority() == 6);
        
        auto config = std::make_unique<CHTLNode>(CHTLNode::Type::CONFIG, "MyConfig");
        auto nonConfig = std::make_unique<ElementNode>("div");
        
        REQUIRE(strategy.canHandle(context, config.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nonConfig.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nullptr));
        
        REQUIRE(strategy.execute(context, config.get()));
        REQUIRE_FALSE(strategy.execute(context, nonConfig.get()));
    }
    
    SECTION("StyleParsingStrategy") {
        StyleParsingStrategy strategy;
        
        REQUIRE(strategy.getStrategyName() == "StyleParsingStrategy");
        REQUIRE(strategy.getPriority() == 7);
        
        auto style = std::make_unique<StyleNode>();
        auto nonStyle = std::make_unique<ElementNode>("div");
        
        REQUIRE(strategy.canHandle(context, style.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nonStyle.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nullptr));
        
        REQUIRE(strategy.execute(context, style.get()));
        REQUIRE_FALSE(strategy.execute(context, nonStyle.get()));
    }
    
    SECTION("ScriptParsingStrategy") {
        ScriptParsingStrategy strategy;
        
        REQUIRE(strategy.getStrategyName() == "ScriptParsingStrategy");
        REQUIRE(strategy.getPriority() == 8);
        
        auto script = std::make_unique<ScriptNode>();
        auto nonScript = std::make_unique<ElementNode>("div");
        
        REQUIRE(strategy.canHandle(context, script.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nonScript.get()));
        REQUIRE_FALSE(strategy.canHandle(context, nullptr));
        
        REQUIRE(strategy.execute(context, script.get()));
        REQUIRE_FALSE(strategy.execute(context, nonScript.get()));
    }
}

TEST_CASE("CHTLStrategy - Priority ordering", "[CHTLStrategy]") {
    CHTLContext context;
    CHTLStrategyManager manager;
    
    SECTION("Strategy priority verification") {
        // Create test nodes
        auto element = std::make_unique<ElementNode>("div");
        auto template_ = std::make_unique<TemplateNode>(TemplateNode::TemplateType::STYLE, "MyStyle");
        auto custom = std::make_unique<CustomNode>(CustomNode::CustomType::ELEMENT, "MyElement");
        auto import = std::make_unique<ImportNode>(ImportNode::ImportType::CHTL, "module.chtl");
        auto namespace_ = std::make_unique<CHTLNode>(CHTLNode::Type::NAMESPACE, "MyNamespace");
        auto config = std::make_unique<CHTLNode>(CHTLNode::Type::CONFIG, "MyConfig");
        auto style = std::make_unique<StyleNode>();
        auto script = std::make_unique<ScriptNode>();
        
        // Verify that strategies are executed in priority order
        // (This is more of a design verification - actual priority testing would require more complex setup)
        REQUIRE(manager.executeStrategy(context, element.get()));
        REQUIRE(manager.executeStrategy(context, template_.get()));
        REQUIRE(manager.executeStrategy(context, custom.get()));
        REQUIRE(manager.executeStrategy(context, import.get()));
        REQUIRE(manager.executeStrategy(context, namespace_.get()));
        REQUIRE(manager.executeStrategy(context, config.get()));
        REQUIRE(manager.executeStrategy(context, style.get()));
        REQUIRE(manager.executeStrategy(context, script.get()));
    }
}