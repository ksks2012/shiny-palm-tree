#pragma once
#include <vector>
#include <memory>
#include "UIComponent.h"

/**
 * Abstract base class for all layout managers
 * Defines the interface for positioning and sizing child components
 */
class Layout {
public:
    virtual ~Layout() = default;
    
    /**
     * Calculate and apply layout to all child components
     * @param children Vector of child components to layout
     * @param containerX X position of the container
     * @param containerY Y position of the container
     * @param containerWidth Width of the container
     * @param containerHeight Height of the container
     */
    virtual void layoutChildren(
        std::vector<std::shared_ptr<UIComponent>>& children,
        int containerX, int containerY, 
        int containerWidth, int containerHeight
    ) = 0;
    
    /**
     * Calculate the preferred size needed to contain all children
     * @param children Vector of child components
     * @return Pair of (width, height) required
     */
    virtual std::pair<int, int> calculatePreferredSize(
        const std::vector<std::shared_ptr<UIComponent>>& children
    ) const = 0;
};

/**
 * Vertical layout manager - arranges components vertically from top to bottom
 * This is the existing layout used by UIContainer
 */
class VerticalLayout : public Layout {
private:
    int spacing_ = 0; // Space between components
    
public:
    explicit VerticalLayout(int spacing = 0) : spacing_(spacing) {}
    
    void setSpacing(int spacing) { spacing_ = spacing; }
    int getSpacing() const { return spacing_; }
    
    void layoutChildren(
        std::vector<std::shared_ptr<UIComponent>>& children,
        int containerX, int containerY, 
        int containerWidth, int containerHeight
    ) override;
    
    std::pair<int, int> calculatePreferredSize(
        const std::vector<std::shared_ptr<UIComponent>>& children
    ) const override;
};

/**
 * Horizontal layout manager - arranges components horizontally from left to right
 */
class HorizontalLayout : public Layout {
private:
    int spacing_ = 0; // Space between components
    
public:
    explicit HorizontalLayout(int spacing = 0) : spacing_(spacing) {}
    
    void setSpacing(int spacing) { spacing_ = spacing; }
    int getSpacing() const { return spacing_; }
    
    void layoutChildren(
        std::vector<std::shared_ptr<UIComponent>>& children,
        int containerX, int containerY, 
        int containerWidth, int containerHeight
    ) override;
    
    std::pair<int, int> calculatePreferredSize(
        const std::vector<std::shared_ptr<UIComponent>>& children
    ) const override;
};

/**
 * Grid layout manager - arranges components in a grid pattern
 */
class GridLayout : public Layout {
private:
    int rows_;
    int cols_;
    int spacing_ = 0; // Space between grid cells
    
public:
    GridLayout(int rows, int cols, int spacing = 0) 
        : rows_(rows), cols_(cols), spacing_(spacing) {}
    
    void setGrid(int rows, int cols) { rows_ = rows; cols_ = cols; }
    void setSpacing(int spacing) { spacing_ = spacing; }
    
    int getRows() const { return rows_; }
    int getCols() const { return cols_; }
    int getSpacing() const { return spacing_; }
    
    void layoutChildren(
        std::vector<std::shared_ptr<UIComponent>>& children,
        int containerX, int containerY, 
        int containerWidth, int containerHeight
    ) override;
    
    std::pair<int, int> calculatePreferredSize(
        const std::vector<std::shared_ptr<UIComponent>>& children
    ) const override;
};

/**
 * Border layout manager - arranges components in border regions (North, South, East, West, Center)
 */
enum class BorderPosition {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    CENTER
};

class BorderLayout : public Layout {
private:
    struct BorderComponent {
        std::shared_ptr<UIComponent> component;
        BorderPosition position;
    };
    
    std::vector<BorderComponent> borderComponents_;
    int gap_ = 0; // Gap between regions
    
public:
    explicit BorderLayout(int gap = 0) : gap_(gap) {}
    
    void setGap(int gap) { gap_ = gap; }
    int getGap() const { return gap_; }
    
    /**
     * Add component to specific border position
     * Note: This is different from other layouts as it needs position info
     */
    void addComponent(std::shared_ptr<UIComponent> component, BorderPosition position);
    void removeComponent(std::shared_ptr<UIComponent> component);
    void clearComponents() { borderComponents_.clear(); }
    
    void layoutChildren(
        std::vector<std::shared_ptr<UIComponent>>& children,
        int containerX, int containerY, 
        int containerWidth, int containerHeight
    ) override;
    
    std::pair<int, int> calculatePreferredSize(
        const std::vector<std::shared_ptr<UIComponent>>& children
    ) const override;
};

/**
 * Flex layout manager - flexible layout with grow/shrink capabilities
 */
enum class FlexDirection {
    ROW,         // Horizontal, left to right
    ROW_REVERSE, // Horizontal, right to left
    COLUMN,      // Vertical, top to bottom
    COLUMN_REVERSE // Vertical, bottom to top
};

enum class FlexJustify {
    FLEX_START,    // Pack items at start
    FLEX_END,      // Pack items at end
    CENTER,        // Center items
    SPACE_BETWEEN, // Distribute evenly with space between
    SPACE_AROUND,  // Distribute evenly with space around
    SPACE_EVENLY   // Distribute evenly with equal space
};

enum class FlexAlign {
    FLEX_START,  // Align at start of cross axis
    FLEX_END,    // Align at end of cross axis
    CENTER,      // Center on cross axis
    STRETCH      // Stretch to fill cross axis
};

class FlexLayout : public Layout {
private:
    FlexDirection direction_ = FlexDirection::ROW;
    FlexJustify justifyContent_ = FlexJustify::FLEX_START;
    FlexAlign alignItems_ = FlexAlign::STRETCH;
    int gap_ = 0;
    
public:
    explicit FlexLayout(FlexDirection direction = FlexDirection::ROW) 
        : direction_(direction) {}
    
    void setDirection(FlexDirection direction) { direction_ = direction; }
    void setJustifyContent(FlexJustify justify) { justifyContent_ = justify; }
    void setAlignItems(FlexAlign align) { alignItems_ = align; }
    void setGap(int gap) { gap_ = gap; }
    
    FlexDirection getDirection() const { return direction_; }
    FlexJustify getJustifyContent() const { return justifyContent_; }
    FlexAlign getAlignItems() const { return alignItems_; }
    int getGap() const { return gap_; }
    
    void layoutChildren(
        std::vector<std::shared_ptr<UIComponent>>& children,
        int containerX, int containerY, 
        int containerWidth, int containerHeight
    ) override;
    
    std::pair<int, int> calculatePreferredSize(
        const std::vector<std::shared_ptr<UIComponent>>& children
    ) const override;
};

/**
 * Absolute layout manager - components positioned with absolute coordinates
 */
struct AbsolutePosition {
    int x, y;
    int width = -1;  // -1 means use component's preferred width
    int height = -1; // -1 means use component's preferred height
};

class AbsoluteLayout : public Layout {
private:
    struct AbsoluteComponent {
        std::shared_ptr<UIComponent> component;
        AbsolutePosition position;
    };
    
    std::vector<AbsoluteComponent> absoluteComponents_;
    
public:
    AbsoluteLayout() = default;
    
    /**
     * Set absolute position for a component
     */
    void setComponentPosition(std::shared_ptr<UIComponent> component, const AbsolutePosition& position);
    void removeComponent(std::shared_ptr<UIComponent> component);
    void clearComponents() { absoluteComponents_.clear(); }
    
    void layoutChildren(
        std::vector<std::shared_ptr<UIComponent>>& children,
        int containerX, int containerY, 
        int containerWidth, int containerHeight
    ) override;
    
    std::pair<int, int> calculatePreferredSize(
        const std::vector<std::shared_ptr<UIComponent>>& children
    ) const override;
};
