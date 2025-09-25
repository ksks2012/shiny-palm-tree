#include <catch2/catch.hpp>
#include "Interface/ui/SimpleContainer.h"
#include "Interface/ui/ContainerCompat.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UILabel.h"
#include "Systems/SDLManager.h"
#include <memory>

TEST_CASE("Forward Compatibility - UIContainer Replacement", "[ForwardCompat][UIContainer]") {
    SDLManager sdl;
    
    SECTION("Direct replacement with auto-layout") {
        // Phase 4A: Direct replacement (recommended)
        auto container = SimpleContainer::createUIContainerReplacement(0, 0, 200, 300, sdl);
        
        REQUIRE(container->isScrollable()); // Should be scrollable like UIContainer
        REQUIRE(container->isAutoLayout()); // Should have auto-layout enabled
        
        // Add children - should automatically layout
        auto button1 = std::make_shared<UIButton>("Button1", 10, 10, 80, 30, sdl);
        auto button2 = std::make_shared<UIButton>("Button2", 10, 50, 80, 30, sdl);
        
        container->addChild(button1);
        container->addChild(button2);
        
        // Auto-layout should have positioned them vertically
        SDL_Rect rect1 = button1->getRect();
        SDL_Rect rect2 = button2->getRect();
        REQUIRE(rect1.y < rect2.y); // button1 should be above button2
        
        // Should be scrollable with content
        REQUIRE(container->getMaxScroll() >= 0);
    }
    
    SECTION("Macro-based replacement") {
        // Using convenience macro
        auto container = SIMPLE_CONTAINER(0, 0, 200, 300, sdl);
        
        REQUIRE(container->isScrollable());
        REQUIRE(container->isAutoLayout());
        
        // Should behave exactly like UIContainer
        auto label = std::make_shared<UILabel>(0, 0, 180, 20, sdl, "Test Label");
        container->addChild(label);
        
        // Auto-positioned
        SDL_Rect labelRect = label->getRect();
        REQUIRE(labelRect.x >= 0);
        REQUIRE(labelRect.y >= 0);
    }
    
    SECTION("Helper function replacement") {
        // Using helper function
        auto container = ContainerMigration::replaceUIContainer(0, 0, 200, 300, sdl);
        
        REQUIRE(container->isScrollable());
        REQUIRE(container->isAutoLayout());
    }
}

TEST_CASE("Forward Compatibility - UILayoutContainer Replacement", "[ForwardCompat][UILayoutContainer]") {
    SDLManager sdl;
    
    SECTION("Direct replacement with auto-layout") {
        // Phase 4A: Direct replacement (recommended)
        auto container = SimpleContainer::createUILayoutContainerReplacement(0, 0, 200, 300, sdl);
        
        REQUIRE_FALSE(container->isScrollable()); // UILayoutContainer wasn't scrollable by default
        REQUIRE(container->isAutoLayout()); // Should have auto-layout enabled
        
        // Add children - should automatically layout
        auto button1 = std::make_shared<UIButton>("Button1", 0, 0, 80, 30, sdl);
        auto button2 = std::make_shared<UIButton>("Button2", 0, 0, 80, 30, sdl);
        
        container->addChild(button1);
        container->addChild(button2);
        
        // Auto-layout should have positioned them
        SDL_Rect rect1 = button1->getRect();
        SDL_Rect rect2 = button2->getRect();
        REQUIRE(rect1.y < rect2.y); // Vertical layout by default
    }
    
    SECTION("Layout switching with auto-layout") {
        auto container = SIMPLE_LAYOUT_CONTAINER(0, 0, 200, 300, sdl);
        
        auto button1 = std::make_shared<UIButton>("Button1", 0, 0, 80, 30, sdl);
        auto button2 = std::make_shared<UIButton>("Button2", 0, 0, 80, 30, sdl);
        
        container->addChild(button1);
        container->addChild(button2);
        
        // Switch to horizontal layout
        container->setHorizontalLayout(10);
        // Auto-layout should re-position them horizontally
        
        SDL_Rect rect1 = button1->getRect();
        SDL_Rect rect2 = button2->getRect();
        REQUIRE(rect1.x < rect2.x); // Should now be horizontal
        REQUIRE(rect1.y == rect2.y); // Same row
    }
}

TEST_CASE("Forward Compatibility - Auto Layout Behavior", "[ForwardCompat][AutoLayout]") {
    SDLManager sdl;
    
    SECTION("Auto-layout on child management") {
        SimpleContainer container(0, 0, 200, 300, sdl);
        container.setAutoLayout(true);
        container.setVerticalLayout(5);
        
        auto button1 = std::make_shared<UIButton>("Button1", 100, 100, 80, 30, sdl); // Wrong position
        auto button2 = std::make_shared<UIButton>("Button2", 200, 200, 80, 30, sdl); // Wrong position
        
        // Add children - should auto-correct positions
        container.addChild(button1);
        container.addChild(button2);
        
        SDL_Rect rect1 = button1->getRect();
        SDL_Rect rect2 = button2->getRect();
        
        // Positions should be corrected by auto-layout
        REQUIRE(rect1.x == 0); // Should be at container's x
        REQUIRE(rect1.y == 0); // Should be at container's y
        REQUIRE(rect2.y > rect1.y + rect1.h); // Should be below button1
        
        // Remove child - should auto-layout remaining
        container.removeChild(button1);
        SDL_Rect newRect2 = button2->getRect();
        REQUIRE(newRect2.y == 0); // Should move up to top
    }
    
    SECTION("Manual vs Auto layout control") {
        SimpleContainer container(0, 0, 200, 300, sdl);
        
        auto button = std::make_shared<UIButton>("Button", 50, 50, 80, 30, sdl);
        
        // Without auto-layout - position preserved
        container.setAutoLayout(false);
        container.addChild(button);
        SDL_Rect manualRect = button->getRect();
        REQUIRE(manualRect.x == 50);
        REQUIRE(manualRect.y == 50);
        
        // Enable auto-layout and trigger manually
        container.setAutoLayout(true);
        container.layout();
        SDL_Rect autoRect = button->getRect();
        REQUIRE(autoRect.x == 0); // Should be repositioned
        REQUIRE(autoRect.y == 0);
    }
}

TEST_CASE("Forward Compatibility - Migration Examples", "[ForwardCompat][Examples]") {
    SDLManager sdl;
    
    SECTION("Complete UIContainer migration example") {
        // OLD WAY: UIContainer container(0, 0, 200, 100, sdl);
        // NEW WAY: Phase 4A direct replacement
        auto container = SimpleContainer::createUIContainerReplacement(0, 0, 200, 100, sdl);
        
        // Add content just like before
        for (int i = 0; i < 5; i++) {
            auto label = std::make_shared<UILabel>(0, 0, 180, 20, sdl, 
                                                   "Item " + std::to_string(i));
            container->addChild(label); // Auto-layout positions it
        }
        
        // Scrolling works out of the box
        REQUIRE(container->getMaxScroll() > 0);
        container->setScrollOffset(20);
        REQUIRE(container->getScrollOffset() == 20);
        
        // All 5 items properly positioned
        REQUIRE(container->getChildCount() == 5);
        for (int i = 0; i < 5; i++) {
            auto child = container->getChild(i);
            REQUIRE(child != nullptr);
            SDL_Rect rect = child->getRect();
            REQUIRE(rect.y == i * (20 + 5)); // 20px height + 5px spacing
        }
    }
    
    SECTION("Complete UILayoutContainer migration example") {
        // OLD WAY: UILayoutContainer container(0, 0, 200, 300, sdl);
        // NEW WAY: Phase 4A direct replacement  
        auto container = SimpleContainer::createUILayoutContainerReplacement(0, 0, 200, 300, sdl);
        
        // Set different layouts
        container->setHorizontalLayout(10);
        
        auto btn1 = std::make_shared<UIButton>("OK", 0, 0, 60, 30, sdl);
        auto btn2 = std::make_shared<UIButton>("Cancel", 0, 0, 60, 30, sdl);
        
        container->addChild(btn1); // Auto-positioned
        container->addChild(btn2); // Auto-positioned
        
        // Check horizontal layout
        SDL_Rect rect1 = btn1->getRect();
        SDL_Rect rect2 = btn2->getRect();
        
        REQUIRE(rect1.x < rect2.x); // btn1 left of btn2
        REQUIRE(rect1.y == rect2.y); // Same row
        REQUIRE(rect2.x >= rect1.x + rect1.w + 10); // 10px spacing
    }
}
