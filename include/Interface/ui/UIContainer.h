#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "UIComponent.h"
#include "UILayoutContainer.h"

/**
 * UIContainer - holds child components in a vertical list with scroll support.
 * Provides clipping and hit-testing that accounts for scroll offset.
 * Now uses the new layout system internally for backward compatibility.
 */
class UIContainer : public UILayoutContainer {
public:
    UIContainer(int x, int y, int width, int height, SDLManager& sdl);
    ~UIContainer() override = default;

    // Original interface maintained for backward compatibility
    void addChild(std::shared_ptr<UIComponent> child) { UILayoutContainer::addChild(child); }
    void clearChildren() { UILayoutContainer::clearChildren(); }

    void setScrollOffset(int offset); // Custom implementation for backward compatibility
    int getScrollOffset() const { return UILayoutContainer::getScrollOffset(); }
    int getMaxScroll() const override { return UILayoutContainer::getMaxScroll(); }

    // Layout & render separation
    void layout() override;
    void render() override { UILayoutContainer::render(); }
    void handleEvent(const SDL_Event& event) override { UILayoutContainer::handleEvent(event); }

    // Hit test takes scroll into account
    std::shared_ptr<UIComponent> hitTest(int x, int y) const { return UILayoutContainer::hitTest(x, y); }

    // Expose visible count for tests (original interface)
    int getVisibleCount() const;
};
