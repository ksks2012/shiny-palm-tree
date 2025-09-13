# UI Scene Manager

The UI framework's scene management system provides complete scene switching, stack management, and transition effects.

## Core Concepts

### UIScene base class
Each scene inherits from the `UIScene` base class and provides full lifecycle management:

```cpp
class MyScene : public UIScene {
public:
    MyScene(SDLManager& sdl) : sdlManager_(sdl) {}
    
    // Must implement
    void onCreate() override;      // Initialize scene resources
    
    // Optional overrides
    void onEnter() override;       // Called when scene becomes active
    void onExit() override;        // Called when scene becomes inactive
    void onDestroy() override;     // Clean up scene resources
    void update(float deltaTime) override;  // Update scene logic
    void render() override;        // Render scene background
    void handleEvent(const SDL_Event& event) override; // Handle input events

private:
    SDLManager& sdlManager_;
};
```

### UISceneManager scene manager
Manages the lifecycle, switching, and transitions of all scenes:

```cpp
SDLManager sdl;
UISceneManager sceneManager(sdl);

// Register scenes
sceneManager.registerScene<MainMenuScene>("mainmenu", sdl);
sceneManager.registerScene<GameScene>("game", sdl);
sceneManager.registerScene<PauseScene>("pause", sdl);

// Scene navigation
sceneManager.switchToScene("mainmenu");                    // Switch to a scene
sceneManager.pushScene("pause", SceneTransition::FADE);    // Push a scene (stack)
sceneManager.popScene(SceneTransition::SLIDE_UP);          // Pop a scene
```

## Scene Navigation Modes

### 1. Switch to scene (switchToScene)
Completely replaces the current scene and clears the scene stack:
```cpp
// Switch from the main menu to the game scene
sceneManager.switchToScene("game", SceneTransition::FADE, 0.5f);
```

### 2. Push scene (pushScene)
Pushes a new scene on top of the current one to form a stack (good for dialogs or pause menus):
```cpp
// Push the pause menu on top of the game scene
sceneManager.pushScene("pause", SceneTransition::SLIDE_DOWN, 0.3f);
```

### 3. Pop scene (popScene)
Removes the current scene and returns to the scene below:
```cpp
// Close the pause menu and return to the game
sceneManager.popScene(SceneTransition::SLIDE_UP, 0.2f);
```

### 4. Pop to specific scene (popToScene)
Removes multiple scenes from the stack until the specified scene is found:
```cpp
// Return directly to the main menu from a deep menu
sceneManager.popToScene("mainmenu");
```

## Transition Types

```cpp
enum class SceneTransition {
    NONE,           // Instant switch
    FADE,           // Fade in/out
    SLIDE_LEFT,     // Slide left
    SLIDE_RIGHT,    // Slide right
    SLIDE_UP,       // Slide up
    SLIDE_DOWN      // Slide down
};
```

## Global Data Sharing

Scenes can share global data:
```cpp
// Set global data
sceneManager.setGlobalData("player_name", "Alice");
sceneManager.setGlobalData("player_score", "1000");

// Retrieve in any scene
std::string playerName = sceneManager.getGlobalData("player_name");
bool hasScore = sceneManager.hasGlobalData("player_score");
```

## Key Features

### 🎯 Scene stack management
- Supports scene stacking, suitable for modal dialogs and overlay scenes
- Lower scenes are automatically paused; when an upper scene closes the lower scene is restored

### 🎨 Transition system
- Multiple built-in transitions (fade, slide, etc.)
- Customizable transition durations
- Automatically manages scene states during transitions

### 🔄 Full lifecycle
- onCreate: called once when the scene is created
- onEnter/onExit: called when the scene's active state changes
- onDestroy: called when the scene is destroyed

### 📊 State management
- Automatically manages active/paused states of scenes
- Only active scenes receive updates and events
- Supports checking scene state and transition progress

### 🔄 Event handling integration
- Integrates seamlessly with UIManager
- Automatically routes events to active scene UI components
- Supports keyboard shortcuts (e.g. Tab to change focus)

## Usage Example

### Complete scene management application

```cpp
// Main menu scene
class MainMenuScene : public UIScene {
public:
    MainMenuScene(SDLManager& sdl, UISceneManager* manager) 
        : sdlManager_(sdl), sceneManager_(manager) {}
    
    void onCreate() override {
        // Create main menu UI
        container_ = std::make_shared<UILayoutContainer>(100, 100, 600, 400, sdlManager_);
        container_->setVerticalLayout(20);
        
        auto playButton = std::make_shared<UIButton>("Play Game", 0, 0, 200, 50, sdlManager_, [this]() {
            sceneManager_->switchToScene("game", SceneTransition::FADE, 0.5f);
        });
        container_->addChild(playButton);
        
        getUIManager().addComponent(container_, true);
    }
    
    void render() override {
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 30, 30, 60, 255);
        SDL_RenderClear(sdlManager_.getRenderer());
    }

private:
    SDLManager& sdlManager_;
    UISceneManager* sceneManager_;
    std::shared_ptr<UILayoutContainer> container_;
};

// Game scene
class GameScene : public UIScene {
public:
    GameScene(SDLManager& sdl, UISceneManager* manager) 
        : sdlManager_(sdl), sceneManager_(manager) {}
    
    void onCreate() override {
        // Create game UI
        // ...
    }
    
    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            // ESC opens the pause menu
            sceneManager_->pushScene("pause", SceneTransition::SLIDE_DOWN, 0.2f);
        }
    }
    
    void render() override {
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 60, 30, 30, 255);
        SDL_RenderClear(sdlManager_.getRenderer());
    }

private:
    SDLManager& sdlManager_;
    UISceneManager* sceneManager_;
};

// Main program
int main() {
    SDLManager sdl;
    UISceneManager sceneManager(sdl);
    
    // Register scenes
    sceneManager.registerScene<MainMenuScene>("mainmenu", sdl, &sceneManager);
    sceneManager.registerScene<GameScene>("game", sdl, &sceneManager);
    sceneManager.registerScene<PauseScene>("pause", sdl, &sceneManager);
    
    // Start at the main menu
    sceneManager.switchToScene("mainmenu");
    
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
        
        sceneManager.update(deltaTime);
        sceneManager.render();
        
        SDL_RenderPresent(sdl.getRenderer());
        SDL_Delay(16);
    }
    
    return 0;
}
```

## Best Practices

### 🔧 Scene design
- Each scene should be self-contained and own its UI components
- Use global data to share state between scenes
- Avoid direct references between scenes; use the scene manager for navigation

### 📈 Performance considerations
- Only active scenes run update() and receive events
- Lower scenes in the stack are automatically paused
- Event processing is temporarily suspended during transitions

### 🎨 UI design
- Use UILayoutContainer to organize a scene's UI components
- Use transitions to improve user experience
- Choose appropriate navigation methods for different types of scenes

### 🐛 Error handling
- The scene manager safely handles non-existent scene names
- Stack operations handle boundary cases safely
- Supports checking scene state and transition progress

This scene management system provides enterprise-grade scene navigation capabilities for the UI framework, supporting complex application architectures and smooth user experiences.
