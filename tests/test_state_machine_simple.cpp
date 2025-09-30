#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CHTL/CHTLState.h"

TEST_CASE("State Machine Basic Tests", "[state_machine]") {
    CHTL::CHTLStateManager stateManager;
    stateManager.initialize();
    
    SECTION("Initial state should be INITIAL") {
        REQUIRE(stateManager.getCurrentState()->getStateType() == CHTL::StateType::INITIAL);
        REQUIRE(stateManager.getCurrentState()->getStateName() == "Initial");
    }
    
    SECTION("Processing valid token should change state") {
        bool success = stateManager.processToken("html");
        REQUIRE(success == true);
        REQUIRE(stateManager.getCurrentState()->getStateType() == CHTL::StateType::ELEMENT);
    }
    
    SECTION("Processing invalid token should cause error") {
        bool success = stateManager.processToken("invalid_token");
        REQUIRE(success == false);
        REQUIRE(stateManager.isError() == true);
        REQUIRE(stateManager.getErrorMessage() != "");
    }
    
    SECTION("Reset should return to initial state") {
        stateManager.processToken("html");
        stateManager.reset();
        REQUIRE(stateManager.getCurrentState()->getStateType() == CHTL::StateType::INITIAL);
        REQUIRE(stateManager.isError() == false);
    }
}