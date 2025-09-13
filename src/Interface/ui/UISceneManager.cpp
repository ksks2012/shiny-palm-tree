#include "Interface/ui/UISceneManager.h"
#include "Systems/SDLManager.h"
#include <algorithm>
#include <iostream>

UISceneManager::UISceneManager(SDLManager& sdlManager) 
    : sdlManager_(sdlManager) {
}

UISceneManager::~UISceneManager() {
    // Cleanup all scenes
    while (!sceneStack_.empty()) {
        auto scene = sceneStack_.top();
        sceneStack_.pop();
        scene->onExit();
        scene->onDestroy();
    }
    
    for (auto& [name, scene] : scenes_) {
        scene->onDestroy();
    }
}

void UISceneManager::switchToScene(const std::string& name, 
                                  SceneTransition transition, 
                                  float transitionDuration) {
    auto it = scenes_.find(name);
    if (it == scenes_.end()) {
        std::cerr << "UISceneManager: Scene '" << name << "' not found!" << std::endl;
        return;
    }
    
    auto newScene = it->second;
    auto currentScene = getCurrentScene();
    
    if (currentScene == newScene) {
        return; // Already current scene
    }
    
    // Setup transition
    if (transition != SceneTransition::NONE && transitionDuration > 0.0f) {
        transitionActive_ = true;
        currentTransition_ = transition;
        transitionDuration_ = transitionDuration;
        transitionTime_ = 0.0f;
        transitionFromScene_ = currentScene;
        transitionToScene_ = newScene;
        
        std::cout << "UISceneManager: Starting transition to scene '" << name << "'" << std::endl;
    } else {
        // Immediate switch
        if (currentScene) {
            deactivateScene(currentScene);
        }
        
        // Clear stack and set new scene
        while (!sceneStack_.empty()) {
            sceneStack_.pop();
        }
        
        sceneStack_.push(newScene);
        activateScene(newScene);
        std::cout << "UISceneManager: Switched to scene '" << name << "'" << std::endl;
    }
}

void UISceneManager::pushScene(const std::string& name, 
                              SceneTransition transition, 
                              float transitionDuration) {
    auto it = scenes_.find(name);
    if (it == scenes_.end()) {
        std::cerr << "UISceneManager: Scene '" << name << "' not found!" << std::endl;
        return;
    }
    
    auto newScene = it->second;
    auto currentScene = getCurrentScene();
    
    // Pause current scene
    if (currentScene) {
        currentScene->setPaused(true);
    }
    
    // Setup transition
    if (transition != SceneTransition::NONE && transitionDuration > 0.0f) {
        transitionActive_ = true;
        currentTransition_ = transition;
        transitionDuration_ = transitionDuration;
        transitionTime_ = 0.0f;
        transitionFromScene_ = currentScene;
        transitionToScene_ = newScene;
        
        std::cout << "UISceneManager: Starting push transition to scene '" << name << "'" << std::endl;
    } else {
        // Immediate push
        sceneStack_.push(newScene);
        activateScene(newScene);
        std::cout << "UISceneManager: Pushed scene '" << name << "'" << std::endl;
    }
}

void UISceneManager::popScene(SceneTransition transition, float transitionDuration) {
    if (sceneStack_.size() <= 1) {
        std::cerr << "UISceneManager: Cannot pop last scene!" << std::endl;
        return;
    }
    
    auto currentScene = getCurrentScene();
    sceneStack_.pop();
    auto newScene = getCurrentScene();
    
    // Setup transition
    if (transition != SceneTransition::NONE && transitionDuration > 0.0f) {
        transitionActive_ = true;
        currentTransition_ = transition;
        transitionDuration_ = transitionDuration;
        transitionTime_ = 0.0f;
        transitionFromScene_ = currentScene;
        transitionToScene_ = newScene;
        
        std::cout << "UISceneManager: Starting pop transition" << std::endl;
    } else {
        // Immediate pop
        if (currentScene) {
            deactivateScene(currentScene);
        }
        
        if (newScene) {
            newScene->setPaused(false);
            activateScene(newScene);
        }
        std::cout << "UISceneManager: Popped scene" << std::endl;
    }
}

void UISceneManager::popToScene(const std::string& name, 
                               SceneTransition transition, 
                               float transitionDuration) {
    // Find the scene in the stack
    std::stack<std::shared_ptr<UIScene>> tempStack;
    bool found = false;
    
    while (!sceneStack_.empty()) {
        auto scene = sceneStack_.top();
        sceneStack_.pop();
        
        // Check if this is a scene with the target name
        for (auto& [sceneName, storedScene] : scenes_) {
            if (storedScene == scene && sceneName == name) {
                found = true;
                sceneStack_.push(scene);
                break;
            }
        }
        
        if (found) break;
        tempStack.push(scene);
    }
    
    if (!found) {
        // Restore the stack if target scene not found
        while (!tempStack.empty()) {
            sceneStack_.push(tempStack.top());
            tempStack.pop();
        }
        std::cerr << "UISceneManager: Scene '" << name << "' not found in stack!" << std::endl;
        return;
    }
    
    auto newScene = getCurrentScene();
    if (newScene) {
        newScene->setPaused(false);
        activateScene(newScene);
    }
    
    std::cout << "UISceneManager: Popped to scene '" << name << "'" << std::endl;
}

std::shared_ptr<UIScene> UISceneManager::getCurrentScene() const {
    if (sceneStack_.empty()) {
        return nullptr;
    }
    return sceneStack_.top();
}

std::shared_ptr<UIScene> UISceneManager::getScene(const std::string& name) const {
    auto it = scenes_.find(name);
    return (it != scenes_.end()) ? it->second : nullptr;
}

bool UISceneManager::hasScene(const std::string& name) const {
    return scenes_.find(name) != scenes_.end();
}

void UISceneManager::update(float deltaTime) {
    if (transitionActive_) {
        transitionTime_ += deltaTime;
        
        if (transitionTime_ >= transitionDuration_) {
            finishTransition();
        }
    }
    
    // Update current scene (only if not transitioning or transition allows updates)
    auto currentScene = getCurrentScene();
    if (currentScene && !currentScene->isPaused()) {
        currentScene->update(deltaTime);
    }
}

void UISceneManager::render() {
    if (transitionActive_) {
        renderTransition();
    } else {
        // Render all scenes in stack (for overlay effects)
        std::stack<std::shared_ptr<UIScene>> tempStack = sceneStack_;
        std::vector<std::shared_ptr<UIScene>> scenesToRender;
        
        while (!tempStack.empty()) {
            scenesToRender.push_back(tempStack.top());
            tempStack.pop();
        }
        
        // Render from bottom to top
        for (auto it = scenesToRender.rbegin(); it != scenesToRender.rend(); ++it) {
            (*it)->render();
            (*it)->getUIManager().renderAll();
        }
    }
}

void UISceneManager::handleEvent(const SDL_Event& event) {
    if (transitionActive_) {
        return; // Don't handle events during transitions
    }
    
    auto currentScene = getCurrentScene();
    if (currentScene && !currentScene->isPaused()) {
        // For keyboard events, let scene handle first (for shortcuts like ESC)
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            currentScene->handleEvent(event);
        }
        
        // For all events (including mouse events), let UI components handle
        currentScene->getUIManager().handleEvent(event);
        
        // For non-keyboard events, let scene handle afterwards if needed
        if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) {
            currentScene->handleEvent(event);
        }
    }
}

void UISceneManager::setGlobalData(const std::string& key, const std::string& value) {
    globalData_[key] = value;
}

std::string UISceneManager::getGlobalData(const std::string& key) const {
    auto it = globalData_.find(key);
    return (it != globalData_.end()) ? it->second : "";
}

bool UISceneManager::hasGlobalData(const std::string& key) const {
    return globalData_.find(key) != globalData_.end();
}

void UISceneManager::activateScene(std::shared_ptr<UIScene> scene) {
    if (!scene) return;
    
    scene->active_ = true;
    scene->onEnter();
}

void UISceneManager::deactivateScene(std::shared_ptr<UIScene> scene) {
    if (!scene) return;
    
    scene->active_ = false;
    scene->onExit();
}

void UISceneManager::finishTransition() {
    if (transitionFromScene_) {
        deactivateScene(transitionFromScene_);
    }
    
    if (transitionToScene_) {
        // Handle scene stack changes for push transitions
        if (currentTransition_ == SceneTransition::SLIDE_UP || 
            currentTransition_ == SceneTransition::SLIDE_DOWN ||
            currentTransition_ == SceneTransition::FADE) {
            // For push-like transitions, add the scene to stack
            auto currentTop = getCurrentScene();
            if (currentTop != transitionToScene_) {
                sceneStack_.push(transitionToScene_);
            }
        }
        
        activateScene(transitionToScene_);
    }
    
    transitionActive_ = false;
    transitionFromScene_.reset();
    transitionToScene_.reset();
    
    std::cout << "UISceneManager: Transition finished" << std::endl;
}

void UISceneManager::renderTransition() {
    float progress = transitionTime_ / transitionDuration_;
    progress = std::clamp(progress, 0.0f, 1.0f);
    
    switch (currentTransition_) {
        case SceneTransition::FADE:
            renderFadeTransition();
            break;
        case SceneTransition::SLIDE_LEFT:
        case SceneTransition::SLIDE_RIGHT:
        case SceneTransition::SLIDE_UP:
        case SceneTransition::SLIDE_DOWN:
            renderSlideTransition();
            break;
        default:
            // Render both scenes normally
            if (transitionFromScene_) {
                transitionFromScene_->render();
                transitionFromScene_->getUIManager().renderAll();
            }
            if (transitionToScene_) {
                transitionToScene_->render();
                transitionToScene_->getUIManager().renderAll();
            }
            break;
    }
}

void UISceneManager::renderFadeTransition() {
    float progress = transitionTime_ / transitionDuration_;
    
    // Render the old scene
    if (transitionFromScene_) {
        transitionFromScene_->render();
        transitionFromScene_->getUIManager().renderAll();
        
        // Apply fade out effect
        SDL_SetRenderDrawBlendMode(sdlManager_.getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 0, 0, 0, 
                              static_cast<Uint8>(255 * progress));
        SDL_RenderFillRect(sdlManager_.getRenderer(), nullptr);
    }
    
    // Render the new scene with fade in
    if (progress > 0.5f && transitionToScene_) {
        float fadeIn = (progress - 0.5f) * 2.0f;
        
        transitionToScene_->render();
        transitionToScene_->getUIManager().renderAll();
        
        SDL_SetRenderDrawBlendMode(sdlManager_.getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 0, 0, 0, 
                              static_cast<Uint8>(255 * (1.0f - fadeIn)));
        SDL_RenderFillRect(sdlManager_.getRenderer(), nullptr);
    }
}

void UISceneManager::renderSlideTransition() {
    // Implementation for slide transitions would require viewport manipulation
    // For now, just render both scenes with some basic effects
    float progress = transitionTime_ / transitionDuration_;
    
    if (transitionFromScene_) {
        transitionFromScene_->render();
        transitionFromScene_->getUIManager().renderAll();
        
        // Apply some transparency based on progress
        SDL_SetRenderDrawBlendMode(sdlManager_.getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 0, 0, 0, 
                              static_cast<Uint8>(100 * progress));
        SDL_RenderFillRect(sdlManager_.getRenderer(), nullptr);
    }
    
    if (transitionToScene_ && progress > 0.3f) {
        float slideIn = (progress - 0.3f) / 0.7f;
        
        transitionToScene_->render();
        transitionToScene_->getUIManager().renderAll();
        
        // Apply fade in effect for slide
        SDL_SetRenderDrawBlendMode(sdlManager_.getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 0, 0, 0, 
                              static_cast<Uint8>(255 * (1.0f - slideIn)));
        SDL_RenderFillRect(sdlManager_.getRenderer(), nullptr);
    }
}
