#include <catch.hpp>
#include "CHTL/CHTLState.h"

TEST_CASE("CHTLStateManager - Basic Functionality", "[state_machine]") {
    CHTL::CHTLStateManager stateManager;
    stateManager.initialize();
    
    SECTION("Initial state") {
        REQUIRE(stateManager.getCurrentState() != nullptr);
        REQUIRE(stateManager.getCurrentState()->getStateType() == CHTL::StateType::INITIAL);
        REQUIRE(!stateManager.isError());
    }
    
    SECTION("Token processing") {
        bool success = stateManager.processToken("div");
        REQUIRE(success);
        REQUIRE(!stateManager.isError());
    }
    
    SECTION("Reset") {
        stateManager.processToken("div");
        stateManager.reset();
        REQUIRE(stateManager.getCurrentState()->getStateType() == CHTL::StateType::INITIAL);
        REQUIRE(!stateManager.isError());
    }
}

TEST_CASE("CHTLState - State Transitions", "[state_machine]") {
    CHTL::CHTLStateManager stateManager;
    stateManager.initialize();
    
    SECTION("Element state transition") {
        bool success = stateManager.processToken("div");
        REQUIRE(success);
        REQUIRE(stateManager.getCurrentState()->getStateType() == CHTL::StateType::ELEMENT);
    }
    
    SECTION("Text state transition") {
        bool success = stateManager.processToken("text");
        REQUIRE(success);
        REQUIRE(stateManager.getCurrentState()->getStateType() == CHTL::StateType::TEXT);
    }
}