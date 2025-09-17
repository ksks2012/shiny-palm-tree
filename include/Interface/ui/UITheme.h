#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

/**
 * UITheme - Basic theme system for UI framework
 * 
 * Provides a structured approach to managing UI colors and styles.
 * Currently uses hardcoded values from UIConstants, but designed
 * for future extension to support external theme files.
 */

// Forward declarations
class SDLManager;

/**
 * Theme color palette structure
 * Groups related colors for consistent theming
 */
struct ThemeColorPalette {
    // Basic UI colors
    SDL_Color background;
    SDL_Color text;
    SDL_Color secondaryText;
    SDL_Color border;
    SDL_Color overlay;
    
    // Interactive element colors
    SDL_Color buttonBackground;
    SDL_Color buttonText;
    SDL_Color buttonHover;
    SDL_Color buttonPressed;
    SDL_Color buttonFocused;
    
    // Input element colors
    SDL_Color inputBackground;
    SDL_Color inputText;
    SDL_Color inputBorder;
    SDL_Color inputFocused;
    
    // Progress element colors
    SDL_Color progressBackground;
    SDL_Color progressFill;
    SDL_Color progressBorder;
    SDL_Color progressText;
    
    // Status colors
    SDL_Color success;
    SDL_Color warning;
    SDL_Color error;
    SDL_Color info;
    
    // Panel colors
    SDL_Color panelBackground;
    SDL_Color panelBorder;
    SDL_Color tooltipBackground;
    SDL_Color tooltipText;
};

/**
 * Theme typography structure
 * Font settings and text styling options
 */
struct ThemeTypography {
    std::string fontPath;
    int baseFontSize;
    int titleFontSize;
    int subtitleFontSize;
    int captionFontSize;
    
    // Text spacing
    int lineHeight;
    int paragraphSpacing;
    int letterSpacing;
};

/**
 * Theme dimensions structure
 * Standard sizing and spacing values
 */
struct ThemeDimensions {
    // Spacing
    int paddingSmall;
    int paddingMedium;
    int paddingLarge;
    int marginSmall;
    int marginMedium;
    int marginLarge;
    
    // Component sizes
    int buttonHeight;
    int inputHeight;
    int progressBarHeight;
    int borderWidth;
    int borderRadius;  // For future rounded corners support
    
    // Layout
    int gridSpacing;
    int containerSpacing;
};

/**
 * Complete theme definition
 * Combines color palette, typography, and dimensions
 */
struct UIThemeData {
    std::string name;
    std::string description;
    ThemeColorPalette colors;
    ThemeTypography typography;
    ThemeDimensions dimensions;
};

/**
 * UITheme manager class
 * Handles theme application and switching
 */
class UITheme {
public:
    // Theme types
    enum class ThemeType {
        DARK,
        LIGHT,
        HIGH_CONTRAST,
        CUSTOM
    };
    
    // Singleton pattern for global theme access
    static UITheme& getInstance();
    
    // Theme management
    void setCurrentTheme(ThemeType type);
    void setCustomTheme(const UIThemeData& themeData);
    ThemeType getCurrentThemeType() const { return currentThemeType_; }
    const UIThemeData& getCurrentTheme() const { return *currentTheme_; }
    
    // Color access methods
    SDL_Color getColor(const std::string& colorName) const;
    SDL_Color getBackgroundColor() const { return currentTheme_->colors.background; }
    SDL_Color getTextColor() const { return currentTheme_->colors.text; }
    SDL_Color getBorderColor() const { return currentTheme_->colors.border; }
    SDL_Color getButtonColor() const { return currentTheme_->colors.buttonBackground; }
    SDL_Color getButtonHoverColor() const { return currentTheme_->colors.buttonHover; }
    SDL_Color getButtonPressedColor() const { return currentTheme_->colors.buttonPressed; }
    SDL_Color getProgressFillColor() const { return currentTheme_->colors.progressFill; }
    SDL_Color getProgressBackgroundColor() const { return currentTheme_->colors.progressBackground; }
    
    // Typography access
    const ThemeTypography& getTypography() const { return currentTheme_->typography; }
    int getFontSize() const { return currentTheme_->typography.baseFontSize; }
    const std::string& getFontPath() const { return currentTheme_->typography.fontPath; }
    
    // Dimensions access
    const ThemeDimensions& getDimensions() const { return currentTheme_->dimensions; }
    int getPadding() const { return currentTheme_->dimensions.paddingMedium; }
    int getMargin() const { return currentTheme_->dimensions.marginMedium; }
    int getBorderWidth() const { return currentTheme_->dimensions.borderWidth; }
    
    // Theme validation and utilities
    bool isValidTheme(const UIThemeData& theme) const;
    void applyThemeToComponent(const std::string& componentType, void* component) const;
    
    // Future extension points
    bool loadThemeFromFile(const std::string& filePath);
    bool saveThemeToFile(const std::string& filePath, const UIThemeData& theme) const;
    std::vector<std::string> getAvailableThemes() const;

private:
    UITheme();
    ~UITheme() = default;
    
    // Prevent copying
    UITheme(const UITheme&) = delete;
    UITheme& operator=(const UITheme&) = delete;
    
    // Internal theme management
    void initializeDefaultThemes();
    UIThemeData createDarkTheme() const;
    UIThemeData createLightTheme() const;
    UIThemeData createHighContrastTheme() const;
    
    // Theme storage
    ThemeType currentThemeType_;
    std::shared_ptr<UIThemeData> currentTheme_;
    std::unordered_map<ThemeType, std::shared_ptr<UIThemeData>> predefinedThemes_;
    std::unordered_map<std::string, std::shared_ptr<UIThemeData>> customThemes_;
    
    // Color lookup map for string-based access
    mutable std::unordered_map<std::string, SDL_Color> colorLookup_;
    void updateColorLookup() const;
};
