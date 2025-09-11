#include "catch2/catch.hpp"
#include "Interface/ui/Layout.h"
#include "Interface/ui/UILayoutContainer.h"
#include "Interface/ui/UIContainer.h"
#include "Interface/ui/UILabel.h"
#include "Systems/SDLManager.h"
#include <memory>

class MockComponent : public UIComponent {
public:
    MockComponent(int x, int y, int width, int height, SDLManager& sdl)
        : UIComponent(x, y, width, height, sdl), layoutCalled_(false), renderCalled_(false) {}

    void layout() override {
        layoutCalled_ = true;
    }

    void render() override {
        renderCalled_ = true;
    }

    bool wasLayoutCalled() const { return layoutCalled_; }
    bool wasRenderCalled() const { return renderCalled_; }
    void reset() { layoutCalled_ = false; renderCalled_ = false; }

private:
    bool layoutCalled_;
    bool renderCalled_;
};

TEST_CASE("VerticalLayout positions components correctly", "[ui][layout][vertical]") {
    SDLManager sdl;
    VerticalLayout layout(10); // 10px spacing
    
    std::vector<std::shared_ptr<UIComponent>> children;
    children.push_back(std::make_shared<MockComponent>(0, 0, 100, 30, sdl));
    children.push_back(std::make_shared<MockComponent>(0, 0, 100, 40, sdl));
    children.push_back(std::make_shared<MockComponent>(0, 0, 100, 20, sdl));
    
    layout.layoutChildren(children, 50, 50, 200, 200);
    
    // Check positions
    REQUIRE(children[0]->getX() == 50);
    REQUIRE(children[0]->getY() == 50);
    
    REQUIRE(children[1]->getX() == 50);
    REQUIRE(children[1]->getY() == 90); // 50 + 30 + 10
    
    REQUIRE(children[2]->getX() == 50);
    REQUIRE(children[2]->getY() == 140); // 90 + 40 + 10
    
    // Check preferred size calculation
    auto preferredSize = layout.calculatePreferredSize(children);
    REQUIRE(preferredSize.first == 100); // Max width
    REQUIRE(preferredSize.second == 110); // 30 + 40 + 20 + 2*10 spacing
}

TEST_CASE("HorizontalLayout positions components correctly", "[ui][layout][horizontal]") {
    SDLManager sdl;
    HorizontalLayout layout(15); // 15px spacing
    
    std::vector<std::shared_ptr<UIComponent>> children;
    children.push_back(std::make_shared<MockComponent>(0, 0, 80, 50, sdl));
    children.push_back(std::make_shared<MockComponent>(0, 0, 60, 50, sdl));
    children.push_back(std::make_shared<MockComponent>(0, 0, 70, 50, sdl));
    
    layout.layoutChildren(children, 20, 30, 300, 100);
    
    // Check positions
    REQUIRE(children[0]->getX() == 20);
    REQUIRE(children[0]->getY() == 30);
    
    REQUIRE(children[1]->getX() == 115); // 20 + 80 + 15
    REQUIRE(children[1]->getY() == 30);
    
    REQUIRE(children[2]->getX() == 190); // 115 + 60 + 15
    REQUIRE(children[2]->getY() == 30);
    
    // Check preferred size calculation
    auto preferredSize = layout.calculatePreferredSize(children);
    REQUIRE(preferredSize.first == 240); // 80 + 60 + 70 + 2*15 spacing
    REQUIRE(preferredSize.second == 50); // Max height
}

TEST_CASE("GridLayout positions components in grid", "[ui][layout][grid]") {
    SDLManager sdl;
    GridLayout layout(2, 3, 5); // 2 rows, 3 cols, 5px spacing
    
    std::vector<std::shared_ptr<UIComponent>> children;
    for (int i = 0; i < 6; ++i) {
        children.push_back(std::make_shared<MockComponent>(0, 0, 50, 40, sdl));
    }
    
    layout.layoutChildren(children, 0, 0, 320, 240);
    
    // Cell size should be (320 - 2*5) / 3 = 103 width, (240 - 1*5) / 2 = 117 height
    int expectedCellW = (320 - 10) / 3; // 103
    int expectedCellH = (240 - 5) / 2;  // 117
    
    // Check first row
    REQUIRE(children[0]->getX() == 0);
    REQUIRE(children[0]->getY() == 0);
    REQUIRE(children[0]->getWidth() == expectedCellW);
    REQUIRE(children[0]->getHeight() == expectedCellH);
    
    REQUIRE(children[1]->getX() == expectedCellW + 5);
    REQUIRE(children[1]->getY() == 0);
    
    // Check second row
    REQUIRE(children[3]->getX() == 0);
    REQUIRE(children[3]->getY() == expectedCellH + 5);
}

TEST_CASE("BorderLayout positions components in regions", "[ui][layout][border]") {
    SDLManager sdl;
    BorderLayout layout(10); // 10px gap
    
    auto north = std::make_shared<MockComponent>(0, 0, 200, 50, sdl);
    auto south = std::make_shared<MockComponent>(0, 0, 200, 30, sdl);
    auto west = std::make_shared<MockComponent>(0, 0, 80, 100, sdl);
    auto east = std::make_shared<MockComponent>(0, 0, 70, 100, sdl);
    auto center = std::make_shared<MockComponent>(0, 0, 100, 100, sdl);
    
    layout.addComponent(north, BorderPosition::NORTH);
    layout.addComponent(south, BorderPosition::SOUTH);
    layout.addComponent(west, BorderPosition::WEST);
    layout.addComponent(east, BorderPosition::EAST);
    layout.addComponent(center, BorderPosition::CENTER);
    
    std::vector<std::shared_ptr<UIComponent>> children = {north, south, west, east, center};
    layout.layoutChildren(children, 0, 0, 300, 200);
    
    // Check north position (full width at top)
    REQUIRE(north->getX() == 0);
    REQUIRE(north->getY() == 0);
    REQUIRE(north->getWidth() == 300);
    
    // Check south position (full width at bottom)
    REQUIRE(south->getX() == 0);
    REQUIRE(south->getY() == 170); // 200 - 30
    REQUIRE(south->getWidth() == 300);
    
    // Check west position (left side between north and south)
    REQUIRE(west->getX() == 0);
    REQUIRE(west->getY() == 60); // 50 + 10
    
    // Check center position (remaining space)
    REQUIRE(center->getX() == 90); // 80 + 10
    REQUIRE(center->getY() == 60); // 50 + 10
}

TEST_CASE("FlexLayout with different justify-content values", "[ui][layout][flex]") {
    SDLManager sdl;
    FlexLayout layout(FlexDirection::ROW);
    
    std::vector<std::shared_ptr<UIComponent>> children;
    children.push_back(std::make_shared<MockComponent>(0, 0, 50, 40, sdl));
    children.push_back(std::make_shared<MockComponent>(0, 0, 60, 40, sdl));
    children.push_back(std::make_shared<MockComponent>(0, 0, 40, 40, sdl));
    
    SECTION("FLEX_START justification") {
        layout.setJustifyContent(FlexJustify::FLEX_START);
        layout.layoutChildren(children, 0, 0, 300, 100);
        
        REQUIRE(children[0]->getX() == 0);
        REQUIRE(children[1]->getX() == 50);
        REQUIRE(children[2]->getX() == 110);
    }
    
    SECTION("CENTER justification") {
        layout.setJustifyContent(FlexJustify::CENTER);
        layout.layoutChildren(children, 0, 0, 300, 100);
        
        // Total width needed: 50 + 60 + 40 = 150
        // Free space: 300 - 150 = 150
        // Start position: 150 / 2 = 75
        REQUIRE(children[0]->getX() == 75);
        REQUIRE(children[1]->getX() == 125);
        REQUIRE(children[2]->getX() == 185);
    }
    
    SECTION("SPACE_BETWEEN justification") {
        layout.setJustifyContent(FlexJustify::SPACE_BETWEEN);
        layout.layoutChildren(children, 0, 0, 300, 100);
        
        REQUIRE(children[0]->getX() == 0);
        // Space between: 150 / 2 = 75
        REQUIRE(children[1]->getX() == 125); // 50 + 75
        REQUIRE(children[2]->getX() == 260); // 125 + 60 + 75
    }
}

TEST_CASE("AbsoluteLayout positions components at specified coordinates", "[ui][layout][absolute]") {
    SDLManager sdl;
    AbsoluteLayout layout;
    
    auto comp1 = std::make_shared<MockComponent>(0, 0, 100, 50, sdl);
    auto comp2 = std::make_shared<MockComponent>(0, 0, 80, 60, sdl);
    auto comp3 = std::make_shared<MockComponent>(0, 0, 90, 40, sdl);
    
    layout.setComponentPosition(comp1, {10, 20, -1, -1}); // Use original size
    layout.setComponentPosition(comp2, {150, 100, 120, 80}); // Override size
    layout.setComponentPosition(comp3, {50, 200, -1, -1}); // Use original size
    
    std::vector<std::shared_ptr<UIComponent>> children = {comp1, comp2, comp3};
    layout.layoutChildren(children, 100, 50, 400, 300);
    
    // Check positions (relative to container)
    REQUIRE(comp1->getX() == 110); // 100 + 10
    REQUIRE(comp1->getY() == 70);  // 50 + 20
    REQUIRE(comp1->getWidth() == 100); // Original width
    REQUIRE(comp1->getHeight() == 50); // Original height
    
    REQUIRE(comp2->getX() == 250); // 100 + 150
    REQUIRE(comp2->getY() == 150); // 50 + 100
    REQUIRE(comp2->getWidth() == 120); // Overridden width
    REQUIRE(comp2->getHeight() == 80); // Overridden height
    
    REQUIRE(comp3->getX() == 150); // 100 + 50
    REQUIRE(comp3->getY() == 250); // 50 + 200
}

TEST_CASE("UILayoutContainer integrates with layout managers", "[ui][container][layout]") {
    SDLManager sdl;
    
    SECTION("Container uses vertical layout by default") {
        UILayoutContainer container(0, 0, 200, 300, sdl);
        
        auto child1 = std::make_shared<MockComponent>(0, 0, 180, 50, sdl);
        auto child2 = std::make_shared<MockComponent>(0, 0, 180, 40, sdl);
        
        container.addChild(child1);
        container.addChild(child2);
        container.layout();
        
        REQUIRE(child1->getY() == 0);
        REQUIRE(child2->getY() == 50);
    }
    
    SECTION("Container can switch to horizontal layout") {
        UILayoutContainer container(0, 0, 400, 100, sdl);
        container.setHorizontalLayout(20); // 20px spacing
        
        auto child1 = std::make_shared<MockComponent>(0, 0, 80, 50, sdl);
        auto child2 = std::make_shared<MockComponent>(0, 0, 60, 50, sdl);
        
        container.addChild(child1);
        container.addChild(child2);
        container.layout();
        
        REQUIRE(child1->getX() == 0);
        REQUIRE(child2->getX() == 100); // 80 + 20
    }
    
    SECTION("Container calculates preferred size") {
        UILayoutContainer container(0, 0, 200, 200, sdl);
        container.setVerticalLayout(10);
        
        auto child1 = std::make_shared<MockComponent>(0, 0, 150, 30, sdl);
        auto child2 = std::make_shared<MockComponent>(0, 0, 120, 40, sdl);
        
        container.addChild(child1);
        container.addChild(child2);
        
        auto preferredSize = container.calculatePreferredSize();
        REQUIRE(preferredSize.first == 150); // Max width
        REQUIRE(preferredSize.second == 80); // 30 + 40 + 10 spacing
    }
}

TEST_CASE("Legacy UIContainer maintains backward compatibility", "[ui][container][legacy]") {
    SDLManager sdl;
    UIContainer container(50, 50, 200, 300, sdl);
    
    auto child1 = std::make_shared<MockComponent>(0, 0, 180, 50, sdl);
    auto child2 = std::make_shared<MockComponent>(0, 0, 180, 40, sdl);
    auto child3 = std::make_shared<MockComponent>(0, 0, 180, 35, sdl);
    
    container.addChild(child1);
    container.addChild(child2);
    container.addChild(child3);
    
    container.layout();
    
    // Should behave exactly like the old UIContainer (vertical layout)
    REQUIRE(child1->getX() == 50);
    REQUIRE(child1->getY() == 50);
    
    REQUIRE(child2->getX() == 50);
    REQUIRE(child2->getY() == 100);
    
    REQUIRE(child3->getX() == 50);
    REQUIRE(child3->getY() == 140);
    
    // Test scrolling
    container.setScrollOffset(25);
    container.layout();
    
    REQUIRE(child1->getY() == 25); // 50 - 25
    REQUIRE(child2->getY() == 75); // 100 - 25
    REQUIRE(child3->getY() == 115); // 140 - 25
}
