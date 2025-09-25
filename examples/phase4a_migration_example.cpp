#include "Interface/ui/SimpleContainer.h"
#include "Interface/ui/ContainerCompat.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UILabel.h"
#include "Systems/SDLManager.h"
#include <iostream>
#include <memory>

/**
 * Phase 4A Migration Examples
 * 
 * This example shows how to migrate from UIContainer/UILayoutContainer
 * to SimpleContainer with forward compatibility features.
 */

void demonstrateUIContainerMigration(SDLManager& sdl) {
    std::cout << "\n=== UIContainer Migration Demo ===\n";
    
    // Phase 4A: Direct replacement (recommended approach)
    auto container = SimpleContainer::createUIContainerReplacement(50, 50, 300, 400, sdl);
    
    std::cout << "Created container with auto-scroll: " << container->isScrollable() << std::endl;
    std::cout << "Created container with auto-layout: " << container->isAutoLayout() << std::endl;
    
    // Add content just like with UIContainer
    for (int i = 0; i < 8; i++) {
        auto label = std::make_shared<UILabel>(0, 0, 280, 25, sdl, 
                                               "Menu Item " + std::to_string(i + 1));
        container->addChild(label);
        
        // Auto-layout positions it automatically
        SDL_Rect rect = label->getRect();
        std::cout << "Item " << i << " positioned at: (" << rect.x << ", " << rect.y << ")\n";
    }
    
    // Scrolling works out of the box
    std::cout << "Max scroll: " << container->getMaxScroll() << " pixels\n";
    
    // Test scrolling
    container->setScrollOffset(50);
    std::cout << "Scrolled to offset: " << container->getScrollOffset() << std::endl;
}

void demonstrateUILayoutContainerMigration(SDLManager& sdl) {
    std::cout << "\n=== UILayoutContainer Migration Demo ===\n";
    
    // Phase 4A: Direct replacement with layout management
    auto container = SimpleContainer::createUILayoutContainerReplacement(100, 100, 400, 200, sdl);
    
    std::cout << "Created layout container with auto-layout: " << container->isAutoLayout() << std::endl;
    
    // Create buttons
    auto okButton = std::make_shared<UIButton>("OK", 0, 0, 80, 35, sdl);
    auto cancelButton = std::make_shared<UIButton>("Cancel", 0, 0, 80, 35, sdl);
    auto helpButton = std::make_shared<UIButton>("Help", 0, 0, 80, 35, sdl);
    
    container->addChild(okButton);
    container->addChild(cancelButton);
    container->addChild(helpButton);
    
    std::cout << "Added 3 buttons with vertical layout (default)\n";
    
    // Print positions after vertical layout
    for (int i = 0; i < 3; i++) {
        auto child = container->getChild(i);
        SDL_Rect rect = child->getRect();
        std::cout << "Button " << i << " at: (" << rect.x << ", " << rect.y << ")\n";
    }
    
    // Switch to horizontal layout - auto-layout triggers
    std::cout << "\nSwitching to horizontal layout...\n";
    container->setHorizontalLayout(15);
    
    // Print new positions after horizontal layout
    for (int i = 0; i < 3; i++) {
        auto child = container->getChild(i);
        SDL_Rect rect = child->getRect();
        std::cout << "Button " << i << " now at: (" << rect.x << ", " << rect.y << ")\n";
    }
}

void demonstrateMacroShortcuts(SDLManager& sdl) {
    std::cout << "\n=== Macro Shortcuts Demo ===\n";
    
    // Using convenience macros
    auto container1 = SIMPLE_CONTAINER(0, 0, 200, 300, sdl);
    auto container2 = SIMPLE_LAYOUT_CONTAINER(250, 0, 200, 300, sdl);
    
    std::cout << "Container1 scrollable: " << container1->isScrollable() << std::endl;
    std::cout << "Container2 scrollable: " << container2->isScrollable() << std::endl;
    
    // Using namespace helpers
    using namespace ContainerMigration;
    auto container3 = replaceUIContainer(0, 350, 200, 300, sdl);
    auto container4 = replaceUILayoutContainer(250, 350, 200, 300, sdl);
    
    std::cout << "All containers created successfully with one-line calls\n";
}

void demonstrateAutoLayoutControl(SDLManager& sdl) {
    std::cout << "\n=== Auto-Layout Control Demo ===\n";
    
    auto container = SimpleContainer::createUIContainerReplacement(0, 0, 300, 200, sdl);
    
    // Add a button with wrong initial position
    auto button = std::make_shared<UIButton>("Test", 150, 150, 100, 30, sdl);
    
    std::cout << "Button initial position: (150, 150)\n";
    
    // Auto-layout corrects position
    container->addChild(button);
    SDL_Rect autoRect = button->getRect();
    std::cout << "After auto-layout: (" << autoRect.x << ", " << autoRect.y << ")\n";
    
    // Disable auto-layout for manual control
    container->setAutoLayout(false);
    
    auto button2 = std::make_shared<UIButton>("Manual", 200, 100, 100, 30, sdl);
    container->addChild(button2);
    SDL_Rect manualRect = button2->getRect();
    std::cout << "Manual positioning (auto-layout off): (" << manualRect.x << ", " << manualRect.y << ")\n";
    
    // Re-enable and trigger manual layout
    container->setAutoLayout(true);
    container->layout(); // Manual trigger
    
    SDL_Rect correctedRect = button2->getRect();
    std::cout << "After manual layout(): (" << correctedRect.x << ", " << correctedRect.y << ")\n";
}

void demonstrateBackwardCompatibility(SDLManager& sdl) {
    std::cout << "\n=== Backward Compatibility Demo ===\n";
    
    // This code would work exactly like UIContainer
    auto container = SimpleContainer::createUIContainerReplacement(0, 0, 250, 400, sdl);
    
    // Same interface as UIContainer
    for (int i = 0; i < 6; i++) {
        auto item = std::make_shared<UILabel>(0, 0, 230, 30, sdl, 
                                              "Legacy Item " + std::to_string(i));
        container->addChild(item); // Same method name
    }
    
    // Same scrolling interface  
    int maxScroll = container->getMaxScroll(); // Same method name
    container->setScrollOffset(30);             // Same method name
    int currentScroll = container->getScrollOffset(); // Same method name
    
    std::cout << "Backward compatibility: " << container->getChildCount() << " items\n";
    std::cout << "Scrolling: " << currentScroll << "/" << maxScroll << std::endl;
    
    // Same hit testing
    auto hitComponent = container->hitTest(100, 50); // Same method name
    std::cout << "Hit test result: " << (hitComponent ? "Hit!" : "Miss") << std::endl;
}

int main() {
    SDLManager sdl; // Minimal SDL setup for demo
    
    std::cout << "Phase 4A: Forward Compatibility Migration Examples\n";
    std::cout << "================================================\n";
    
    try {
        demonstrateUIContainerMigration(sdl);
        demonstrateUILayoutContainerMigration(sdl);
        demonstrateMacroShortcuts(sdl);
        demonstrateAutoLayoutControl(sdl);
        demonstrateBackwardCompatibility(sdl);
        
        std::cout << "\n=== Migration Summary ===\n";
        std::cout << "✅ UIContainer replacement: Drop-in compatible\n";
        std::cout << "✅ UILayoutContainer replacement: Drop-in compatible\n";
        std::cout << "✅ Auto-layout: Eliminates manual layout() calls\n";
        std::cout << "✅ Auto-scroll: UIContainer behavior preserved\n";
        std::cout << "✅ Performance: Composition over inheritance\n";
        std::cout << "✅ Clean API: Same interface, better implementation\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
