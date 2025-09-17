# UITheme — Basic Theme System

## Overview

UITheme is a comprehensive theme management system that provides consistent visual styles and color management for the UI framework. It is designed as a singleton, supports several predefined themes, and allows creating custom themes.

## Features

### Core Features
- Singleton design: centralized global theme management
- Predefined themes: Dark, Light, High Contrast
- Custom themes: create and apply custom themes
- Color lookup system: fetch theme colors by name
- Complete theme structure: unified management of colors, typography, and dimensions

### Theme Structure
- ThemeColorPalette: full color palette
- ThemeTypography: font and typography settings
- ThemeDimensions: spacing and size definitions

## Usage

### Basic Usage

```cpp
#include "Interface/ui/UITheme.h"

// Get theme instance
UITheme& theme = UITheme::getInstance();

// Switch predefined themes
theme.setCurrentTheme(UITheme::ThemeType::DARK);
theme.setCurrentTheme(UITheme::ThemeType::LIGHT);
theme.setCurrentTheme(UITheme::ThemeType::HIGH_CONTRAST);

// Get current theme info
const UIThemeData& currentTheme = theme.getCurrentTheme();
std::cout << "Current theme: " << currentTheme.name << std::endl;
std::cout << "Description: " << currentTheme.description << std::endl;

// Query specific colors
SDL_Color buttonBg = theme.getColor("buttonBackground");
SDL_Color textColor = theme.getColor("text");
SDL_Color progressFill = theme.getColor("progressFill");
```

### Custom Theme

```cpp
// Create a custom theme
UIThemeData customTheme = theme.getCurrentTheme();  // start from current theme
customTheme.name = "My Custom Theme";
customTheme.description = "Custom theme with blue accents";

// Custom colors
customTheme.colors.buttonBackground = {30, 60, 120, 255};     // deep blue button
customTheme.colors.buttonHover = {40, 80, 160, 255};          // lighter blue on hover
customTheme.colors.progressFill = {60, 120, 200, 255};        // blue progress fill
customTheme.colors.inputFocused = {80, 140, 220, 255};        // blue focus border

// Custom font settings
customTheme.typography.baseFontSize = 16;
customTheme.typography.titleFontSize = 24;

// Apply custom theme
theme.setCustomTheme(customTheme);
```

### Get Available Theme List

```cpp
std::vector<std::string> availableThemes = theme.getAvailableThemes();
std::cout << "Available themes:" << std::endl;
for (const std::string& themeName : availableThemes) {
    std::cout << "- " << themeName << std::endl;
}
```

## Color System

### Basic Colors
- `"background"` - Main background color
- `"text"` - Main text color
- `"secondaryText"` - Secondary text color
- `"border"` - Border color
- `"overlay"` - Overlay color

### Button Colors
- `"buttonBackground"` - Button background
- `"buttonText"` - Button text
- `"buttonHover"` - Hover state
- `"buttonPressed"` - Pressed state
- `"buttonFocused"` - Focused state

### Input Control Colors
- `"inputBackground"` - Input field background
- `"inputText"` - Input field text
- `"inputBorder"` - Input field border
- `"inputFocused"` - Input field focused state

### Progress Bar Colors
- `"progressBackground"` - Progress bar background
- `"progressFill"` - Progress bar fill
- `"progressBorder"` - Progress bar border
- `"progressText"` - Progress bar text

### Status Colors
- `"success"` - Success state (green)
- `"warning"` - Warning state (orange)
- `"error"` - Error state (red)
- `"info"` - Information state (blue)

### Panel Colors
- `"panelBackground"` - Panel background
- `"panelBorder"` - Panel border
- `"tooltipBackground"` - Tooltip background
- `"tooltipText"` - Tooltip text

## Predefined Themes

### Dark Theme
- Uses existing dark definitions from UIConstants
- Suitable for long-term use, reduces eye strain
- Modern dark interface style

### Light Theme
- Bright background color scheme
- High readability text contrast
- Suitable for bright environments

### High Contrast Theme
- Pure black background with pure white text
- Vivid accent colors
- Compliant with accessibility design standards

## Typography Settings

```cpp
const ThemeTypography& typography = theme.getCurrentTheme().typography;

// Available font sizes
int baseSize = typography.baseFontSize;        // Base font size
int titleSize = typography.titleFontSize;      // Title font size
int subtitleSize = typography.subtitleFontSize; // Subtitle font size
int captionSize = typography.captionFontSize;   // Caption font size

// Other typography settings
int lineHeight = typography.lineHeight;         // Line height
int paragraphSpacing = typography.paragraphSpacing; // Paragraph spacing
```

## Dimension Settings

```cpp
const ThemeDimensions& dimensions = theme.getCurrentTheme().dimensions;

// Padding
int smallPadding = dimensions.paddingSmall;     // Small padding (4px)
int mediumPadding = dimensions.paddingMedium;   // Medium padding (8px)
int largePadding = dimensions.paddingLarge;     // Large padding (16px)

// Margins
int smallMargin = dimensions.marginSmall;       // Small margin
int mediumMargin = dimensions.marginMedium;     // Medium margin
int largeMargin = dimensions.marginLarge;       // Large margin

// Control dimensions
int buttonHeight = dimensions.buttonHeight;     // Button height
int inputHeight = dimensions.inputHeight;       // Input field height
int progressHeight = dimensions.progressBarHeight; // Progress bar height

// Borders and spacing
int borderWidth = dimensions.borderWidth;       // Border width
int gridSpacing = dimensions.gridSpacing;       // Grid spacing
int containerSpacing = dimensions.containerSpacing; // Container spacing
```

## Practical Usage Examples

### Using Theme in UI Components

```cpp
// Create button and apply theme colors
auto button = std::make_shared<UIButton>("Theme Button");
UITheme& theme = UITheme::getInstance();

// Set button size using theme definitions
const ThemeDimensions& dims = theme.getCurrentTheme().dimensions;
button->setSize(120, dims.buttonHeight);

// Manually apply theme colors (will be automated in future versions)
SDL_Color buttonBg = theme.getColor("buttonBackground");
SDL_Color buttonText = theme.getColor("buttonText");
```

### Theme Switching Interface

```cpp
// Create theme switching buttons
auto darkButton = std::make_shared<UIButton>("Dark Theme");
darkButton->setOnClick([&]() {
    theme.setCurrentTheme(UITheme::ThemeType::DARK);
    updateAllUIColors();  // Update all UI component colors
});

auto lightButton = std::make_shared<UIButton>("Light Theme");
lightButton->setOnClick([&]() {
    theme.setCurrentTheme(UITheme::ThemeType::LIGHT);
    updateAllUIColors();
});
```

## Extensibility Design

### Future Features (Reserved Interfaces)
- **File Loading Support**: `loadThemeFromFile()` and `saveThemeToFile()`
- **Component Auto-Application**: `applyThemeToComponent()` automatically applies theme to specific components
- **Theme Inheritance**: Support for inheritance relationships between themes
- **Dynamic Themes**: Support for runtime dynamic modification of theme properties

### Custom Extensions
```cpp
// Future supported file loading
theme.loadThemeFromFile("custom_theme.json");

// Future supported component auto-application
theme.applyThemeToComponent("UIButton", buttonComponent);
```

## Compilation and Testing

### Compile Examples
```bash
# Compile theme system example
cd build
make theme_example

# Run example
./theme_example
```

### Run Tests
```bash
# Compile and run theme system tests
make UIFrameworkTests
./UIFrameworkTests "[UITheme]"
```

## Performance Considerations

- **Singleton Pattern**: Ensures there's only one global theme management instance
- **Color Query Caching**: Uses map for fast color lookup, avoiding repeated calculations
- **Theme Switching Efficiency**: Theme data is preloaded, switching only updates references
- **Memory Management**: Uses shared_ptr to manage theme data, preventing memory leaks

## Integration with Other Components

UITheme is designed to integrate seamlessly with existing UI components:

- **UIButton**: Uses theme's button color system
- **UILabel**: Applies theme's text and font settings
- **UITextInput**: Uses theme's input field styling
- **UIProgressBar**: Applies theme's progress bar color scheme
- **All Components**: Uniformly use theme's dimension and spacing settings

This theme system provides unified, flexible, and extensible visual style management for the entire UI framework.
