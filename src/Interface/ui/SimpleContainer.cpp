#include "Interface/ui/SimpleContainer.h"
#include "Systems/SDLManager.h"
#include <algorithm>

SimpleContainer::SimpleContainer(int x, int y, int width, int height, SDLManager& sdl)
    : UIComponent(x, y, width, height, sdl) {
    // Default to most common case - vertical layout
    layout_ = LayoutFactory::vertical();
}

std::shared_ptr<SimpleContainer> SimpleContainer::createUIContainerReplacement(
    int x, int y, int width, int height, SDLManager& sdl) {
    auto container = std::make_shared<SimpleContainer>(x, y, width, height, sdl);
    // UIContainer behavior: scrollable by default with vertical layout
    container->setScrollable(true);
    container->setAutoLayout(true); // Auto-layout for convenience
    container->setVerticalLayout(5); // Default spacing like UIContainer
    return container;
}

std::shared_ptr<SimpleContainer> SimpleContainer::createUILayoutContainerReplacement(
    int x, int y, int width, int height, SDLManager& sdl) {
    auto container = std::make_shared<SimpleContainer>(x, y, width, height, sdl);
    // UILayoutContainer behavior: layout management with auto-layout
    container->setAutoLayout(true); // Auto-layout for convenience  
    container->setVerticalLayout(5); // Default layout
    return container;
}

void SimpleContainer::addChild(std::shared_ptr<UIComponent> child) {
    if (child) {
        children_.push_back(child);
        // Auto-layout for forward compatibility with UIContainer/UILayoutContainer
        if (autoLayout_) {
            layout();
        }
    }
}

void SimpleContainer::removeChild(std::shared_ptr<UIComponent> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
    }
    // Clean layout removal - no dynamic_cast needed
    if (layout_) {
        layout_->removeComponent(child);
    }
    // Auto-layout for forward compatibility
    if (autoLayout_) {
        layout();
    }
}

void SimpleContainer::clearChildren() {
    children_.clear();
    if (layout_) {
        layout_->clearComponents();
    }
    // Auto-layout for forward compatibility
    if (autoLayout_) {
        layout();
    }
}

void SimpleContainer::setVerticalLayout(int spacing) {
    layout_ = LayoutFactory::vertical(spacing);
    // Auto-layout when layout type changes
    if (autoLayout_) {
        layout();
    }
}

void SimpleContainer::setHorizontalLayout(int spacing) {
    layout_ = LayoutFactory::horizontal(spacing);
    // Auto-layout when layout type changes
    if (autoLayout_) {
        layout();
    }
}

void SimpleContainer::setGridLayout(int rows, int cols, int spacing) {
    layout_ = LayoutFactory::grid(rows, cols, spacing);
    // Auto-layout when layout type changes
    if (autoLayout_) {
        layout();
    }
}

void SimpleContainer::setCustomLayout(std::unique_ptr<Layout> layout) {
    layout_ = std::move(layout);
    // Auto-layout when layout type changes
    if (autoLayout_) {
        this->layout();
    }
}

void SimpleContainer::setScrollable(bool scrollable) { 
    scrollable_ = scrollable; 
    if (!scrollable_) scrollOffset_ = 0;
}

void SimpleContainer::setScrollOffset(int offset) {
    if (!scrollable_) return;
    scrollOffset_ = std::max(0, std::min(offset, getMaxScroll()));
}

int SimpleContainer::getMaxScroll() const {
    if (!scrollable_ || !layout_) return 0;
    
    auto preferredSize = layout_->calculatePreferredSize(children_);
    int contentHeight = preferredSize.second;
    return std::max(0, contentHeight - height_);
}

void SimpleContainer::layout() {
    if (!layout_) return;
    
    // Calculate layout area with scroll offset
    int layoutX = x_;
    int layoutY = y_;
    if (scrollable_) {
        layoutY -= scrollOffset_;  // Apply scroll offset
    }
    
    layout_->layoutChildren(children_, layoutX, layoutY, width_, height_);
}

void SimpleContainer::render() {
    if (!layout_) return;
    
    // Set up clipping for scrollable content
    SDL_Rect clip = {x_, y_, width_, height_};
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_RenderSetClipRect(renderer, &clip);
    
    // Render visible children only
    for (auto& child : children_) {
        if (child && isChildVisible(child)) {
            child->render();
        }
    }
    
    SDL_RenderSetClipRect(renderer, nullptr);
}

void SimpleContainer::handleEvent(const SDL_Event& event) {
    // Handle scroll wheel
    if (scrollable_ && event.type == SDL_MOUSEWHEEL) {
        setScrollOffset(scrollOffset_ - event.wheel.y * 10);
    }
    
    // Forward events to visible children
    for (auto& child : children_) {
        if (child && isChildVisible(child)) {
            child->handleEvent(event);
        }
    }
}

std::shared_ptr<UIComponent> SimpleContainer::hitTest(int x, int y) const {
    if (x < x_ || x > x_ + width_ || y < y_ || y > y_ + height_) {
        return nullptr;
    }
    
    // Test children in reverse order (last added = on top)
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        auto& child = *it;
        if (child && isChildVisible(child) && child->isPointInside(x, y)) {
            return child;
        }
    }
    
    return nullptr;
}

std::shared_ptr<UIComponent> SimpleContainer::getChild(int index) const {
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        return nullptr;
    }
    return children_[index];
}

std::pair<int, int> SimpleContainer::calculatePreferredSize() const {
    if (!layout_) return {width_, height_};
    return layout_->calculatePreferredSize(children_);
}

void SimpleContainer::autoResizeToFitChildren() {
    auto preferredSize = calculatePreferredSize();
    setSize(preferredSize.first, preferredSize.second);
}

int SimpleContainer::getVisibleCount() const {
    int count = 0;
    for (auto& child : children_) {
        if (child && isChildVisible(child)) {
            count++;
        }
    }
    return count;
}

bool SimpleContainer::isChildVisible(const std::shared_ptr<UIComponent>& child) const {
    if (!child) return false;
    
    SDL_Rect childRect = child->getRect();
    
    // Check if child intersects with container bounds
    return !(childRect.x + childRect.w < x_ || 
             childRect.x > x_ + width_ ||
             childRect.y + childRect.h < y_ || 
             childRect.y > y_ + height_);
}
