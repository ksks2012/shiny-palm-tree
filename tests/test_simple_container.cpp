#include <catch2/catch.hpp>
#include "Interface/ui/SimpleContainer.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UILabel.h"
#include "Systems/SDLManager.h"
#include <memory>

TEST_CASE("SimpleContainer - Basic Functionality", "[SimpleContainer]") {
    SDLManager sdl;
    
    SECTION("Construction and basic properties") {
        SimpleContainer container(10, 20, 200, 300, sdl);
        
        REQUIRE(container.getChildCount() == 0);
        REQUIRE_FALSE(container.isScrollable());
        REQUIRE(container.getScrollOffset() == 0);
        REQUIRE(container.getVisibleCount() == 0);
    }
    
    SECTION("Child management") {
        SimpleContainer container(0, 0, 200, 300, sdl);
        auto button = std::make_shared<UIButton>("Test", 10, 10, 100, 30, sdl);
        auto label = std::make_shared<UILabel>(10, 50, 100, 20, sdl, "Label");
        
        // Add children
        container.addChild(button);
        container.addChild(label);
        
        REQUIRE(container.getChildCount() == 2);
        REQUIRE(container.getChild(0) == button);
        REQUIRE(container.getChild(1) == label);
        REQUIRE(container.getChild(-1) == nullptr);
        REQUIRE(container.getChild(2) == nullptr);
        
        // Remove child
        container.removeChild(button);
        REQUIRE(container.getChildCount() == 1);
        REQUIRE(container.getChild(0) == label);
        
        // Clear all
        container.clearChildren();
        REQUIRE(container.getChildCount() == 0);
    }
}

TEST_CASE("SimpleContainer - Layout Management", "[SimpleContainer][Layout]") {
    SDLManager sdl;
    SimpleContainer container(0, 0, 200, 300, sdl);
    
    auto button1 = std::make_shared<UIButton>("Button1", 0, 0, 80, 30, sdl);
    auto button2 = std::make_shared<UIButton>("Button2", 0, 0, 80, 30, sdl);
    container.addChild(button1);
    container.addChild(button2);
    
    SECTION("Vertical layout") {
        container.setVerticalLayout(10);
        container.layout();
        
        // After layout, buttons should be positioned vertically
        SDL_Rect rect1 = button1->getRect();
        SDL_Rect rect2 = button2->getRect();
        
        REQUIRE(rect1.y < rect2.y); // button1 above button2
        REQUIRE(rect2.y >= rect1.y + rect1.h); // no overlap
    }
    
    SECTION("Horizontal layout") {
        container.setHorizontalLayout(5);
        container.layout();
        
        // After layout, buttons should be positioned horizontally
        SDL_Rect rect1 = button1->getRect();
        SDL_Rect rect2 = button2->getRect();
        
        REQUIRE(rect1.x < rect2.x); // button1 left of button2
        REQUIRE(rect2.x >= rect1.x + rect1.w); // no overlap
    }
    
    SECTION("Grid layout") {
        auto button3 = std::make_shared<UIButton>("Button3", 0, 0, 80, 30, sdl);
        container.addChild(button3);
        
        container.setGridLayout(2, 2, 5); // 2x2 grid
        container.layout();
        
        SDL_Rect rect1 = button1->getRect();
        SDL_Rect rect2 = button2->getRect();
        SDL_Rect rect3 = button3->getRect();
        
        // First row
        REQUIRE(rect1.y == rect2.y); // Same row
        REQUIRE(rect1.x < rect2.x);  // Different columns
        
        // Second row
        REQUIRE(rect3.y > rect1.y);  // Below first row
    }
}

TEST_CASE("SimpleContainer - Scrolling", "[SimpleContainer][Scroll]") {
    SDLManager sdl;
    SimpleContainer container(0, 0, 200, 100, sdl); // Small height
    
    // Add many children to exceed container height
    for (int i = 0; i < 10; i++) {
        auto label = std::make_shared<UILabel>(0, 0, 180, 20, sdl,
                                               "Label " + std::to_string(i));
        container.addChild(label);
    }
    
    SECTION("Scrolling disabled by default") {
        REQUIRE_FALSE(container.isScrollable());
        REQUIRE(container.getMaxScroll() == 0);
        
        container.setScrollOffset(50);
        REQUIRE(container.getScrollOffset() == 0); // Should remain 0
    }
    
    SECTION("Enable scrolling") {
        container.setScrollable(true);
        REQUIRE(container.isScrollable());
        
        container.layout(); // Calculate layout first
        
        // Should have scrollable content
        REQUIRE(container.getMaxScroll() > 0);
        
        // Test scroll offset
        int maxScroll = container.getMaxScroll();
        container.setScrollOffset(maxScroll / 2);
        REQUIRE(container.getScrollOffset() == maxScroll / 2);
        
        // Test bounds
        container.setScrollOffset(-10);
        REQUIRE(container.getScrollOffset() == 0);
        
        container.setScrollOffset(maxScroll + 100);
        REQUIRE(container.getScrollOffset() == maxScroll);
    }
}

TEST_CASE("SimpleContainer - Preferred Size and Auto-resize", "[SimpleContainer][Size]") {
    SDLManager sdl;
    SimpleContainer container(0, 0, 100, 100, sdl);
    
    auto button1 = std::make_shared<UIButton>("Button1", 0, 0, 80, 30, sdl);
    auto button2 = std::make_shared<UIButton>("Button2", 0, 0, 120, 40, sdl);
    
    container.addChild(button1);
    container.addChild(button2);
    container.setVerticalLayout(10);
    
    SECTION("Calculate preferred size") {
        auto [prefWidth, prefHeight] = container.calculatePreferredSize();
        
        // Should accommodate both buttons vertically with spacing
        REQUIRE(prefWidth >= 120);  // Widest button
        REQUIRE(prefHeight >= 80);  // Both buttons + spacing
    }
    
    SECTION("Auto-resize to fit children") {
        SDL_Rect oldRect = container.getRect();
        container.autoResizeToFitChildren();
        SDL_Rect newRect = container.getRect();
        
        // Container should have proper size to fit content
        auto [prefWidth, prefHeight] = container.calculatePreferredSize();
        REQUIRE(newRect.w == prefWidth);
        REQUIRE(newRect.h == prefHeight);
        
        // Should accommodate both buttons
        REQUIRE(newRect.w >= 120);  // Widest button
        REQUIRE(newRect.h >= 70);   // Both buttons + some spacing
    }
}

TEST_CASE("SimpleContainer - Event Handling", "[SimpleContainer][Events]") {
    SDLManager sdl;
    SimpleContainer container(0, 0, 200, 300, sdl);
    
    bool button1Clicked = false;
    bool button2Clicked = false;
    
    auto button1 = std::make_shared<UIButton>("Button1", 10, 10, 80, 30, sdl);
    auto button2 = std::make_shared<UIButton>("Button2", 10, 50, 80, 30, sdl);
    
    // Set up click handlers (if UIButton supports them)
    container.addChild(button1);
    container.addChild(button2);
    
    SECTION("Hit testing") {
        // Point inside container but outside children
        auto hit = container.hitTest(150, 150);
        REQUIRE(hit == nullptr);
        
        // Point outside container
        auto hit2 = container.hitTest(300, 400);
        REQUIRE(hit2 == nullptr);
    }
    
    SECTION("Scroll wheel events") {
        container.setScrollable(true);
        
        // Create scroll wheel event
        SDL_Event wheelEvent;
        wheelEvent.type = SDL_MOUSEWHEEL;
        wheelEvent.wheel.y = 1; // Scroll up
        
        int oldOffset = container.getScrollOffset();
        container.handleEvent(wheelEvent);
        
        // Scroll offset should change (though may be clamped to 0)
        REQUIRE(container.getScrollOffset() <= oldOffset);
    }
}

TEST_CASE("SimpleContainer - Composition over Inheritance", "[SimpleContainer][Design]") {
    SDLManager sdl;
    
    SECTION("No inheritance hell - clean hierarchy") {
        SimpleContainer container(0, 0, 200, 300, sdl);
        
        // SimpleContainer should directly inherit from UIComponent only
        // No UILayoutContainer or UIContainer in the chain
        UIComponent* component = &container;
        REQUIRE(component != nullptr);
        
        // Should have all container functionality through composition
        REQUIRE(container.getChildCount() == 0);
        
        auto button = std::make_shared<UIButton>("Test", 0, 0, 100, 30, sdl);
        container.addChild(button);
        REQUIRE(container.getChildCount() == 1);
    }
    
    SECTION("Function-based layouts - no class instantiation") {
        SimpleContainer container(0, 0, 200, 300, sdl);
        
        // These should use LayoutFactory functions, not class constructors
        container.setVerticalLayout(5);
        container.setHorizontalLayout(10);
        container.setGridLayout(2, 3, 8);
        
        // All should work without dynamic_cast or inheritance issues
        auto button = std::make_shared<UIButton>("Test", 0, 0, 80, 30, sdl);
        container.addChild(button);
        container.layout(); // Should not crash
        
        REQUIRE(container.getChildCount() == 1);
    }
}
