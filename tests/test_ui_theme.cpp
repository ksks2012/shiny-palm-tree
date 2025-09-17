#include "../lib/catch2/catch.hpp"
#include "Interface/ui/UITheme.h"
#include <iostream>

TEST_CASE("UITheme Singleton", "[UITheme]") {
    SECTION("Singleton instance") {
        UITheme& theme1 = UITheme::getInstance();
        UITheme& theme2 = UITheme::getInstance();
        
        REQUIRE(&theme1 == &theme2);
    }
}

TEST_CASE("UITheme Default Initialization", "[UITheme]") {
    UITheme& theme = UITheme::getInstance();
    
    SECTION("Default theme is Dark") {
        const UIThemeData& currentTheme = theme.getCurrentTheme();
        REQUIRE(currentTheme.name == "Dark Theme");
        REQUIRE(theme.getCurrentThemeType() == UITheme::ThemeType::DARK);
    }
    
    SECTION("Theme has valid data") {
        const UIThemeData& currentTheme = theme.getCurrentTheme();
        REQUIRE(!currentTheme.name.empty());
        REQUIRE(!currentTheme.description.empty());
        REQUIRE(currentTheme.typography.baseFontSize > 0);
        REQUIRE(!currentTheme.typography.fontPath.empty());
    }
}

TEST_CASE("UITheme Predefined Themes", "[UITheme]") {
    UITheme& theme = UITheme::getInstance();
    
    SECTION("Dark Theme") {
        theme.setCurrentTheme(UITheme::ThemeType::DARK);
        const UIThemeData& darkTheme = theme.getCurrentTheme();
        
        REQUIRE(darkTheme.name == "Dark Theme");
        REQUIRE(theme.getCurrentThemeType() == UITheme::ThemeType::DARK);
        
        // Check that it has dark characteristics
        REQUIRE(darkTheme.colors.background.r < 128);  // Dark background
        REQUIRE(darkTheme.colors.text.r > 128);        // Light text
    }
    
    SECTION("Light Theme") {
        theme.setCurrentTheme(UITheme::ThemeType::LIGHT);
        const UIThemeData& lightTheme = theme.getCurrentTheme();
        
        REQUIRE(lightTheme.name == "Light Theme");
        REQUIRE(theme.getCurrentThemeType() == UITheme::ThemeType::LIGHT);
        
        // Check that it has light characteristics
        REQUIRE(lightTheme.colors.background.r > 200);  // Light background
        REQUIRE(lightTheme.colors.text.r < 64);         // Dark text
    }
    
    SECTION("High Contrast Theme") {
        theme.setCurrentTheme(UITheme::ThemeType::HIGH_CONTRAST);
        const UIThemeData& contrastTheme = theme.getCurrentTheme();
        
        REQUIRE(contrastTheme.name == "High Contrast Theme");
        REQUIRE(theme.getCurrentThemeType() == UITheme::ThemeType::HIGH_CONTRAST);
        
        // Check high contrast characteristics
        REQUIRE(contrastTheme.colors.background.r == 0);    // Pure black
        REQUIRE(contrastTheme.colors.text.r == 255);        // Pure white
        REQUIRE(contrastTheme.colors.success.g == 255);     // Bright green
    }
}

TEST_CASE("UITheme Color Palette", "[UITheme]") {
    UITheme& theme = UITheme::getInstance();
    
    SECTION("All colors are defined") {
        const UIThemeData& currentTheme = theme.getCurrentTheme();
        const ThemeColorPalette& colors = currentTheme.colors;
        
        // Basic colors
        REQUIRE(colors.background.a > 0);
        REQUIRE(colors.text.a > 0);
        REQUIRE(colors.border.a > 0);
        
        // Interactive elements
        REQUIRE(colors.buttonBackground.a > 0);
        REQUIRE(colors.buttonHover.a > 0);
        REQUIRE(colors.buttonPressed.a > 0);
        
        // Status colors
        REQUIRE(colors.success.a > 0);
        REQUIRE(colors.warning.a > 0);
        REQUIRE(colors.error.a > 0);
        REQUIRE(colors.info.a > 0);
    }
    
    SECTION("Color lookup functionality") {
        SDL_Color buttonBg = theme.getColor("buttonBackground");
        SDL_Color textColor = theme.getColor("text");
        SDL_Color borderColor = theme.getColor("border");
        
        REQUIRE(buttonBg.a > 0);
        REQUIRE(textColor.a > 0);
        REQUIRE(borderColor.a > 0);
        
        // Test invalid color (should return default)
        SDL_Color invalidColor = theme.getColor("nonExistentColor");
        REQUIRE(invalidColor.a > 0);  // Should still return a valid color
    }
}

TEST_CASE("UITheme Typography", "[UITheme]") {
    UITheme& theme = UITheme::getInstance();
    
    SECTION("Typography settings are valid") {
        const UIThemeData& currentTheme = theme.getCurrentTheme();
        const ThemeTypography& typography = currentTheme.typography;
        
        REQUIRE(!typography.fontPath.empty());
        REQUIRE(typography.baseFontSize > 0);
        REQUIRE(typography.titleFontSize > typography.baseFontSize);
        REQUIRE(typography.subtitleFontSize > typography.baseFontSize);
        REQUIRE(typography.captionFontSize <= typography.baseFontSize);
        REQUIRE(typography.lineHeight > 0);
    }
}

TEST_CASE("UITheme Dimensions", "[UITheme]") {
    UITheme& theme = UITheme::getInstance();
    
    SECTION("Dimension settings are valid") {
        const UIThemeData& currentTheme = theme.getCurrentTheme();
        const ThemeDimensions& dimensions = currentTheme.dimensions;
        
        REQUIRE(dimensions.paddingSmall > 0);
        REQUIRE(dimensions.paddingMedium > dimensions.paddingSmall);
        REQUIRE(dimensions.paddingLarge > dimensions.paddingMedium);
        
        REQUIRE(dimensions.marginSmall >= 0);
        REQUIRE(dimensions.marginMedium >= dimensions.marginSmall);
        REQUIRE(dimensions.marginLarge >= dimensions.marginMedium);
        
        REQUIRE(dimensions.buttonHeight > 0);
        REQUIRE(dimensions.inputHeight > 0);
        REQUIRE(dimensions.progressBarHeight > 0);
        REQUIRE(dimensions.borderWidth >= 0);
    }
}

TEST_CASE("UITheme Custom Themes", "[UITheme]") {
    UITheme& theme = UITheme::getInstance();
    
    SECTION("Custom theme creation and application") {
        UIThemeData customTheme = theme.getCurrentTheme();  // Start with current
        customTheme.name = "Test Custom Theme";
        customTheme.description = "A theme for testing";
        customTheme.colors.buttonBackground = {100, 150, 200, 255};
        
        theme.setCustomTheme(customTheme);
        
        REQUIRE(theme.getCurrentThemeType() == UITheme::ThemeType::CUSTOM);
        REQUIRE(theme.getCurrentTheme().name == "Test Custom Theme");
        
        SDL_Color customButtonBg = theme.getColor("buttonBackground");
        REQUIRE(customButtonBg.r == 100);
        REQUIRE(customButtonBg.g == 150);
        REQUIRE(customButtonBg.b == 200);
    }
    
    SECTION("Invalid custom theme handling") {
        UIThemeData invalidTheme;
        // Leave name empty to make it invalid
        invalidTheme.description = "Invalid theme";
        
        UITheme::ThemeType originalType = theme.getCurrentThemeType();
        std::string originalName = theme.getCurrentTheme().name;
        
        theme.setCustomTheme(invalidTheme);
        
        // Should keep original theme
        REQUIRE(theme.getCurrentThemeType() == originalType);
        REQUIRE(theme.getCurrentTheme().name == originalName);
    }
}

TEST_CASE("UITheme Available Themes", "[UITheme]") {
    UITheme& theme = UITheme::getInstance();
    
    SECTION("List of available themes") {
        std::vector<std::string> themes = theme.getAvailableThemes();
        
        REQUIRE(themes.size() >= 3);  // At least Dark, Light, High Contrast
        
        bool hasDark = false, hasLight = false, hasContrast = false;
        for (const std::string& themeName : themes) {
            if (themeName == "Dark Theme") hasDark = true;
            if (themeName == "Light Theme") hasLight = true;
            if (themeName == "High Contrast Theme") hasContrast = true;
        }
        
        REQUIRE(hasDark);
        REQUIRE(hasLight);
        REQUIRE(hasContrast);
    }
    
    SECTION("Custom themes appear in list") {
        UIThemeData customTheme = theme.getCurrentTheme();
        customTheme.name = "Test List Theme";
        theme.setCustomTheme(customTheme);
        
        std::vector<std::string> themes = theme.getAvailableThemes();
        
        bool hasCustom = false;
        for (const std::string& themeName : themes) {
            if (themeName == "Test List Theme") {
                hasCustom = true;
                break;
            }
        }
        
        REQUIRE(hasCustom);
    }
}

TEST_CASE("UITheme Color Consistency", "[UITheme]") {
    UITheme& theme = UITheme::getInstance();
    
    SECTION("Theme switching maintains color consistency") {
        // Test all theme types
        std::vector<UITheme::ThemeType> themeTypes = {
            UITheme::ThemeType::DARK,
            UITheme::ThemeType::LIGHT,
            UITheme::ThemeType::HIGH_CONTRAST
        };
        
        for (UITheme::ThemeType themeType : themeTypes) {
            theme.setCurrentTheme(themeType);
            
            // All basic colors should be defined
            REQUIRE(theme.getColor("background").a > 0);
            REQUIRE(theme.getColor("text").a > 0);
            REQUIRE(theme.getColor("buttonBackground").a > 0);
            REQUIRE(theme.getColor("progressFill").a > 0);
            
            // Status colors should be defined
            REQUIRE(theme.getColor("success").a > 0);
            REQUIRE(theme.getColor("warning").a > 0);
            REQUIRE(theme.getColor("error").a > 0);
            REQUIRE(theme.getColor("info").a > 0);
        }
    }
}

TEST_CASE("UITheme Future Extensions", "[UITheme]") {
    UITheme& theme = UITheme::getInstance();
    
    SECTION("File loading placeholder") {
        // These should not crash and return false (not implemented)
        REQUIRE_FALSE(theme.loadThemeFromFile("nonexistent.json"));
        REQUIRE_FALSE(theme.saveThemeToFile("test.json", theme.getCurrentTheme()));
    }
    
    SECTION("Component application placeholder") {
        // Should not crash
        REQUIRE_NOTHROW(theme.applyThemeToComponent("UIButton", nullptr));
    }
}
