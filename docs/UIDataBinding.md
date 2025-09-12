# UIDataBinding - Data Binding System

The UIDataBinding system provides automatic synchronization between UI components and data models, making it easy to create reactive user interfaces.

## Key Components

### BindableProperty<T>
A template class that wraps a value and notifies observers when it changes.

```cpp
// Create bindable properties
BindableProperty<std::string> playerName{"John Doe"};
BindableProperty<int> playerLevel{1};
BindableProperty<bool> isOnline{true};

// Subscribe to changes
playerName.subscribe([](const std::string& oldValue, const std::string& newValue) {
    std::cout << "Player name changed from " << oldValue << " to " << newValue << std::endl;
});

// Update values (triggers notification)
playerName.set("Jane Smith");
playerLevel = 5; // Using operator overload
```

### UIDataBinding
The main binding manager that connects UI components to data properties.

```cpp
UIDataBinding binding;

// Bind text to UI components
binding.bindText(myLabel, playerName);
binding.bindVisibility(myButton, isOnline);
binding.bindEnabled(myInput, canEdit);
```

## Binding Types

### One-way Binding (Data → UI)
```cpp
// When playerName changes, label text updates automatically
binding.bindText(nameLabel, playerName);
```

### Custom Binding
```cpp
// Custom update logic
binding.bindCustom<int>(
    healthBar,
    [healthBar](const int& health) {
        healthBar->setWidth(health * 2); // Custom rendering logic
    },
    []() { return playerHealth.get(); },
    playerHealth
);
```

### Position Binding
```cpp
// Bind component position to data
binding.bindPosition(movingComponent, xProperty, yProperty);
```

## Usage Examples

### Basic Player UI
```cpp
struct PlayerData {
    BindableProperty<std::string> name{"Player"};
    BindableProperty<int> health{100};
    BindableProperty<bool> isAlive{true};
};

PlayerData player;
UIDataBinding binding;

auto nameLabel = std::make_shared<UILabel>(0, 0, 200, 30, sdl);
auto healthLabel = std::make_shared<UILabel>(0, 30, 200, 30, sdl);

// Bind UI to data
binding.bindText(nameLabel, player.name);
binding.bindCustom<int>(
    healthLabel,
    [healthLabel](const int& health) {
        healthLabel->setText("Health: " + std::to_string(health));
    },
    []() { return player.health.get(); },
    player.health
);

// Changes automatically update UI
player.name.set("Hero");
player.health.set(75);
binding.updateBindings(); // Apply all updates
```

### Settings Panel
```cpp
struct GameSettings {
    BindableProperty<bool> soundEnabled{true};
    BindableProperty<int> volume{50};
    BindableProperty<std::string> difficulty{"Normal"};
};

GameSettings settings;
UIDataBinding binding;

auto soundButton = std::make_shared<UIButton>("Sound: ON", 0, 0, 150, 40, sdl, 
    [&settings]() {
        settings.soundEnabled.set(!settings.soundEnabled.get());
    });

// Button text updates when setting changes
binding.bindCustom<bool>(
    soundButton,
    [soundButton](const bool& enabled) {
        soundButton->setText("Sound: " + std::string(enabled ? "ON" : "OFF"));
    },
    []() { return settings.soundEnabled.get(); },
    settings.soundEnabled
);
```

## RAII Management

Use `DataBindingScope` for automatic cleanup:

```cpp
void createUI() {
    DataBindingScope scope(binding);
    
    // Setup bindings...
    // Automatically cleaned up when scope ends
}
```

## Convenience Macros

```cpp
// Shorthand for common bindings
BIND_TEXT(binding, myLabel, textProperty);
BIND_VISIBILITY(binding, myButton, visibleProperty);
BIND_ENABLED(binding, myInput, enabledProperty);
```

## Best Practices

1. **Use RAII**: Always manage binding lifecycle with `DataBindingScope`
2. **Batch Updates**: Call `updateBindings()` after multiple data changes
3. **Weak References**: Bindings automatically handle component destruction
4. **Custom Logic**: Use `bindCustom` for complex update behaviors
5. **Type Safety**: BindableProperty is strongly typed for compile-time safety

## Integration with Layouts

Data binding works seamlessly with all layout managers:

```cpp
auto container = std::make_shared<UILayoutContainer>(0, 0, 400, 300, sdl);
container->setVerticalLayout(10);

// Add bindable components
container->addChild(nameLabel);
container->addChild(healthBar);

// Bind data
binding.bindText(nameLabel, player.name);
binding.bindCustom<int>(healthBar, updateHealthBar, getHealth, player.health);

container->layout();
```

The data binding system provides a clean, type-safe way to keep your UI synchronized with your application's data model.
