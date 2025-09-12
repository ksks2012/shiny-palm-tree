#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

/**
 * Generic data binding system for UI components.
 * Enables automatic synchronization between UI elements and data models.
 * Supports bidirectional binding and property change notifications.
 */

// Forward declarations
class UIComponent;

/**
 * Base class for bindable properties that can notify observers of changes
 */
template<typename T>
class BindableProperty {
public:
    using ChangeCallback = std::function<void(const T& oldValue, const T& newValue)>;
    
    BindableProperty() = default;
    explicit BindableProperty(const T& initialValue) : value_(initialValue) {}
    
    // Get current value
    const T& get() const { return value_; }
    
    // Set new value and notify observers
    void set(const T& newValue) {
        T oldValue = value_;
        if (oldValue != newValue) {
            value_ = newValue;
            notifyObservers(oldValue, newValue);
        }
    }
    
    // Subscribe to value changes
    void subscribe(const ChangeCallback& callback) {
        observers_.push_back(callback);
    }
    
    // Unsubscribe all observers
    void clearObservers() {
        observers_.clear();
    }
    
    // Operator overloads for convenience
    BindableProperty& operator=(const T& value) {
        set(value);
        return *this;
    }
    
    operator const T&() const {
        return get();
    }

private:
    T value_{};
    std::vector<ChangeCallback> observers_;
    
    void notifyObservers(const T& oldValue, const T& newValue) {
        for (const auto& observer : observers_) {
            observer(oldValue, newValue);
        }
    }
};

/**
 * Data binding context that manages bindings between UI components and data properties
 */
class UIDataBinding {
public:
    UIDataBinding() = default;
    ~UIDataBinding() = default;
    
    // Bind UI component property to a bindable data property (one-way: data -> UI)
    template<typename T>
    void bindOneWay(std::shared_ptr<UIComponent> component, 
                    const std::string& componentProperty,
                    BindableProperty<T>& dataProperty);
    
    // Bind UI component property to a bindable data property (two-way: data <-> UI)
    template<typename T>
    void bindTwoWay(std::shared_ptr<UIComponent> component,
                    const std::string& componentProperty,
                    BindableProperty<T>& dataProperty);
    
    // Bind to custom setter/getter functions
    template<typename T>
    void bindCustom(std::shared_ptr<UIComponent> component,
                    std::function<void(const T&)> setter,
                    std::function<T()> getter,
                    BindableProperty<T>& dataProperty);
    
    // Remove all bindings for a component
    void unbindComponent(std::shared_ptr<UIComponent> component);
    
    // Remove all bindings
    void unbindAll();
    
    // Update all bindings (call this periodically or when data changes)
    void updateBindings();
    
    // Utility methods for common UI property bindings
    void bindText(std::shared_ptr<UIComponent> component, BindableProperty<std::string>& textProperty);
    void bindVisibility(std::shared_ptr<UIComponent> component, BindableProperty<bool>& visibilityProperty);
    void bindEnabled(std::shared_ptr<UIComponent> component, BindableProperty<bool>& enabledProperty);
    void bindPosition(std::shared_ptr<UIComponent> component, BindableProperty<int>& xProperty, BindableProperty<int>& yProperty);

private:
    // Internal binding representation
    struct Binding {
        std::weak_ptr<UIComponent> component;
        std::string propertyName;
        std::function<void()> updateFunction;
        bool isTwoWay;
        
        Binding(std::shared_ptr<UIComponent> comp, const std::string& prop, 
                std::function<void()> update, bool twoWay = false)
            : component(comp), propertyName(prop), updateFunction(update), isTwoWay(twoWay) {}
    };
    
    std::vector<Binding> bindings_;
    
    // Helper methods
    void cleanupInvalidBindings();
    bool isComponentValid(const std::weak_ptr<UIComponent>& component) const;
};

/**
 * RAII wrapper for managing data binding lifecycle
 */
class DataBindingScope {
public:
    explicit DataBindingScope(UIDataBinding& binding) : binding_(binding) {}
    
    ~DataBindingScope() {
        binding_.unbindAll();
    }
    
    UIDataBinding& getBinding() { return binding_; }

private:
    UIDataBinding& binding_;
};

// Convenience macros for common binding patterns
#define BIND_TEXT(binding, component, property) \
    (binding).bindText(component, property)

#define BIND_VISIBILITY(binding, component, property) \
    (binding).bindVisibility(component, property)

#define BIND_ENABLED(binding, component, property) \
    (binding).bindEnabled(component, property)

// Template implementations
template<typename T>
void UIDataBinding::bindOneWay(std::shared_ptr<UIComponent> component,
                               const std::string& componentProperty,
                               BindableProperty<T>& dataProperty) {
    // Create update function that will be called when data changes
    auto updateFunction = [component, componentProperty, &dataProperty]() {
        if (component) {
            // This will be handled by specific implementations like bindText, bindVisibility etc.
            // For now, this is a placeholder for generic property binding
        }
    };
    
    // Subscribe to data property changes
    dataProperty.subscribe([updateFunction](const T& oldValue, const T& newValue) {
        updateFunction();
    });
    
    // Store binding for management
    bindings_.emplace_back(component, componentProperty, updateFunction, false);
    
    // Initial update
    updateFunction();
}

template<typename T>
void UIDataBinding::bindTwoWay(std::shared_ptr<UIComponent> component,
                               const std::string& componentProperty,
                               BindableProperty<T>& dataProperty) {
    // First establish one-way binding
    bindOneWay(component, componentProperty, dataProperty);
    
    // Mark the last binding as two-way
    if (!bindings_.empty()) {
        bindings_.back().isTwoWay = true;
    }
    
    // TODO: Add component -> data binding when component property changes
    // This would require components to support property change notifications
}

template<typename T>
void UIDataBinding::bindCustom(std::shared_ptr<UIComponent> component,
                               std::function<void(const T&)> setter,
                               std::function<T()> getter,
                               BindableProperty<T>& dataProperty) {
    // Create custom update function
    auto updateFunction = [setter, &dataProperty]() {
        setter(dataProperty.get());
    };
    
    // Subscribe to data changes
    dataProperty.subscribe([updateFunction](const T& oldValue, const T& newValue) {
        updateFunction();
    });
    
    // Store binding
    bindings_.emplace_back(component, "custom", updateFunction, false);
    
    // Initial update
    updateFunction();
}
