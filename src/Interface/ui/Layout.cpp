#include "Interface/ui/Layout.h"
#include <algorithm>
#include <cmath>

// ============================================================================
// VerticalLayout Implementation
// ============================================================================

void VerticalLayout::layoutChildren(
    std::vector<std::shared_ptr<UIComponent>>& children,
    int containerX, int containerY, 
    int containerWidth, int containerHeight
) {
    int currentY = containerY;
    
    for (auto& child : children) {
        if (!child) continue;
        
        // First layout the child itself
        child->layout();
        
        // Then position it within the container
        child->setPosition(containerX, currentY);
        
        // Move to next position with spacing
        currentY += child->getHeight() + spacing_;
    }
}

std::pair<int, int> VerticalLayout::calculatePreferredSize(
    const std::vector<std::shared_ptr<UIComponent>>& children
) const {
    if (children.empty()) return {0, 0};
    
    int maxWidth = 0;
    int totalHeight = 0;
    
    for (size_t i = 0; i < children.size(); ++i) {
        const auto& child = children[i];
        if (!child) continue;
        
        maxWidth = std::max(maxWidth, child->getWidth());
        totalHeight += child->getHeight();
        
        // Add spacing between components (but not after the last one)
        if (i < children.size() - 1) {
            totalHeight += spacing_;
        }
    }
    
    return {maxWidth, totalHeight};
}

// ============================================================================
// HorizontalLayout Implementation
// ============================================================================

void HorizontalLayout::layoutChildren(
    std::vector<std::shared_ptr<UIComponent>>& children,
    int containerX, int containerY, 
    int containerWidth, int containerHeight
) {
    int currentX = containerX;
    
    for (auto& child : children) {
        if (!child) continue;
        
        // First layout the child itself
        child->layout();
        
        // Then position it within the container
        child->setPosition(currentX, containerY);
        
        // Move to next position with spacing
        currentX += child->getWidth() + spacing_;
    }
}

std::pair<int, int> HorizontalLayout::calculatePreferredSize(
    const std::vector<std::shared_ptr<UIComponent>>& children
) const {
    if (children.empty()) return {0, 0};
    
    int totalWidth = 0;
    int maxHeight = 0;
    
    for (size_t i = 0; i < children.size(); ++i) {
        const auto& child = children[i];
        if (!child) continue;
        
        totalWidth += child->getWidth();
        maxHeight = std::max(maxHeight, child->getHeight());
        
        // Add spacing between components (but not after the last one)
        if (i < children.size() - 1) {
            totalWidth += spacing_;
        }
    }
    
    return {totalWidth, maxHeight};
}

// ============================================================================
// GridLayout Implementation
// ============================================================================

void GridLayout::layoutChildren(
    std::vector<std::shared_ptr<UIComponent>>& children,
    int containerX, int containerY, 
    int containerWidth, int containerHeight
) {
    if (children.empty() || rows_ <= 0 || cols_ <= 0) return;
    
    // Calculate cell dimensions
    int totalSpacingW = (cols_ - 1) * spacing_;
    int totalSpacingH = (rows_ - 1) * spacing_;
    int cellWidth = (containerWidth - totalSpacingW) / cols_;
    int cellHeight = (containerHeight - totalSpacingH) / rows_;
    
    int index = 0;
    for (int row = 0; row < rows_ && index < static_cast<int>(children.size()); ++row) {
        for (int col = 0; col < cols_ && index < static_cast<int>(children.size()); ++col) {
            auto& child = children[index];
            if (!child) {
                ++index;
                continue;
            }
            
            // First layout the child itself
            child->layout();
            
            // Calculate position
            int x = containerX + col * (cellWidth + spacing_);
            int y = containerY + row * (cellHeight + spacing_);
            
            // Position and size the child to fit the grid cell
            child->setPosition(x, y);
            child->setSize(cellWidth, cellHeight);
            
            ++index;
        }
    }
}

std::pair<int, int> GridLayout::calculatePreferredSize(
    const std::vector<std::shared_ptr<UIComponent>>& children
) const {
    if (children.empty() || rows_ <= 0 || cols_ <= 0) return {0, 0};
    
    // Find the maximum preferred size among all children
    int maxChildWidth = 0;
    int maxChildHeight = 0;
    
    for (const auto& child : children) {
        if (!child) continue;
        maxChildWidth = std::max(maxChildWidth, child->getWidth());
        maxChildHeight = std::max(maxChildHeight, child->getHeight());
    }
    
    // Calculate total size needed
    int totalWidth = cols_ * maxChildWidth + (cols_ - 1) * spacing_;
    int totalHeight = rows_ * maxChildHeight + (rows_ - 1) * spacing_;
    
    return {totalWidth, totalHeight};
}

// ============================================================================
// BorderLayout Implementation
// ============================================================================

void BorderLayout::addComponent(std::shared_ptr<UIComponent> component, BorderPosition position) {
    if (!component) return;
    
    // Remove component if it already exists
    removeComponent(component);
    
    // Add component with position
    borderComponents_.push_back({component, position});
}

void BorderLayout::removeComponent(std::shared_ptr<UIComponent> component) {
    if (!component) return;
    
    borderComponents_.erase(
        std::remove_if(borderComponents_.begin(), borderComponents_.end(),
            [component](const BorderComponent& bc) {
                return bc.component == component;
            }),
        borderComponents_.end()
    );
}

void BorderLayout::layoutChildren(
    std::vector<std::shared_ptr<UIComponent>>& children,
    int containerX, int containerY, 
    int containerWidth, int containerHeight
) {
    // First, find components for each region
    std::shared_ptr<UIComponent> north, south, east, west, center;
    
    for (const auto& bc : borderComponents_) {
        switch (bc.position) {
            case BorderPosition::NORTH: north = bc.component; break;
            case BorderPosition::SOUTH: south = bc.component; break;
            case BorderPosition::EAST: east = bc.component; break;
            case BorderPosition::WEST: west = bc.component; break;
            case BorderPosition::CENTER: center = bc.component; break;
        }
    }
    
    // Layout children first
    for (auto& child : children) {
        if (child) child->layout();
    }
    
    // Calculate available space after accounting for border components
    int availableX = containerX;
    int availableY = containerY;
    int availableWidth = containerWidth;
    int availableHeight = containerHeight;
    
    // Layout NORTH (top)
    if (north) {
        north->setPosition(availableX, availableY);
        north->setSize(availableWidth, north->getHeight());
        availableY += north->getHeight() + gap_;
        availableHeight -= north->getHeight() + gap_;
    }
    
    // Layout SOUTH (bottom)
    if (south) {
        int southY = containerY + containerHeight - south->getHeight();
        south->setPosition(availableX, southY);
        south->setSize(availableWidth, south->getHeight());
        availableHeight -= south->getHeight() + gap_;
    }
    
    // Layout WEST (left)
    if (west) {
        west->setPosition(availableX, availableY);
        west->setSize(west->getWidth(), availableHeight);
        availableX += west->getWidth() + gap_;
        availableWidth -= west->getWidth() + gap_;
    }
    
    // Layout EAST (right)
    if (east) {
        int eastX = containerX + containerWidth - east->getWidth();
        east->setPosition(eastX, availableY);
        east->setSize(east->getWidth(), availableHeight);
        availableWidth -= east->getWidth() + gap_;
    }
    
    // Layout CENTER (remaining space)
    if (center && availableWidth > 0 && availableHeight > 0) {
        center->setPosition(availableX, availableY);
        center->setSize(availableWidth, availableHeight);
    }
}

std::pair<int, int> BorderLayout::calculatePreferredSize(
    const std::vector<std::shared_ptr<UIComponent>>& children
) const {
    // Find components for each region
    std::shared_ptr<UIComponent> north, south, east, west, center;
    
    for (const auto& bc : borderComponents_) {
        switch (bc.position) {
            case BorderPosition::NORTH: north = bc.component; break;
            case BorderPosition::SOUTH: south = bc.component; break;
            case BorderPosition::EAST: east = bc.component; break;
            case BorderPosition::WEST: west = bc.component; break;
            case BorderPosition::CENTER: center = bc.component; break;
        }
    }
    
    int width = 0;
    int height = 0;
    
    // Calculate width (max of north/south width, or west + center + east)
    int nsWidth = std::max(north ? north->getWidth() : 0, south ? south->getWidth() : 0);
    int ewcWidth = (west ? west->getWidth() + gap_ : 0) + 
                   (center ? center->getWidth() : 0) + 
                   (east ? gap_ + east->getWidth() : 0);
    width = std::max(nsWidth, ewcWidth);
    
    // Calculate height (north + center/west/east + south)
    height = (north ? north->getHeight() + gap_ : 0) + 
             std::max({west ? west->getHeight() : 0, 
                      center ? center->getHeight() : 0, 
                      east ? east->getHeight() : 0}) +
             (south ? gap_ + south->getHeight() : 0);
    
    return {width, height};
}

// ============================================================================
// FlexLayout Implementation
// ============================================================================

void FlexLayout::layoutChildren(
    std::vector<std::shared_ptr<UIComponent>>& children,
    int containerX, int containerY, 
    int containerWidth, int containerHeight
) {
    if (children.empty()) return;
    
    // Layout children first
    for (auto& child : children) {
        if (child) child->layout();
    }
    
    // Filter out null children
    std::vector<std::shared_ptr<UIComponent>> validChildren;
    for (auto& child : children) {
        if (child) validChildren.push_back(child);
    }
    
    if (validChildren.empty()) return;
    
    bool isRow = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE);
    bool isReverse = (direction_ == FlexDirection::ROW_REVERSE || direction_ == FlexDirection::COLUMN_REVERSE);
    
    // If reverse, reverse the children order for processing
    if (isReverse) {
        std::reverse(validChildren.begin(), validChildren.end());
    }
    
    // Calculate main axis total size needed and available space
    int totalMainSize = 0;
    int maxCrossSize = 0;
    
    for (const auto& child : validChildren) {
        if (isRow) {
            totalMainSize += child->getWidth();
            maxCrossSize = std::max(maxCrossSize, child->getHeight());
        } else {
            totalMainSize += child->getHeight();
            maxCrossSize = std::max(maxCrossSize, child->getWidth());
        }
    }
    
    // Add gaps
    totalMainSize += gap_ * (static_cast<int>(validChildren.size()) - 1);
    
    int availableMainSize = isRow ? containerWidth : containerHeight;
    int availableCrossSize = isRow ? containerHeight : containerWidth;
    int freeSpace = availableMainSize - totalMainSize;
    
    // Calculate starting position and spacing based on justify-content
    int startPos = isRow ? containerX : containerY;
    int itemSpacing = gap_;
    
    switch (justifyContent_) {
        case FlexJustify::FLEX_START:
            // Already at start, no change needed
            break;
        case FlexJustify::FLEX_END:
            startPos += freeSpace;
            break;
        case FlexJustify::CENTER:
            startPos += freeSpace / 2;
            break;
        case FlexJustify::SPACE_BETWEEN:
            if (validChildren.size() > 1) {
                itemSpacing = gap_ + freeSpace / (static_cast<int>(validChildren.size()) - 1);
            }
            break;
        case FlexJustify::SPACE_AROUND:
            {
                int spacePerItem = freeSpace / static_cast<int>(validChildren.size());
                startPos += spacePerItem / 2;
                itemSpacing = gap_ + spacePerItem;
            }
            break;
        case FlexJustify::SPACE_EVENLY:
            {
                int spacePerGap = freeSpace / (static_cast<int>(validChildren.size()) + 1);
                startPos += spacePerGap;
                itemSpacing = gap_ + spacePerGap;
            }
            break;
    }
    
    // Position each child
    int currentMainPos = startPos;
    
    for (auto& child : validChildren) {
        int mainPos = currentMainPos;
        int crossPos;
        int crossSize;
        
        if (isRow) {
            // Calculate cross axis position (Y)
            switch (alignItems_) {
                case FlexAlign::FLEX_START:
                    crossPos = containerY;
                    crossSize = child->getHeight();
                    break;
                case FlexAlign::FLEX_END:
                    crossPos = containerY + containerHeight - child->getHeight();
                    crossSize = child->getHeight();
                    break;
                case FlexAlign::CENTER:
                    crossPos = containerY + (containerHeight - child->getHeight()) / 2;
                    crossSize = child->getHeight();
                    break;
                case FlexAlign::STRETCH:
                    crossPos = containerY;
                    crossSize = containerHeight;
                    break;
            }
            
            child->setPosition(mainPos, crossPos);
            if (alignItems_ == FlexAlign::STRETCH) {
                child->setSize(child->getWidth(), crossSize);
            }
            
            currentMainPos += child->getWidth() + itemSpacing;
        } else {
            // Calculate cross axis position (X)
            switch (alignItems_) {
                case FlexAlign::FLEX_START:
                    crossPos = containerX;
                    crossSize = child->getWidth();
                    break;
                case FlexAlign::FLEX_END:
                    crossPos = containerX + containerWidth - child->getWidth();
                    crossSize = child->getWidth();
                    break;
                case FlexAlign::CENTER:
                    crossPos = containerX + (containerWidth - child->getWidth()) / 2;
                    crossSize = child->getWidth();
                    break;
                case FlexAlign::STRETCH:
                    crossPos = containerX;
                    crossSize = containerWidth;
                    break;
            }
            
            child->setPosition(crossPos, mainPos);
            if (alignItems_ == FlexAlign::STRETCH) {
                child->setSize(crossSize, child->getHeight());
            }
            
            currentMainPos += child->getHeight() + itemSpacing;
        }
    }
}

std::pair<int, int> FlexLayout::calculatePreferredSize(
    const std::vector<std::shared_ptr<UIComponent>>& children
) const {
    if (children.empty()) return {0, 0};
    
    bool isRow = (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE);
    
    int mainAxisSize = 0;
    int crossAxisSize = 0;
    int validChildCount = 0;
    
    for (const auto& child : children) {
        if (!child) continue;
        validChildCount++;
        
        if (isRow) {
            mainAxisSize += child->getWidth();
            crossAxisSize = std::max(crossAxisSize, child->getHeight());
        } else {
            mainAxisSize += child->getHeight();
            crossAxisSize = std::max(crossAxisSize, child->getWidth());
        }
    }
    
    // Add gaps
    if (validChildCount > 1) {
        mainAxisSize += gap_ * (validChildCount - 1);
    }
    
    return isRow ? std::make_pair(mainAxisSize, crossAxisSize) 
                 : std::make_pair(crossAxisSize, mainAxisSize);
}

// ============================================================================
// AbsoluteLayout Implementation
// ============================================================================

void AbsoluteLayout::setComponentPosition(std::shared_ptr<UIComponent> component, const AbsolutePosition& position) {
    if (!component) return;
    
    // Remove component if it already exists
    removeComponent(component);
    
    // Add component with position
    absoluteComponents_.push_back({component, position});
}

void AbsoluteLayout::removeComponent(std::shared_ptr<UIComponent> component) {
    if (!component) return;
    
    absoluteComponents_.erase(
        std::remove_if(absoluteComponents_.begin(), absoluteComponents_.end(),
            [component](const AbsoluteComponent& ac) {
                return ac.component == component;
            }),
        absoluteComponents_.end()
    );
}

void AbsoluteLayout::layoutChildren(
    std::vector<std::shared_ptr<UIComponent>>& children,
    int containerX, int containerY, 
    int containerWidth, int containerHeight
) {
    // Layout children first
    for (auto& child : children) {
        if (child) child->layout();
    }
    
    // Position components according to their absolute positions
    for (const auto& ac : absoluteComponents_) {
        if (!ac.component) continue;
        
        int x = containerX + ac.position.x;
        int y = containerY + ac.position.y;
        
        ac.component->setPosition(x, y);
        
        // Set size if specified
        if (ac.position.width > 0 && ac.position.height > 0) {
            ac.component->setSize(ac.position.width, ac.position.height);
        } else if (ac.position.width > 0) {
            ac.component->setSize(ac.position.width, ac.component->getHeight());
        } else if (ac.position.height > 0) {
            ac.component->setSize(ac.component->getWidth(), ac.position.height);
        }
        // If both width and height are -1, use component's current size
    }
}

std::pair<int, int> AbsoluteLayout::calculatePreferredSize(
    const std::vector<std::shared_ptr<UIComponent>>& children
) const {
    if (absoluteComponents_.empty()) return {0, 0};
    
    int maxX = 0;
    int maxY = 0;
    
    for (const auto& ac : absoluteComponents_) {
        if (!ac.component) continue;
        
        int width = ac.position.width > 0 ? ac.position.width : ac.component->getWidth();
        int height = ac.position.height > 0 ? ac.position.height : ac.component->getHeight();
        
        maxX = std::max(maxX, ac.position.x + width);
        maxY = std::max(maxY, ac.position.y + height);
    }
    
    return {maxX, maxY};
}
