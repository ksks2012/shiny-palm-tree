#pragma once
#include <functional>
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
    
    // fix: Make all layouts handle component removal uniformly
    virtual void removeComponent(std::shared_ptr<UIComponent> component) {}
    virtual void clearComponents() {}
};

// Phase 2 Refactor: Function-based layout system
// Replace 6 layout classes with simple functions

/**
 * Function-based layout system
 * Eventually will replace all layout classes
 */
using LayoutFunction = std::function<void(
    std::vector<std::shared_ptr<UIComponent>>&,
    int containerX, int containerY,
    int containerWidth, int containerHeight
)>;

/**
 * Transitional Layout class
 * Wraps functions but maintains old interface for backward compatibility
 */
class FunctionLayout : public Layout {
private:
    LayoutFunction layout_func_;
    std::function<std::pair<int,int>(const std::vector<std::shared_ptr<UIComponent>>&)> size_func_;
    
public:
    FunctionLayout(LayoutFunction layout_func, 
                   std::function<std::pair<int,int>(const std::vector<std::shared_ptr<UIComponent>>&)> size_func)
        : layout_func_(layout_func), size_func_(size_func) {}
    
    void layoutChildren(
        std::vector<std::shared_ptr<UIComponent>>& children,
        int containerX, int containerY, 
        int containerWidth, int containerHeight
    ) override {
        layout_func_(children, containerX, containerY, containerWidth, containerHeight);
    }
    
    std::pair<int, int> calculatePreferredSize(
        const std::vector<std::shared_ptr<UIComponent>>& children
    ) const override {
        return size_func_(children);
    }
    
    void removeComponent(std::shared_ptr<UIComponent> component) override {
        // Default implementation - no special handling needed
    }
    
    void clearComponents() override {
        // Default implementation - no special handling needed  
    }
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

/**
 * Layout Factory - Create layout functions
 * This replaces the need for 6 separate layout classes
 * Simple, direct, no inheritance hell
 */
namespace LayoutFactory {
    
    inline std::unique_ptr<Layout> vertical(int spacing = 0) {
        auto layout_func = [spacing](
            std::vector<std::shared_ptr<UIComponent>>& children,
            int containerX, int containerY,
            int containerWidth, int containerHeight
        ) {
            int currentY = containerY;
            for (auto& child : children) {
                if (!child) continue;
                child->layout();
                child->setPosition(containerX, currentY);
                currentY += child->getHeight() + spacing;
            }
        };
        
        auto size_func = [spacing](const std::vector<std::shared_ptr<UIComponent>>& children) {
            int maxWidth = 0, totalHeight = 0;
            for (size_t i = 0; i < children.size(); ++i) {
                const auto& child = children[i];
                if (!child) continue;
                maxWidth = std::max(maxWidth, child->getWidth());
                totalHeight += child->getHeight();
                if (i < children.size() - 1) totalHeight += spacing;
            }
            return std::make_pair(maxWidth, totalHeight);
        };
        
        return std::make_unique<FunctionLayout>(layout_func, size_func);
    }
    
    inline std::unique_ptr<Layout> horizontal(int spacing = 0) {
        auto layout_func = [spacing](
            std::vector<std::shared_ptr<UIComponent>>& children,
            int containerX, int containerY,
            int containerWidth, int containerHeight
        ) {
            int currentX = containerX;
            for (auto& child : children) {
                if (!child) continue;
                child->layout();
                child->setPosition(currentX, containerY);
                currentX += child->getWidth() + spacing;
            }
        };
        
        auto size_func = [spacing](const std::vector<std::shared_ptr<UIComponent>>& children) {
            int totalWidth = 0, maxHeight = 0;
            for (size_t i = 0; i < children.size(); ++i) {
                const auto& child = children[i];
                if (!child) continue;
                totalWidth += child->getWidth();
                maxHeight = std::max(maxHeight, child->getHeight());
                if (i < children.size() - 1) totalWidth += spacing;
            }
            return std::make_pair(totalWidth, maxHeight);
        };
        
        return std::make_unique<FunctionLayout>(layout_func, size_func);
    }
    
    // Grid layout for the rare cases where it's actually needed
    inline std::unique_ptr<Layout> grid(int rows, int cols, int spacing = 0) {
        auto layout_func = [rows, cols, spacing](
            std::vector<std::shared_ptr<UIComponent>>& children,
            int containerX, int containerY,
            int containerWidth, int containerHeight
        ) {
            if (children.empty() || rows <= 0 || cols <= 0) return;
            
            int totalSpacingW = (cols - 1) * spacing;
            int totalSpacingH = (rows - 1) * spacing;
            int cellWidth = (containerWidth - totalSpacingW) / cols;
            int cellHeight = (containerHeight - totalSpacingH) / rows;
            
            int index = 0;
            for (int row = 0; row < rows && index < static_cast<int>(children.size()); ++row) {
                for (int col = 0; col < cols && index < static_cast<int>(children.size()); ++col) {
                    auto& child = children[index];
                    if (!child) {
                        ++index;
                        continue;
                    }
                    
                    child->layout();
                    int x = containerX + col * (cellWidth + spacing);
                    int y = containerY + row * (cellHeight + spacing);
                    child->setPosition(x, y);
                    child->setSize(cellWidth, cellHeight);
                    
                    ++index;
                }
            }
        };
        
        auto size_func = [rows, cols, spacing](const std::vector<std::shared_ptr<UIComponent>>& children) {
            if (children.empty() || rows <= 0 || cols <= 0) return std::make_pair(0, 0);
            
            int maxChildWidth = 0, maxChildHeight = 0;
            for (const auto& child : children) {
                if (!child) continue;
                maxChildWidth = std::max(maxChildWidth, child->getWidth());
                maxChildHeight = std::max(maxChildHeight, child->getHeight());
            }
            
            int totalWidth = cols * maxChildWidth + (cols - 1) * spacing;
            int totalHeight = rows * maxChildHeight + (rows - 1) * spacing;
            return std::make_pair(totalWidth, totalHeight);
        };
        
        return std::make_unique<FunctionLayout>(layout_func, size_func);
    }
    
    // Most games only need vertical/horizontal anyway
    // BorderLayout, FlexLayout, AbsoluteLayout are usually overkill
}
