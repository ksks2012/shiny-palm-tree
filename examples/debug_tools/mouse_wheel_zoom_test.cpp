#include "Interface/ui/HexGridEditor.h"
#include "Interface/ui/UIManager.h"
#include "Systems/SDLManager.h"
#include <iostream>

/**
 * Simple test to verify mouse wheel zoom functionality
 */
int main() {
    std::cout << "=== Mouse Wheel Zoom Test ===" << std::endl;
    
    SDLManager sdlManager;
    if (!sdlManager.initialize()) {
        std::cout << "ERROR: Failed to initialize SDL" << std::endl;
        return -1;
    }
    
    UIManager uiManager;
    auto hexEditor = std::make_shared<HexGridEditor>(0, 0, 1200, 800, sdlManager);
    uiManager.addComponent(hexEditor, true);
    uiManager.setFocus(hexEditor);
    
    if (hexEditor->getRenderer()) {
        auto& config = hexEditor->getRenderer()->getRenderConfig();
        std::cout << "Initial zoom: " << config.zoomLevel << std::endl;
        
        // Simulate mouse wheel events to test zoom functionality
        SDL_Event wheelEventUp = {};
        wheelEventUp.type = SDL_MOUSEWHEEL;
        wheelEventUp.wheel.y = 1;  // Zoom in
        
        SDL_Event wheelEventDown = {};
        wheelEventDown.type = SDL_MOUSEWHEEL;
        wheelEventDown.wheel.y = -1;  // Zoom out
        
        // Test zoom in
        std::cout << "Testing zoom in..." << std::endl;
        hexEditor->handleEvent(wheelEventUp);
        std::cout << "Zoom after wheel up: " << config.zoomLevel << std::endl;
        
        // Test zoom out
        std::cout << "Testing zoom out..." << std::endl;
        hexEditor->handleEvent(wheelEventDown);
        std::cout << "Zoom after wheel down: " << config.zoomLevel << std::endl;
        
        // Test multiple zoom operations
        std::cout << "Testing multiple zoom in operations..." << std::endl;
        for (int i = 0; i < 3; i++) {
            hexEditor->handleEvent(wheelEventUp);
            std::cout << "Zoom level " << i+1 << ": " << config.zoomLevel << std::endl;
        }
        
        std::cout << "Testing zoom bounds..." << std::endl;
        // Test zoom bounds by zooming out a lot
        for (int i = 0; i < 10; i++) {
            hexEditor->handleEvent(wheelEventDown);
            std::cout << "Zoom out " << i+1 << ": " << config.zoomLevel << std::endl;
        }
        
        std::cout << "Final zoom level: " << config.zoomLevel << std::endl;
        
        if (config.zoomLevel != 1.0f) {
            std::cout << "✅ SUCCESS: Zoom functionality is working!" << std::endl;
        } else {
            std::cout << "❌ FAILURE: Zoom level did not change" << std::endl;
            return -1;
        }
    } else {
        std::cout << "❌ FAILURE: Could not get renderer" << std::endl;
        return -1;
    }
    
    sdlManager.cleanup();
    std::cout << "=== Test Completed ===" << std::endl;
    return 0;
}
