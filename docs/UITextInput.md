# UITextInput Component

UITextInput is a powerful text input component supporting full text editing features, including cursor positioning, text selection, clipboard operations, and input validation.

## Features

### Basic Features
- **Text Input**: Supports keyboard text input and editing
- **Focusable Component**: Can receive keyboard focus for input operations
- **Placeholder Text**: Displays a hint when the input box is empty
- **Max Length Limit**: Allows setting the maximum number of characters

### Cursor and Selection
- **Cursor Positioning**: Supports moving and positioning the cursor
- **Text Selection**: Supports drag selection and keyboard selection
- **Cursor Blinking**: Cursor blinks when focused
- **Mouse Click Positioning**: Clicking the text area positions the cursor

### Keyboard Shortcuts
- **Arrow Keys**: Left/Right to move cursor, Shift+Arrow to select text
- **Home/End**: Move to line start/end
- **Ctrl+A**: Select all text
- **Ctrl+C**: Copy selected text
- **Ctrl+X**: Cut selected text
- **Ctrl+V**: Paste clipboard text
- **Backspace/Delete**: Delete characters

### Input Modes
- **Normal Mode**: Shows actual input text
- **Password Mode**: Hides input with asterisks (*)
- **Read-Only Mode**: Disables editing, displays content only
- **Input Validation**: Supports custom validation functions

### Visual Styles
- **Custom Colors**: Text, background, border, and placeholder colors
- **Focus Effect**: Border and background color change on focus
- **Selection Highlight**: Highlights selected text
- **Scrolling Support**: Automatically scrolls horizontally for long text

## API Usage

### Basic Construction
```cpp
#include "Interface/ui/UITextInput.h"

// Create a text input box
UITextInput input(x, y, width, height, sdlManager, "Initial text");
```

### Text Operations
```cpp
// Set and get text
input.setText("New text content");
std::string text = input.getText();

// Clear text
input.clearText();

// Set placeholder
input.setPlaceholder("Please enter text...");
```

### Input Property Settings
```cpp
// Set max length
input.setMaxLength(50);

// Set read-only mode
input.setReadOnly(true);

// Set password mode
input.setPassword(true);
```

### Style Customization
```cpp
// Text color
SDL_Color textColor = {0, 0, 0, 255};
input.setTextColor(textColor);

// Background
SDL_Color bgColor = {255, 255, 255, 255};
input.setBackgroundColor(bgColor);

// Border color
SDL_Color borderColor = {128, 128, 128, 255};
input.setBorderColor(borderColor);

// Placeholder color
SDL_Color placeholderColor = {160, 160, 160, 255};
input.setPlaceholderColor(placeholderColor);
```

basicInput->setPlaceholder("Enter your name...");
basicInput->setOnTextChanged([](const std::string& text) {
styledInput->setTextColor({0, 100, 0, 255});           // Green text
styledInput->setBackgroundColor({250, 250, 200, 255}); // Light yellow background
styledInput->setBorderColor({100, 100, 0, 255});       // Dark yellow border
styledInput->setPlaceholder("Styled input");
### Event Callbacks
```cpp
// Text change callback
input.setOnTextChanged([](const std::string& newText) {
    std::cout << "Text changed to: " << newText << std::endl;
});

// Enter key pressed callback
input.setOnEnterPressed([]() {
    std::cout << "Enter key pressed!" << std::endl;
});

// Input validation callback (numbers only)
input.setValidationCallback([](const std::string& text) {
    for (char c : text) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
});
```

### Selection and Clipboard Operations
```cpp
// Select all text
input.selectAll();

// Clear selection
input.clearSelection();

// Check if there is a selection
if (input.hasSelection()) {
    // Clipboard operations
    input.copy();  // Copy
    input.cut();   // Cut
    input.paste(); // Paste
}
```

## Usage Examples

### 1. Basic Text Input
```cpp
auto basicInput = std::make_shared<UITextInput>(50, 100, 300, 35, sdlManager);
basicInput->setPlaceholder("Enter your name...");
basicInput->setOnTextChanged([](const std::string& text) {
    std::cout << "Name: " << text << std::endl;
});
```

### 2. Password Input
```cpp
auto passwordInput = std::make_shared<UITextInput>(50, 150, 300, 35, sdlManager);
passwordInput->setPassword(true);
passwordInput->setPlaceholder("Enter password...");
passwordInput->setMaxLength(20);
```

### 3. Numeric Input
```cpp
auto numberInput = std::make_shared<UITextInput>(50, 200, 200, 35, sdlManager);
numberInput->setPlaceholder("Numbers only...");
numberInput->setValidationCallback([](const std::string& text) {
    return std::all_of(text.begin(), text.end(), ::isdigit);
});
```

### 4. Styled Input
```cpp
auto styledInput = std::make_shared<UITextInput>(50, 250, 300, 40, sdlManager);
styledInput->setTextColor({0, 100, 0, 255});           // Green text
styledInput->setBackgroundColor({250, 250, 200, 255}); // Light yellow background
styledInput->setBorderColor({100, 100, 0, 255});       // Dark yellow border
styledInput->setPlaceholder("Styled input");
```

### 5. Multi-Input Form
```cpp
struct LoginForm {
    std::shared_ptr<UITextInput> username;
    std::shared_ptr<UITextInput> password;
    std::shared_ptr<UIButton> loginButton;
    
    LoginForm(SDLManager& sdl, UIManager& ui) {
        username = std::make_shared<UITextInput>(100, 100, 200, 35, sdl);
        username->setPlaceholder("Username");
        
        password = std::make_shared<UITextInput>(100, 150, 200, 35, sdl);
        password->setPassword(true);
        password->setPlaceholder("Password");
        
        loginButton = std::make_shared<UIButton>("Login", 100, 200, 100, 35, sdl,
            [this]() { handleLogin(); });
        
        // Tab key navigation
        username->setOnEnterPressed([this]() {
            ui.setFocus(password);
        });
        
        password->setOnEnterPressed([this]() {
            handleLogin();
        });
        
        ui.addComponent(username);
        ui.addComponent(password);
        ui.addComponent(loginButton);
    }
    
    void handleLogin() {
        std::string user = username->getText();
        std::string pass = password->getText();
        // Handle login logic...
    }
};
```

## Event Handling

UITextInput requires correct event handling to function fully:

```cpp
// Handle events in the main loop
while (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_TAB) {
            // Tab key switches focus
            uiManager.focusNext();
        } else {
            // Forward keyboard events to the focused component
            auto focused = uiManager.getFocusedComponent();
            if (focused) {
                focused->handleEvent(event);
            }
        }
    } else if (event.type == SDL_TEXTINPUT) {
        // Text input event
        auto focused = uiManager.getFocusedComponent();
        if (focused) {
            focused->handleEvent(event);
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        // Mouse click handling
        auto component = uiManager.getComponentAt(event.button.x, event.button.y);
        if (component && component->canReceiveFocus()) {
            uiManager.setFocus(component);
        }
        if (component) {
            component->handleEvent(event);
        }
    }
}
```

## Testing

UITextInput includes a complete set of unit tests covering:
- Basic property setting and getting
- Text input and editing
- Max length enforcement
- Password and read-only modes
- Color style settings
- Selection and clipboard operations
- Callback functions
- Focus handling

To run tests:
```bash
cd build
./UIFrameworkTests
```

## Notes

1. **SDL Text Input**: Call `SDL_StartTextInput()` when focused, and `SDL_StopTextInput()` when focus is lost
2. **Event Handling**: Must handle `SDL_KEYDOWN` and `SDL_TEXTINPUT` events correctly
3. **Memory Management**: Use smart pointers to manage UITextInput instances
4. **Focus Management**: Integrates with UIManager's focus system
5. **Clipboard Operations**: Relies on SDL's clipboard API
6. **Font Dependency**: Text rendering depends on the font resource provided by SDLManager

UITextInput provides all the features of a modern text input control and is a core component for building forms and user input interfaces.
