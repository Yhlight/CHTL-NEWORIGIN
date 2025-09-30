#include <catch.hpp>
#include "CHTL/CHTLStrategy.h"

TEST_CASE("CHTLStrategyManager - Basic Functionality", "[strategy_pattern]") {
    CHTL::CHTLStrategyManager strategyManager;
    strategyManager.initialize();
    
    SECTION("Initial state") {
        REQUIRE(strategyManager.getCurrentStrategy() == nullptr);
    }
    
    SECTION("Strategy processing") {
        bool success = strategyManager.process("div { }");
        REQUIRE(success);
        REQUIRE(strategyManager.getCurrentStrategy() != nullptr);
    }
    
    SECTION("Strategy selection") {
        bool success = strategyManager.setStrategy(CHTL::CHTLStrategyFactory::StrategyType::ELEMENT);
        REQUIRE(success);
        REQUIRE(strategyManager.getCurrentStrategy() != nullptr);
    }
    
    SECTION("Reset") {
        strategyManager.process("div { }");
        strategyManager.reset();
        REQUIRE(strategyManager.getCurrentStrategy() == nullptr);
    }
}

TEST_CASE("CHTLStrategy - Individual Strategies", "[strategy_pattern]") {
    SECTION("Element strategy") {
        auto strategy = CHTL::CHTLStrategyFactory::createStrategy(CHTL::CHTLStrategyFactory::StrategyType::ELEMENT);
        strategy->initialize();
        
        REQUIRE(strategy->getStrategyName() == "Element");
        REQUIRE(strategy->canHandle("div { }"));
        REQUIRE(!strategy->canHandle("invalid input"));
        
        bool success = strategy->process("div { }");
        REQUIRE(success);
        REQUIRE(!strategy->getResult().empty());
    }
    
    SECTION("Style strategy") {
        auto strategy = CHTL::CHTLStrategyFactory::createStrategy(CHTL::CHTLStrategyFactory::StrategyType::STYLE);
        strategy->initialize();
        
        REQUIRE(strategy->getStrategyName() == "Style");
        REQUIRE(strategy->canHandle("style { color: red; }"));
        REQUIRE(!strategy->canHandle("invalid input"));
        
        bool success = strategy->process("style { color: red; }");
        REQUIRE(success);
        REQUIRE(!strategy->getResult().empty());
    }
}