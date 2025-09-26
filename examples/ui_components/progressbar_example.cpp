#include "Systems/SDLManager.h"
#include "Interface/ui/UIProgressBar.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/UIManager.h"
#include <iostream>
#include <vector>
#include <memory>

class ProgressBarDemo {
public:
    ProgressBarDemo(SDLManager& sdl) : sdlManager_(sdl), uiManager_() {
        setupUI();
    }
    
    void handleEvent(const SDL_Event& event) {
        uiManager_.handleEvent(event);
    }
    
    void update(float deltaTime) {
        // Update animated progress bars
        for (auto& progressBar : progressBars_) {
            progressBar->update(deltaTime);
        }
        
        // Update automatic progress simulation
        updateProgressSimulation(deltaTime);
    }
    
    void render() {
        // Clear screen
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 30, 30, 60, 255);
        SDL_RenderClear(sdlManager_.getRenderer());
        
        // Render UI
        uiManager_.renderAll();
    }
    
    bool shouldExit() const { return exitRequested_; }
    
private:
    SDLManager& sdlManager_;
    UIManager uiManager_;
    std::vector<std::shared_ptr<UIProgressBar>> progressBars_;
    bool exitRequested_ = false;
    float simulationTimer_ = 0.0f;
    int currentProgressBar_ = 0;
    
    void setupUI() {
        // Title
        auto title = std::make_shared<UILabel>(0, 20, 800, 50, sdlManager_, "UI Progress Bar Demo");
        title->setBackgroundColor({50, 50, 100, 255});
        title->setTextColor({255, 255, 255, 255});
        uiManager_.addComponent(title, true);
        
        // Basic progress bar
        auto basicBar = std::make_shared<UIProgressBar>(100, 100, 300, 30, sdlManager_);
        basicBar->setProgress(0.65f);
        uiManager_.addComponent(basicBar, true);
        progressBars_.push_back(basicBar);
        
        auto basicLabel = std::make_shared<UILabel>(420, 105, 200, 20, sdlManager_, "Basic (65%)");
        uiManager_.addComponent(basicLabel, true);
        
        // Animated progress bar
        auto animatedBar = std::make_shared<UIProgressBar>(100, 150, 300, 30, sdlManager_);
        animatedBar->setAnimated(true, 1.5f);
        animatedBar->setProgress(0.85f);
        uiManager_.addComponent(animatedBar, true);
        progressBars_.push_back(animatedBar);
        
        auto animatedLabel = std::make_shared<UILabel>(420, 155, 200, 20, sdlManager_, "Animated (85%)");
        uiManager_.addComponent(animatedLabel, true);
        
        // Custom colored progress bar
        auto customBar = std::make_shared<UIProgressBar>(100, 200, 300, 30, sdlManager_);
        customBar->setProgress(0.45f);
        customBar->setColors({60, 20, 20, 255}, {255, 100, 100, 255}, {200, 50, 50, 255}, {255, 255, 255, 255});
        uiManager_.addComponent(customBar, true);
        progressBars_.push_back(customBar);
        
        auto customLabel = std::make_shared<UILabel>(420, 205, 200, 20, sdlManager_, "Custom Colors (45%)");
        uiManager_.addComponent(customLabel, true);
        
        // Progress bar with custom text
        auto textBar = std::make_shared<UIProgressBar>(100, 250, 300, 30, sdlManager_);
        textBar->setProgress(0.75f);
        textBar->setCustomText("Loading Assets...");
        uiManager_.addComponent(textBar, true);
        progressBars_.push_back(textBar);
        
        auto textLabel = std::make_shared<UILabel>(420, 255, 200, 20, sdlManager_, "Custom Text");
        uiManager_.addComponent(textLabel, true);
        
        // Thin progress bar without border
        auto thinBar = std::make_shared<UIProgressBar>(100, 300, 300, 15, sdlManager_);
        thinBar->setProgress(0.90f);
        thinBar->setBorderWidth(0);
        thinBar->setShowText(false);
        uiManager_.addComponent(thinBar, true);
        progressBars_.push_back(thinBar);
        
        auto thinLabel = std::make_shared<UILabel>(420, 302, 200, 20, sdlManager_, "Thin, No Border (90%)");
        uiManager_.addComponent(thinLabel, true);
        
        // Simulation progress bar
        auto simBar = std::make_shared<UIProgressBar>(100, 350, 300, 30, sdlManager_);
        simBar->setAnimated(true, 2.0f);
        simBar->setProgress(0.0f);
        uiManager_.addComponent(simBar, true);
        progressBars_.push_back(simBar);
        
        auto simLabel = std::make_shared<UILabel>(420, 355, 200, 20, sdlManager_, "Auto Simulation");
        uiManager_.addComponent(simLabel, true);
        
        // Control buttons
        auto resetButton = std::make_shared<UIButton>("Reset All", 100, 420, 100, 40, sdlManager_, [this]() {
            resetAllProgress();
        });
        uiManager_.addComponent(resetButton, true);
        
        auto randomButton = std::make_shared<UIButton>("Random", 220, 420, 100, 40, sdlManager_, [this]() {
            setRandomProgress();
        });
        uiManager_.addComponent(randomButton, true);
        
        auto exitButton = std::make_shared<UIButton>("Exit", 340, 420, 100, 40, sdlManager_, [this]() {
            exitRequested_ = true;
        });
        uiManager_.addComponent(exitButton, true);
        
        // Instructions
        auto instructions = std::make_shared<UILabel>(50, 480, 700, 80, sdlManager_, 
            "Instructions: Reset All - Sets all progress bars to 0%. Random - Sets random progress values. Bottom bar automatically cycles through progress values.");
        instructions->setTextColor({200, 200, 200, 255});
        instructions->setWordWrap(true);  // Enable word wrapping
        uiManager_.addComponent(instructions, true);
    }
    
    void resetAllProgress() {
        for (auto& bar : progressBars_) {
            bar->setProgress(0.0f);
        }
        currentProgressBar_ = 0;
        simulationTimer_ = 0.0f;
    }
    
    void setRandomProgress() {
        for (size_t i = 0; i < progressBars_.size() - 1; ++i) { // Skip simulation bar
            float randomProgress = static_cast<float>(rand()) / RAND_MAX;
            progressBars_[i]->setProgress(randomProgress);
        }
    }
    
    void updateProgressSimulation(float deltaTime) {
        simulationTimer_ += deltaTime;
        
        if (simulationTimer_ >= 2.0f) { // Change every 2 seconds
            simulationTimer_ = 0.0f;
            
            // Update simulation bar (last one)
            if (!progressBars_.empty()) {
                auto& simBar = progressBars_.back();
                currentProgressBar_ = (currentProgressBar_ + 1) % 6; // Cycle through 0-5
                float targetProgress = (currentProgressBar_ + 1) * 0.16f; // 0.16, 0.32, 0.48, 0.64, 0.8, 0.96
                simBar->setProgress(targetProgress);
                
                // Update custom text based on progress
                int percentage = static_cast<int>(targetProgress * 100);
                simBar->setCustomText("Step " + std::to_string(currentProgressBar_ + 1) + "/6 (" + std::to_string(percentage) + "%)");
            }
        }
    }
};

int main() {
    // Initialize SDL
    SDLManager sdl;
    if (!sdl.initialize()) {
        std::cerr << "Failed to initialize SDL" << std::endl;
        return -1;
    }
    
    // Set window title
    SDL_SetWindowTitle(sdl.getWindow(), "UI Framework - Progress Bar Demo");
    
    // Create demo
    ProgressBarDemo demo(sdl);
    
    std::cout << "Progress Bar Demo started!" << std::endl;
    std::cout << "Features demonstrated:" << std::endl;
    std::cout << "- Basic progress bar with percentage display" << std::endl;
    std::cout << "- Animated progress bar with smooth transitions" << std::endl;
    std::cout << "- Custom colored progress bar" << std::endl;
    std::cout << "- Progress bar with custom text" << std::endl;
    std::cout << "- Thin progress bar without border" << std::endl;
    std::cout << "- Automatic progress simulation" << std::endl;
    
    // Main loop
    bool running = true;
    Uint32 lastTime = SDL_GetTicks();
    
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            demo.handleEvent(event);
        }
        
        // Update
        demo.update(deltaTime);
        
        // Check for exit
        if (demo.shouldExit()) {
            running = false;
        }
        
        // Render
        demo.render();
        
        SDL_RenderPresent(sdl.getRenderer());
        SDL_Delay(16); // ~60 FPS
    }
    
    std::cout << "Progress Bar Demo finished!" << std::endl;
    return 0;
}
