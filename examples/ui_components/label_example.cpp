#include "Systems/SDLManager.h"
#include "Interface/ui/UIManager.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/FocusableButton.h"
#include "CommonConstants.h"
#include <memory>
#include <iostream>

int main() {
    // Initialize SDL
    SDLManager sdlManager;
    if (!sdlManager.initialize()) {
        std::cerr << "Failed to initialize SDL" << std::endl;
        return -1;
    }
    
    // Create UI Manager
    UIManager uiManager;
    
    // Create various UILabel examples
    
    // 1. Simple text label
    auto titleLabel = std::make_shared<UILabel>(50, 30, 700, 40, sdlManager, "UILabel Component Examples");
    titleLabel->setTextAlignment(UILabel::TextAlignment::CENTER);
    titleLabel->setBackgroundColor({50, 50, 100, 255});
    titleLabel->setTextColor({255, 255, 255, 255});
    
    // 2. Left-aligned label
    auto leftLabel = std::make_shared<UILabel>(50, 100, 300, 30, sdlManager, "Left-aligned text");
    leftLabel->setTextAlignment(UILabel::TextAlignment::LEFT);
    leftLabel->setBackgroundColor({200, 200, 200, 255});
    
    // 3. Center-aligned label
    auto centerLabel = std::make_shared<UILabel>(400, 100, 300, 30, sdlManager, "Center-aligned text");
    centerLabel->setTextAlignment(UILabel::TextAlignment::CENTER);
    centerLabel->setBackgroundColor({200, 220, 200, 255});
    
    // 4. Right-aligned label
    auto rightLabel = std::make_shared<UILabel>(50, 150, 650, 30, sdlManager, "Right-aligned text");
    rightLabel->setTextAlignment(UILabel::TextAlignment::RIGHT);
    rightLabel->setBackgroundColor({220, 200, 200, 255});
    
    // 5. Multi-line label with word wrap
    auto wrapLabel = std::make_shared<UILabel>(50, 200, 300, 100, sdlManager, 
        "This is a longer text that will demonstrate the word wrapping functionality of the UILabel component. "
        "The text should automatically wrap to fit within the specified width.");
    wrapLabel->setWordWrap(true);
    wrapLabel->setBackgroundColor({240, 240, 180, 255});
    
    // 6. Auto-resizing label
    auto autoLabel = std::make_shared<UILabel>(400, 200, 100, 30, sdlManager, "Auto-sized");
    autoLabel->setAutoResize(true);
    autoLabel->setBackgroundColor({180, 240, 240, 255});
    
    // 7. Transparent background label
    auto transparentLabel = std::make_shared<UILabel>(400, 250, 300, 30, sdlManager, 
        "Transparent background label");
    transparentLabel->setTransparentBackground();
    transparentLabel->setTextColor({0, 100, 0, 255});
    
    // 8. Interactive button to change label text
    auto changeButton = std::make_shared<FocusableButton>(50, 350, 150, 40, sdlManager, 
        "Change Text", [&]() {
            static int counter = 0;
            counter++;
            titleLabel->setText("UILabel Examples - Updated " + std::to_string(counter) + " times");
        });
    
    // 9. Button to toggle word wrap
    auto wrapButton = std::make_shared<FocusableButton>(220, 350, 150, 40, sdlManager, 
        "Toggle Wrap", [&]() {
            wrapLabel->setWordWrap(!wrapLabel->getWordWrap());
        });
    
    // 10. Status label that updates
    auto statusLabel = std::make_shared<UILabel>(50, 420, 700, 30, sdlManager, 
        "Status: Ready - Use buttons to interact with labels");
    statusLabel->setTextColor({0, 0, 150, 255});
    
    // Add all components to UI manager
    uiManager.addComponent(titleLabel);
    uiManager.addComponent(leftLabel);
    uiManager.addComponent(centerLabel);
    uiManager.addComponent(rightLabel);
    uiManager.addComponent(wrapLabel);
    uiManager.addComponent(autoLabel);
    uiManager.addComponent(transparentLabel);
    uiManager.addComponent(changeButton);
    uiManager.addComponent(wrapButton);
    uiManager.addComponent(statusLabel);
    
    // Main loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                } else if (event.key.keysym.sym == SDLK_TAB) {
                    uiManager.focusNext();
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                // Handle button clicks
                auto component = uiManager.getComponentAt(event.button.x, event.button.y);
                if (component) {
                    component->handleEvent(event);
                    statusLabel->setText("Status: Clicked on component at (" + 
                                       std::to_string(event.button.x) + ", " + 
                                       std::to_string(event.button.y) + ")");
                }
            }
        }
        
        // Clear screen
        SDL_SetRenderDrawColor(sdlManager.getRenderer(), 240, 240, 240, 255);
        SDL_RenderClear(sdlManager.getRenderer());
        
        // Render UI
        uiManager.renderAll();
        
        // Present
        SDL_RenderPresent(sdlManager.getRenderer());
        
        SDL_Delay(16); // ~60 FPS
    }
    
    return 0;
}
