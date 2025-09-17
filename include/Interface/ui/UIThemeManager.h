#pragma once
#include "UITheme.h"
#include <vector>
#include <memory>

// Forward declarations
class UIComponent;
class UIManager;
class UIButton;
class UILabel;
class UITextInput;
class UIProgressBar;

/**
 * UIThemeManager - Manages theme application to UI components
 * Provides automatic theme updates and component registration
 * Integrates with existing UIConstants.h definitions
 */
class UIThemeManager {
public:
    // Singleton pattern for global theme management
    static UIThemeManager& getInstance();
    
    // Component registration for automatic theme updates
    void registerComponent(const std::string& componentType, UIComponent* component);
    void unregisterComponent(UIComponent* component);
    void registerUIManager(UIManager* uiManager);
    void unregisterUIManager(UIManager* uiManager);
    
    // Theme application methods
    void applyThemeToComponent(UIComponent* component);
    void applyThemeToAllRegisteredComponents();
    void updateAllComponentThemes();
    
    // Theme change handling
    void onThemeChanged(const UIThemeData& newTheme);
    
    // Auto-registration helpers
    template<typename T>
    void registerAndApplyTheme(T* component, const std::string& typeName) {
        if (component) {
            registerComponent(typeName, component);
            applyThemeToComponent(component);
        }
    }
    
    // UIConstants integration
    void syncThemeWithUIConstants();
    void updateUIConstantsFromTheme(const UIThemeData& theme);
    
    // Configuration
    void setAutoApplyTheme(bool autoApply) { autoApplyTheme_ = autoApply; }
    bool isAutoApplyEnabled() const { return autoApplyTheme_; }
    
    // Statistics and debugging
    size_t getRegisteredComponentCount() const { return registeredComponents_.size(); }
    void printRegisteredComponents() const;

private:
    UIThemeManager();
    ~UIThemeManager() = default;
    
    // Prevent copying
    UIThemeManager(const UIThemeManager&) = delete;
    UIThemeManager& operator=(const UIThemeManager&) = delete;
    
    // Component storage
    struct ComponentInfo {
        std::string type;
        UIComponent* component;
        bool isValid() const { return component != nullptr; }
    };
    
    std::vector<ComponentInfo> registeredComponents_;
    std::vector<UIManager*> registeredManagers_;
    
    // Configuration
    bool autoApplyTheme_;
    bool initialized_;
    
    // Helper methods
    void cleanupInvalidComponents();
    std::string getComponentTypeName(UIComponent* component);
    void initializeThemeCallbacks();
};

/**
 * RAII helper class for automatic component theme management
 * Automatically registers component on construction and unregisters on destruction
 */
template<typename ComponentType>
class ThemeComponentGuard {
public:
    ThemeComponentGuard(ComponentType* component, const std::string& typeName)
        : component_(component), typeName_(typeName) {
        if (component_) {
            UIThemeManager::getInstance().registerAndApplyTheme(component_, typeName_);
        }
    }
    
    ~ThemeComponentGuard() {
        if (component_) {
            UIThemeManager::getInstance().unregisterComponent(component_);
        }
    }
    
    // Non-copyable, movable
    ThemeComponentGuard(const ThemeComponentGuard&) = delete;
    ThemeComponentGuard& operator=(const ThemeComponentGuard&) = delete;
    
    ThemeComponentGuard(ThemeComponentGuard&& other) noexcept
        : component_(other.component_), typeName_(std::move(other.typeName_)) {
        other.component_ = nullptr;
    }
    
    ThemeComponentGuard& operator=(ThemeComponentGuard&& other) noexcept {
        if (this != &other) {
            if (component_) {
                UIThemeManager::getInstance().unregisterComponent(component_);
            }
            component_ = other.component_;
            typeName_ = std::move(other.typeName_);
            other.component_ = nullptr;
        }
        return *this;
    }

private:
    ComponentType* component_;
    std::string typeName_;
};

// Convenience macros for theme-managed component creation
#define THEME_MANAGED_BUTTON(name, ...) \
    auto name = std::make_unique<UIButton>(__VA_ARGS__); \
    UIThemeManager::getInstance().registerAndApplyTheme(name.get(), "UIButton")

#define THEME_MANAGED_LABEL(name, ...) \
    auto name = std::make_unique<UILabel>(__VA_ARGS__); \
    UIThemeManager::getInstance().registerAndApplyTheme(name.get(), "UILabel")

#define THEME_MANAGED_TEXTINPUT(name, ...) \
    auto name = std::make_unique<UITextInput>(__VA_ARGS__); \
    UIThemeManager::getInstance().registerAndApplyTheme(name.get(), "UITextInput")

#define THEME_MANAGED_PROGRESSBAR(name, ...) \
    auto name = std::make_unique<UIProgressBar>(__VA_ARGS__); \
    UIThemeManager::getInstance().registerAndApplyTheme(name.get(), "UIProgressBar")
