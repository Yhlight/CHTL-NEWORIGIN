#include "catch.hpp"
#include "CHTLState.h"
#include "CHTLContext.h"
#include "CHTLNode.h"

using namespace CHTL;

TEST_CASE("CHTLState - State transitions", "[CHTLState]") {
    CHTLContext context;
    auto element = std::make_unique<ElementNode>("div");
    auto template_ = std::make_unique<TemplateNode>(TemplateNode::TemplateType::STYLE, "MyStyle");
    auto custom = std::make_unique<CustomNode>(CustomNode::CustomType::ELEMENT, "MyElement");
    auto import = std::make_unique<ImportNode>(ImportNode::ImportType::CHTL, "module.chtl");
    auto namespace_ = std::make_unique<CHTLNode>(CHTLNode::Type::NAMESPACE, "MyNamespace");
    auto config = std::make_unique<CHTLNode>(CHTLNode::Type::CONFIG, "MyConfig");
    
    SECTION("InitialState transitions") {
        InitialState initialState;
        
        // Test element handling
        auto newState = initialState.handleElement(context, element.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "ParsingState");
        
        // Test template handling
        newState = initialState.handleTemplate(context, template_.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "TemplateState");
        
        // Test custom handling
        newState = initialState.handleCustom(context, custom.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "ParsingState");
        
        // Test import handling
        newState = initialState.handleImport(context, import.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "ParsingState");
        
        // Test namespace handling
        newState = initialState.handleNamespace(context, namespace_.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "ParsingState");
        
        // Test configuration handling
        newState = initialState.handleConfiguration(context, config.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "ParsingState");
    }
    
    SECTION("ParsingState behavior") {
        ParsingState parsingState;
        
        // ParsingState should stay in same state for elements
        auto newState = parsingState.handleElement(context, element.get());
        REQUIRE(newState == nullptr); // No state change
        
        // ParsingState should transition to TemplateState for templates
        newState = parsingState.handleTemplate(context, template_.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "TemplateState");
        
        // ParsingState should stay in same state for other operations
        newState = parsingState.handleCustom(context, custom.get());
        REQUIRE(newState == nullptr); // No state change
        
        newState = parsingState.handleImport(context, import.get());
        REQUIRE(newState == nullptr); // No state change
        
        newState = parsingState.handleNamespace(context, namespace_.get());
        REQUIRE(newState == nullptr); // No state change
        
        newState = parsingState.handleConfiguration(context, config.get());
        REQUIRE(newState == nullptr); // No state change
    }
    
    SECTION("TemplateState behavior") {
        TemplateState templateState;
        
        // TemplateState should stay in same state for elements and templates
        auto newState = templateState.handleElement(context, element.get());
        REQUIRE(newState == nullptr); // No state change
        
        newState = templateState.handleTemplate(context, template_.get());
        REQUIRE(newState == nullptr); // No state change
        
        newState = templateState.handleCustom(context, custom.get());
        REQUIRE(newState == nullptr); // No state change
        
        // TemplateState should transition to ErrorState for invalid operations
        newState = templateState.handleImport(context, import.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "ErrorState");
        REQUIRE(context.hasErrors());
        
        // Clear errors for next test
        context.clearErrors();
        
        newState = templateState.handleNamespace(context, namespace_.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "ErrorState");
        REQUIRE(context.hasErrors());
        
        // Clear errors for next test
        context.clearErrors();
        
        newState = templateState.handleConfiguration(context, config.get());
        REQUIRE(newState != nullptr);
        REQUIRE(newState->getStateName() == "ErrorState");
        REQUIRE(context.hasErrors());
    }
    
    SECTION("ErrorState behavior") {
        ErrorState errorState;
        
        // ErrorState should stay in same state and add errors for all operations
        auto newState = errorState.handleElement(context, element.get());
        REQUIRE(newState == nullptr); // No state change
        REQUIRE(context.hasErrors());
        
        context.clearErrors();
        
        newState = errorState.handleTemplate(context, template_.get());
        REQUIRE(newState == nullptr); // No state change
        REQUIRE(context.hasErrors());
        
        context.clearErrors();
        
        newState = errorState.handleCustom(context, custom.get());
        REQUIRE(newState == nullptr); // No state change
        REQUIRE(context.hasErrors());
        
        context.clearErrors();
        
        newState = errorState.handleImport(context, import.get());
        REQUIRE(newState == nullptr); // No state change
        REQUIRE(context.hasErrors());
        
        context.clearErrors();
        
        newState = errorState.handleNamespace(context, namespace_.get());
        REQUIRE(newState == nullptr); // No state change
        REQUIRE(context.hasErrors());
        
        context.clearErrors();
        
        newState = errorState.handleConfiguration(context, config.get());
        REQUIRE(newState == nullptr); // No state change
        REQUIRE(context.hasErrors());
    }
}

TEST_CASE("CHTLState - State names", "[CHTLState]") {
    SECTION("State name consistency") {
        InitialState initialState;
        ParsingState parsingState;
        TemplateState templateState;
        ErrorState errorState;
        
        REQUIRE(initialState.getStateName() == "InitialState");
        REQUIRE(parsingState.getStateName() == "ParsingState");
        REQUIRE(templateState.getStateName() == "TemplateState");
        REQUIRE(errorState.getStateName() == "ErrorState");
    }
}