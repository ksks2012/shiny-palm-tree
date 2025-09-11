#include "Interface/ui/UITextInput.h"
#include "Systems/SDLManager.h"
#include "Constants.h"
#include <algorithm>
#include <SDL2/SDL.h>

UITextInput::UITextInput(int x, int y, int width, int height, SDLManager& sdlManager,
                         const std::string& initialText)
    : UIComponent(x, y, width, height, sdlManager),
      text_(initialText),
      placeholderText_(""),
      maxLength_(256),
      readOnly_(false),
      isPassword_(false),
      textColor_(Constants::TEXT_COLOR),
      backgroundColor_({255, 255, 255, 255}),
      borderColor_(Constants::BORDER_COLOR),
      placeholderColor_({128, 128, 128, 255}),
      selectionColor_({100, 149, 237, 128}),
      cursorColor_(Constants::TEXT_COLOR),
      cursorPosition_(initialText.length()),
      selectionStart_(0),
      selectionEnd_(0),
      textOffset_(0),
      lastCursorBlink_(SDL_GetTicks()),
      showCursor_(true) {
}

void UITextInput::layout() {
    // Update text offset if cursor is out of view
    updateTextOffset();
}

void UITextInput::render() {
    // Render background
    SDL_Color bgColor = backgroundColor_;
    if (hasFocus()) {
        // Slightly brighten background when focused
        bgColor.r = std::min(255, bgColor.r + 10);
        bgColor.g = std::min(255, bgColor.g + 10);
        bgColor.b = std::min(255, bgColor.b + 10);
    }
    UIComponent::renderBackground(bgColor);
    
    // Render border
    SDL_Color borderCol = borderColor_;
    if (hasFocus()) {
        // Use a different color when focused
        borderCol = {100, 149, 237, 255}; // Light blue
    }
    UIComponent::renderBorder(borderCol, 2);
    
    if (hasSelection()) {
        renderSelection();
    }
    
    if (text_.empty() && !hasFocus()) {
        renderPlaceholder();
    } else {
        renderTextContent();
    }
    
    if (hasFocus() && !readOnly_ && showCursor_) {
        renderCursor();
    }
    
    // Handle cursor blinking
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastCursorBlink_ > CURSOR_BLINK_RATE) {
        showCursor_ = !showCursor_;
        lastCursorBlink_ = currentTime;
    }
}

void UITextInput::handleEvent(const SDL_Event& event) {
    if (!hasFocus() || readOnly_) {
        return;
    }
    
    switch (event.type) {
        case SDL_KEYDOWN: {
            bool shift = (event.key.keysym.mod & KMOD_SHIFT) != 0;
            bool ctrl = (event.key.keysym.mod & KMOD_CTRL) != 0;
            
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    moveCursor(-1, shift);
                    break;
                    
                case SDLK_RIGHT:
                    moveCursor(1, shift);
                    break;
                    
                case SDLK_HOME:
                    cursorPosition_ = 0;
                    if (!shift) clearSelection();
                    updateTextOffset();
                    break;
                    
                case SDLK_END:
                    cursorPosition_ = text_.length();
                    if (!shift) clearSelection();
                    updateTextOffset();
                    break;
                    
                case SDLK_BACKSPACE:
                    if (hasSelection()) {
                        deleteSelection();
                    } else {
                        deleteCharAtCursor(false);
                    }
                    break;
                    
                case SDLK_DELETE:
                    if (hasSelection()) {
                        deleteSelection();
                    } else {
                        deleteCharAtCursor(true);
                    }
                    break;
                    
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    if (onEnterPressed_) {
                        onEnterPressed_();
                    }
                    break;
                    
                case SDLK_a:
                    if (ctrl) {
                        selectAll();
                    }
                    break;
                    
                case SDLK_c:
                    if (ctrl && hasSelection()) {
                        copy();
                    }
                    break;
                    
                case SDLK_x:
                    if (ctrl && hasSelection()) {
                        cut();
                    }
                    break;
                    
                case SDLK_v:
                    if (ctrl) {
                        paste();
                    }
                    break;
            }
            
            // Reset cursor blink on any key press
            showCursor_ = true;
            lastCursorBlink_ = SDL_GetTicks();
            break;
        }
            
        case SDL_TEXTINPUT:
            insertTextAtCursor(event.text.text);
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT && isPointInside(event.button.x, event.button.y)) {
                setCursorFromMousePosition(event.button.x);
                clearSelection();
                // Reset cursor blink
                showCursor_ = true;
                lastCursorBlink_ = SDL_GetTicks();
            }
            break;
    }
}

void UITextInput::onFocusGained() {
    // Start text input
    SDL_StartTextInput();
    showCursor_ = true;
    lastCursorBlink_ = SDL_GetTicks();
}

void UITextInput::onFocusLost() {
    // Stop text input
    SDL_StopTextInput();
    clearSelection();
}

void UITextInput::setText(const std::string& text) {
    std::string limitedText = text;
    if (limitedText.length() > maxLength_) {
        limitedText = limitedText.substr(0, maxLength_);
    }
    validateAndUpdateText(limitedText);
    cursorPosition_ = std::min(cursorPosition_, text_.length());
    clearSelection();
    updateTextOffset();
}

void UITextInput::clearText() {
    setText("");
}

void UITextInput::selectAll() {
    selectionStart_ = 0;
    selectionEnd_ = text_.length();
    cursorPosition_ = text_.length();
    updateTextOffset();
}

void UITextInput::clearSelection() {
    selectionStart_ = selectionEnd_ = cursorPosition_;
}

void UITextInput::cut() {
    if (hasSelection()) {
        copy();
        deleteSelection();
    }
}

void UITextInput::copy() {
    if (hasSelection() && !isPassword_) {
        size_t start = std::min(selectionStart_, selectionEnd_);
        size_t end = std::max(selectionStart_, selectionEnd_);
        std::string selectedText = text_.substr(start, end - start);
        SDL_SetClipboardText(selectedText.c_str());
    }
}

void UITextInput::paste() {
    if (SDL_HasClipboardText()) {
        char* clipboardText = SDL_GetClipboardText();
        if (clipboardText) {
            insertTextAtCursor(clipboardText);
            SDL_free(clipboardText);
        }
    }
}

#include <iostream>

void UITextInput::insertTextAtCursor(const std::string& insertText) {
    if (insertText.empty()) return;
    
    // Delete selected text first
    if (hasSelection()) {
        deleteSelection();
    }
    
    std::string newText = text_;
    // Insert the new text at the safe cursor position
    size_t safeCursorPos = std::min(cursorPosition_, newText.size());
    newText.insert(safeCursorPos, insertText);
    
    // Check max length
    if (newText.length() > maxLength_) {
        size_t maxInsertLength = maxLength_ - text_.length();
        if (maxInsertLength > 0) {
            newText = text_;
            newText.insert(cursorPosition_, insertText.substr(0, maxInsertLength));
        } else {
            return; // Cannot insert any text
        }
    }
    
    validateAndUpdateText(newText);
    cursorPosition_ += std::min(insertText.length(), maxLength_ - text_.length());
    clearSelection();
    updateTextOffset();
}

void UITextInput::deleteSelection() {
    if (!hasSelection()) return;
    
    size_t start = std::min(selectionStart_, selectionEnd_);
    size_t end = std::max(selectionStart_, selectionEnd_);
    
    std::string newText = text_;
    newText.erase(start, end - start);
    
    validateAndUpdateText(newText);
    cursorPosition_ = start;
    clearSelection();
    updateTextOffset();
}

void UITextInput::deleteCharAtCursor(bool forward) {
    if (text_.empty()) return;
    
    if (forward) {
        if (cursorPosition_ < text_.length()) {
            std::string newText = text_;
            newText.erase(cursorPosition_, 1);
            validateAndUpdateText(newText);
        }
    } else {
        if (cursorPosition_ > 0) {
            std::string newText = text_;
            newText.erase(cursorPosition_ - 1, 1);
            validateAndUpdateText(newText);
            cursorPosition_--;
        }
    }
    
    updateTextOffset();
}

void UITextInput::moveCursor(int direction, bool selecting) {
    size_t newPosition = cursorPosition_;
    
    if (direction < 0 && newPosition > 0) {
        newPosition--;
    } else if (direction > 0 && newPosition < text_.length()) {
        newPosition++;
    }
    
    if (!selecting) {
        clearSelection();
    } else if (!hasSelection()) {
        selectionStart_ = cursorPosition_;
    }
    
    cursorPosition_ = newPosition;
    
    if (selecting) {
        selectionEnd_ = cursorPosition_;
    }
    
    updateTextOffset();
}

void UITextInput::setCursorFromMousePosition(int mouseX) {
    int relativeX = mouseX - x_ - TEXT_PADDING + textOffset_;
    cursorPosition_ = getCharacterIndexFromX(relativeX);
    updateTextOffset();
}

void UITextInput::updateTextOffset() {
    int cursorX = getCursorXPosition();
    int availableWidth = width_ - 2 * TEXT_PADDING;
    
    // Scroll right if cursor is beyond right edge
    if (cursorX - textOffset_ > availableWidth) {
        textOffset_ = cursorX - availableWidth;
    }
    
    // Scroll left if cursor is before left edge
    if (cursorX - textOffset_ < 0) {
        textOffset_ = cursorX;
    }
    
    // Don't scroll past the beginning
    if (textOffset_ < 0) {
        textOffset_ = 0;
    }
}

void UITextInput::validateAndUpdateText(const std::string& newText) {
    if (validationCallback_ && !validationCallback_(newText)) {
        return; // Validation failed
    }
    
    text_ = newText;
    
    if (onTextChanged_) {
        onTextChanged_(text_);
    }
}

void UITextInput::renderTextContent() {
    if (text_.empty()) return;
    
    std::string displayText = getDisplayText();
    
    // Calculate visible portion
    int textX = x_ + TEXT_PADDING - textOffset_;
    int textY = y_ + (height_ - 16) / 2; // Assuming font height ~16
    
    // Set clipping rectangle to prevent text from drawing outside component
    SDL_Rect clipRect = {x_ + TEXT_PADDING, y_, width_ - 2 * TEXT_PADDING, height_};
    SDL_RenderSetClipRect(sdlManager_.getRenderer(), &clipRect);
    
    UIComponent::renderText(displayText, textX - x_, textY - y_, textColor_);
    
    // Reset clipping
    SDL_RenderSetClipRect(sdlManager_.getRenderer(), nullptr);
}

void UITextInput::renderCursor() {
    int cursorX = x_ + TEXT_PADDING + getCursorXPosition() - textOffset_;
    int cursorY = y_ + TEXT_PADDING;
    int cursorHeight = height_ - 2 * TEXT_PADDING;
    
    // Only draw cursor if it's within the visible area
    if (cursorX >= x_ + TEXT_PADDING && cursorX < x_ + width_ - TEXT_PADDING) {
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 
                              cursorColor_.r, cursorColor_.g, cursorColor_.b, cursorColor_.a);
        SDL_Rect cursorRect = {cursorX, cursorY, CURSOR_WIDTH, cursorHeight};
        SDL_RenderFillRect(sdlManager_.getRenderer(), &cursorRect);
    }
}

void UITextInput::renderSelection() {
    if (!hasSelection()) return;
    
    size_t start = std::min(selectionStart_, selectionEnd_);
    size_t end = std::max(selectionStart_, selectionEnd_);
    
    std::string displayText = getDisplayText();
    
    int startX = getTextWidth(displayText, 0, start);
    int endX = getTextWidth(displayText, 0, end);
    
    int selectionX = x_ + TEXT_PADDING + startX - textOffset_;
    int selectionWidth = endX - startX;
    int selectionY = y_ + TEXT_PADDING;
    int selectionHeight = height_ - 2 * TEXT_PADDING;
    
    // Clip selection to visible area
    if (selectionX < x_ + TEXT_PADDING) {
        selectionWidth -= (x_ + TEXT_PADDING - selectionX);
        selectionX = x_ + TEXT_PADDING;
    }
    
    if (selectionX + selectionWidth > x_ + width_ - TEXT_PADDING) {
        selectionWidth = (x_ + width_ - TEXT_PADDING) - selectionX;
    }
    
    if (selectionWidth > 0) {
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(),
                              selectionColor_.r, selectionColor_.g, selectionColor_.b, selectionColor_.a);
        SDL_Rect selectionRect = {selectionX, selectionY, selectionWidth, selectionHeight};
        SDL_RenderFillRect(sdlManager_.getRenderer(), &selectionRect);
    }
}

void UITextInput::renderPlaceholder() {
    if (placeholderText_.empty()) return;
    
    int textX = TEXT_PADDING;
    int textY = (height_ - 16) / 2; // Assuming font height ~16
    
    UIComponent::renderText(placeholderText_, textX, textY, placeholderColor_);
}

int UITextInput::getTextWidth(const std::string& text, size_t start, size_t length) {
    if (text.empty() || start >= text.length()) return 0;
    
    size_t endPos = (length == std::string::npos) ? text.length() : std::min(start + length, text.length());
    std::string substring = text.substr(start, endPos - start);
    
    int width, height;
    getTextSize(substring, width, height);
    return width;
}

size_t UITextInput::getCharacterIndexFromX(int x) {
    std::string displayText = getDisplayText();
    
    for (size_t i = 0; i <= displayText.length(); ++i) {
        int charX = getTextWidth(displayText, 0, i);
        if (x <= charX) {
            return i;
        }
    }
    
    return displayText.length();
}

int UITextInput::getCursorXPosition() {
    std::string displayText = getDisplayText();
    return getTextWidth(displayText, 0, cursorPosition_);
}

std::string UITextInput::getDisplayText() const {
    if (isPassword_ && !text_.empty()) {
        return std::string(text_.length(), '*');
    }
    return text_;
}
