#include <catch2/catch.hpp>
#include "Interface/ui/UITextInput.h"
#include "Systems/SDLManager.h"

// Mock SDLManager for testing
class MockSDLManager : public SDLManager {
public:
    MockSDLManager() {
        // Initialize minimal SDL for testing
    }
};

TEST_CASE("UITextInput Basic Functionality", "[UITextInput]") {
    MockSDLManager sdl;
    
    SECTION("Construction and basic properties") {
        UITextInput input(10, 20, 200, 30, sdl, "Initial Text");
        
        REQUIRE(input.getX() == 10);
        REQUIRE(input.getY() == 20);
        REQUIRE(input.getWidth() == 200);
        REQUIRE(input.getHeight() == 30);
        REQUIRE(input.getText() == "Initial Text");
        REQUIRE(input.canReceiveFocus()); // Text inputs are focusable
    }
    
    SECTION("Text property changes") {
        UITextInput input(0, 0, 200, 30, sdl);
        
        // Initially empty
        REQUIRE(input.getText().empty());
        
        // Set text
        input.setText("New Text");
        REQUIRE(input.getText() == "New Text");
        
        // Clear text
        input.clearText();
        REQUIRE(input.getText().empty());
    }
    
    SECTION("Placeholder text") {
        UITextInput input(0, 0, 200, 30, sdl);
        
        // Set placeholder
        input.setPlaceholder("Enter text here...");
        REQUIRE(input.getPlaceholder() == "Enter text here...");
        
        // Change placeholder
        input.setPlaceholder("Type something");
        REQUIRE(input.getPlaceholder() == "Type something");
    }
    
    SECTION("Input properties") {
        UITextInput input(0, 0, 200, 30, sdl);
        
        // Max length
        input.setMaxLength(10);
        REQUIRE(input.getMaxLength() == 10);
        
        // Read-only
        REQUIRE_FALSE(input.isReadOnly());
        input.setReadOnly(true);
        REQUIRE(input.isReadOnly());
        
        // Password mode
        REQUIRE_FALSE(input.isPassword());
        input.setPassword(true);
        REQUIRE(input.isPassword());
    }
    
    SECTION("Max length enforcement") {
        UITextInput input(0, 0, 200, 30, sdl);
        input.setMaxLength(5);
        
        input.setText("Hello World");
        REQUIRE(input.getText() == "Hello"); // Should be truncated to 5 chars
        
        input.setText("Hi");
        REQUIRE(input.getText() == "Hi"); // Should accept shorter text
    }
    
    SECTION("Selection functionality") {
        UITextInput input(0, 0, 200, 30, sdl, "Hello World");
        
        // Initially no selection
        REQUIRE_FALSE(input.hasSelection());
        
        // Select all
        input.selectAll();
        REQUIRE(input.hasSelection());
        
        // Clear selection
        input.clearSelection();
        REQUIRE_FALSE(input.hasSelection());
    }
    
    SECTION("Color properties") {
        UITextInput input(0, 0, 200, 30, sdl);
        
        // Text color
        SDL_Color red = {255, 0, 0, 255};
        input.setTextColor(red);
        SDL_Color retrievedColor = input.getTextColor();
        REQUIRE(retrievedColor.r == 255);
        REQUIRE(retrievedColor.g == 0);
        REQUIRE(retrievedColor.b == 0);
        
        // Background color
        SDL_Color white = {255, 255, 255, 255};
        input.setBackgroundColor(white);
        SDL_Color bgColor = input.getBackgroundColor();
        REQUIRE(bgColor.r == 255);
        REQUIRE(bgColor.g == 255);
        REQUIRE(bgColor.b == 255);
        
        // Border color
        SDL_Color blue = {0, 0, 255, 255};
        input.setBorderColor(blue);
        SDL_Color borderColor = input.getBorderColor();
        REQUIRE(borderColor.r == 0);
        REQUIRE(borderColor.g == 0);
        REQUIRE(borderColor.b == 255);
        
        // Placeholder color
        SDL_Color gray = {128, 128, 128, 255};
        input.setPlaceholderColor(gray);
        SDL_Color placeholderColor = input.getPlaceholderColor();
        REQUIRE(placeholderColor.r == 128);
        REQUIRE(placeholderColor.g == 128);
        REQUIRE(placeholderColor.b == 128);
    }
    
    SECTION("Focus handling") {
        UITextInput input(0, 0, 200, 30, sdl);
        
        // Simulate focus gain
        input.onFocusGained();
        
        // Simulate focus loss
        input.onFocusLost();
        
        // Should not crash and should be callable multiple times
        REQUIRE_NOTHROW(input.onFocusGained());
        REQUIRE_NOTHROW(input.onFocusLost());
    }
    
    SECTION("Layout calculations") {
        UITextInput input(0, 0, 200, 30, sdl, "Test Text");
        
        // Layout should not throw and should be callable multiple times
        REQUIRE_NOTHROW(input.layout());
        REQUIRE_NOTHROW(input.layout());
        
        // After layout, properties should remain consistent
        REQUIRE(input.getText() == "Test Text");
        REQUIRE(input.getWidth() == 200);
        REQUIRE(input.getHeight() == 30);
    }
    
    SECTION("Callback functionality") {
        UITextInput input(0, 0, 200, 30, sdl);
        
        bool textChanged = false;
        bool enterPressed = false;
        std::string lastText;
        
        // Set callbacks
        input.setOnTextChanged([&](const std::string& text) {
            textChanged = true;
            lastText = text;
        });
        
        input.setOnEnterPressed([&]() {
            enterPressed = true;
        });
        
        // Test text change callback (would be triggered by actual text input)
        input.setText("New Text");
        // Note: In actual usage, callbacks would be triggered by user input
        
        // Test validation callback
        input.setValidationCallback([](const std::string& text) {
            return text.length() <= 10; // Only allow up to 10 characters
        });
        
        // This should work (validation passes)
        input.setText("Short");
        REQUIRE(input.getText() == "Short");
        
        // Note: Full validation testing would require simulating actual key events
    }
}
