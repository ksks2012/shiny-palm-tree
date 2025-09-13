#include <catch2/catch.hpp>
#include "Interface/ui/UILabel.h"
#include "Systems/SDLManager.h"

// Mock SDLManager for testing
class MockSDLManager : public SDLManager {
public:
    MockSDLManager() {
        // Initialize minimal SDL for testing
    }
};

TEST_CASE("UILabel Basic Functionality", "[UILabel]") {
    MockSDLManager sdl;
    
    SECTION("Construction and basic properties") {
        UILabel label(10, 20, 100, 30, sdl, "Test Label");
        
        REQUIRE(label.getX() == 10);
        REQUIRE(label.getY() == 20);
        REQUIRE(label.getWidth() == 100);
        REQUIRE(label.getHeight() == 30);
        REQUIRE(label.getText() == "Test Label");
        REQUIRE_FALSE(label.canReceiveFocus()); // Labels are not focusable by default
    }
    
    SECTION("Text property changes") {
        UILabel label(0, 0, 100, 30, sdl);
        
        // Initially empty
        REQUIRE(label.getText().empty());
        
        // Set text
        label.setText("New Text");
        REQUIRE(label.getText() == "New Text");
        
        // Change text
        label.setText("Different Text");
        REQUIRE(label.getText() == "Different Text");
    }
    
    SECTION("Text alignment options") {
        UILabel label(0, 0, 100, 30, sdl, "Test");
        
        // Default alignment
        REQUIRE(label.getTextAlignment() == UILabel::TextAlignment::LEFT);
        
        // Change alignment
        label.setTextAlignment(UILabel::TextAlignment::CENTER);
        REQUIRE(label.getTextAlignment() == UILabel::TextAlignment::CENTER);
        
        label.setTextAlignment(UILabel::TextAlignment::RIGHT);
        REQUIRE(label.getTextAlignment() == UILabel::TextAlignment::RIGHT);
    }
    
    SECTION("Background color settings") {
        UILabel label(0, 0, 100, 30, sdl);
        
        // Initially no background
        REQUIRE_FALSE(label.hasBackground());
        
        // Set background
        SDL_Color red = {255, 0, 0, 255};
        label.setBackgroundColor(red);
        REQUIRE(label.hasBackground());
        REQUIRE(label.getTextColor().r == red.r);
        
        // Remove background
        label.setTransparentBackground();
        REQUIRE_FALSE(label.hasBackground());
    }
    
    SECTION("Word wrap settings") {
        UILabel label(0, 0, 100, 60, sdl);
        
        // Default no word wrap
        REQUIRE_FALSE(label.getWordWrap());
        
        // Enable word wrap
        label.setWordWrap(true);
        REQUIRE(label.getWordWrap());
        
        // Disable word wrap
        label.setWordWrap(false);
        REQUIRE_FALSE(label.getWordWrap());
    }
    
    SECTION("Auto resize functionality") {
        UILabel label(0, 0, 100, 30, sdl);
        
        // Default no auto resize
        REQUIRE_FALSE(label.getAutoResize());
        
        // Enable auto resize
        label.setAutoResize(true);
        REQUIRE(label.getAutoResize());
        
        // Note: Actual resizing behavior would require a real SDL context
        // so we just test the property setting here
    }
    
    SECTION("Layout calculations") {
        UILabel label(0, 0, 100, 30, sdl, "Test Text");
        
        // Layout should not throw and should be callable multiple times
        REQUIRE_NOTHROW(label.layout());
        REQUIRE_NOTHROW(label.layout());
        
        // After layout, properties should remain consistent
        REQUIRE(label.getText() == "Test Text");
        REQUIRE(label.getWidth() == 100);
        REQUIRE(label.getHeight() == 30);
    }
}
