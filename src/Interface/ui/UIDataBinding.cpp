#include "Interface/ui/UIDataBinding.h"
#include "Interface/ui/UIComponent.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UITextInput.h"
#include <algorithm>
#include <iostream>

void UIDataBinding::unbindComponent(std::shared_ptr<UIComponent> component) {
    // Remove all bindings associated with this component
    bindings_.erase(
        std::remove_if(bindings_.begin(), bindings_.end(),
            [&component](const Binding& binding) {
                auto boundComponent = binding.component.lock();
                return !boundComponent || boundComponent == component;
            }),
        bindings_.end()
    );
}

void UIDataBinding::unbindAll() {
    bindings_.clear();
}

void UIDataBinding::updateBindings() {
    cleanupInvalidBindings();
    
    // Execute all valid binding update functions
    for (const auto& binding : bindings_) {
        if (isComponentValid(binding.component)) {
            try {
                binding.updateFunction();
            } catch (const std::exception& e) {
                std::cerr << "Error updating binding for property '" 
                          << binding.propertyName << "': " << e.what() << std::endl;
            }
        }
    }
}

void UIDataBinding::bindText(std::shared_ptr<UIComponent> component, 
                            BindableProperty<std::string>& textProperty) {
    // Create update function that sets component text
    auto updateFunction = [component, &textProperty]() {
        // Try to cast to different UI component types that support text
        if (auto label = std::dynamic_pointer_cast<UILabel>(component)) {
            label->setText(textProperty.get());
        } else if (auto button = std::dynamic_pointer_cast<UIButton>(component)) {
            button->setText(textProperty.get());
        } else if (auto textInput = std::dynamic_pointer_cast<UITextInput>(component)) {
            textInput->setText(textProperty.get());
        }
    };
    
    // Subscribe to text property changes
    textProperty.subscribe([updateFunction](const std::string& oldValue, const std::string& newValue) {
        updateFunction();
    });
    
    // Store binding
    bindings_.emplace_back(component, "text", updateFunction, false);
    
    // Initial update
    updateFunction();
}

void UIDataBinding::bindVisibility(std::shared_ptr<UIComponent> component, 
                                  BindableProperty<bool>& visibilityProperty) {
    auto updateFunction = [component, &visibilityProperty]() {
        if (component) {
            component->setVisible(visibilityProperty.get());
        }
    };
    
    visibilityProperty.subscribe([updateFunction](const bool& oldValue, const bool& newValue) {
        updateFunction();
    });
    
    bindings_.emplace_back(component, "visible", updateFunction, false);
    updateFunction();
}

void UIDataBinding::bindEnabled(std::shared_ptr<UIComponent> component, 
                               BindableProperty<bool>& enabledProperty) {
    auto updateFunction = [component, &enabledProperty]() {
        if (component) {
            component->setEnabled(enabledProperty.get());
        }
    };
    
    enabledProperty.subscribe([updateFunction](const bool& oldValue, const bool& newValue) {
        updateFunction();
    });
    
    bindings_.emplace_back(component, "enabled", updateFunction, false);
    updateFunction();
}

void UIDataBinding::bindPosition(std::shared_ptr<UIComponent> component, 
                                BindableProperty<int>& xProperty, 
                                BindableProperty<int>& yProperty) {
    auto updateFunction = [component, &xProperty, &yProperty]() {
        if (component) {
            component->setPosition(xProperty.get(), yProperty.get());
        }
    };
    
    // Subscribe to both x and y changes
    xProperty.subscribe([updateFunction](const int& oldValue, const int& newValue) {
        updateFunction();
    });
    
    yProperty.subscribe([updateFunction](const int& oldValue, const int& newValue) {
        updateFunction();
    });
    
    bindings_.emplace_back(component, "position", updateFunction, false);
    updateFunction();
}

void UIDataBinding::cleanupInvalidBindings() {
    bindings_.erase(
        std::remove_if(bindings_.begin(), bindings_.end(),
            [this](const Binding& binding) {
                return !isComponentValid(binding.component);
            }),
        bindings_.end()
    );
}

bool UIDataBinding::isComponentValid(const std::weak_ptr<UIComponent>& component) const {
    return !component.expired();
}
