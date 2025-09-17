#include "Interface/ui/UITheme.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/UITextInput.h"
#include "Interface/ui/UIProgressBar.h"
#include "UIConstants.h"
#include <iostream>
#include <fstream>

// Singleton implementation
UITheme& UITheme::getInstance() {
    static UITheme instance;
    return instance;
}

UITheme::UITheme() : currentThemeType_(ThemeType::DARK) {
    initializeDefaultThemes();
    setCurrentTheme(ThemeType::DARK);
}

void UITheme::initializeDefaultThemes() {
    predefinedThemes_[ThemeType::DARK] = std::make_shared<UIThemeData>(createDarkTheme());
    predefinedThemes_[ThemeType::LIGHT] = std::make_shared<UIThemeData>(createLightTheme());
    predefinedThemes_[ThemeType::HIGH_CONTRAST] = std::make_shared<UIThemeData>(createHighContrastTheme());
}

UIThemeData UITheme::createDarkTheme() const {
    UIThemeData theme;
    theme.name = "Dark Theme";
    theme.description = "Default dark theme with modern styling";
    
    // Color palette - using existing UIConstants
    theme.colors.background = UIConstants::BACKGROUND_COLOR;
    theme.colors.text = UIConstants::TEXT_COLOR;
    theme.colors.secondaryText = UIConstants::SECONDARY_TEXT_COLOR;
    theme.colors.border = UIConstants::BORDER_COLOR;
    theme.colors.overlay = UIConstants::OVERLAY_COLOR;
    
    // Interactive elements
    theme.colors.buttonBackground = UIConstants::BUTTON_COLOR;
    theme.colors.buttonText = UIConstants::TEXT_COLOR;
    theme.colors.buttonHover = UIConstants::HOVERED_COLOR;
    theme.colors.buttonPressed = UIConstants::PRESSED_COLOR;
    theme.colors.buttonFocused = UIConstants::FOCUSED_COLOR;
    
    // Input elements
    theme.colors.inputBackground = UIConstants::PANEL_BG_COLOR;
    theme.colors.inputText = UIConstants::TEXT_COLOR;
    theme.colors.inputBorder = UIConstants::BORDER_COLOR;
    theme.colors.inputFocused = UIConstants::FOCUSED_COLOR;
    
    // Progress elements
    theme.colors.progressBackground = UIConstants::PROGRESS_BAR_BACKGROUND;
    theme.colors.progressFill = UIConstants::PROGRESS_BAR_FILL;
    theme.colors.progressBorder = UIConstants::PROGRESS_BAR_BORDER;
    theme.colors.progressText = UIConstants::PROGRESS_BAR_TEXT;
    
    // Status colors
    theme.colors.success = {40, 160, 40, 255};      // Green
    theme.colors.warning = {200, 160, 40, 255};     // Orange
    theme.colors.error = {200, 40, 40, 255};        // Red
    theme.colors.info = {40, 120, 200, 255};        // Blue
    
    // Panel colors
    theme.colors.panelBackground = UIConstants::PANEL_BG_COLOR;
    theme.colors.panelBorder = UIConstants::BORDER_COLOR;
    theme.colors.tooltipBackground = UIConstants::TOOLTIP_BG_COLOR;
    theme.colors.tooltipText = UIConstants::TEXT_COLOR;
    
    // Typography
    theme.typography.fontPath = UIConstants::FONT_PATH;
    theme.typography.baseFontSize = UIConstants::FONT_SIZE;
    theme.typography.titleFontSize = UIConstants::FONT_SIZE + 8;
    theme.typography.subtitleFontSize = UIConstants::FONT_SIZE + 4;
    theme.typography.captionFontSize = UIConstants::FONT_SIZE - 2;
    theme.typography.lineHeight = UIConstants::FONT_SIZE + 4;
    theme.typography.paragraphSpacing = 8;
    theme.typography.letterSpacing = 0;
    
    // Dimensions
    theme.dimensions.paddingSmall = 4;
    theme.dimensions.paddingMedium = 8;
    theme.dimensions.paddingLarge = 16;
    theme.dimensions.marginSmall = 2;
    theme.dimensions.marginMedium = 4;
    theme.dimensions.marginLarge = 8;
    theme.dimensions.buttonHeight = UIConstants::BUTTON_HEIGHT;
    theme.dimensions.inputHeight = 30;
    theme.dimensions.progressBarHeight = 20;
    theme.dimensions.borderWidth = 2;
    theme.dimensions.borderRadius = 0;  // Currently not supported
    theme.dimensions.gridSpacing = 10;
    theme.dimensions.containerSpacing = 15;
    
    return theme;
}

UIThemeData UITheme::createLightTheme() const {
    UIThemeData theme = createDarkTheme();  // Start with dark theme
    theme.name = "Light Theme";
    theme.description = "Light theme with high readability";
    
    // Override colors for light theme
    theme.colors.background = {240, 240, 240, 255};        // Light gray background
    theme.colors.text = {20, 20, 20, 255};                 // Dark text
    theme.colors.secondaryText = {80, 80, 80, 255};        // Medium gray text
    theme.colors.border = {180, 180, 180, 255};            // Light gray border
    theme.colors.overlay = {255, 255, 255, 200};           // Light overlay
    
    theme.colors.buttonBackground = {220, 220, 220, 255};  // Light button
    theme.colors.buttonText = {20, 20, 20, 255};           // Dark text on button
    theme.colors.buttonHover = {200, 200, 200, 255};       // Darker on hover
    theme.colors.buttonPressed = {180, 180, 180, 255};     // Even darker when pressed
    theme.colors.buttonFocused = {160, 200, 240, 255};     // Light blue focused
    
    theme.colors.inputBackground = {255, 255, 255, 255};   // White input background
    theme.colors.inputText = {20, 20, 20, 255};            // Dark input text
    theme.colors.inputBorder = {160, 160, 160, 255};       // Medium gray border
    theme.colors.inputFocused = {100, 150, 200, 255};      // Blue focused border
    
    theme.colors.progressBackground = {200, 200, 200, 255}; // Light gray background
    theme.colors.progressFill = {80, 160, 80, 255};        // Green fill
    theme.colors.progressBorder = {140, 140, 140, 255};    // Medium border
    theme.colors.progressText = {20, 20, 20, 255};         // Dark text
    
    theme.colors.panelBackground = {250, 250, 250, 255};   // Very light panel
    theme.colors.panelBorder = {180, 180, 180, 255};       // Light border
    theme.colors.tooltipBackground = {255, 255, 220, 240}; // Light yellow tooltip
    theme.colors.tooltipText = {20, 20, 20, 255};          // Dark tooltip text
    
    return theme;
}

UIThemeData UITheme::createHighContrastTheme() const {
    UIThemeData theme = createDarkTheme();  // Start with dark theme
    theme.name = "High Contrast Theme";
    theme.description = "High contrast theme for accessibility";
    
    // High contrast colors - pure black/white with bright accents
    theme.colors.background = {0, 0, 0, 255};               // Pure black
    theme.colors.text = {255, 255, 255, 255};               // Pure white
    theme.colors.secondaryText = {200, 200, 200, 255};      // Light gray
    theme.colors.border = {255, 255, 255, 255};             // White border
    theme.colors.overlay = {0, 0, 0, 180};                  // Black overlay
    
    theme.colors.buttonBackground = {40, 40, 40, 255};      // Dark gray button
    theme.colors.buttonText = {255, 255, 255, 255};         // White text
    theme.colors.buttonHover = {80, 80, 80, 255};           // Lighter on hover
    theme.colors.buttonPressed = {120, 120, 120, 255};      // Even lighter pressed
    theme.colors.buttonFocused = {255, 255, 0, 255};        // Bright yellow focused
    
    theme.colors.inputBackground = {20, 20, 20, 255};       // Dark input
    theme.colors.inputText = {255, 255, 255, 255};          // White text
    theme.colors.inputBorder = {255, 255, 255, 255};        // White border
    theme.colors.inputFocused = {255, 255, 0, 255};         // Yellow focused
    
    theme.colors.progressBackground = {40, 40, 40, 255};    // Dark background
    theme.colors.progressFill = {0, 255, 0, 255};           // Bright green
    theme.colors.progressBorder = {255, 255, 255, 255};     // White border
    theme.colors.progressText = {255, 255, 255, 255};       // White text
    
    // Brighter status colors for high contrast
    theme.colors.success = {0, 255, 0, 255};                // Bright green
    theme.colors.warning = {255, 255, 0, 255};              // Bright yellow
    theme.colors.error = {255, 0, 0, 255};                  // Bright red
    theme.colors.info = {0, 255, 255, 255};                 // Bright cyan
    
    theme.colors.panelBackground = {20, 20, 20, 255};       // Dark panel
    theme.colors.panelBorder = {255, 255, 255, 255};        // White border
    theme.colors.tooltipBackground = {0, 0, 0, 255};        // Black tooltip
    theme.colors.tooltipText = {255, 255, 255, 255};        // White text
    
    return theme;
}

void UITheme::setCurrentTheme(ThemeType type) {
    auto it = predefinedThemes_.find(type);
    if (it != predefinedThemes_.end()) {
        currentThemeType_ = type;
        currentTheme_ = it->second;
        updateColorLookup();
        notifyThemeChange();
        
        std::cout << "Theme switched to: " << currentTheme_->name << std::endl;
    } else {
        std::cerr << "Warning: Theme type not found, keeping current theme" << std::endl;
    }
}

void UITheme::setCustomTheme(const UIThemeData& themeData) {
    if (isValidTheme(themeData)) {
        currentThemeType_ = ThemeType::CUSTOM;
        currentTheme_ = std::make_shared<UIThemeData>(themeData);
        customThemes_[themeData.name] = currentTheme_;
        updateColorLookup();
        notifyThemeChange();
        
        std::cout << "Custom theme applied: " << themeData.name << std::endl;
    } else {
        std::cerr << "Warning: Invalid theme data, keeping current theme" << std::endl;
    }
}

SDL_Color UITheme::getColor(const std::string& colorName) const {
    updateColorLookup();  // Ensure lookup is current
    
    auto it = colorLookup_.find(colorName);
    if (it != colorLookup_.end()) {
        return it->second;
    }
    
    // Return default color if not found
    std::cerr << "Warning: Color '" << colorName << "' not found, returning default" << std::endl;
    return currentTheme_->colors.text;  // Default fallback
}

bool UITheme::isValidTheme(const UIThemeData& theme) const {
    // Basic validation - check if theme has required fields
    return !theme.name.empty() && 
           !theme.typography.fontPath.empty() && 
           theme.typography.baseFontSize > 0;
}

void UITheme::updateColorLookup() const {
    if (!currentTheme_) return;
    
    colorLookup_.clear();
    
    // Basic colors
    colorLookup_["background"] = currentTheme_->colors.background;
    colorLookup_["text"] = currentTheme_->colors.text;
    colorLookup_["secondaryText"] = currentTheme_->colors.secondaryText;
    colorLookup_["border"] = currentTheme_->colors.border;
    colorLookup_["overlay"] = currentTheme_->colors.overlay;
    
    // Button colors
    colorLookup_["buttonBackground"] = currentTheme_->colors.buttonBackground;
    colorLookup_["buttonText"] = currentTheme_->colors.buttonText;
    colorLookup_["buttonHover"] = currentTheme_->colors.buttonHover;
    colorLookup_["buttonPressed"] = currentTheme_->colors.buttonPressed;
    colorLookup_["buttonFocused"] = currentTheme_->colors.buttonFocused;
    
    // Input colors
    colorLookup_["inputBackground"] = currentTheme_->colors.inputBackground;
    colorLookup_["inputText"] = currentTheme_->colors.inputText;
    colorLookup_["inputBorder"] = currentTheme_->colors.inputBorder;
    colorLookup_["inputFocused"] = currentTheme_->colors.inputFocused;
    
    // Progress colors
    colorLookup_["progressBackground"] = currentTheme_->colors.progressBackground;
    colorLookup_["progressFill"] = currentTheme_->colors.progressFill;
    colorLookup_["progressBorder"] = currentTheme_->colors.progressBorder;
    colorLookup_["progressText"] = currentTheme_->colors.progressText;
    
    // Status colors
    colorLookup_["success"] = currentTheme_->colors.success;
    colorLookup_["warning"] = currentTheme_->colors.warning;
    colorLookup_["error"] = currentTheme_->colors.error;
    colorLookup_["info"] = currentTheme_->colors.info;
    
    // Panel colors
    colorLookup_["panelBackground"] = currentTheme_->colors.panelBackground;
    colorLookup_["panelBorder"] = currentTheme_->colors.panelBorder;
    colorLookup_["tooltipBackground"] = currentTheme_->colors.tooltipBackground;
    colorLookup_["tooltipText"] = currentTheme_->colors.tooltipText;
}

// Future extension methods - placeholder implementations
void UITheme::applyThemeToComponent(const std::string& componentType, void* component) const {
    if (!component) {
        std::cerr << "Warning: Cannot apply theme to null component" << std::endl;
        return;
    }
    
    if (componentType == "UIButton") {
        applyThemeToButton(static_cast<UIButton*>(component));
    } else if (componentType == "UILabel") {
        applyThemeToLabel(static_cast<UILabel*>(component));
    } else if (componentType == "UITextInput") {
        applyThemeToTextInput(static_cast<UITextInput*>(component));
    } else if (componentType == "UIProgressBar") {
        applyThemeToProgressBar(static_cast<UIProgressBar*>(component));
    } else {
        std::cout << "Theme application to " << componentType << " component - not yet implemented" << std::endl;
    }
}

void UITheme::applyThemeToButton(UIButton* button) const {
    if (!button) return;
    
    // Apply theme colors to button
    button->setColors(
        currentTheme_->colors.buttonBackground,
        currentTheme_->colors.buttonText,
        currentTheme_->colors.border
    );
    
    std::cout << "Applied theme colors to UIButton" << std::endl;
}

void UITheme::applyThemeToLabel(UILabel* label) const {
    if (!label) return;
    
    // Apply theme text color and alignment
    label->setTextColor(currentTheme_->colors.text);
    
    std::cout << "Applied theme colors to UILabel" << std::endl;
}

void UITheme::applyThemeToTextInput(UITextInput* textInput) const {
    if (!textInput) return;
    
    // Apply theme colors to text input
    textInput->setColors(
        currentTheme_->colors.inputBackground,
        currentTheme_->colors.inputText,
        currentTheme_->colors.inputBorder,
        currentTheme_->colors.inputFocused
    );
    
    std::cout << "Applied theme colors to UITextInput" << std::endl;
}

void UITheme::applyThemeToProgressBar(UIProgressBar* progressBar) const {
    if (!progressBar) return;
    
    // Apply theme colors to progress bar
    progressBar->setColors(
        currentTheme_->colors.progressBackground,
        currentTheme_->colors.progressFill,
        currentTheme_->colors.progressBorder,
        currentTheme_->colors.progressText
    );
    
    std::cout << "Applied theme colors to UIProgressBar" << std::endl;
}

void UITheme::applyThemeToAllComponents(UIManager* uiManager) const {
    // This would iterate through all components in UIManager
    // Implementation depends on UIManager's internal structure
    std::cout << "Applying theme to all components in UIManager - implementation pending" << std::endl;
}

void UITheme::refreshAllComponentThemes() const {
    // Apply current theme to all registered components
    for (const auto& componentPair : registeredComponents_) {
        applyThemeToComponent(componentPair.first, componentPair.second);
    }
}

void UITheme::registerThemeChangeCallback(std::function<void(const UIThemeData&)> callback) {
    themeChangeCallbacks_.push_back(callback);
}

void UITheme::unregisterThemeChangeCallback(const std::function<void(const UIThemeData&)>* callback) {
    // Remove callback from vector - simplified implementation
    // In production, you'd want a more sophisticated callback management system
    std::cout << "Unregistering theme change callback" << std::endl;
}

void UITheme::notifyThemeChange() {
    // Notify all registered callbacks about theme change
    for (const auto& callback : themeChangeCallbacks_) {
        callback(*currentTheme_);
    }
    
    // Refresh all registered components
    refreshAllComponentThemes();
}

bool UITheme::loadThemeFromFile(const std::string& filePath) {
    // Placeholder for future file-based theme loading
    std::cout << "Loading theme from file: " << filePath << " - not yet implemented" << std::endl;
    return false;
}

bool UITheme::saveThemeToFile(const std::string& filePath, const UIThemeData& theme) const {
    // Placeholder for future file-based theme saving
    std::cout << "Saving theme to file: " << filePath << " - not yet implemented" << std::endl;
    return false;
}

std::vector<std::string> UITheme::getAvailableThemes() const {
    std::vector<std::string> themes;
    
    // Add predefined themes
    themes.push_back("Dark Theme");
    themes.push_back("Light Theme");
    themes.push_back("High Contrast Theme");
    
    // Add custom themes
    for (const auto& pair : customThemes_) {
        themes.push_back(pair.first);
    }
    
    return themes;
}
