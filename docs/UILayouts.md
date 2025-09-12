# UI Framework Layout System

The UI Framework now supports multiple layout managers for flexible component positioning and sizing. This document explains how to use each layout type.

## Overview

The layout system consists of:
- **Layout Interface**: Base class for all layout managers
- **UILayoutContainer**: Enhanced container with pluggable layout support  
- **UIContainer**: Legacy container (backward compatible, uses vertical layout)
- **Multiple Layout Implementations**: Various layout strategies

## Available Layouts

### 1. VerticalLayout
Arranges components vertically from top to bottom.

```cpp
auto container = std::make_shared<UILayoutContainer>(0, 0, 300, 400, sdl);
container->setVerticalLayout(10); // 10px spacing between components

// Add components - they will be arranged vertically
container->addChild(titleLabel);
container->addChild(button1);
container->addChild(textInput);
```

### 2. HorizontalLayout  
Arranges components horizontally from left to right.

```cpp
auto container = std::make_shared<UILayoutContainer>(0, 0, 500, 60, sdl);
container->setHorizontalLayout(15); // 15px spacing between components
container->setScrollable(false); // Usually disable scrolling for horizontal

// Add components - they will be arranged horizontally
container->addChild(saveButton);
container->addChild(loadButton);
container->addChild(exitButton);
```

### 3. GridLayout
Arranges components in a grid pattern with specified rows and columns.

```cpp
auto container = std::make_shared<UILayoutContainer>(0, 0, 320, 240, sdl);
container->setGridLayout(2, 3, 5); // 2 rows, 3 columns, 5px spacing
container->setScrollable(false);

// Add 6 components - they will fill the 2x3 grid
for (int i = 1; i <= 6; ++i) {
    container->addChild(createGridButton(i));
}
```

### 4. BorderLayout
Arranges components in border regions: North, South, East, West, and Center.

```cpp
auto container = std::make_shared<UILayoutContainer>(0, 0, 400, 300, sdl);
container->setBorderLayout(10); // 10px gap between regions
container->setScrollable(false);

// Add components to specific border positions
container->addBorderComponent(headerLabel, BorderPosition::NORTH);
container->addBorderComponent(footerLabel, BorderPosition::SOUTH);
container->addBorderComponent(sidebarButton, BorderPosition::WEST);
container->addBorderComponent(toolButton, BorderPosition::EAST);
container->addBorderComponent(mainContent, BorderPosition::CENTER);
```

### 5. FlexLayout
Flexible layout with CSS Flexbox-like behavior, supporting various alignment and justification options.

```cpp
auto container = std::make_shared<UILayoutContainer>(0, 0, 500, 80, sdl);
auto flexLayout = std::make_unique<FlexLayout>(FlexDirection::ROW);
flexLayout->setJustifyContent(FlexJustify::SPACE_BETWEEN);
flexLayout->setAlignItems(FlexAlign::CENTER);
flexLayout->setGap(10);
container->setLayout(std::move(flexLayout));

// Add components - they will be distributed with space between
container->addChild(smallButton);
container->addChild(mediumButton); 
container->addChild(largeButton);
```

**FlexLayout Options:**
- **Direction**: ROW, ROW_REVERSE, COLUMN, COLUMN_REVERSE
- **Justify Content**: FLEX_START, FLEX_END, CENTER, SPACE_BETWEEN, SPACE_AROUND, SPACE_EVENLY
- **Align Items**: FLEX_START, FLEX_END, CENTER, STRETCH

### 6. AbsoluteLayout
Positions components at absolute coordinates within the container.

```cpp
auto container = std::make_shared<UILayoutContainer>(0, 0, 300, 300, sdl);
container->setAbsoluteLayout();
container->setScrollable(false);

// Set absolute positions for components
container->setAbsolutePosition(titleLabel, {10, 10, 200, 30});
container->setAbsolutePosition(floatingButton1, {50, 100, 80, 30});
container->setAbsolutePosition(floatingButton2, {150, 150, 80, 30});
container->setAbsolutePosition(cornerLabel, {200, 270, 100, 20});
```

**AbsolutePosition struct:**
```cpp
struct AbsolutePosition {
    int x, y;           // Position relative to container
    int width = -1;     // -1 means use component's preferred width
    int height = -1;    // -1 means use component's preferred height
};
```

## Advanced Usage

### Custom Layout Implementation
You can create custom layouts by inheriting from the `Layout` base class:

```cpp
class CustomLayout : public Layout {
public:
    void layoutChildren(
        std::vector<std::shared_ptr<UIComponent>>& children,
        int containerX, int containerY, 
        int containerWidth, int containerHeight
    ) override {
        // Your custom layout logic here
    }
    
    std::pair<int, int> calculatePreferredSize(
        const std::vector<std::shared_ptr<UIComponent>>& children
    ) const override {
        // Calculate preferred size for your layout
        return {preferredWidth, preferredHeight};
    }
};

// Usage
auto container = std::make_shared<UILayoutContainer>(0, 0, 400, 300, sdl);
container->setLayout(std::make_unique<CustomLayout>());
```

### Nested Layouts
You can nest containers with different layouts for complex UI structures:

```cpp
// Main container with border layout
auto mainContainer = std::make_shared<UILayoutContainer>(0, 0, 800, 600, sdl);
mainContainer->setBorderLayout(5);

// Header with horizontal layout
auto header = std::make_shared<UILayoutContainer>(0, 0, 790, 50, sdl);
header->setHorizontalLayout(10);
header->addChild(logoLabel);
header->addChild(menuButton1);
header->addChild(menuButton2);

// Sidebar with vertical layout  
auto sidebar = std::make_shared<UILayoutContainer>(0, 0, 150, 500, sdl);
sidebar->setVerticalLayout(5);
sidebar->addChild(toolButton1);
sidebar->addChild(toolButton2);

// Content area with grid layout
auto content = std::make_shared<UILayoutContainer>(0, 0, 635, 500, sdl);
content->setGridLayout(2, 2, 10);
content->addChild(panel1);
content->addChild(panel2);
content->addChild(panel3);
content->addChild(panel4);

// Assemble the layout
mainContainer->addBorderComponent(header, BorderPosition::NORTH);
mainContainer->addBorderComponent(sidebar, BorderPosition::WEST);
mainContainer->addBorderComponent(content, BorderPosition::CENTER);
```

### Auto-sizing
Containers can automatically resize to fit their children:

```cpp
auto container = std::make_shared<UILayoutContainer>(0, 0, 200, 200, sdl);
container->setVerticalLayout(10);

// Add children
container->addChild(child1);
container->addChild(child2);

// Auto-resize to fit children based on layout
container->autoResizeToFitChildren();
```

### Scrolling Support
Most layouts support scrolling (currently vertical scrolling only):

```cpp
auto container = std::make_shared<UILayoutContainer>(0, 0, 300, 200, sdl);
container->setVerticalLayout(5);
container->setScrollable(true); // Default is true

// Add many children that exceed container height
for (int i = 0; i < 20; ++i) {
    container->addChild(createListItem(i));
}

// Container will automatically handle mouse wheel scrolling
```

## Backward Compatibility

The original `UIContainer` class is maintained for backward compatibility:

```cpp
// This still works exactly as before
UIContainer legacyContainer(0, 0, 300, 400, sdl);
legacyContainer.addChild(component1);
legacyContainer.addChild(component2);
// Uses vertical layout internally
```

## Best Practices

1. **Choose the Right Layout**: 
   - Use `VerticalLayout` for lists and forms
   - Use `HorizontalLayout` for toolbars and button groups
   - Use `GridLayout` for uniform item grids
   - Use `BorderLayout` for main application windows
   - Use `FlexLayout` for responsive layouts
   - Use `AbsoluteLayout` for overlays and precise positioning

2. **Disable Scrolling When Appropriate**: 
   - Horizontal, Grid, Border, Flex, and Absolute layouts often don't need scrolling
   - Use `setScrollable(false)` for these cases

3. **Use Nested Layouts**: 
   - Combine different layouts for complex UIs
   - Each container can have its own layout strategy

4. **Consider Performance**: 
   - Layout calculation happens during the layout phase
   - Avoid excessive nesting for better performance

5. **Auto-sizing**: 
   - Use `autoResizeToFitChildren()` when you want containers to size themselves
   - Use `calculatePreferredSize()` to query space requirements

## Migration Guide

To migrate from the old vertical-only system:

1. **No changes needed** if you're using `UIContainer` - it maintains full backward compatibility

2. **Switch to `UILayoutContainer`** for new features:
   ```cpp
   // Old way
   UIContainer container(0, 0, 300, 400, sdl);
   
   // New way (same behavior)
   UILayoutContainer container(0, 0, 300, 400, sdl);
   container->setVerticalLayout(); // This is the default
   ```

3. **Use new layout types** for enhanced functionality:
   ```cpp
   // Now you can do this
   container->setHorizontalLayout(10);
   container->setGridLayout(3, 3, 5);
   container->setBorderLayout(8);
   ```
