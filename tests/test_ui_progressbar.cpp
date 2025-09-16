#include "catch.hpp"
#include "Interface/ui/UIProgressBar.h"
#include "Systems/SDLManager.h"

TEST_CASE("UIProgressBar Basic Functionality", "[UIProgressBar]") {
    SDLManager sdl;
    sdl.initialize();
    
    SECTION("Construction and default values") {
        UIProgressBar progressBar(10, 20, 200, 30, sdl);
        
        REQUIRE(progressBar.getX() == 10);
        REQUIRE(progressBar.getY() == 20);
        REQUIRE(progressBar.getWidth() == 200);
        REQUIRE(progressBar.getHeight() == 30);
        REQUIRE(progressBar.getProgress() == 0.0f);
    }
    
    SECTION("Progress setting and getting") {
        UIProgressBar progressBar(0, 0, 100, 20, sdl);
        
        // Test normal progress values
        progressBar.setProgress(0.5f);
        REQUIRE(progressBar.getProgress() == 0.5f);
        
        progressBar.setProgress(0.0f);
        REQUIRE(progressBar.getProgress() == 0.0f);
        
        progressBar.setProgress(1.0f);
        REQUIRE(progressBar.getProgress() == 1.0f);
        
        // Test clamping of out-of-range values
        progressBar.setProgress(-0.5f);
        REQUIRE(progressBar.getProgress() == 0.0f);
        
        progressBar.setProgress(1.5f);
        REQUIRE(progressBar.getProgress() == 1.0f);
    }
    
    SECTION("Text display options") {
        UIProgressBar progressBar(0, 0, 100, 20, sdl);
        
        // Test show/hide text
        progressBar.setShowText(false);
        progressBar.setShowText(true);
        
        // Test custom text
        progressBar.setCustomText("Loading...");
        progressBar.clearCustomText();
    }
    
    SECTION("Color customization") {
        UIProgressBar progressBar(0, 0, 100, 20, sdl);
        
        SDL_Color red = {255, 0, 0, 255};
        SDL_Color green = {0, 255, 0, 255};
        SDL_Color blue = {0, 0, 255, 255};
        SDL_Color white = {255, 255, 255, 255};
        
        // Test 3-color version
        progressBar.setColors(red, green, blue);
        
        // Test 4-color version
        progressBar.setColors(red, green, blue, white);
    }
    
    SECTION("Border width") {
        UIProgressBar progressBar(0, 0, 100, 20, sdl);
        
        progressBar.setBorderWidth(0);
        progressBar.setBorderWidth(5);
        progressBar.setBorderWidth(-1); // Should clamp to 0
    }
    
    SECTION("Animation settings") {
        UIProgressBar progressBar(0, 0, 100, 20, sdl);
        
        // Test animation enable/disable
        progressBar.setAnimated(true);
        progressBar.setAnimated(false);
        
        // Test animation with custom speed
        progressBar.setAnimated(true, 3.0f);
    }
}

TEST_CASE("UIProgressBar Animation", "[UIProgressBar]") {
    SDLManager sdl;
    sdl.initialize();
    
    SECTION("Animated progress updates") {
        UIProgressBar progressBar(0, 0, 100, 20, sdl);
        progressBar.setAnimated(true, 2.0f); // 2.0 units per second
        
        // Set target progress
        progressBar.setProgress(1.0f);
        
        // Simulate time passing
        progressBar.update(0.1f); // 0.1 seconds
        
        // Animation should be in progress (not instant)
        progressBar.update(0.5f); // Total 0.6 seconds
        
        // After enough time, should reach target
        progressBar.update(1.0f); // Total 1.6 seconds
    }
    
    SECTION("Non-animated progress updates") {
        UIProgressBar progressBar(0, 0, 100, 20, sdl);
        progressBar.setAnimated(false);
        
        progressBar.setProgress(0.75f);
        progressBar.update(0.1f);
        
        // Should be immediate with no animation
    }
}

TEST_CASE("UIProgressBar Rendering", "[UIProgressBar]") {
    SDLManager sdl;
    sdl.initialize();
    
    SECTION("Rendering without crashes") {
        UIProgressBar progressBar(0, 0, 100, 20, sdl);
        
        // Test rendering at different progress values
        progressBar.setProgress(0.0f);
        REQUIRE_NOTHROW(progressBar.render());
        
        progressBar.setProgress(0.5f);
        REQUIRE_NOTHROW(progressBar.render());
        
        progressBar.setProgress(1.0f);
        REQUIRE_NOTHROW(progressBar.render());
    }
    
    SECTION("Layout calculation") {
        UIProgressBar progressBar(0, 0, 100, 20, sdl);
        REQUIRE_NOTHROW(progressBar.layout());
    }
}
