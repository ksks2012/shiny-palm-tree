#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "UIComponent.h"
#include "Layout.h"

/**
 * @deprecated UILayoutContainer is deprecated. Use SimpleContainer instead.
 * 
 * Enhanced UIContainer with pluggable layout managers
 * Supports vertical, horizontal, grid, border, flex, and absolute layouts
 * Maintains backward compatibility with the original vertical layout
 * 
 * MIGRATION: Replace UILayoutContainer with SimpleContainer:
 * - UILayoutContainer(x, y, w, h, sdl) -> SimpleContainer(x, y, w, h, sdl)  
 * - setVerticalLayout() -> setVerticalLayout() (same interface)
 * - setHorizontalLayout() -> setHorizontalLayout() (same interface)
 * - setGridLayout() -> setGridLayout() (same interface)
 * - Enable scrolling with setScrollable(true) if needed
 */
class UILayoutContainer : public UIComponent {
public:
    UILayoutContainer(int x, int y, int width, int height, SDLManager& sdl);
    ~UILayoutContainer() override = default;

    // Child management
    void addChild(std::shared_ptr<UIComponent> child);
    void removeChild(std::shared_ptr<UIComponent> child);
    void clearChildren();
    
    // Layout management
    void setLayout(std::unique_ptr<Layout> layout);
    Layout* getLayout() const { return layout_.get(); }
    
    // Convenience methods for common layouts
    void setVerticalLayout(int spacing = 0);
    void setHorizontalLayout(int spacing = 0);
    void setGridLayout(int rows, int cols, int spacing = 0);
    void setBorderLayout(int gap = 0);
    void setFlexLayout(FlexDirection direction = FlexDirection::ROW);
    void setAbsoluteLayout();
    
    // Special methods for BorderLayout
    void addBorderComponent(std::shared_ptr<UIComponent> component, BorderPosition position);
    
    // Special methods for AbsoluteLayout  
    void setAbsolutePosition(std::shared_ptr<UIComponent> component, const AbsolutePosition& position);
    
    // Scrolling support (similar to original UIContainer)
    void setScrollOffset(int offset);
    int getScrollOffset() const { return scrollOffset_; }
    virtual int getMaxScroll() const;
    
    // Enable/disable scrolling
    void setScrollable(bool scrollable) { scrollable_ = scrollable; }
    bool isScrollable() const { return scrollable_; }

    // Layout & render separation
    void layout() override;
    void render() override;
    void handleEvent(const SDL_Event& event) override;

    // Hit test takes scroll and layout into account
    std::shared_ptr<UIComponent> hitTest(int x, int y) const;

    // Utility methods
    int getChildCount() const { return static_cast<int>(children_.size()); }
    std::shared_ptr<UIComponent> getChild(int index) const;
    
    // Auto-sizing based on layout
    void autoResizeToFitChildren();
    std::pair<int, int> calculatePreferredSize() const;

protected:
    std::vector<std::shared_ptr<UIComponent>> children_;
    std::unique_ptr<Layout> layout_;
    int scrollOffset_ = 0;
    bool scrollable_ = true; // Enable scrolling by default for backward compatibility
    
    // Helper methods
    bool isChildVisible(const std::shared_ptr<UIComponent>& child) const;
    void updateScrollBounds();

private:
    // Currently no private members needed
};
