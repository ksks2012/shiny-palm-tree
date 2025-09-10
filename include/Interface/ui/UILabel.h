#pragma once
#include "UIComponent.h"
#include <string>
#include <vector>

/**
 * Simple text display component for showing static or dynamic text
 * Supports text alignment, word wrapping, and custom styling
 * Non-focusable by default (pure display component)
 */
class UILabel : public UIComponent {
public:
    enum class TextAlignment {
        LEFT,
        CENTER,
        RIGHT
    };
    
    UILabel(int x, int y, int width, int height, SDLManager& sdlManager, 
            const std::string& text = "");
    
    // UIComponent overrides
    void layout() override;
    void render() override;
    
    // Text properties
    void setText(const std::string& text);
    const std::string& getText() const { return text_; }
    
    // Styling options
    void setTextColor(SDL_Color color) { textColor_ = color; }
    SDL_Color getTextColor() const { return textColor_; }
    
    void setBackgroundColor(SDL_Color color) { backgroundColor_ = color; hasBackground_ = true; }
    void setTransparentBackground() { hasBackground_ = false; }
    bool hasBackground() const { return hasBackground_; }
    
    void setTextAlignment(TextAlignment alignment) { textAlignment_ = alignment; }
    TextAlignment getTextAlignment() const { return textAlignment_; }
    
    void setWordWrap(bool wrap) { wordWrap_ = wrap; }
    bool getWordWrap() const { return wordWrap_; }
    
    // Auto-sizing functionality
    void setAutoResize(bool autoResize) { autoResize_ = autoResize; }
    bool getAutoResize() const { return autoResize_; }
    void autoResizeToFitText(); // Resize to fit current text content

private:
    std::string text_;
    SDL_Color textColor_;
    SDL_Color backgroundColor_;
    bool hasBackground_;
    TextAlignment textAlignment_;
    bool wordWrap_;
    bool autoResize_;
    
    // Layout calculation helpers
    void calculateTextLayout();
    void renderSingleLine();
    void renderMultiLine();
    
    // Text wrapping helpers
    std::vector<std::string> wrapText(const std::string& text, int maxWidth);
    
    // Cached layout data (calculated in layout(), used in render())
    std::vector<std::string> wrappedLines_;
    int totalTextHeight_;
    int lineHeight_;
};
