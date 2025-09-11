#include "Systems/SDLManager.h"
#include "Interface/ui/UIManager.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/UITextInput.h"
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
    
    // Create title
    auto titleLabel = std::make_shared<UILabel>(50, 20, 700, 40, sdlManager, "UITextInput Component Examples");
    titleLabel->setTextAlignment(UILabel::TextAlignment::CENTER);
    titleLabel->setBackgroundColor({50, 50, 100, 255});
    titleLabel->setTextColor({255, 255, 255, 255});
    
    // 1. Basic text input
    auto basicLabel = std::make_shared<UILabel>(50, 80, 200, 25, sdlManager, "Basic Text Input:");
    basicLabel->setTransparentBackground();
    
    auto basicInput = std::make_shared<UITextInput>(50, 110, 300, 35, sdlManager, "Initial text");
    basicInput->setPlaceholder("Enter some text...");
    
    // 2. Password input
    auto passwordLabel = std::make_shared<UILabel>(50, 160, 200, 25, sdlManager, "Password Input:");
    passwordLabel->setTransparentBackground();
    
    auto passwordInput = std::make_shared<UITextInput>(50, 190, 300, 35, sdlManager);
    passwordInput->setPassword(true);
    passwordInput->setPlaceholder("Enter password...");
    
    // 3. Limited length input
    auto limitLabel = std::make_shared<UILabel>(50, 240, 200, 25, sdlManager, "Limited Length (max 10):");
    limitLabel->setTransparentBackground();
    
    auto limitInput = std::make_shared<UITextInput>(50, 270, 300, 35, sdlManager);
    limitInput->setMaxLength(10);
    limitInput->setPlaceholder("Max 10 characters");
    
    // 4. Read-only input
    auto readonlyLabel = std::make_shared<UILabel>(50, 320, 200, 25, sdlManager, "Read-only Input:");
    readonlyLabel->setTransparentBackground();
    
    auto readonlyInput = std::make_shared<UITextInput>(50, 350, 300, 35, sdlManager, "Cannot edit this");
    readonlyInput->setReadOnly(true);
    readonlyInput->setBackgroundColor({240, 240, 240, 255});
    
    // 5. Styled input
    auto styledLabel = std::make_shared<UILabel>(400, 80, 200, 25, sdlManager, "Styled Input:");
    styledLabel->setTransparentBackground();
    
    auto styledInput = std::make_shared<UITextInput>(400, 110, 300, 35, sdlManager);
    styledInput->setTextColor({0, 100, 0, 255}); // Green text
    styledInput->setBackgroundColor({250, 250, 200, 255}); // Light yellow background
    styledInput->setBorderColor({100, 100, 0, 255}); // Dark yellow border
    styledInput->setPlaceholder("Styled input field");
    styledInput->setPlaceholderColor({150, 150, 100, 255});
    
    // 6. Validation input (numbers only)
    auto validationLabel = std::make_shared<UILabel>(400, 160, 200, 25, sdlManager, "Numbers Only:");
    validationLabel->setTransparentBackground();
    
    auto validationInput = std::make_shared<UITextInput>(400, 190, 300, 35, sdlManager);
    validationInput->setPlaceholder("Enter numbers only...");
    validationInput->setValidationCallback([](const std::string& text) {
        // Allow only digits
        for (char c : text) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        return true;
    });
    
    // Status display
    auto statusLabel = std::make_shared<UILabel>(50, 400, 650, 30, sdlManager, 
        "Status: Click on text inputs to focus them. Use Tab to move between inputs.");
    statusLabel->setTextColor({0, 0, 150, 255});
    statusLabel->setTransparentBackground();
    
    // Interactive buttons
    auto clearButton = std::make_shared<FocusableButton>(50, 450, 120, 35, sdlManager, 
        "Clear All", [&]() {
            basicInput->clearText();
            passwordInput->clearText();
            limitInput->clearText();
            styledInput->clearText();
            validationInput->clearText();
            statusLabel->setText("Status: All text inputs cleared.");
        });
    
    auto showValuesButton = std::make_shared<FocusableButton>(180, 450, 120, 35, sdlManager,
        "Show Values", [&]() {
            std::string values = "Values: Basic='" + basicInput->getText() + 
                               "', Limited='" + limitInput->getText() + 
                               "', Styled='" + styledInput->getText() + 
                               "', Numbers='" + validationInput->getText() + "'";
            statusLabel->setText(values);
        });
    
    auto testSelectAllButton = std::make_shared<FocusableButton>(310, 450, 120, 35, sdlManager,
        "Select All Basic", [&]() {
            uiManager.setFocus(basicInput);
            basicInput->selectAll();
            statusLabel->setText("Status: Selected all text in basic input. Try Ctrl+C to copy.");
        });
    
    // Set up text change callbacks
    basicInput->setOnTextChanged([&](const std::string& text) {
        statusLabel->setText("Status: Basic input changed to: '" + text + "'");
    });
    
    styledInput->setOnEnterPressed([&]() {
        statusLabel->setText("Status: Enter pressed in styled input! Text: '" + styledInput->getText() + "'");
    });
    
    // Add all components to UI manager
    uiManager.addComponent(titleLabel);
    uiManager.addComponent(basicLabel);
    uiManager.addComponent(basicInput);
    uiManager.addComponent(passwordLabel);
    uiManager.addComponent(passwordInput);
    uiManager.addComponent(limitLabel);
    uiManager.addComponent(limitInput);
    uiManager.addComponent(readonlyLabel);
    uiManager.addComponent(readonlyInput);
    uiManager.addComponent(styledLabel);
    uiManager.addComponent(styledInput);
    uiManager.addComponent(validationLabel);
    uiManager.addComponent(validationInput);
    uiManager.addComponent(statusLabel);
    uiManager.addComponent(clearButton);
    uiManager.addComponent(showValuesButton);
    uiManager.addComponent(testSelectAllButton);
    
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
                    if (event.key.keysym.mod & KMOD_SHIFT) {
                        uiManager.focusPrevious();
                    } else {
                        uiManager.focusNext();
                    }
                }
                
                // Forward events to focused component
                auto focused = uiManager.getFocusedComponent();
                if (focused) {
                    focused->handleEvent(event);
                }
            } else if (event.type == SDL_TEXTINPUT) {
                // Forward text input events to focused component
                auto focused = uiManager.getFocusedComponent();
                if (focused) {
                    focused->handleEvent(event);
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                // Handle component clicks and focus
                auto component = uiManager.getComponentAt(event.button.x, event.button.y);
                if (component) {
                    if (component->canReceiveFocus()) {
                        uiManager.setFocus(component);
                    }
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
