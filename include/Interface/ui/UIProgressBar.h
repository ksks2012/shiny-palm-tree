#pragma once
#include "UIComponent.h"

/**
 * Progress bar UI component with customizable appearance and progress tracking
 */
class UIProgressBar : public UIComponent {
public:
    UIProgressBar(int x, int y, int width, int height, SDLManager& sdlManager);
    
    void render() override;
    void layout() override;
    
    // Progress management
    void setProgress(float progress);  // Progress from 0.0 to 1.0
    float getProgress() const { return progress_; }
    
    // Text display options
    void setShowText(bool showText);
    void setCustomText(const std::string& text);
    void clearCustomText();
    
    // Appearance customization
    void setColors(SDL_Color background, SDL_Color fill, SDL_Color border);
    void setColors(SDL_Color background, SDL_Color fill, SDL_Color border, SDL_Color text);
    void setBorderWidth(int borderWidth);
    
    // Animation
    void setAnimated(bool animated, float animationSpeed = 2.0f);
    void update(float deltaTime);

private:
    float progress_;           // Current progress (0.0 to 1.0)
    float displayProgress_;    // Visual progress for animation
    bool showText_;
    std::string customText_;
    bool hasCustomText_;
    
    // Appearance
    SDL_Color backgroundColor_;
    SDL_Color fillColor_;
    SDL_Color borderColor_;
    SDL_Color textColor_;
    int borderWidth_;
    
    // Animation
    bool animated_;
    float animationSpeed_;
    
    void updateDisplayProgress(float deltaTime);
    std::string getDisplayText() const;
};
