#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <stack>
#include <SDL2/SDL.h>
#include "UIManager.h"

// Forward declaration
class SDLManager;

/**
 * Scene base class - represents a complete UI view/screen
 * Each scene manages its own UI components and lifecycle
 */
class UIScene {
public:
    virtual ~UIScene() = default;
    
    // Scene lifecycle methods
    virtual void onCreate() = 0;      // Initialize scene resources
    virtual void onEnter() {}         // Called when scene becomes active
    virtual void onExit() {}          // Called when scene becomes inactive
    virtual void onDestroy() {}       // Cleanup scene resources
    
    // Frame methods
    virtual void update(float deltaTime) {}  // Update scene logic
    virtual void render() {}                 // Render scene
    virtual void handleEvent(const SDL_Event& event) {} // Handle input
    
    // Scene state
    bool isActive() const { return active_; }
    bool isPaused() const { return paused_; }
    void setPaused(bool paused) { paused_ = paused; }
    
    // UI Manager access
    UIManager& getUIManager() { return uiManager_; }
    const UIManager& getUIManager() const { return uiManager_; }

protected:
    bool active_ = false;
    bool paused_ = false;
    UIManager uiManager_;
    
    friend class UISceneManager;
};

/**
 * Scene transition types for different visual effects
 */
enum class SceneTransition {
    NONE,           // Instant switch
    FADE,           // Fade out/in effect  
    SLIDE_LEFT,     // Slide to left
    SLIDE_RIGHT,    // Slide to right
    SLIDE_UP,       // Slide up
    SLIDE_DOWN      // Slide down
};

/**
 * Scene manager handles scene lifecycle, transitions, and stacking
 * Supports scene stack for overlay scenes (like pause menus, dialogs)
 */
class UISceneManager {
public:
    explicit UISceneManager(SDLManager& sdlManager);
    ~UISceneManager();
    
    // Scene registration and creation
    template<typename T, typename... Args>
    void registerScene(const std::string& name, Args&&... args);
    
    // Scene navigation
    void switchToScene(const std::string& name, 
                      SceneTransition transition = SceneTransition::NONE,
                      float transitionDuration = 0.3f);
    
    void pushScene(const std::string& name,
                   SceneTransition transition = SceneTransition::NONE,
                   float transitionDuration = 0.3f);
    
    void popScene(SceneTransition transition = SceneTransition::NONE,
                  float transitionDuration = 0.3f);
    
    void popToScene(const std::string& name,
                    SceneTransition transition = SceneTransition::NONE,
                    float transitionDuration = 0.3f);
    
    // Scene access
    std::shared_ptr<UIScene> getCurrentScene() const;
    std::shared_ptr<UIScene> getScene(const std::string& name) const;
    bool hasScene(const std::string& name) const;
    
    // Frame methods
    void update(float deltaTime);
    void render();
    void handleEvent(const SDL_Event& event);
    
    // Scene stack info
    size_t getSceneStackSize() const { return sceneStack_.size(); }
    bool isTransitioning() const { return transitionActive_; }
    
    // Global scene data sharing
    void setGlobalData(const std::string& key, const std::string& value);
    std::string getGlobalData(const std::string& key) const;
    bool hasGlobalData(const std::string& key) const;

private:
    SDLManager& sdlManager_;
    
    // Scene storage and stack
    std::unordered_map<std::string, std::shared_ptr<UIScene>> scenes_;
    std::stack<std::shared_ptr<UIScene>> sceneStack_;
    
    // Transition state
    bool transitionActive_ = false;
    SceneTransition currentTransition_ = SceneTransition::NONE;
    float transitionDuration_ = 0.3f;
    float transitionTime_ = 0.0f;
    std::shared_ptr<UIScene> transitionFromScene_;
    std::shared_ptr<UIScene> transitionToScene_;
    
    // Global data sharing between scenes
    std::unordered_map<std::string, std::string> globalData_;
    
    // Transition rendering
    void renderTransition();
    void renderFadeTransition();
    void renderSlideTransition();
    
    // Scene lifecycle helpers
    void activateScene(std::shared_ptr<UIScene> scene);
    void deactivateScene(std::shared_ptr<UIScene> scene);
    void finishTransition();
};

// Template implementation
template<typename T, typename... Args>
void UISceneManager::registerScene(const std::string& name, Args&&... args) {
    static_assert(std::is_base_of_v<UIScene, T>, "T must inherit from UIScene");
    
    auto scene = std::make_shared<T>(std::forward<Args>(args)...);
    scenes_[name] = scene;
    scene->onCreate();
}

/**
 * Convenient macros for common scene patterns
 */
#define SCENE_CLASS(className) \
class className : public UIScene { \
public: \
    className(SDLManager& sdl) : sdlManager_(sdl) {} \
    void onCreate() override; \
    void onEnter() override; \
    void onExit() override; \
    void update(float deltaTime) override; \
    void render() override; \
    void handleEvent(const SDL_Event& event) override; \
protected: \
    SDLManager& sdlManager_; \
private:

#define END_SCENE_CLASS };
