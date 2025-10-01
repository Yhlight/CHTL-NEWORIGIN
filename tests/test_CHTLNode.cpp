#include "catch.hpp"
#include "CHTLNode.h"
#include "CHTLContext.h"

using namespace CHTL;

TEST_CASE("CHTLNode - Basic functionality", "[CHTLNode]") {
    SECTION("ElementNode creation") {
        ElementNode element("div");
        REQUIRE(element.getType() == CHTLNode::Type::ELEMENT);
        REQUIRE(element.getTagName() == "div");
        REQUIRE(element.getChildCount() == 0);
    }
    
    SECTION("TextNode creation") {
        TextNode text("Hello World");
        REQUIRE(text.getType() == CHTLNode::Type::TEXT);
        REQUIRE(text.getContent() == "Hello World");
        REQUIRE(text.getChildCount() == 0);
    }
    
    SECTION("TemplateNode creation") {
        TemplateNode template_(TemplateNode::TemplateType::STYLE, "MyStyle");
        REQUIRE(template_.getType() == CHTLNode::Type::TEMPLATE);
        REQUIRE(template_.getTemplateType() == TemplateNode::TemplateType::STYLE);
        REQUIRE(template_.getName() == "MyStyle");
    }
    
    SECTION("CustomNode creation") {
        CustomNode custom(CustomNode::CustomType::ELEMENT, "MyElement");
        REQUIRE(custom.getType() == CHTLNode::Type::CUSTOM);
        REQUIRE(custom.getCustomType() == CustomNode::CustomType::ELEMENT);
        REQUIRE(custom.getName() == "MyElement");
    }
    
    SECTION("StyleNode creation") {
        StyleNode style;
        REQUIRE(style.getType() == CHTLNode::Type::STYLE);
        REQUIRE(style.getName() == "style");
    }
    
    SECTION("ScriptNode creation") {
        ScriptNode script;
        REQUIRE(script.getType() == CHTLNode::Type::SCRIPT);
        REQUIRE(script.getName() == "script");
    }
    
    SECTION("ImportNode creation") {
        ImportNode import(ImportNode::ImportType::CHTL, "module.chtl");
        REQUIRE(import.getType() == CHTLNode::Type::IMPORT);
        REQUIRE(import.getImportType() == ImportNode::ImportType::CHTL);
        REQUIRE(import.getPath() == "module.chtl");
    }
}

TEST_CASE("CHTLNode - Child management", "[CHTLNode]") {
    auto parent = std::make_unique<ElementNode>("div");
    auto child1 = std::make_unique<ElementNode>("span");
    auto child2 = std::make_unique<TextNode>("Hello");
    
    SECTION("Adding children") {
        REQUIRE(parent->getChildCount() == 0);
        
        parent->addChild(std::move(child1));
        REQUIRE(parent->getChildCount() == 1);
        
        parent->addChild(std::move(child2));
        REQUIRE(parent->getChildCount() == 2);
    }
    
    SECTION("Accessing children") {
        parent->addChild(std::move(child1));
        parent->addChild(std::move(child2));
        
        auto firstChild = parent->getChild(0);
        REQUIRE(firstChild != nullptr);
        REQUIRE(firstChild->getType() == CHTLNode::Type::ELEMENT);
        
        auto secondChild = parent->getChild(1);
        REQUIRE(secondChild != nullptr);
        REQUIRE(secondChild->getType() == CHTLNode::Type::TEXT);
        
        auto invalidChild = parent->getChild(2);
        REQUIRE(invalidChild == nullptr);
    }
    
    SECTION("Getting children vector") {
        parent->addChild(std::make_unique<ElementNode>("span"));
        parent->addChild(std::make_unique<TextNode>("Hello"));
        
        auto children = parent->getChildren();
        REQUIRE(children.size() == 2);
        REQUIRE(children[0]->getType() == CHTLNode::Type::ELEMENT);
        REQUIRE(children[1]->getType() == CHTLNode::Type::TEXT);
    }
}

TEST_CASE("CHTLNode - Attribute management", "[CHTLNode]") {
    ElementNode element("div");
    
    SECTION("Setting and getting attributes") {
        element.setAttribute("id", "myDiv");
        element.setAttribute("class", "container");
        
        REQUIRE(element.hasAttribute("id"));
        REQUIRE(element.hasAttribute("class"));
        REQUIRE_FALSE(element.hasAttribute("style"));
        
        REQUIRE(element.getAttribute("id") == "myDiv");
        REQUIRE(element.getAttribute("class") == "container");
        REQUIRE(element.getAttribute("style").empty());
    }
    
    SECTION("Getting all attributes") {
        element.setAttribute("id", "myDiv");
        element.setAttribute("class", "container");
        
        auto attributes = element.getAttributes();
        REQUIRE(attributes.size() == 2);
        REQUIRE(attributes["id"] == "myDiv");
        REQUIRE(attributes["class"] == "container");
    }
}

TEST_CASE("CHTLNode - Position management", "[CHTLNode]") {
    ElementNode element("div");
    
    SECTION("Setting and getting position") {
        REQUIRE(element.getLine() == -1);
        REQUIRE(element.getColumn() == -1);
        
        element.setPosition(10, 5);
        REQUIRE(element.getLine() == 10);
        REQUIRE(element.getColumn() == 5);
    }
}

TEST_CASE("CHTLNode - Validation", "[CHTLNode]") {
    CHTLContext context;
    
    SECTION("Valid element node") {
        ElementNode element("div");
        REQUIRE(element.validate(context));
        REQUIRE_FALSE(context.hasErrors());
    }
    
    SECTION("Invalid element node - empty name") {
        ElementNode element("");
        REQUIRE_FALSE(element.validate(context));
        REQUIRE(context.hasErrors());
    }
    
    SECTION("Valid template node") {
        TemplateNode template_(TemplateNode::TemplateType::STYLE, "MyStyle");
        REQUIRE(template_.validate(context));
        REQUIRE_FALSE(context.hasErrors());
    }
    
    SECTION("Invalid template node - empty name") {
        TemplateNode template_(TemplateNode::TemplateType::STYLE, "");
        REQUIRE_FALSE(template_.validate(context));
        REQUIRE(context.hasErrors());
    }
    
    SECTION("Valid custom node") {
        CustomNode custom(CustomNode::CustomType::ELEMENT, "MyElement");
        REQUIRE(custom.validate(context));
        REQUIRE_FALSE(context.hasErrors());
    }
    
    SECTION("Invalid custom node - empty name") {
        CustomNode custom(CustomNode::CustomType::ELEMENT, "");
        REQUIRE_FALSE(custom.validate(context));
        REQUIRE(context.hasErrors());
    }
    
    SECTION("Valid import node") {
        ImportNode import(ImportNode::ImportType::CHTL, "module.chtl");
        REQUIRE(import.validate(context));
        REQUIRE_FALSE(context.hasErrors());
    }
    
    SECTION("Invalid import node - empty path") {
        ImportNode import(ImportNode::ImportType::CHTL, "");
        REQUIRE_FALSE(import.validate(context));
        REQUIRE(context.hasErrors());
    }
}