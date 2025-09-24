#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "UIComponent.h"
#include "Layout.h"

// Forward declaration
class SDLManager;

/**
 * SimpleContainer: Clean replacement for inheritance hell
 * 
 * Instead of:  UIComponent -> UILayoutContainer -> UIContainer
 * We have:     UIComponent -> SimpleContainer (composition over inheritance)
 * 
 * This follows "good taste" principle:
 * - No dynamic_cast needed
 * - Composition over inheritance  
 * - Function-based layouts
 * - Clean, predictable interface
 */
class SimpleContainer : public UIComponent {
private:
    std::vector<std::shared_ptr<UIComponent>> children_;
    std::unique_ptr<Layout> layout_;
    bool scrollable_ = false;
    int scrollOffset_ = 0;

    bool isChildVisible(const std::shared_ptr<UIComponent>& child) const;

public:
    SimpleContainer(int x, int y, int width, int height, SDLManager& sdl);

    // === Child Management (composition-based) ===
    void addChild(std::shared_ptr<UIComponent> child);
    void removeChild(std::shared_ptr<UIComponent> child);
    void clearChildren();

    // === Layout Management (function-based) ===
    void setVerticalLayout(int spacing = 5);
    void setHorizontalLayout(int spacing = 5);
    void setGridLayout(int rows, int cols, int spacing = 5);
    void setCustomLayout(std::unique_ptr<Layout> layout);

    // === Scrolling Support ===
    void setScrollable(bool scrollable);
    bool isScrollable() const { return scrollable_; }
    void setScrollOffset(int offset);
    int getScrollOffset() const { return scrollOffset_; }
    int getMaxScroll() const;

    // === Core UIComponent Interface ===
    void layout() override;
    void render() override;
    void handleEvent(const SDL_Event& event) override;
    std::shared_ptr<UIComponent> hitTest(int x, int y) const;

    // === Query Interface ===
    std::shared_ptr<UIComponent> getChild(int index) const;
    int getChildCount() const { return static_cast<int>(children_.size()); }
    std::pair<int, int> calculatePreferredSize() const;
    void autoResizeToFitChildren();
    int getVisibleCount() const;
};
