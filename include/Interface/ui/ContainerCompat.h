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
    auto container = std::make_shared<SimpleContainer>(x, y, width, height, sdl);
    container->setScrollable(true); // Enable scrolling by default like UIContainer
    return container;
}

/**
 * Helper function to create a UILayoutContainer-like SimpleContainer  
 * with layout management for backward compatibility
 */
inline std::shared_ptr<SimpleContainer> createLayoutContainer(
    int x, int y, int width, int height, SDLManager& sdl) {
    auto container = std::make_shared<SimpleContainer>(x, y, width, height, sdl);
    // Default to vertical layout like UILayoutContainer
    container->setVerticalLayout();
    return container;
}
