#include "Interface/ui/UILabel.h"
#include "Systems/SDLManager.h"
#include "Constants.h"
#include <algorithm>

UILabel::UILabel(int x, int y, int width, int height, SDLManager& sdlManager, 
                 const std::string& text)
    : UIComponent(x, y, width, height, sdlManager),
      text_(text),
      textColor_(Constants::TEXT_COLOR),
      backgroundColor_(Constants::BACKGROUND_COLOR),
      hasBackground_(false),
      textAlignment_(TextAlignment::LEFT),
      wordWrap_(false),
      autoResize_(false),
      totalTextHeight_(0),
      lineHeight_(0) {
}

void UILabel::layout() {
    calculateTextLayout();
    
    // Auto-resize if enabled and no word wrap
    if (autoResize_ && !wordWrap_) {
        autoResizeToFitText();
    }
}

void UILabel::render() {
    // Render background if enabled
    if (hasBackground_) {
        renderBackground(backgroundColor_);
    }
    
    // Render text
    if (!text_.empty()) {
        if (wordWrap_ && !wrappedLines_.empty()) {
            renderMultiLine();
        } else {
            renderSingleLine();
        }
    }
}

void UILabel::setText(const std::string& text) {
    text_ = text;
    // Clear cached layout data - will be recalculated in next layout() call
    wrappedLines_.clear();
    totalTextHeight_ = 0;
}

void UILabel::calculateTextLayout() {
    if (text_.empty()) {
        totalTextHeight_ = 0;
        lineHeight_ = 0;
        wrappedLines_.clear();
        return;
    }
    
    // Get basic text metrics
    int singleLineWidth, singleLineHeight;
    getTextSize(text_, singleLineWidth, singleLineHeight);
    lineHeight_ = singleLineHeight;
    
    if (wordWrap_ && singleLineWidth > width_) {
        // Text needs wrapping
        wrappedLines_ = wrapText(text_, width_);
        totalTextHeight_ = static_cast<int>(wrappedLines_.size()) * lineHeight_;
    } else {
        // Single line or no wrapping needed
        wrappedLines_.clear();
        totalTextHeight_ = lineHeight_;
    }
}

void UILabel::renderSingleLine() {
    int textW, textH;
    getTextSize(text_, textW, textH);
    
    // Calculate horizontal alignment
    int textX = 0;
    switch (textAlignment_) {
        case TextAlignment::LEFT:
            textX = 0;
            break;
        case TextAlignment::CENTER:
            textX = (width_ - textW) / 2;
            break;
        case TextAlignment::RIGHT:
            textX = width_ - textW;
            break;
    }
    
    // Center vertically
    int textY = (height_ - textH) / 2;
    
    // Ensure text stays within bounds
    textX = std::max(0, std::min(textX, width_ - textW));
    textY = std::max(0, std::min(textY, height_ - textH));
    
    renderText(text_, textX, textY, textColor_);
}

void UILabel::renderMultiLine() {
    int startY = 0;
    
    // Center block vertically if there's extra space
    if (totalTextHeight_ < height_) {
        startY = (height_ - totalTextHeight_) / 2;
    }
    
    for (size_t i = 0; i < wrappedLines_.size(); ++i) {
        const std::string& line = wrappedLines_[i];
        
        int lineY = startY + static_cast<int>(i) * lineHeight_;
        
        // Don't render lines that are outside the component bounds
        if (lineY + lineHeight_ > height_) {
            break;
        }
        
        if (line.empty()) {
            continue;
        }
        
        // Calculate horizontal alignment for this line
        int lineW, lineH;
        getTextSize(line, lineW, lineH);
        
        int lineX = 0;
        switch (textAlignment_) {
            case TextAlignment::LEFT:
                lineX = 0;
                break;
            case TextAlignment::CENTER:
                lineX = (width_ - lineW) / 2;
                break;
            case TextAlignment::RIGHT:
                lineX = width_ - lineW;
                break;
        }
        
        // Ensure line stays within bounds
        lineX = std::max(0, std::min(lineX, width_ - lineW));
        
        renderText(line, lineX, lineY, textColor_);
    }
}

void UILabel::autoResizeToFitText() {
    if (text_.empty()) {
        return;
    }
    
    int textW, textH;
    getTextSize(text_, textW, textH);
    
    // Resize to fit text with some padding
    const int padding = 4;
    setSize(textW + padding * 2, textH + padding * 2);
}

std::vector<std::string> UILabel::wrapText(const std::string& text, int maxWidth) {
    std::vector<std::string> lines;
    
    if (text.empty() || maxWidth <= 0) {
        return lines;
    }
    
    // Simple word wrapping algorithm
    std::string currentLine;
    std::string word;
    
    for (char c : text) {
        if (c == ' ' || c == '\n' || c == '\t') {
            // End of word or explicit line break
            if (!word.empty()) {
                std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
                
                int testWidth, testHeight;
                getTextSize(testLine, testWidth, testHeight);
                
                if (testWidth <= maxWidth) {
                    // Word fits on current line
                    currentLine = testLine;
                } else {
                    // Word doesn't fit, start new line
                    if (!currentLine.empty()) {
                        lines.push_back(currentLine);
                    }
                    currentLine = word;
                }
                word.clear();
            }
            
            if (c == '\n') {
                // Explicit line break
                lines.push_back(currentLine);
                currentLine.clear();
            }
        } else {
            word += c;
        }
    }
    
    // Handle last word
    if (!word.empty()) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        
        int testWidth, testHeight;
        getTextSize(testLine, testWidth, testHeight);
        
        if (testWidth <= maxWidth) {
            currentLine = testLine;
        } else {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
            }
            currentLine = word;
        }
    }
    
    // Add the last line
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    
    // Ensure we have at least one line (even if empty)
    if (lines.empty()) {
        lines.push_back("");
    }
    
    return lines;
}
