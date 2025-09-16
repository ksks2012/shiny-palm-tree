#include "Interface/ui/UIProgressBar.h"
#include "Systems/SDLManager.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

UIProgressBar::UIProgressBar(int x, int y, int width, int height, SDLManager& sdlManager)
    : UIComponent(x, y, width, height, sdlManager),
      progress_(0.0f),
      displayProgress_(0.0f),
      showText_(true),
      hasCustomText_(false),
      backgroundColor_(Constants::PROGRESS_BAR_BACKGROUND),
      fillColor_(Constants::PROGRESS_BAR_FILL),
      borderColor_(Constants::PROGRESS_BAR_BORDER),
      textColor_(Constants::PROGRESS_BAR_TEXT),
      borderWidth_(2),
      animated_(false),
      animationSpeed_(2.0f) {
}

void UIProgressBar::render() {
    // Render background
    renderBackground(backgroundColor_);
    
    // Render border
    if (borderWidth_ > 0) {
        renderBorder(borderColor_, borderWidth_);
    }
    
    // Calculate fill area (inside border)
    int fillX = x_ + borderWidth_;
    int fillY = y_ + borderWidth_;
    int fillWidth = static_cast<int>((width_ - 2 * borderWidth_) * displayProgress_);
    int fillHeight = height_ - 2 * borderWidth_;
    
    // Render progress fill
    if (fillWidth > 0) {
        SDL_Rect fillRect = {fillX, fillY, fillWidth, fillHeight};
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 
                              fillColor_.r, fillColor_.g, fillColor_.b, fillColor_.a);
        SDL_RenderFillRect(sdlManager_.getRenderer(), &fillRect);
    }
    
    // Render text if enabled
    if (showText_) {
        std::string text = getDisplayText();
        if (!text.empty()) {
            int textW, textH;
            getTextSize(text, textW, textH);
            int textX = (width_ - textW) / 2;
            int textY = (height_ - textH) / 2;
            renderText(text, textX, textY, textColor_);
        }
    }
}

void UIProgressBar::layout() {
    // Progress bar doesn't need special layout calculations
    // Size is determined by constructor parameters
}

void UIProgressBar::setProgress(float progress) {
    progress_ = std::clamp(progress, 0.0f, 1.0f);
    if (!animated_) {
        displayProgress_ = progress_;
    }
}

void UIProgressBar::setShowText(bool showText) {
    showText_ = showText;
}

void UIProgressBar::setCustomText(const std::string& text) {
    customText_ = text;
    hasCustomText_ = true;
}

void UIProgressBar::clearCustomText() {
    customText_.clear();
    hasCustomText_ = false;
}

void UIProgressBar::setColors(SDL_Color background, SDL_Color fill, SDL_Color border) {
    backgroundColor_ = background;
    fillColor_ = fill;
    borderColor_ = border;
}

void UIProgressBar::setColors(SDL_Color background, SDL_Color fill, SDL_Color border, SDL_Color text) {
    backgroundColor_ = background;
    fillColor_ = fill;
    borderColor_ = border;
    textColor_ = text;
}

void UIProgressBar::setBorderWidth(int borderWidth) {
    borderWidth_ = std::max(0, borderWidth);
}

void UIProgressBar::setAnimated(bool animated, float animationSpeed) {
    animated_ = animated;
    animationSpeed_ = animationSpeed;
    if (!animated_) {
        displayProgress_ = progress_;
    }
}

void UIProgressBar::update(float deltaTime) {
    if (animated_) {
        updateDisplayProgress(deltaTime);
    }
}

void UIProgressBar::updateDisplayProgress(float deltaTime) {
    if (displayProgress_ < progress_) {
        displayProgress_ += animationSpeed_ * deltaTime;
        displayProgress_ = std::min(displayProgress_, progress_);
    } else if (displayProgress_ > progress_) {
        displayProgress_ -= animationSpeed_ * deltaTime;
        displayProgress_ = std::max(displayProgress_, progress_);
    }
}

std::string UIProgressBar::getDisplayText() const {
    if (hasCustomText_) {
        return customText_;
    }
    
    // Default: show percentage
    int percentage = static_cast<int>(progress_ * 100);
    std::ostringstream oss;
    oss << percentage << "%";
    return oss.str();
}
