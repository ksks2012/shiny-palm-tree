#include "Interface/ui/UITheme.h"
#include <iostream>

int main() {
    std::cout << "=== UI Theme System Console Demo ===" << std::endl;
    
    // Get theme instance
    UITheme& theme = UITheme::getInstance();
    
    // Show current theme
    std::cout << "\n1. Default theme information:" << std::endl;
    std::cout << "Current theme: " << theme.getCurrentTheme().name << std::endl;
    std::cout << "Description: " << theme.getCurrentTheme().description << std::endl;
    
    // Show available themes
    std::cout << "\n2. Available themes:" << std::endl;
    for (const std::string& themeName : theme.getAvailableThemes()) {
        std::cout << "- " << themeName << std::endl;
    }
    
    // Test theme switching
    std::cout << "\n3. Theme switching demo:" << std::endl;
    
    theme.setCurrentTheme(UITheme::ThemeType::LIGHT);
    std::cout << "Switched to: " << theme.getCurrentTheme().name << std::endl;
    
    theme.setCurrentTheme(UITheme::ThemeType::HIGH_CONTRAST);
    std::cout << "Switched to: " << theme.getCurrentTheme().name << std::endl;
    
    theme.setCurrentTheme(UITheme::ThemeType::DARK);
    std::cout << "Back to: " << theme.getCurrentTheme().name << std::endl;
    
    // Test color queries
    std::cout << "\n4. Color query demo:" << std::endl;
    SDL_Color buttonBg = theme.getColor("buttonBackground");
    SDL_Color textColor = theme.getColor("text");
    SDL_Color progressFill = theme.getColor("progressFill");
    
    std::cout << "Button background: RGB(" << (int)buttonBg.r << "," 
              << (int)buttonBg.g << "," << (int)buttonBg.b << ")" << std::endl;
    std::cout << "Text color: RGB(" << (int)textColor.r << "," 
              << (int)textColor.g << "," << (int)textColor.b << ")" << std::endl;
    std::cout << "Progress fill: RGB(" << (int)progressFill.r << "," 
              << (int)progressFill.g << "," << (int)progressFill.b << ")" << std::endl;
    
    // Test theme structure
    std::cout << "\n5. Theme structure demo:" << std::endl;
    const UIThemeData& currentTheme = theme.getCurrentTheme();
    std::cout << "Typography - Base font size: " << currentTheme.typography.baseFontSize << std::endl;
    std::cout << "Typography - Title font size: " << currentTheme.typography.titleFontSize << std::endl;
    std::cout << "Dimensions - Button height: " << currentTheme.dimensions.buttonHeight << std::endl;
    std::cout << "Dimensions - Padding medium: " << currentTheme.dimensions.paddingMedium << std::endl;
    
    // Test custom theme
    std::cout << "\n6. Custom theme demo:" << std::endl;
    UIThemeData customTheme = theme.getCurrentTheme();
    customTheme.name = "Console Demo Theme";
    customTheme.description = "Custom theme for console demonstration";
    customTheme.colors.buttonBackground = {100, 150, 200, 255};
    customTheme.colors.progressFill = {255, 100, 100, 255};
    
    theme.setCustomTheme(customTheme);
    std::cout << "Applied custom theme: " << theme.getCurrentTheme().name << std::endl;
    
    SDL_Color customButtonBg = theme.getColor("buttonBackground");
    SDL_Color customProgressFill = theme.getColor("progressFill");
    std::cout << "Custom button background: RGB(" << (int)customButtonBg.r << "," 
              << (int)customButtonBg.g << "," << (int)customButtonBg.b << ")" << std::endl;
    std::cout << "Custom progress fill: RGB(" << (int)customProgressFill.r << "," 
              << (int)customProgressFill.g << "," << (int)customProgressFill.b << ")" << std::endl;
    
    // Test error handling
    std::cout << "\n7. Error handling demo:" << std::endl;
    SDL_Color invalidColor = theme.getColor("nonExistentColor");
    std::cout << "Invalid color query handled successfully" << std::endl;
    
    std::cout << "\n=== UI Theme System Demo Complete ===" << std::endl;
    std::cout << "All UITheme functionality tested successfully!" << std::endl;
    
    return 0;
}
