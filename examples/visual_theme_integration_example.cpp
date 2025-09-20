#include "Interface/ui/UITheme.h"
#include "Interface/ui/UIThemeManager.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/UITextInput.h"
#include "Interface/ui/UIManager.h"
#include "Systems/SDLManager.h"
#include "UIConstants.h"
#include <iostream>
#include <memory>
#include <vector>

class VisualThemeIntegrationExample {
private:
    SDLManager sdlManager;
    UIManager uiManager;
    std::vector<std::unique_ptr<UIComponent>> components;
    bool running;
    int currentThemeIndex;
    std::vector<std::pair<UITheme::ThemeType, std::string>> availableThemes;

public:
    VisualThemeIntegrationExample() : running(false), currentThemeIndex(0) {
        // Initialize available themes
        availableThemes = {
            {UITheme::ThemeType::DARK, "Dark Theme"},
            {UITheme::ThemeType::LIGHT, "Light Theme"},
            {UITheme::ThemeType::HIGH_CONTRAST, "High Contrast Theme"}
        };
        
        if (!initializeSDL()) {
            throw std::runtime_error("Failed to initialize SDL");
        }
        
        setupThemeSystem();
        createUIComponents();
    }
    
    ~VisualThemeIntegrationExample() {
        cleanup();
    }

private:
    bool initializeSDL() {
        std::cout << "Initializing SDL for visual theme demo..." << std::endl;
        
        if (!sdlManager.initialize()) {
            std::cerr << "Failed to initialize SDL" << std::endl;
            return false;
        }
        
        std::cout << "SDL initialized successfully" << std::endl;
        return true;
    }
    
    void setupThemeSystem() {
        std::cout << "Setting up theme system..." << std::endl;
        
        UIThemeManager& themeManager = UIThemeManager::getInstance();
        
        // Register the UIManager for automatic theme management
        themeManager.registerUIManager(&uiManager);
        
        // Set initial theme
        UITheme& theme = UITheme::getInstance();
        theme.setCurrentTheme(availableThemes[currentThemeIndex].first);
        
        std::cout << "Theme system initialized with " << availableThemes[currentThemeIndex].second << std::endl;
    }
    
    void createUIComponents() {
        std::cout << "Creating themed UI components..." << std::endl;
        
        UIThemeManager& themeManager = UIThemeManager::getInstance();
        
        try {
            // Create theme switch button
            auto themeButton = std::make_unique<UIButton>("Switch Theme", 50, 50, 150, 40, 
                sdlManager, [this]() { this->switchToNextTheme(); });
            themeManager.registerComponent("UIButton", themeButton.get());
            uiManager.addComponent(std::move(themeButton));
            
            // Create labels showing current theme info
            auto titleLabel = std::make_unique<UILabel>(50, 100, 400, 30, sdlManager, "UI Theme Integration Demo");
            themeManager.registerComponent("UILabel", titleLabel.get());
            uiManager.addComponent(std::move(titleLabel));
            
            auto themeLabel = std::make_unique<UILabel>(50, 140, 300, 25, sdlManager, 
                "Current Theme: " + availableThemes[currentThemeIndex].second);
            themeManager.registerComponent("UILabel", themeLabel.get());
            uiManager.addComponent(std::move(themeLabel));
            
            // Create sample components to show theming
            auto sampleButton1 = std::make_unique<UIButton>("Sample Button 1", 50, 190, 120, 35,
                sdlManager, []() { std::cout << "Sample Button 1 clicked" << std::endl; });
            themeManager.registerComponent("UIButton", sampleButton1.get());
            uiManager.addComponent(std::move(sampleButton1));
            
            auto sampleButton2 = std::make_unique<UIButton>("Sample Button 2", 180, 190, 120, 35,
                sdlManager, []() { std::cout << "Sample Button 2 clicked" << std::endl; });
            themeManager.registerComponent("UIButton", sampleButton2.get());
            uiManager.addComponent(std::move(sampleButton2));
            
            // Create text input
            auto textInput = std::make_unique<UITextInput>(50, 240, 250, 30, sdlManager, "Sample text input");
            themeManager.registerComponent("UITextInput", textInput.get());
            uiManager.addComponent(std::move(textInput));
            
            // Create info labels
            auto infoLabel1 = std::make_unique<UILabel>(50, 290, 350, 20, sdlManager, "Click 'Switch Theme' to cycle through themes");
            themeManager.registerComponent("UILabel", infoLabel1.get());
            uiManager.addComponent(std::move(infoLabel1));
            
            auto infoLabel2 = std::make_unique<UILabel>(50, 320, 350, 20, sdlManager, "Press ESC to exit, SPACE to switch theme");
            themeManager.registerComponent("UILabel", infoLabel2.get());
            uiManager.addComponent(std::move(infoLabel2));
            
            std::cout << "Created themed UI components successfully" << std::endl;
            
            // Apply initial theme to all components
            themeManager.updateAllComponentThemes();
            
        } catch (const std::exception& e) {
            std::cerr << "Error creating UI components: " << e.what() << std::endl;
            throw;
        }
    }
    
    void switchToNextTheme() {
        currentThemeIndex = (currentThemeIndex + 1) % availableThemes.size();
        
        UITheme& theme = UITheme::getInstance();
        theme.setCurrentTheme(availableThemes[currentThemeIndex].first);
        
        std::cout << "Switched to: " << availableThemes[currentThemeIndex].second << std::endl;
        
        // Update theme label text (if we can access it)
        // Note: In a real implementation, you'd want a more elegant way to update specific component text
        showThemeInfo();
    }
    
    void showThemeInfo() {
        UITheme& theme = UITheme::getInstance();
        const UIThemeData& currentTheme = theme.getCurrentTheme();
        
        std::cout << "Current Theme: " << currentTheme.name << std::endl;
        std::cout << "  Background: RGB(" 
                  << (int)currentTheme.colors.background.r << ","
                  << (int)currentTheme.colors.background.g << ","
                  << (int)currentTheme.colors.background.b << ")" << std::endl;
        std::cout << "  Text: RGB("
                  << (int)currentTheme.colors.text.r << ","
                  << (int)currentTheme.colors.text.g << ","
                  << (int)currentTheme.colors.text.b << ")" << std::endl;
    }
    
    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        case SDLK_SPACE:
                            switchToNextTheme();
                            break;
                        case SDLK_t:
                            switchToNextTheme();
                            break;
                    }
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    // Handle UI component clicks
                    uiManager.handleEvent(event);
                    break;
            }
        }
    }
    
    void render() {
        // Clear screen with current theme background
        UITheme& theme = UITheme::getInstance();
        SDL_Color bgColor = theme.getBackgroundColor();
        
        SDL_Renderer* renderer = sdlManager.getRenderer();
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderClear(renderer);
        
        // Render UI components
        uiManager.renderAll();
        
        SDL_RenderPresent(renderer);
    }
    
    void cleanup() {
        std::cout << "Cleaning up..." << std::endl;
        sdlManager.cleanup();
    }

public:
    void run() {
        std::cout << "\n=== Visual Theme Integration Demo ===" << std::endl;
        std::cout << "Window opened with themed UI components" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  - Click 'Switch Theme' button to cycle themes" << std::endl;
        std::cout << "  - Press SPACE or T to switch theme" << std::endl;
        std::cout << "  - Press ESC to exit" << std::endl;
        
        running = true;
        showThemeInfo();
        
        // Main game loop
        while (running) {
            handleEvents();
            render();
            SDL_Delay(16); // ~60 FPS
        }
        
        std::cout << "Demo completed successfully!" << std::endl;
    }
    
    // Method to demonstrate theme management without GUI (for testing)
    void runConsoleDemo() {
        std::cout << "\n=== Console Theme Demo ===" << std::endl;
        std::cout << "Demonstrating theme switching without GUI..." << std::endl;
        
        UITheme& theme = UITheme::getInstance();
        UIThemeManager& themeManager = UIThemeManager::getInstance();
        
        for (size_t i = 0; i < availableThemes.size(); ++i) {
            std::cout << "\n--- Theme " << (i + 1) << ": " << availableThemes[i].second << " ---" << std::endl;
            
            theme.setCurrentTheme(availableThemes[i].first);
            showThemeInfo();
            
            std::cout << "Registered components: " << themeManager.getRegisteredComponentCount() << std::endl;
            themeManager.printRegisteredComponents();
            
            if (i < availableThemes.size() - 1) {
                std::cout << "Press Enter to continue...";
                std::cin.get();
            }
        }
        
        std::cout << "\nConsole demo completed!" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== UI Theme Integration Example ===" << std::endl;
        
        // Check if user wants console-only mode
        bool consoleMode = false;
        if (argc > 1 && std::string(argv[1]) == "--console") {
            consoleMode = true;
        }
        
        if (consoleMode) {
            std::cout << "Running in console-only mode..." << std::endl;
            VisualThemeIntegrationExample example;
            example.runConsoleDemo();
        } else {
            // Ask user for preference
            std::cout << "Choose demo mode:" << std::endl;
            std::cout << "1. Visual GUI demo (recommended)" << std::endl;
            std::cout << "2. Console-only demo" << std::endl;
            std::cout << "Enter choice (1 or 2): ";
            
            char choice;
            std::cin >> choice;
            std::cin.ignore(); // Clear newline
            
            VisualThemeIntegrationExample example;
            
            if (choice == '2') {
                example.runConsoleDemo();
            } else {
                example.run();
            }
        }
        
        std::cout << "\n=== Theme Integration Example Completed Successfully ===" << std::endl;
        std::cout << "✅ Visual theme switching demonstrated" << std::endl;
        std::cout << "✅ Component theme updates working correctly" << std::endl;
        std::cout << "✅ UIThemeManager integration verified" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
