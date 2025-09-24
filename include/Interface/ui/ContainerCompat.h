#pragma once
#include "SimpleContainer.h"

/**
 * Backward compatibility aliases for deprecated container classes
 * 
 * These aliases allow existing code to compile without changes
 * while migrating to the new SimpleContainer architecture.
 * 
 * Usage:
 * - Replace #include "UIContainer.h" with #include "ContainerCompat.h" 
 * - Add setScrollable(true) calls where needed
 * - Add layout() calls after adding children
 * 
 * TODO: Remove these aliases after migration is complete
 */

// Backward compatibility alias
// Note: SimpleContainer has scrolling disabled by default, unlike UIContainer
using UIContainerLegacy [[deprecated("Use SimpleContainer instead")]] = SimpleContainer;

// For UILayoutContainer users who need layout functionality  
using UILayoutContainerLegacy [[deprecated("Use SimpleContainer instead")]] = SimpleContainer;

/**
 * Helper function to create a UIContainer-like SimpleContainer
 * with scrolling enabled by default for backward compatibility
 */
inline std::shared_ptr<SimpleContainer> createScrollableContainer(
    int x, int y, int width, int height, SDLManager& sdl) {
    return SimpleContainer::createUIContainerReplacement(x, y, width, height, sdl);
}

/**
 * Helper function to create a UILayoutContainer-like SimpleContainer  
 * with layout management for backward compatibility
 */
inline std::shared_ptr<SimpleContainer> createLayoutContainer(
    int x, int y, int width, int height, SDLManager& sdl) {
    return SimpleContainer::createUILayoutContainerReplacement(x, y, width, height, sdl);
}

/**
 * Phase 4A: Direct replacement macros for easy migration
 * 
 * Usage:
 * OLD: auto container = std::make_shared<UIContainer>(x, y, w, h, sdl);
 * NEW: auto container = SIMPLE_CONTAINER(x, y, w, h, sdl);
 */
#define SIMPLE_CONTAINER(x, y, w, h, sdl) \
    SimpleContainer::createUIContainerReplacement(x, y, w, h, sdl)

#define SIMPLE_LAYOUT_CONTAINER(x, y, w, h, sdl) \
    SimpleContainer::createUILayoutContainerReplacement(x, y, w, h, sdl)

/**
 * One-line migration helpers with auto-configuration
 */
namespace ContainerMigration {
    // Phase 4A: Direct replacement (recommended)
    inline auto replaceUIContainer(int x, int y, int w, int h, SDLManager& sdl) {
        auto container = SimpleContainer::createUIContainerReplacement(x, y, w, h, sdl);
        // Ready to use - scrolling enabled, auto-layout enabled
        return container;
    }
    
    inline auto replaceUILayoutContainer(int x, int y, int w, int h, SDLManager& sdl) {
        auto container = SimpleContainer::createUILayoutContainerReplacement(x, y, w, h, sdl);
        // Ready to use - auto-layout enabled, vertical layout set
        return container;
    }
}
