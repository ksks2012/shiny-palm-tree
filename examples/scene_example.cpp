#include "Interface/ui/UISceneManager.h"
#include "Interface/ui/UILayoutContainer.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UILabel.h"
#include "Systems/SDLManager.h"
#include <iostream>

// Forward declaration for scene manager reference
class ExampleSceneManager;

/**
 * Main Menu Scene
 */
class MainMenuScene : public UIScene {
public:
    MainMenuScene(SDLManager& sdl, ExampleSceneManager* sceneManager) 
        : sdlManager_(sdl), sceneManager_(sceneManager) {}
    
    void onCreate() override;
    void onEnter() override;
    void onExit() override;
    void render() override;
    void handleEvent(const SDL_Event& event) override;
    
    bool shouldExit() const { return exitRequested_; }

private:
    SDLManager& sdlManager_;
    ExampleSceneManager* sceneManager_;
    std::shared_ptr<UILayoutContainer> container_;
    bool exitRequested_ = false;
};

/**
 * Game Scene
 */
class GameScene : public UIScene {
public:
    GameScene(SDLManager& sdl, ExampleSceneManager* sceneManager) 
        : sdlManager_(sdl), sceneManager_(sceneManager) {}
    
    void onCreate() override;
    void onEnter() override;
    void onExit() override;
    void render() override;
    void handleEvent(const SDL_Event& event) override;

private:
    SDLManager& sdlManager_;
    ExampleSceneManager* sceneManager_;
    std::shared_ptr<UILayoutContainer> container_;
};

/**
 * Pause Menu Scene (Overlay)
 */
class PauseScene : public UIScene {
public:
    PauseScene(SDLManager& sdl, ExampleSceneManager* sceneManager) 
        : sdlManager_(sdl), sceneManager_(sceneManager) {}
    
    void onCreate() override;
    void onEnter() override;
    void onExit() override;
    void render() override;
    void handleEvent(const SDL_Event& event) override;

private:
    SDLManager& sdlManager_;
    ExampleSceneManager* sceneManager_;
    std::shared_ptr<UILayoutContainer> container_;
};

/**
 * Settings Scene (Overlay)  
 */
class SettingsScene : public UIScene {
public:
    SettingsScene(SDLManager& sdl, ExampleSceneManager* sceneManager) 
        : sdlManager_(sdl), sceneManager_(sceneManager) {}
    
    void onCreate() override;
    void onEnter() override;
    void onExit() override;
    void render() override;
    void handleEvent(const SDL_Event& event) override;

private:
    SDLManager& sdlManager_;
    ExampleSceneManager* sceneManager_;
    std::shared_ptr<UILayoutContainer> container_;
};

/**
 * Custom Scene Manager with scene references
 */
class ExampleSceneManager : public UISceneManager {
public:
    ExampleSceneManager(SDLManager& sdl) : UISceneManager(sdl) {}
    
    void setupScenes(SDLManager& sdl) {
        // Register scenes with self-reference for callbacks
        registerScene<MainMenuScene>("mainmenu", sdl, this);
        registerScene<GameScene>("game", sdl, this);
        registerScene<PauseScene>("pause", sdl, this);
        registerScene<SettingsScene>("settings", sdl, this);
    }
    
    MainMenuScene* getMainMenuScene() {
        return static_cast<MainMenuScene*>(getScene("mainmenu").get());
    }
};

// Scene implementations
void MainMenuScene::onCreate() {
    std::cout << "MainMenuScene: Created" << std::endl;
    
    // Create main menu UI
    container_ = std::make_shared<UILayoutContainer>(100, 100, 600, 400, sdlManager_);
    container_->setVerticalLayout(20);
    
    // Title
    auto title = std::make_shared<UILabel>(0, 0, 600, 80, sdlManager_, "Main Menu");
    title->setBackgroundColor({50, 50, 100, 255});
    title->setTextColor({255, 255, 255, 255});
    container_->addChild(title);
    
    // Play button
    auto playButton = std::make_shared<UIButton>("Play Game", 0, 0, 200, 50, sdlManager_, [this]() {
        std::cout << "Play button clicked!" << std::endl;
        if (sceneManager_) {
            sceneManager_->switchToScene("game", SceneTransition::FADE, 0.5f);
        }
    });
    container_->addChild(playButton);
    
    // Settings button
    auto settingsButton = std::make_shared<UIButton>("Settings", 0, 0, 200, 50, sdlManager_, [this]() {
        std::cout << "Settings button clicked!" << std::endl;
        if (sceneManager_) {
            sceneManager_->pushScene("settings", SceneTransition::SLIDE_UP, 0.3f);
        }
    });
    container_->addChild(settingsButton);
    
    // Exit button
    auto exitButton = std::make_shared<UIButton>("Exit", 0, 0, 200, 50, sdlManager_, [this]() {
        std::cout << "Exit button clicked!" << std::endl;
        exitRequested_ = true;
    });
    container_->addChild(exitButton);
    
    // Add to UI manager
    getUIManager().addComponent(container_, true);
}

void MainMenuScene::onEnter() {
    std::cout << "MainMenuScene: Entered" << std::endl;
    exitRequested_ = false;
}

void MainMenuScene::onExit() {
    std::cout << "MainMenuScene: Exited" << std::endl;
}

void MainMenuScene::render() {
    // Clear background
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 30, 30, 60, 255);
    SDL_RenderClear(sdlManager_.getRenderer());
}

void MainMenuScene::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        std::cout << "ESC pressed in main menu - requesting exit" << std::endl;
        exitRequested_ = true;
    }
}

void GameScene::onCreate() {
    std::cout << "GameScene: Created" << std::endl;
    
    // Create game UI
    container_ = std::make_shared<UILayoutContainer>(50, 50, 700, 500, sdlManager_);
    container_->setVerticalLayout(15);
    
    // Game info
    auto gameLabel = std::make_shared<UILabel>(0, 0, 700, 60, sdlManager_, 
        "Game Scene - Press ESC for pause menu");
    gameLabel->setBackgroundColor({100, 50, 50, 255});
    gameLabel->setTextColor({255, 255, 255, 255});
    container_->addChild(gameLabel);
    
    // Game stats
    auto statsLabel = std::make_shared<UILabel>(0, 0, 700, 40, sdlManager_, 
        "Score: 1000 | Lives: 3 | Level: 1");
    statsLabel->setBackgroundColor({70, 70, 70, 255});
    statsLabel->setTextColor({255, 255, 255, 255});
    container_->addChild(statsLabel);
    
    // Back button
    auto backButton = std::make_shared<UIButton>("Back to Menu", 0, 0, 150, 40, sdlManager_, [this]() {
        std::cout << "Back to menu clicked!" << std::endl;
        if (sceneManager_) {
            sceneManager_->switchToScene("mainmenu", SceneTransition::FADE, 0.3f);
        }
    });
    container_->addChild(backButton);
    
    getUIManager().addComponent(container_, true);
}

void GameScene::onEnter() {
    std::cout << "GameScene: Entered" << std::endl;
}

void GameScene::onExit() {
    std::cout << "GameScene: Exited" << std::endl;
}

void GameScene::render() {
    // Clear background
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 60, 30, 30, 255);
    SDL_RenderClear(sdlManager_.getRenderer());
}

void GameScene::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        std::cout << "ESC pressed in game - opening pause menu" << std::endl;
        if (sceneManager_) {
            sceneManager_->pushScene("pause", SceneTransition::SLIDE_DOWN, 0.2f);
        }
    }
}

void PauseScene::onCreate() {
    std::cout << "PauseScene: Created" << std::endl;
    
    // Create semi-transparent overlay
    container_ = std::make_shared<UILayoutContainer>(250, 200, 300, 200, sdlManager_);
    container_->setVerticalLayout(15);
    
    // Title
    auto title = std::make_shared<UILabel>(0, 0, 300, 50, sdlManager_, "Game Paused");
    title->setBackgroundColor({0, 0, 0, 200});
    title->setTextColor({255, 255, 255, 255});
    container_->addChild(title);
    
    // Resume button
    auto resumeButton = std::make_shared<UIButton>("Resume", 0, 0, 120, 35, sdlManager_, [this]() {
        std::cout << "Resume clicked!" << std::endl;
        if (sceneManager_) {
            sceneManager_->popScene(SceneTransition::SLIDE_UP, 0.2f);
        }
    });
    container_->addChild(resumeButton);
    
    // Main menu button
    auto menuButton = std::make_shared<UIButton>("Main Menu", 0, 0, 120, 35, sdlManager_, [this]() {
        std::cout << "Main Menu clicked from pause!" << std::endl;
        if (sceneManager_) {
            sceneManager_->switchToScene("mainmenu", SceneTransition::FADE, 0.4f);
        }
    });
    container_->addChild(menuButton);
    
    getUIManager().addComponent(container_, true);
}

void PauseScene::onEnter() {
    std::cout << "PauseScene: Entered" << std::endl;
}

void PauseScene::onExit() {
    std::cout << "PauseScene: Exited" << std::endl;
}

void PauseScene::render() {
    // Render semi-transparent overlay
    SDL_SetRenderDrawBlendMode(sdlManager_.getRenderer(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 0, 0, 0, 128);
    SDL_RenderFillRect(sdlManager_.getRenderer(), nullptr);
}

void PauseScene::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        std::cout << "ESC pressed in pause menu - resuming" << std::endl;
        if (sceneManager_) {
            sceneManager_->popScene(SceneTransition::SLIDE_UP, 0.2f);
        }
    }
}

void SettingsScene::onCreate() {
    std::cout << "SettingsScene: Created" << std::endl;
    
    container_ = std::make_shared<UILayoutContainer>(200, 150, 400, 300, sdlManager_);
    container_->setVerticalLayout(15);
    
    // Title
    auto title = std::make_shared<UILabel>(0, 0, 400, 50, sdlManager_, "Settings");
    title->setBackgroundColor({50, 100, 50, 255});
    title->setTextColor({255, 255, 255, 255});
    container_->addChild(title);
    
    // Sample settings
    auto soundLabel = std::make_shared<UILabel>(0, 0, 400, 35, sdlManager_, "Sound: ON");
    soundLabel->setBackgroundColor({70, 70, 70, 255});
    soundLabel->setTextColor({255, 255, 255, 255});
    container_->addChild(soundLabel);
    
    auto musicLabel = std::make_shared<UILabel>(0, 0, 400, 35, sdlManager_, "Music: ON");
    musicLabel->setBackgroundColor({70, 70, 70, 255});
    musicLabel->setTextColor({255, 255, 255, 255});
    container_->addChild(musicLabel);
    
    auto difficultyLabel = std::make_shared<UILabel>(0, 0, 400, 35, sdlManager_, "Difficulty: Normal");
    difficultyLabel->setBackgroundColor({70, 70, 70, 255});
    difficultyLabel->setTextColor({255, 255, 255, 255});
    container_->addChild(difficultyLabel);
    
    // Close button
    auto closeButton = std::make_shared<UIButton>("Close", 0, 0, 100, 35, sdlManager_, [this]() {
        std::cout << "Settings close clicked!" << std::endl;
        if (sceneManager_) {
            sceneManager_->popScene(SceneTransition::SLIDE_DOWN, 0.3f);
        }
    });
    container_->addChild(closeButton);
    
    getUIManager().addComponent(container_, true);
}

void SettingsScene::onEnter() {
    std::cout << "SettingsScene: Entered" << std::endl;
}

void SettingsScene::onExit() {
    std::cout << "SettingsScene: Exited" << std::endl;
}

void SettingsScene::render() {
    // Render semi-transparent overlay
    SDL_SetRenderDrawBlendMode(sdlManager_.getRenderer(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 0, 0, 0, 100);
    SDL_RenderFillRect(sdlManager_.getRenderer(), nullptr);
}

void SettingsScene::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        std::cout << "ESC pressed in settings - closing" << std::endl;
        if (sceneManager_) {
            sceneManager_->popScene(SceneTransition::SLIDE_DOWN, 0.3f);
        }
    }
}

int main() {
    // Initialize SDL
    SDLManager sdl;
    if (!sdl.initialize()) {
        std::cerr << "Failed to initialize SDL" << std::endl;
        return -1;
    }
    
    // Set window title
    SDL_SetWindowTitle(sdl.getWindow(), "UI Framework - Scene Manager Example");
    
    // Create scene manager
    ExampleSceneManager sceneManager(sdl);
    
    // Setup and register scenes
    sceneManager.setupScenes(sdl);
    
    // Start with main menu
    sceneManager.switchToScene("mainmenu");
    
    std::cout << "Scene Manager Example started!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- Click buttons to navigate between scenes" << std::endl;
    std::cout << "- Press ESC in game to open pause menu" << std::endl;
    std::cout << "- Press ESC in pause/settings to close overlay" << std::endl;
    std::cout << "- Press ESC in main menu to exit" << std::endl;
    
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
            
            sceneManager.handleEvent(event);
        }
        
        // Update and render
        sceneManager.update(deltaTime);
        sceneManager.render();
        
        SDL_RenderPresent(sdl.getRenderer());
        
        // Check for exit from main menu
        if (auto mainMenu = sceneManager.getMainMenuScene()) {
            if (mainMenu->shouldExit()) {
                running = false;
            }
        }
        
        SDL_Delay(16); // ~60 FPS
    }
    
    std::cout << "Scene Manager Example finished!" << std::endl;
    return 0;
}
