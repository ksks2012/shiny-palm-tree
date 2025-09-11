#include "Interface/ui/UIContainer.h"
#include "Systems/SDLManager.h"
#include <algorithm>

UIContainer::UIContainer(int x, int y, int width, int height, SDLManager& sdl)
    : UILayoutContainer(x, y, width, height, sdl) {
    // UILayoutContainer constructor already sets up vertical layout
    // which maintains backward compatibility
}

void UIContainer::setScrollOffset(int offset) {
    // For UIContainer, we manage scroll offset directly for backward compatibility
    // No need to check scrollable_ or calculate maxScroll like UILayoutContainer
    scrollOffset_ = std::max(0, offset);  // Allow any positive offset for legacy behavior
}

void UIContainer::layout() {
    // Original UIContainer layout logic for exact backward compatibility
    int offsetY = y_ - scrollOffset_;  // Apply scroll offset to the starting position
    
    for (auto& child : children_) {
        if (!child) continue;
        
        // First layout the child itself
        child->layout();
        
        // Then position it within this container, with scroll offset applied
        child->setPosition(x_, offsetY);
        
        offsetY += child->getHeight();
    }
}

int UIContainer::getVisibleCount() const {
    int count = 0;
    int offsetY = -scrollOffset_;
    for (auto& child : children_) {
        if (!child) continue;
        if (offsetY + child->getHeight() >= 0 && offsetY <= height_) {
            count++;
        }
        offsetY += child->getHeight();
    }
    return count;
}
