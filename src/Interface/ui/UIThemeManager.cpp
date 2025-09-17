#include "Interface/ui/UIThemeManager.h"
#include "Interface/ui/UIComponent.h"
#include "Interface/ui/UIManager.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/UITextInput.h"
#include "Interface/ui/UIProgressBar.h"
#include "UIConstants.h"
#include <iostream>
#include <algorithm>
#include <typeinfo>

// Singleton implementation
UIThemeManager& UIThemeManager::getInstance() {
    static UIThemeManager instance;
    return instance;
}

UIThemeManager::UIThemeManager() 
    : autoApplyTheme_(true), initialized_(false) {
    initializeThemeCallbacks();
    initialized_ = true;
    std::cout << "UIThemeManager initialized with auto-apply theme enabled" << std::endl;
}

void UIThemeManager::initializeThemeCallbacks() {
    // Register with UITheme for theme change notifications
    UITheme& theme = UITheme::getInstance();
    theme.registerThemeChangeCallback([this](const UIThemeData& newTheme) {
        this->onThemeChanged(newTheme);
    });
}

void UIThemeManager::registerComponent(const std::string& componentType, UIComponent* component) {
    if (!component) {
        std::cerr << "Warning: Attempted to register null component of type " << componentType << std::endl;
        return;
    }
    
    // Check if component is already registered
    auto it = std::find_if(registeredComponents_.begin(), registeredComponents_.end(),
        [component](const ComponentInfo& info) { return info.component == component; });
    
    if (it != registeredComponents_.end()) {
        std::cout << "Component already registered, updating type to " << componentType << std::endl;
        it->type = componentType;
        return;
    }
    
    // Add new component
    registeredComponents_.push_back({componentType, component});
    
    // Apply theme immediately if auto-apply is enabled
    if (autoApplyTheme_) {
        applyThemeToComponent(component);
    }
    
    std::cout << "Registered " << componentType << " component for theme management" << std::endl;
}

void UIThemeManager::unregisterComponent(UIComponent* component) {
    if (!component) return;
    
    auto it = std::remove_if(registeredComponents_.begin(), registeredComponents_.end(),
        [component](const ComponentInfo& info) { return info.component == component; });
    
    if (it != registeredComponents_.end()) {
        registeredComponents_.erase(it, registeredComponents_.end());
        std::cout << "Unregistered component from theme management" << std::endl;
    }
}

void UIThemeManager::registerUIManager(UIManager* uiManager) {
    if (!uiManager) return;
    
    auto it = std::find(registeredManagers_.begin(), registeredManagers_.end(), uiManager);
    if (it == registeredManagers_.end()) {
        registeredManagers_.push_back(uiManager);
        std::cout << "Registered UIManager for theme management" << std::endl;
    }
}

void UIThemeManager::unregisterUIManager(UIManager* uiManager) {
    auto it = std::find(registeredManagers_.begin(), registeredManagers_.end(), uiManager);
    if (it != registeredManagers_.end()) {
        registeredManagers_.erase(it);
        std::cout << "Unregistered UIManager from theme management" << std::endl;
    }
}

void UIThemeManager::applyThemeToComponent(UIComponent* component) {
    if (!component) return;
    
    // Clean up invalid components first
    cleanupInvalidComponents();
    
    // Find the component in registered list
    auto it = std::find_if(registeredComponents_.begin(), registeredComponents_.end(),
        [component](const ComponentInfo& info) { return info.component == component; });
    
    if (it != registeredComponents_.end()) {
        const std::string& componentType = it->type;
        
        // Delegate to UITheme for actual theme application
        UITheme& theme = UITheme::getInstance();
        theme.applyThemeToComponent(componentType, component);
        
        std::cout << "Applied theme to " << componentType << std::endl;
    } else {
        // Try to determine component type automatically
        std::string autoType = getComponentTypeName(component);
        if (!autoType.empty()) {
            registerComponent(autoType, component);
            applyThemeToComponent(component);
        } else {
            std::cerr << "Warning: Unknown component type for theme application" << std::endl;
        }
    }
}

void UIThemeManager::applyThemeToAllRegisteredComponents() {
    cleanupInvalidComponents();
    
    std::cout << "Applying theme to " << registeredComponents_.size() << " registered components" << std::endl;
    
    for (const auto& componentInfo : registeredComponents_) {
        if (componentInfo.isValid()) {
            applyThemeToComponent(componentInfo.component);
        }
    }
}

void UIThemeManager::updateAllComponentThemes() {
    applyThemeToAllRegisteredComponents();
}

void UIThemeManager::onThemeChanged(const UIThemeData& newTheme) {
    std::cout << "Theme changed notification received: " << newTheme.name << std::endl;
    
    if (autoApplyTheme_) {
        applyThemeToAllRegisteredComponents();
    }
    
    // Update UIConstants if needed
    if (initialized_) {
        updateUIConstantsFromTheme(newTheme);
    }
}

void UIThemeManager::syncThemeWithUIConstants() {
    // This function would sync current theme with UIConstants values
    // For now, it's a placeholder since UIConstants are constexpr
    std::cout << "Syncing theme with UIConstants (read-only integration)" << std::endl;
}

void UIThemeManager::updateUIConstantsFromTheme(const UIThemeData& theme) {
    // UIConstants are constexpr, so we can't modify them at runtime
    // This function serves as a placeholder for future file-based configuration
    std::cout << "Theme values would update external configuration (UIConstants are constexpr)" << std::endl;
    
    // Log current theme values for reference
    std::cout << "Current theme colors:" << std::endl;
    std::cout << "  Background: RGB(" << (int)theme.colors.background.r << "," 
              << (int)theme.colors.background.g << "," << (int)theme.colors.background.b << ")" << std::endl;
    std::cout << "  Text: RGB(" << (int)theme.colors.text.r << "," 
              << (int)theme.colors.text.g << "," << (int)theme.colors.text.b << ")" << std::endl;
    std::cout << "  Button: RGB(" << (int)theme.colors.buttonBackground.r << "," 
              << (int)theme.colors.buttonBackground.g << "," << (int)theme.colors.buttonBackground.b << ")" << std::endl;
}

void UIThemeManager::cleanupInvalidComponents() {
    // Remove null or invalid component pointers
    auto it = std::remove_if(registeredComponents_.begin(), registeredComponents_.end(),
        [](const ComponentInfo& info) { return !info.isValid(); });
    
    if (it != registeredComponents_.end()) {
        size_t removed = std::distance(it, registeredComponents_.end());
        registeredComponents_.erase(it, registeredComponents_.end());
        if (removed > 0) {
            std::cout << "Cleaned up " << removed << " invalid component references" << std::endl;
        }
    }
}

std::string UIThemeManager::getComponentTypeName(UIComponent* component) {
    // Try to determine component type using dynamic casting
    // This is a simple implementation - in production you might use RTTI or a type registry
    
    if (dynamic_cast<UIButton*>(component)) return "UIButton";
    if (dynamic_cast<UILabel*>(component)) return "UILabel";
    if (dynamic_cast<UITextInput*>(component)) return "UITextInput";
    if (dynamic_cast<UIProgressBar*>(component)) return "UIProgressBar";
    
    return ""; // Unknown type
}

void UIThemeManager::printRegisteredComponents() const {
    std::cout << "Registered components (" << registeredComponents_.size() << "):" << std::endl;
    for (size_t i = 0; i < registeredComponents_.size(); ++i) {
        const auto& info = registeredComponents_[i];
        std::cout << "  [" << i << "] " << info.type 
                  << " @ " << info.component 
                  << (info.isValid() ? " (valid)" : " (invalid)") << std::endl;
    }
    
    std::cout << "Registered UIManagers (" << registeredManagers_.size() << "):" << std::endl;
    for (size_t i = 0; i < registeredManagers_.size(); ++i) {
        std::cout << "  [" << i << "] UIManager @ " << registeredManagers_[i] << std::endl;
    }
}
