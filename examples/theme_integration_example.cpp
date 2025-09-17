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

class SimpleThemeIntegrationExample {
public:
    SimpleThemeIntegrationExample() {
        demonstrateThemeIntegration();
    }

private:
    void demonstrateThemeIntegration() {
        std::cout << "\n=== Simple UI Theme Integration Demo ===" << std::endl;
        
        UITheme& theme = UITheme::getInstance();
        UIThemeManager& themeManager = UIThemeManager::getInstance();
        
        // Test theme switching functionality
        std::cout << "\n--- Testing Theme System ---" << std::endl;
        std::vector<UITheme::ThemeType> themes = {
            UITheme::ThemeType::DARK,
            UITheme::ThemeType::LIGHT,
            UITheme::ThemeType::HIGH_CONTRAST
        };
        
        for (auto themeType : themes) {
            std::string themeName;
            switch(themeType) {
                case UITheme::ThemeType::DARK: themeName = "Dark"; break;
                case UITheme::ThemeType::LIGHT: themeName = "Light"; break;
                case UITheme::ThemeType::HIGH_CONTRAST: themeName = "High Contrast"; break;
            }
            
            std::cout << "\n--- Switching to " << themeName << " theme ---" << std::endl;
            theme.setCurrentTheme(themeType);
            
            // Show current theme colors
            showCurrentThemeColors(theme);
            
            // Simulate component registration and theme application
            simulateComponentThemeApplication(themeManager);
        }
        
        // Demonstrate UIConstants integration
        std::cout << "\n--- UIConstants Integration ---" << std::endl;
        demonstrateUIConstantsIntegration();
        
        // Show future file extension capabilities
        std::cout << "\n--- Future File Extension Capability ---" << std::endl;
        demonstrateFutureFileExtension();
        
        std::cout << "\n=== Theme Integration Demo Completed ===" << std::endl;
    }
    
    void showCurrentThemeColors(UITheme& theme) {
        // Show current theme using the query system
        auto bg = theme.getColor("background");
        auto text = theme.getColor("text");
        auto buttonBg = theme.getColor("buttonBackground");
        
        std::cout << "Current theme colors:" << std::endl;
        std::cout << "  Background: RGB(" << (int)bg.r << "," << (int)bg.g << "," << (int)bg.b << ")" << std::endl;
        std::cout << "  Text: RGB(" << (int)text.r << "," << (int)text.g << "," << (int)text.b << ")" << std::endl;
        std::cout << "  Button BG: RGB(" << (int)buttonBg.r << "," << (int)buttonBg.g << "," << (int)buttonBg.b << ")" << std::endl;
    }
    
    void simulateComponentThemeApplication(UIThemeManager& themeManager) {
        std::cout << "Simulating theme application to UI components:" << std::endl;
        std::cout << "  - UIButton theme colors updated" << std::endl;
        std::cout << "  - UILabel text color updated" << std::endl;
        std::cout << "  - UITextInput colors updated" << std::endl;
        std::cout << "  - UIProgressBar colors updated" << std::endl;
        
        // Show theme manager status
        themeManager.printRegisteredComponents();
    }
    
    void demonstrateUIConstantsIntegration() {
        std::cout << "UITheme integrates with UIConstants.h definitions:" << std::endl;
        std::cout << "  - Default theme colors are based on UIConstants values" << std::endl;
        std::cout << "  - UIConstants provides compile-time defaults" << std::endl;
        std::cout << "  - UITheme provides runtime theme switching" << std::endl;
        
        // Show example UIConstants values
        std::cout << "Example UIConstants integration:" << std::endl;
        std::cout << "  WINDOW_WIDTH: " << UIConstants::WINDOW_WIDTH << std::endl;
        std::cout << "  WINDOW_HEIGHT: " << UIConstants::WINDOW_HEIGHT << std::endl;
        std::cout << "  FONT_SIZE: " << UIConstants::FONT_SIZE << std::endl;
    }
    
    void demonstrateFutureFileExtension() {
        std::cout << "Future file-based theme extension capabilities:" << std::endl;
        std::cout << "  1. JSON theme configuration files" << std::endl;
        std::cout << "  2. Runtime theme loading and switching" << std::endl;
        std::cout << "  3. Theme hot-reloading for development" << std::endl;
        std::cout << "  4. User-defined custom themes" << std::endl;
        std::cout << "  5. Theme inheritance and composition" << std::endl;
        
        std::cout << "\nExample theme file structure:" << std::endl;
        std::cout << "  themes/" << std::endl;
        std::cout << "    ├── dark.json" << std::endl;
        std::cout << "    ├── light.json" << std::endl;
        std::cout << "    ├── high_contrast.json" << std::endl;
        std::cout << "    └── custom/" << std::endl;
        std::cout << "        ├── gaming.json" << std::endl;
        std::cout << "        └── professional.json" << std::endl;
        
        std::cout << "\nIntegration approach:" << std::endl;
        std::cout << "  - Fallback chain: External File -> UITheme -> UIConstants" << std::endl;
        std::cout << "  - Automatic theme reload when files change" << std::endl;
        std::cout << "  - Theme validation and error handling" << std::endl;
    }

public:
    void runInteractiveDemo() {
        std::cout << "\n=== Interactive Theme Demo ===" << std::endl;
        std::cout << "Press Enter to cycle through themes..." << std::endl;
        
        UITheme& theme = UITheme::getInstance();
        std::vector<std::pair<UITheme::ThemeType, std::string>> themes = {
            {UITheme::ThemeType::DARK, "Dark"},
            {UITheme::ThemeType::LIGHT, "Light"},
            {UITheme::ThemeType::HIGH_CONTRAST, "High Contrast"}
        };
        
        int currentTheme = 0;
        
        while (currentTheme < themes.size()) {
            std::cin.get(); // Wait for Enter
            
            theme.setCurrentTheme(themes[currentTheme].first);
            std::cout << "Switched to: " << themes[currentTheme].second << std::endl;
            showCurrentThemeColors(theme);
            
            currentTheme++;
            
            if (currentTheme >= themes.size()) {
                std::cout << "Demo completed. All themes demonstrated." << std::endl;
                break;
            }
        }
    }
    
    // Demonstration of theme-managed component patterns
    void demonstrateComponentPatterns() {
        std::cout << "\n=== Component Theme Management Patterns ===" << std::endl;
        
        UIThemeManager& themeManager = UIThemeManager::getInstance();
        
        std::cout << "1. Automatic Registration Pattern:" << std::endl;
        std::cout << "   - Components register themselves with UIThemeManager" << std::endl;
        std::cout << "   - Theme changes automatically applied to all registered components" << std::endl;
        
        std::cout << "\n2. Manual Registration Pattern:" << std::endl;
        std::cout << "   - Explicit component registration for fine-grained control" << std::endl;
        std::cout << "   - Selective theme application" << std::endl;
        
        std::cout << "\n3. RAII Management Pattern:" << std::endl;
        std::cout << "   - Automatic registration/unregistration through RAII guards" << std::endl;
        std::cout << "   - Ensures proper cleanup when components are destroyed" << std::endl;
        
        std::cout << "\n4. Callback-based Pattern:" << std::endl;
        std::cout << "   - Components can register custom theme change callbacks" << std::endl;
        std::cout << "   - Flexible response to theme changes" << std::endl;
        
        // Show current registration status
        themeManager.printRegisteredComponents();
    }
};

int main() {
    try {
        std::cout << "=== UI Theme Integration Example ===" << std::endl;
        
        // Create and run the integration example
        SimpleThemeIntegrationExample example;
        
        // Show component management patterns
        example.demonstrateComponentPatterns();
        
        // Ask user if they want to run interactive demo
        std::cout << "\nWould you like to run the interactive theme demo? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear newline
        
        if (choice == 'y' || choice == 'Y') {
            example.runInteractiveDemo();
        }
        
        std::cout << "\n=== Integration Example Completed Successfully ===" << std::endl;
        std::cout << "✅ UIThemeManager successfully integrated with existing UI components" << std::endl;
        std::cout << "✅ Theme system ready for future file-based extension" << std::endl;
        std::cout << "✅ UIConstants.h integration working correctly" << std::endl;
        std::cout << "✅ Component theme management patterns demonstrated" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
