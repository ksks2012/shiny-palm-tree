# UIProgressBar Documentation

## Overview
UIProgressBar is a customizable progress bar component that can display progress from 0% to 100% with various visual styles and animation options.

## Features
- **Progress Tracking**: Set and display progress values from 0.0 to 1.0
- **Text Display**: Show percentage or custom text
- **Animation Support**: Smooth animated transitions between progress values
- **Color Customization**: Customize background, fill, border, and text colors
- **Border Control**: Adjustable border width or borderless design
- **Flexible Sizing**: Support for various dimensions and aspect ratios

## Constructor
```cpp
UIProgressBar(int x, int y, int width, int height, SDLManager& sdlManager);
```

### Parameters
- `x, y`: Position coordinates
- `width, height`: Dimensions of the progress bar
- `sdlManager`: Reference to the SDL manager for rendering

## Basic Usage

### Creating a Progress Bar
```cpp
// Create a basic progress bar
auto progressBar = std::make_shared<UIProgressBar>(100, 100, 300, 30, sdlManager);

// Set progress (0.0 = 0%, 1.0 = 100%)
progressBar->setProgress(0.65f);

// Add to UI manager
uiManager.addComponent(progressBar, true);
```

### Setting Progress
```cpp
// Set progress value (automatically clamped to 0.0-1.0)
progressBar->setProgress(0.75f);  // 75%

// Get current progress
float currentProgress = progressBar->getProgress();
```

## Text Display

### Default Percentage Display
```cpp
// Shows "65%" by default
progressBar->setProgress(0.65f);
```

### Custom Text
```cpp
// Set custom text instead of percentage
progressBar->setCustomText("Loading Assets...");

// Clear custom text to return to percentage display
progressBar->clearCustomText();

// Hide text completely
progressBar->setShowText(false);
```

## Animation

### Enable Animation
```cpp
// Enable smooth animation with default speed
progressBar->setAnimated(true);

// Enable animation with custom speed (units per second)
progressBar->setAnimated(true, 2.5f);

// Disable animation for instant updates
progressBar->setAnimated(false);
```

### Update Animation
```cpp
// Call in your main loop to update animation
progressBar->update(deltaTime);
```

## Customization

### Colors
```cpp
// Set background, fill, and border colors
SDL_Color bg = {40, 40, 40, 255};     // Dark gray background
SDL_Color fill = {60, 150, 60, 255};  // Green fill
SDL_Color border = {100, 100, 100, 255}; // Gray border
progressBar->setColors(bg, fill, border);

// Set all colors including text
SDL_Color text = {255, 255, 255, 255}; // White text
progressBar->setColors(bg, fill, border, text);
```

### Border
```cpp
// Set border width
progressBar->setBorderWidth(3);

// Remove border completely
progressBar->setBorderWidth(0);
```

## Common Patterns

### Loading Progress Bar
```cpp
auto loadingBar = std::make_shared<UIProgressBar>(200, 300, 400, 25, sdlManager);
loadingBar->setCustomText("Loading...");
loadingBar->setAnimated(true, 1.0f);
loadingBar->setColors({20, 20, 20, 255}, {0, 150, 255, 255}, {100, 100, 100, 255});
```

### Health Bar
```cpp
auto healthBar = std::make_shared<UIProgressBar>(50, 50, 200, 20, sdlManager);
healthBar->setColors({60, 20, 20, 255}, {200, 50, 50, 255}, {150, 50, 50, 255});
healthBar->setBorderWidth(2);
healthBar->setProgress(0.75f); // 75% health
```

### Experience Bar
```cpp
auto expBar = std::make_shared<UIProgressBar>(100, 500, 300, 15, sdlManager);
expBar->setBorderWidth(0);
expBar->setShowText(false);
expBar->setColors({30, 30, 30, 255}, {255, 215, 0, 255}, {0, 0, 0, 0}); // Gold fill
expBar->setAnimated(true, 0.5f);
```

### Download Progress
```cpp
auto downloadBar = std::make_shared<UIProgressBar>(150, 200, 350, 30, sdlManager);
downloadBar->setAnimated(true, 2.0f);
downloadBar->setCustomText("Downloading... 0%");

// Update progress and text
void updateDownload(float progress, const std::string& filename) {
    downloadBar->setProgress(progress);
    int percentage = static_cast<int>(progress * 100);
    downloadBar->setCustomText("Downloading " + filename + "... " + std::to_string(percentage) + "%");
}
```

## Integration with Scene Management

```cpp
// In a scene's onCreate method
void LoadingScene::onCreate() {
    auto progressBar = std::make_shared<UIProgressBar>(300, 350, 400, 35, sdlManager_);
    progressBar->setAnimated(true, 1.5f);
    progressBar->setCustomText("Initializing...");
    
    getUIManager().addComponent(progressBar, true);
    progressBar_ = progressBar; // Store for updates
}

// In scene's update method
void LoadingScene::update(float deltaTime) {
    if (progressBar_) {
        progressBar_->update(deltaTime);
        
        // Update progress based on loading state
        float loadingProgress = getLoadingProgress();
        progressBar_->setProgress(loadingProgress);
        
        if (loadingProgress >= 1.0f) {
            sceneManager_->switchToScene("main");
        }
    }
}
```

## Best Practices

1. **Always call update()** for animated progress bars in your main loop
2. **Use appropriate dimensions** - thin bars (10-20px) for subtle indicators, thicker bars (25-40px) for prominent displays
3. **Choose contrasting colors** for good visibility
4. **Consider animation speed** - slower for smooth, faster for responsive updates
5. **Use custom text for context** - "Loading...", "Installing...", "Processing..." instead of just percentages
6. **Test with different progress values** to ensure good visual appearance at all stages

## Color Constants
The framework provides default color constants in UIConstants.h:
- `PROGRESS_BAR_BACKGROUND`: Default dark gray background
- `PROGRESS_BAR_FILL`: Default green fill color
- `PROGRESS_BAR_BORDER`: Default gray border
- `PROGRESS_BAR_TEXT`: Default white text color
