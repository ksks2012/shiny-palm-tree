#include "Interface/ui/UIDataBinding.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UITextInput.h"
#include "Interface/ui/UILayoutContainer.h"
#include "Systems/SDLManager.h"
#include <iostream>
#include <memory>

/**
 * Data Binding Example - Demonstrates how to use the UIDataBinding system
 * Shows various binding patterns including one-way and two-way data binding
 */

// Example data model with bindable properties
struct PlayerData {
    BindableProperty<std::string> name{"Unknown Player"};
    BindableProperty<int> level{1};
    BindableProperty<int> health{100};
    BindableProperty<int> maxHealth{100};
    BindableProperty<bool> isAlive{true};
    BindableProperty<std::string> status{"Ready"};
};

struct GameSettings {
    BindableProperty<bool> soundEnabled{true};
    BindableProperty<bool> debugMode{false};
    BindableProperty<int> volume{50};
    BindableProperty<std::string> difficulty{"Normal"};
};

std::shared_ptr<UILayoutContainer> createDataBindingExample(SDLManager& sdl) {
    std::cout << "Creating Data Binding Example..." << std::endl;
    
    // Create data models
    static PlayerData playerData;
    static GameSettings settings;
    
    // Create UI components
    auto uiContainer = std::make_shared<UILayoutContainer>(50, 50, 500, 400, sdl);
    uiContainer->setVerticalLayout(15);
    
    // Player info display with data binding
    auto playerNameLabel = std::make_shared<UILabel>(0, 0, 400, 30, sdl, "Player Name:");
    playerNameLabel->setBackgroundColor({100, 150, 200, 255}); // Light blue
    playerNameLabel->setTextColor({255, 255, 255, 255}); // White text
    
    auto playerLevelLabel = std::make_shared<UILabel>(0, 0, 400, 30, sdl, "Level: 1");
    playerLevelLabel->setBackgroundColor({150, 200, 100, 255}); // Light green
    playerLevelLabel->setTextColor({0, 0, 0, 255}); // Black text
    
    auto playerHealthLabel = std::make_shared<UILabel>(0, 0, 400, 30, sdl, "Health: 100/100");
    playerHealthLabel->setBackgroundColor({200, 100, 100, 255}); // Light red
    playerHealthLabel->setTextColor({255, 255, 255, 255}); // White text
    
    auto playerStatusLabel = std::make_shared<UILabel>(0, 0, 400, 30, sdl, "Status: Ready");
    playerStatusLabel->setBackgroundColor({200, 200, 100, 255}); // Light yellow
    playerStatusLabel->setTextColor({0, 0, 0, 255}); // Black text
    
    // Settings controls
    auto soundToggleButton = std::make_shared<UIButton>("Sound: ON", 0, 0, 200, 40, sdl, []() {
        settings.soundEnabled.set(!settings.soundEnabled.get());
        std::cout << "Sound toggled to: " << (settings.soundEnabled.get() ? "ON" : "OFF") << std::endl;
    });
    
    auto debugToggleButton = std::make_shared<UIButton>("Debug: OFF", 0, 0, 200, 40, sdl, []() {
        settings.debugMode.set(!settings.debugMode.get());
        std::cout << "Debug mode toggled to: " << (settings.debugMode.get() ? "ON" : "OFF") << std::endl;
    });
    
    // Player name input (two-way binding example)
    auto nameInput = std::make_shared<UITextInput>(0, 0, 300, 35, sdl, "Enter player name...");
    
    // Add components to container
    uiContainer->addChild(playerNameLabel);
    uiContainer->addChild(nameInput);
    uiContainer->addChild(playerLevelLabel);
    uiContainer->addChild(playerHealthLabel);
    uiContainer->addChild(playerStatusLabel);
    uiContainer->addChild(soundToggleButton);
    uiContainer->addChild(debugToggleButton);
    
    // Create data binding context
    UIDataBinding binding;
    
    // Bind player data to UI components
    binding.bindCustom<std::string>(
        playerNameLabel,
        [playerNameLabel](const std::string& value) {
            playerNameLabel->setText("Player: " + value);
        },
        []() { return playerData.name.get(); },
        playerData.name
    );
    
    binding.bindCustom<int>(
        playerLevelLabel,
        [playerLevelLabel](const int& value) {
            playerLevelLabel->setText("Level: " + std::to_string(value));
        },
        []() { return playerData.level.get(); },
        playerData.level
    );
    
    binding.bindCustom<int>(
        playerHealthLabel,
        [playerHealthLabel](const int& value) {
            std::string healthText = "Health: " + std::to_string(value) + "/" + std::to_string(playerData.maxHealth.get());
            playerHealthLabel->setText(healthText);
            
            // Change color based on health percentage
            float healthPercent = static_cast<float>(value) / playerData.maxHealth.get();
            if (healthPercent > 0.7f) {
                playerHealthLabel->setTextColor({0, 255, 0, 255}); // Green
            } else if (healthPercent > 0.3f) {
                playerHealthLabel->setTextColor({255, 255, 0, 255}); // Yellow
            } else {
                playerHealthLabel->setTextColor({255, 0, 0, 255}); // Red
            }
        },
        []() { return playerData.health.get(); },
        playerData.health
    );
    
    binding.bindText(playerStatusLabel, playerData.status);
    
    // Bind settings to buttons
    binding.bindCustom<bool>(
        soundToggleButton,
        [soundToggleButton](const bool& value) {
            soundToggleButton->setText("Sound: " + std::string(value ? "ON" : "OFF"));
        },
        []() { return settings.soundEnabled.get(); },
        settings.soundEnabled
    );
    
    binding.bindCustom<bool>(
        debugToggleButton,
        [debugToggleButton](const bool& value) {
            debugToggleButton->setText("Debug: " + std::string(value ? "ON" : "OFF"));
        },
        []() { return settings.debugMode.get(); },
        settings.debugMode
    );
    
    // Two-way binding for name input (simplified - would need more complex implementation)
    binding.bindText(nameInput, playerData.name);
    
    // Layout the container
    uiContainer->layout();
    
    std::cout << "Data binding setup complete!" << std::endl;
    std::cout << "Simulating data changes..." << std::endl;
    
    // Simulate data changes to demonstrate binding
    playerData.level.set(5);
    playerData.health.set(75);
    playerData.status.set("In Combat");
    
    // Update all bindings
    binding.updateBindings();
    
    std::cout << "Data binding example created with live updates!" << std::endl;
    
    return uiContainer;
}

int main() {
    SDLManager sdl;
    
    if (!sdl.initialize()) {
        std::cerr << "Failed to initialize SDL!" << std::endl;
        return -1;
    }
    
    // Set window title
    SDL_SetWindowTitle(sdl.getWindow(), "UI Framework - Data Binding Example");
    
    auto uiContainer = createDataBindingExample(sdl);
    
    std::cout << "Data binding example created successfully!" << std::endl;
    std::cout << "Press ESC to exit." << std::endl;
    
    // Simple render loop for demonstration
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            
            // Handle events for UI components
            if (uiContainer) {
                uiContainer->handleEvent(event);
            }
        }
        
        // Clear screen
        SDL_Renderer* renderer = sdl.getRenderer();
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);
        
        // Render UI components
        if (uiContainer) {
            uiContainer->render();
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    return 0;
}
