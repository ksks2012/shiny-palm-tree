#include "Interface/ui/UITheme.h"
#include "Systems/SDLManager.h"
#include "Interface/ui/UIManager.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/UIProgressBar.h"
#include "Interface/ui/UITextInput.h"
#include <iostream>

int main() {
    // Initialize SDL
    SDLManager sdlManager;
    if (!sdlManager.initialize()) {
        std::cerr << "Failed to initialize SDL" << std::endl;
        return -1;
    }

    SDL_Window* window = sdlManager.getWindow();
    SDL_Renderer* renderer = sdlManager.getRenderer();
    UIManager uiManager;
    
    // Get theme instance
    UITheme& theme = UITheme::getInstance();
    
    std::cout << "=== UI Theme System Demo ===" << std::endl;
    std::cout << "Current theme: " << theme.getCurrentTheme().name << std::endl;
    std::cout << "Available themes:" << std::endl;
    for (const std::string& themeName : theme.getAvailableThemes()) {
        std::cout << "- " << themeName << std::endl;
    }
    
    // Create simple demonstration UI elements
    // Using proper constructors for each UI element
    
    // Theme buttons
    auto darkButton = std::make_unique<UIButton>("Dark Theme", 50, 50, 120, 30, sdlManager, [&]() {
        theme.setCurrentTheme(UITheme::ThemeType::DARK);
        std::cout << "Switched to Dark Theme" << std::endl;
    });
    
    auto lightButton = std::make_unique<UIButton>("Light Theme", 180, 50, 120, 30, sdlManager, [&]() {
        theme.setCurrentTheme(UITheme::ThemeType::LIGHT);
        std::cout << "Switched to Light Theme" << std::endl;
    });
    
    auto contrastButton = std::make_unique<UIButton>("High Contrast", 310, 50, 120, 30, sdlManager, [&]() {
        theme.setCurrentTheme(UITheme::ThemeType::HIGH_CONTRAST);
        std::cout << "Switched to High Contrast Theme" << std::endl;
    });
    
    // Labels to show theme information
    auto titleLabel = std::make_unique<UILabel>(50, 100, 400, 25, sdlManager, "UI Theme System Demo");
    auto infoLabel = std::make_unique<UILabel>(50, 130, 400, 20, sdlManager, "Click buttons to switch themes");
    
    // Progress bar to demonstrate themed components
    auto progressBar = std::make_unique<UIProgressBar>(50, 160, 300, 25, sdlManager);
    progressBar->setProgress(0.75f);
    progressBar->setShowText(true);
    
    // Text input
    auto textInput = std::make_unique<UITextInput>(50, 200, 300, 30, sdlManager, "Sample text");
    
    // Add components to a container for easy management
    std::vector<UIComponent*> components;
    components.push_back(darkButton.get());
    components.push_back(lightButton.get());
    components.push_back(contrastButton.get());
    components.push_back(titleLabel.get());
    components.push_back(infoLabel.get());
    components.push_back(progressBar.get());
    components.push_back(textInput.get());
    
    // Color information display
    std::cout << "\n=== Theme Color Query Demo ===" << std::endl;
    SDL_Color buttonBg = theme.getColor("buttonBackground");
    SDL_Color textColor = theme.getColor("text");
    SDL_Color progressFill = theme.getColor("progressFill");
    
    std::cout << "Button background color: RGB(" << (int)buttonBg.r << "," 
              << (int)buttonBg.g << "," << (int)buttonBg.b << ")" << std::endl;
    std::cout << "Text color: RGB(" << (int)textColor.r << "," 
              << (int)textColor.g << "," << (int)textColor.b << ")" << std::endl;
    std::cout << "Progress fill color: RGB(" << (int)progressFill.r << "," 
              << (int)progressFill.g << "," << (int)progressFill.b << ")" << std::endl;
    
    // Custom theme demonstration
    std::cout << "\n=== Creating Custom Theme ===" << std::endl;
    UIThemeData customTheme = theme.getCurrentTheme();  // Start with current theme
    customTheme.name = "Custom Blue Theme";
    customTheme.description = "A custom theme with blue accents";
    
    // Customize colors
    customTheme.colors.buttonBackground = {30, 60, 120, 255};     // Dark blue
    customTheme.colors.buttonHover = {40, 80, 160, 255};          // Lighter blue on hover
    customTheme.colors.progressFill = {60, 120, 200, 255};        // Blue progress
    customTheme.colors.inputFocused = {80, 140, 220, 255};        // Blue focus
    customTheme.colors.info = {100, 160, 255, 255};               // Bright blue info
    
    // Test invalid color query
    SDL_Color invalidColor = theme.getColor("nonExistentColor");  // Should show warning
    
    // Main event loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            // Handle events for all components
            for (UIComponent* component : components) {
                component->handleEvent(event);
            }
        }
        
        // Clear screen with current theme background color
        SDL_Color bgColor = theme.getCurrentTheme().colors.background;
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(renderer);
        
        // Update and render all components
        for (UIComponent* component : components) {
            if (component->isVisible()) {
                component->render();
            }
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // ~60 FPS
    }
    
    std::cout << "\n=== Theme System Demo Complete ===" << std::endl;
    
    sdlManager.cleanup();
    return 0;
}
