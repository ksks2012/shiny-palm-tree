# UILabel Component

UILabel is a pure text display component used to show static or dynamic text in the UI framework.

## Features

### Basic Features
- **Text Display**: Supports single-line and multi-line text display
- **Non-focusable Component**: By default, does not accept keyboard focus (display-only)
- **Dynamic Text Update**: Supports modifying text content at runtime

### Text Alignment
- `TextAlignment::LEFT` - Left aligned (default)
- `TextAlignment::CENTER` - Center aligned  
- `TextAlignment::RIGHT` - Right aligned

### Word Wrapping
- **Automatic Wrapping**: When `setWordWrap(true)` is enabled, text automatically wraps to fit the component width
- **Smart Word Splitting**: Splits text based on spaces and line breaks
- **Multi-line Support**: Supports displaying multi-line text content

### Background Style
- **Transparent Background**: Transparent by default
- **Custom Background**: Can set background color
- **Text Color**: Customizable text color

### Auto Resize
- **Auto Resize**: When `setAutoResize(true)` is enabled, the component automatically resizes to fit the text
- **Content Adaptation**: Dynamically adjusts size based on text content

## API Usage

### Basic Construction
```cpp
#include "Interface/ui/UILabel.h"

// Create label
UILabel label(x, y, width, height, sdlManager, "Display text");
```

### Text Operations
```cpp
// Set text
label.setText("New text content");

// Get text
std::string text = label.getText();
```

### Style Settings
```cpp
// Set text alignment
label.setTextAlignment(UILabel::TextAlignment::CENTER);

// Set background color
SDL_Color backgroundColor = {200, 200, 200, 255};
label.setBackgroundColor(backgroundColor);

// Set text color
SDL_Color textColor = {0, 0, 0, 255};
label.setTextColor(textColor);

// Set transparent background
label.setTransparentBackground();
```

### Word Wrapping
```cpp
// Enable word wrap
label.setWordWrap(true);

// Set longer text, will wrap automatically
label.setText("This is a longer text. When word wrap is enabled, it will automatically display in multiple lines.");
```

### Auto Resize
```cpp
// Enable auto resize
label.setAutoResize(true);

// Manually resize to fit current text
label.autoResizeToFitText();
```

## Usage Examples

### 1. Title Label
```cpp
auto titleLabel = std::make_shared<UILabel>(50, 30, 700, 40, sdlManager, 
    "Application Title");
titleLabel->setTextAlignment(UILabel::TextAlignment::CENTER);
titleLabel->setBackgroundColor({50, 50, 100, 255});
titleLabel->setTextColor({255, 255, 255, 255});
```

### 2. Multi-line Description Label
```cpp
auto descLabel = std::make_shared<UILabel>(50, 100, 300, 120, sdlManager,
    "This is a multi-line description text, demonstrating UILabel's word wrapping feature."
    "The text will automatically wrap based on the component width.");
descLabel->setWordWrap(true);
descLabel->setBackgroundColor({240, 240, 240, 255});
```

### 3. Status Display Label
```cpp
auto statusLabel = std::make_shared<UILabel>(10, 500, 400, 25, sdlManager,
    "Ready");
statusLabel->setTextColor({0, 128, 0, 255}); // Green text
statusLabel->setTransparentBackground();

// Dynamically update status
statusLabel->setText("Processing...");
```

### 4. Auto-resizing Label
```cpp
auto autoLabel = std::make_shared<UILabel>(100, 200, 50, 25, sdlManager,
    "Auto Resize");
autoLabel->setAutoResize(true);
autoLabel->setBackgroundColor({200, 255, 200, 255});
```

## Testing
UILabel includes comprehensive unit tests covering:
- Basic property setting and retrieval
- Text alignment functionality
- Background color setting
- Word wrapping feature
- Auto resize feature
- Layout calculation

To run tests:
```bash
cd build
./UIFrameworkTests
```

## Notes

1. **Font Dependency**: UILabel depends on font resources provided by SDLManager
2. **Performance Considerations**: For large amounts of text or frequent updates, batch operations are recommended
3. **Coordinate System**: Uses the framework's standard coordinate system (origin at top-left)
4. **Memory Management**: Use smart pointers to manage UILabel instances
5. **Rendering Order**: Follows the framework's Z-order rendering sequence

UILabel is a fundamental component for building user interfaces, providing a flexible and feature-rich solution for displaying various text information.

