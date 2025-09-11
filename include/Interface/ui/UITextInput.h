#pragma once
#include "UIComponent.h"
#include <string>
#include <functional>

/**
 * Text input component for user text entry
 * Supports cursor positioning, text selection, placeholder text, and input validation
 * Focusable component with keyboard input handling
 */
class UITextInput : public UIComponent {
public:
    // Callback function type for text change events
    using TextChangeCallback = std::function<void(const std::string&)>;
    using ValidationCallback = std::function<bool(const std::string&)>;
    
    UITextInput(int x, int y, int width, int height, SDLManager& sdlManager,
                const std::string& initialText = "");
    
    // UIComponent overrides
    void layout() override;
    void render() override;
    void handleEvent(const SDL_Event& event) override;
    
    // Focus handling
    void onFocusGained() override;
    void onFocusLost() override;
    bool canReceiveFocus() const override { return true; }
    
    // Text properties
    void setText(const std::string& text);
    const std::string& getText() const { return text_; }
    void clearText();
    
    // Placeholder text
    void setPlaceholder(const std::string& placeholder) { placeholderText_ = placeholder; }
    const std::string& getPlaceholder() const { return placeholderText_; }
    
    // Input properties
    void setMaxLength(size_t maxLength) { maxLength_ = maxLength; }
    size_t getMaxLength() const { return maxLength_; }
    
    void setReadOnly(bool readOnly) { readOnly_ = readOnly; }
    bool isReadOnly() const { return readOnly_; }
    
    void setPassword(bool isPassword) { isPassword_ = isPassword; }
    bool isPassword() const { return isPassword_; }
    
    // Styling
    void setTextColor(SDL_Color color) { textColor_ = color; }
    SDL_Color getTextColor() const { return textColor_; }
    
    void setBackgroundColor(SDL_Color color) { backgroundColor_ = color; }
    SDL_Color getBackgroundColor() const { return backgroundColor_; }
    
    void setBorderColor(SDL_Color color) { borderColor_ = color; }
    SDL_Color getBorderColor() const { return borderColor_; }
    
    void setPlaceholderColor(SDL_Color color) { placeholderColor_ = color; }
    SDL_Color getPlaceholderColor() const { return placeholderColor_; }
    
    // Callbacks
    void setOnTextChanged(TextChangeCallback callback) { onTextChanged_ = callback; }
    void setOnEnterPressed(std::function<void()> callback) { onEnterPressed_ = callback; }
    void setValidationCallback(ValidationCallback callback) { validationCallback_ = callback; }
    
    // Selection and cursor
    void selectAll();
    void clearSelection();
    bool hasSelection() const { return selectionStart_ != selectionEnd_; }
    
    // Clipboard operations
    void cut();
    void copy();
    void paste();

private:
    std::string text_;
    std::string placeholderText_;
    
    // Input properties
    size_t maxLength_;
    bool readOnly_;
    bool isPassword_;
    
    // Visual properties
    SDL_Color textColor_;
    SDL_Color backgroundColor_;
    SDL_Color borderColor_;
    SDL_Color placeholderColor_;
    SDL_Color selectionColor_;
    SDL_Color cursorColor_;
    
    // Cursor and selection
    size_t cursorPosition_;
    size_t selectionStart_;
    size_t selectionEnd_;
    
    // Scrolling for long text
    int textOffset_; // Horizontal scroll offset in pixels
    
    // Cursor blinking
    Uint32 lastCursorBlink_;
    bool showCursor_;
    
    // Callbacks
    TextChangeCallback onTextChanged_;
    std::function<void()> onEnterPressed_;
    ValidationCallback validationCallback_;
    
    // Internal helper methods
    void insertTextAtCursor(const std::string& insertText);
    void deleteSelection();
    void deleteCharAtCursor(bool forward = true);
    void moveCursor(int direction, bool selecting = false);
    void setCursorFromMousePosition(int mouseX);
    void updateTextOffset();
    void validateAndUpdateText(const std::string& newText);
    
    // Rendering helpers
    void renderTextContent();
    void renderCursor();
    void renderSelection();
    void renderPlaceholder();
    
    // Text measurement helpers
    int getTextWidth(const std::string& text, size_t start = 0, size_t length = std::string::npos);
    size_t getCharacterIndexFromX(int x);
    int getCursorXPosition();
    
    // Display text (for password fields)
    std::string getDisplayText() const;
    
    // Constants
    static constexpr int CURSOR_WIDTH = 1;
    static constexpr int TEXT_PADDING = 4;
    static constexpr Uint32 CURSOR_BLINK_RATE = 530; // milliseconds
};
