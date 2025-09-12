#include "Interface/ui/UILayoutContainer.h"
#include "Systems/SDLManager.h"
#include <algorithm>

// ============================================================================
// UILayoutContainer Implementation
// ============================================================================

UILayoutContainer::UILayoutContainer(int x, int y, int width, int height, SDLManager& sdl)
    : UIComponent(x, y, width, height, sdl) {
    // Default to vertical layout for backward compatibility
    setVerticalLayout();
}

void UILayoutContainer::addChild(std::shared_ptr<UIComponent> child) {
    if (!child) return;
    
    children_.push_back(child);
}

void UILayoutContainer::removeChild(std::shared_ptr<UIComponent> child) {
    if (!child) return;
    
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
    }
    
    // Also remove from special layout managers if needed
    if (auto borderLayout = dynamic_cast<BorderLayout*>(layout_.get())) {
        borderLayout->removeComponent(child);
    } else if (auto absoluteLayout = dynamic_cast<AbsoluteLayout*>(layout_.get())) {
        absoluteLayout->removeComponent(child);
    }
}

void UILayoutContainer::clearChildren() {
    children_.clear();
    
    // Clear special layout managers
    if (auto borderLayout = dynamic_cast<BorderLayout*>(layout_.get())) {
        borderLayout->clearComponents();
    } else if (auto absoluteLayout = dynamic_cast<AbsoluteLayout*>(layout_.get())) {
        absoluteLayout->clearComponents();
    }
}

void UILayoutContainer::setLayout(std::unique_ptr<Layout> layout) {
    layout_ = std::move(layout);
}

void UILayoutContainer::setVerticalLayout(int spacing) {
    layout_ = std::make_unique<VerticalLayout>(spacing);
}

void UILayoutContainer::setHorizontalLayout(int spacing) {
    layout_ = std::make_unique<HorizontalLayout>(spacing);
}

void UILayoutContainer::setGridLayout(int rows, int cols, int spacing) {
    layout_ = std::make_unique<GridLayout>(rows, cols, spacing);
}

void UILayoutContainer::setBorderLayout(int gap) {
    layout_ = std::make_unique<BorderLayout>(gap);
    // Note: Components need to be added using addBorderComponent
}

void UILayoutContainer::setFlexLayout(FlexDirection direction) {
    layout_ = std::make_unique<FlexLayout>(direction);
}

void UILayoutContainer::setAbsoluteLayout() {
    layout_ = std::make_unique<AbsoluteLayout>();
    // Note: Components need positions set using setAbsolutePosition
}

void UILayoutContainer::addBorderComponent(std::shared_ptr<UIComponent> component, BorderPosition position) {
    if (!component) return;
    
    // Add to children list if not already there
    if (std::find(children_.begin(), children_.end(), component) == children_.end()) {
        addChild(component);
    }
    
    // Add to border layout
    if (auto borderLayout = dynamic_cast<BorderLayout*>(layout_.get())) {
        borderLayout->addComponent(component, position);
    }
}

void UILayoutContainer::setAbsolutePosition(std::shared_ptr<UIComponent> component, const AbsolutePosition& position) {
    if (!component) return;
    
    // Add to children list if not already there
    if (std::find(children_.begin(), children_.end(), component) == children_.end()) {
        addChild(component);
    }
    
    // Set absolute position
    if (auto absoluteLayout = dynamic_cast<AbsoluteLayout*>(layout_.get())) {
        absoluteLayout->setComponentPosition(component, position);
    }
}

void UILayoutContainer::setScrollOffset(int offset) {
    if (!scrollable_) return;
    
    // Clamp to [0, max]
    scrollOffset_ = std::max(0, std::min(offset, getMaxScroll()));
}

int UILayoutContainer::getMaxScroll() const {
    if (!scrollable_ || !layout_) return 0;
    
    auto preferredSize = layout_->calculatePreferredSize(children_);
    int contentHeight = preferredSize.second;
    return std::max(0, contentHeight - height_);
}

void UILayoutContainer::layout() {
    if (!layout_) return;
    
    // Calculate layout area (accounting for scroll offset if vertical layout)
    int layoutX = x_;
    int layoutY = y_;
    int layoutWidth = width_;
    int layoutHeight = height_;
    
    // Apply scroll offset for scrollable containers
    if (scrollable_) {
        // For now, only support vertical scrolling
        layoutY -= scrollOffset_;
    }
    
    // Delegate to the layout manager
    layout_->layoutChildren(children_, layoutX, layoutY, layoutWidth, layoutHeight);
}

void UILayoutContainer::render() {
    if (!layout_) return;
    
    // Set up clipping rectangle
    SDL_Rect clip = {x_, y_, width_, height_};
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_RenderSetClipRect(renderer, &clip);

    // Render visible children
    for (auto& child : children_) {
        if (child && isChildVisible(child)) {
            child->render();
        }
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}

void UILayoutContainer::handleEvent(const SDL_Event& event) {
    if (!scrollable_) return;
    
    if (event.type == SDL_MOUSEWHEEL) {
        // Scroll by wheel y * some constant (10 pixels per wheel unit)
        setScrollOffset(scrollOffset_ - event.wheel.y * 10);
    }
    
    // Forward events to visible children
    for (auto& child : children_) {
        if (child && isChildVisible(child)) {
            child->handleEvent(event);
        }
    }
}

std::shared_ptr<UIComponent> UILayoutContainer::hitTest(int x, int y) const {
    if (x < x_ || x > x_ + width_ || y < y_ || y > y_ + height_) {
        return nullptr;
    }
    
    // Test children in reverse order (last added = on top)
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        auto& child = *it;
        if (!child || !isChildVisible(child)) continue;
        
        if (child->isPointInside(x, y)) {
            return child;
        }
    }
    
    return nullptr;
}

std::shared_ptr<UIComponent> UILayoutContainer::getChild(int index) const {
    if (index < 0 || index >= static_cast<int>(children_.size())) {
        return nullptr;
    }
    return children_[index];
}

void UILayoutContainer::autoResizeToFitChildren() {
    if (!layout_) return;
    
    auto preferredSize = layout_->calculatePreferredSize(children_);
    setSize(preferredSize.first, preferredSize.second);
}

std::pair<int, int> UILayoutContainer::calculatePreferredSize() const {
    if (!layout_) return {width_, height_};
    
    return layout_->calculatePreferredSize(children_);
}

bool UILayoutContainer::isChildVisible(const std::shared_ptr<UIComponent>& child) const {
    if (!child) return false;
    
    SDL_Rect childRect = child->getRect();
    
    // Check if child intersects with container bounds
    return !(childRect.x + childRect.w < x_ || 
             childRect.x > x_ + width_ ||
             childRect.y + childRect.h < y_ || 
             childRect.y > y_ + height_);
}

void UILayoutContainer::updateScrollBounds() {
    if (!scrollable_) return;
    
    // Clamp current scroll offset to valid range
    int maxScroll = getMaxScroll();
    scrollOffset_ = std::max(0, std::min(scrollOffset_, maxScroll));
}
