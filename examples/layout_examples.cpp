#include "Interface/ui/UILayoutContainer.h"
#include "Interface/ui/UILabel.h"
#include "Interface/ui/UIButton.h"
#include "Interface/ui/UITextInput.h"
#include "Systems/SDLManager.h"
#include "Constants.h"
#include <memory>
#include <iostream>

/**
 * Layout Examples - Demonstrates various layout managers
 * Shows how to use different layout types with UI components
 */

void createVerticalLayoutExample(SDLManager& sdl) {
    std::cout << "Creating Vertical Layout Example..." << std::endl;
    
    // Create container with vertical layout
    auto container = std::make_shared<UILayoutContainer>(50, 50, 300, 400, sdl);
    container->setVerticalLayout(10); // 10px spacing between items
    
    // Add some components
    auto title = std::make_shared<UILabel>(0, 0, 280, 30, sdl, "Vertical Layout");
    auto button1 = std::make_shared<UIButton>("Button 1", 0, 0, 280, 40, sdl, [](){});
    auto textInput = std::make_shared<UITextInput>(0, 0, 280, 35, sdl, "Enter text...");
    auto button2 = std::make_shared<UIButton>("Button 2", 0, 0, 280, 40, sdl, [](){});
    
    container->addChild(title);
    container->addChild(button1);
    container->addChild(textInput);
    container->addChild(button2);
}

void createHorizontalLayoutExample(SDLManager& sdl) {
    std::cout << "Creating Horizontal Layout Example..." << std::endl;
    
    // Create container with horizontal layout
    auto container = std::make_shared<UILayoutContainer>(400, 50, 500, 60, sdl);
    container->setHorizontalLayout(15); // 15px spacing between items
    container->setScrollable(false); // Disable scrolling for horizontal layout
    
    // Add some components
    auto button1 = std::make_shared<UIButton>("Save", 0, 0, 100, 50, sdl, [](){});
    auto button2 = std::make_shared<UIButton>("Load", 0, 0, 100, 50, sdl, [](){});
    auto button3 = std::make_shared<UIButton>("Exit", 0, 0, 100, 50, sdl, [](){});
    
    container->addChild(button1);
    container->addChild(button2);
    container->addChild(button3);
}

void createGridLayoutExample(SDLManager& sdl) {
    std::cout << "Creating Grid Layout Example..." << std::endl;
    
    // Create container with 2x3 grid layout
    auto container = std::make_shared<UILayoutContainer>(50, 500, 320, 240, sdl);
    container->setGridLayout(2, 3, 5); // 2 rows, 3 columns, 5px spacing
    container->setScrollable(false);
    
    // Add 6 buttons to fill the grid
    for (int i = 1; i <= 6; ++i) {
        std::string buttonText = "Grid " + std::to_string(i);
        auto button = std::make_shared<UIButton>(buttonText, 0, 0, 100, 50, sdl, [](){});
        container->addChild(button);
    }
}

void createBorderLayoutExample(SDLManager& sdl) {
    std::cout << "Creating Border Layout Example..." << std::endl;
    
    // Create container with border layout
    auto container = std::make_shared<UILayoutContainer>(400, 150, 400, 300, sdl);
    container->setBorderLayout(10); // 10px gap between regions
    container->setScrollable(false);
    
    // Create components for each border region
    auto northLabel = std::make_shared<UILabel>(0, 0, 380, 40, sdl, "North Header");
    auto southLabel = std::make_shared<UILabel>(0, 0, 380, 30, sdl, "South Footer");
    auto westButton = std::make_shared<UIButton>("West", 0, 0, 80, 200, sdl, [](){});
    auto eastButton = std::make_shared<UIButton>("East", 0, 0, 80, 200, sdl, [](){});
    auto centerTextInput = std::make_shared<UITextInput>(0, 0, 200, 200, sdl, "Center Content");
    
    // Add components to specific border positions
    container->addBorderComponent(northLabel, BorderPosition::NORTH);
    container->addBorderComponent(southLabel, BorderPosition::SOUTH);
    container->addBorderComponent(westButton, BorderPosition::WEST);
    container->addBorderComponent(eastButton, BorderPosition::EAST);
    container->addBorderComponent(centerTextInput, BorderPosition::CENTER);
}

void createFlexLayoutExample(SDLManager& sdl) {
    std::cout << "Creating Flex Layout Example..." << std::endl;
    
    // Create container with flex layout
    auto container = std::make_shared<UILayoutContainer>(50, 800, 500, 80, sdl);
    auto flexLayout = std::make_unique<FlexLayout>(FlexDirection::ROW);
    flexLayout->setJustifyContent(FlexJustify::SPACE_BETWEEN);
    flexLayout->setAlignItems(FlexAlign::CENTER);
    flexLayout->setGap(10);
    container->setLayout(std::move(flexLayout));
    container->setScrollable(false);
    
    // Add buttons with different sizes
    auto smallBtn = std::make_shared<UIButton>("Small", 0, 0, 80, 30, sdl, [](){});
    auto mediumBtn = std::make_shared<UIButton>("Medium", 0, 0, 120, 50, sdl, [](){});
    auto largeBtn = std::make_shared<UIButton>("Large", 0, 0, 100, 70, sdl, [](){});
    
    container->addChild(smallBtn);
    container->addChild(mediumBtn);
    container->addChild(largeBtn);
}

void createAbsoluteLayoutExample(SDLManager& sdl) {
    std::cout << "Creating Absolute Layout Example..." << std::endl;
    
    // Create container with absolute layout
    auto container = std::make_shared<UILayoutContainer>(600, 500, 300, 300, sdl);
    container->setAbsoluteLayout();
    container->setScrollable(false);
    
    // Add components at specific positions
    auto title = std::make_shared<UILabel>(0, 0, 200, 30, sdl, "Absolute Layout");
    auto floatingBtn1 = std::make_shared<UIButton>("Btn 1", 0, 0, 80, 30, sdl, [](){});
    auto floatingBtn2 = std::make_shared<UIButton>("Btn 2", 0, 0, 80, 30, sdl, [](){});
    auto cornerLabel = std::make_shared<UILabel>(0, 0, 100, 20, sdl, "Corner");
    
    // Set absolute positions
    container->setAbsolutePosition(title, {10, 10, 200, 30});
    container->setAbsolutePosition(floatingBtn1, {50, 100, 80, 30});
    container->setAbsolutePosition(floatingBtn2, {150, 150, 80, 30});
    container->setAbsolutePosition(cornerLabel, {200, 270, 100, 20});
}

void createComplexLayoutExample(SDLManager& sdl) {
    std::cout << "Creating Complex Nested Layout Example..." << std::endl;
    
    // Main container with border layout
    auto mainContainer = std::make_shared<UILayoutContainer>(50, 950, 800, 400, sdl);
    mainContainer->setBorderLayout(5);
    mainContainer->setScrollable(false);
    
    // Header (North) with horizontal layout
    auto header = std::make_shared<UILayoutContainer>(0, 0, 790, 50, sdl);
    header->setHorizontalLayout(10);
    header->setScrollable(false);
    
    auto logoLabel = std::make_shared<UILabel>(0, 0, 100, 40, sdl, "LOGO");
    auto menuBtn1 = std::make_shared<UIButton>("File", 0, 0, 80, 40, sdl, [](){});
    auto menuBtn2 = std::make_shared<UIButton>("Edit", 0, 0, 80, 40, sdl, [](){});
    auto menuBtn3 = std::make_shared<UIButton>("View", 0, 0, 80, 40, sdl, [](){});
    
    header->addChild(logoLabel);
    header->addChild(menuBtn1);
    header->addChild(menuBtn2);
    header->addChild(menuBtn3);
    
    // Sidebar (West) with vertical layout
    auto sidebar = std::make_shared<UILayoutContainer>(0, 0, 150, 300, sdl);
    sidebar->setVerticalLayout(5);
    
    auto toolBtn1 = std::make_shared<UIButton>("Tool 1", 0, 0, 140, 35, sdl, [](){});
    auto toolBtn2 = std::make_shared<UIButton>("Tool 2", 0, 0, 140, 35, sdl, [](){});
    auto toolBtn3 = std::make_shared<UIButton>("Tool 3", 0, 0, 140, 35, sdl, [](){});
    
    sidebar->addChild(toolBtn1);
    sidebar->addChild(toolBtn2);
    sidebar->addChild(toolBtn3);
    
    // Content area (Center) with grid layout
    auto contentArea = std::make_shared<UILayoutContainer>(0, 0, 635, 300, sdl);
    contentArea->setGridLayout(2, 2, 10); // 2x2 grid
    contentArea->setScrollable(false);
    
    auto contentPanel1 = std::make_shared<UILabel>(0, 0, 300, 140, sdl, "Content 1");
    auto contentPanel2 = std::make_shared<UILabel>(0, 0, 300, 140, sdl, "Content 2");
    auto contentPanel3 = std::make_shared<UILabel>(0, 0, 300, 140, sdl, "Content 3");
    auto contentPanel4 = std::make_shared<UILabel>(0, 0, 300, 140, sdl, "Content 4");
    
    contentArea->addChild(contentPanel1);
    contentArea->addChild(contentPanel2);
    contentArea->addChild(contentPanel3);
    contentArea->addChild(contentPanel4);
    
    // Footer (South) with flex layout
    auto footer = std::make_shared<UILayoutContainer>(0, 0, 790, 40, sdl);
    auto footerFlex = std::make_unique<FlexLayout>(FlexDirection::ROW);
    footerFlex->setJustifyContent(FlexJustify::SPACE_BETWEEN);
    footerFlex->setAlignItems(FlexAlign::CENTER);
    footer->setLayout(std::move(footerFlex));
    footer->setScrollable(false);
    
    auto statusLabel = std::make_shared<UILabel>(0, 0, 200, 30, sdl, "Ready");
    auto progressLabel = std::make_shared<UILabel>(0, 0, 150, 30, sdl, "Progress: 100%");
    
    footer->addChild(statusLabel);
    footer->addChild(progressLabel);
    
    // Assemble the main layout
    mainContainer->addBorderComponent(header, BorderPosition::NORTH);
    mainContainer->addBorderComponent(sidebar, BorderPosition::WEST);
    mainContainer->addBorderComponent(contentArea, BorderPosition::CENTER);
    mainContainer->addBorderComponent(footer, BorderPosition::SOUTH);
}

int main() {
    SDLManager sdl;
    
    if (!sdl.initialize()) {
        std::cerr << "Failed to initialize SDL!" << std::endl;
        return -1;
    }
    
    // Get SDL components from SDLManager
    SDL_Window* window = sdl.getWindow();
    SDL_Renderer* renderer = sdl.getRenderer();
    
    if (!window || !renderer) {
        std::cerr << "Failed to get SDL window or renderer!" << std::endl;
        return -1;
    }
    
    // Set window title
    SDL_SetWindowTitle(window, "UI Framework Layout Examples");
    
    // Create layout examples
    std::cout << "Creating layout examples..." << std::endl;
    
    // Create containers for examples
    auto verticalExample = std::make_shared<UILayoutContainer>(50, 50, 300, 400, sdl);
    verticalExample->setVerticalLayout(10);
    
    auto title1 = std::make_shared<UILabel>(0, 0, 280, 30, sdl, "Vertical Layout");
    title1->setBackgroundColor({100, 150, 200, 255}); // Light blue background
    title1->setTextColor({255, 255, 255, 255}); // White text
    
    auto button1 = std::make_shared<UIButton>("Button 1", 0, 0, 280, 40, sdl, [](){
        std::cout << "Vertical Button 1 clicked!" << std::endl;
    });
    auto textInput1 = std::make_shared<UITextInput>(0, 0, 280, 35, sdl, "Enter text...");
    auto button2 = std::make_shared<UIButton>("Button 2", 0, 0, 280, 40, sdl, [](){
        std::cout << "Vertical Button 2 clicked!" << std::endl;
    });
    
    verticalExample->addChild(title1);
    verticalExample->addChild(button1);
    verticalExample->addChild(textInput1);
    verticalExample->addChild(button2);
    verticalExample->layout();
    
    // Horizontal layout example
    auto horizontalExample = std::make_shared<UILayoutContainer>(400, 50, 350, 60, sdl);
    horizontalExample->setHorizontalLayout(15);
    horizontalExample->setScrollable(false);
    
    auto save_btn = std::make_shared<UIButton>("Save", 0, 0, 100, 50, sdl, [](){
        std::cout << "Save clicked!" << std::endl;
    });
    auto load_btn = std::make_shared<UIButton>("Load", 0, 0, 100, 50, sdl, [](){
        std::cout << "Load clicked!" << std::endl;
    });
    auto exit_btn = std::make_shared<UIButton>("Exit", 0, 0, 100, 50, sdl, [](){
        std::cout << "Exit clicked!" << std::endl;
    });
    
    horizontalExample->addChild(save_btn);
    horizontalExample->addChild(load_btn);
    horizontalExample->addChild(exit_btn);
    horizontalExample->layout();
    
    // Grid layout example
    auto gridExample = std::make_shared<UILayoutContainer>(800, 50, 300, 200, sdl);
    gridExample->setGridLayout(2, 2, 10);
    gridExample->setScrollable(false);
    
    auto grid_btn1 = std::make_shared<UIButton>("Grid 1", 0, 0, 140, 90, sdl, [](){
        std::cout << "Grid 1 clicked!" << std::endl;
    });
    auto grid_btn2 = std::make_shared<UIButton>("Grid 2", 0, 0, 140, 90, sdl, [](){
        std::cout << "Grid 2 clicked!" << std::endl;
    });
    auto grid_btn3 = std::make_shared<UIButton>("Grid 3", 0, 0, 140, 90, sdl, [](){
        std::cout << "Grid 3 clicked!" << std::endl;
    });
    auto grid_btn4 = std::make_shared<UIButton>("Grid 4", 0, 0, 140, 90, sdl, [](){
        std::cout << "Grid 4 clicked!" << std::endl;
    });
    
    gridExample->addChild(grid_btn1);
    gridExample->addChild(grid_btn2);
    gridExample->addChild(grid_btn3);
    gridExample->addChild(grid_btn4);
    gridExample->layout();
    
    // Border layout example
    auto borderExample = std::make_shared<UILayoutContainer>(50, 500, 600, 250, sdl);
    borderExample->setBorderLayout(5);
    borderExample->setScrollable(false);
    
    auto north_panel = std::make_shared<UILabel>(0, 0, 0, 40, sdl, "North Panel");
    north_panel->setBackgroundColor({200, 100, 100, 255}); // Light red
    north_panel->setTextColor({255, 255, 255, 255});
    
    auto south_panel = std::make_shared<UILabel>(0, 0, 0, 40, sdl, "South Panel");
    south_panel->setBackgroundColor({100, 200, 100, 255}); // Light green
    south_panel->setTextColor({255, 255, 255, 255});
    
    auto west_panel = std::make_shared<UILabel>(0, 0, 80, 0, sdl, "West");
    west_panel->setBackgroundColor({100, 100, 200, 255}); // Light blue
    west_panel->setTextColor({255, 255, 255, 255});
    
    auto east_panel = std::make_shared<UILabel>(0, 0, 80, 0, sdl, "East");
    east_panel->setBackgroundColor({200, 200, 100, 255}); // Light yellow
    east_panel->setTextColor({0, 0, 0, 255});
    
    auto center_panel = std::make_shared<UILabel>(0, 0, 0, 0, sdl, "Center Content");
    center_panel->setBackgroundColor({150, 150, 150, 255}); // Gray
    center_panel->setTextColor({255, 255, 255, 255});
    
    borderExample->addBorderComponent(north_panel, BorderPosition::NORTH);
    borderExample->addBorderComponent(south_panel, BorderPosition::SOUTH);
    borderExample->addBorderComponent(west_panel, BorderPosition::WEST);
    borderExample->addBorderComponent(east_panel, BorderPosition::EAST);
    borderExample->addBorderComponent(center_panel, BorderPosition::CENTER);
    borderExample->layout();
    
    std::cout << "Layout examples created successfully!" << std::endl;
    std::cout << "Press ESC to exit." << std::endl;
    
    // Main render loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            
            // Pass events to UI components
            verticalExample->handleEvent(event);
            horizontalExample->handleEvent(event);
            gridExample->handleEvent(event);
            borderExample->handleEvent(event);
        }
        
        // Clear screen with light gray background
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);
        
        // Draw visible borders around each layout container for debugging
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red borders
        
        // Vertical example border
        SDL_Rect vBorder = {45, 45, 310, 410};
        SDL_RenderDrawRect(renderer, &vBorder);
        
        // Horizontal example border  
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green border
        SDL_Rect hBorder = {395, 45, 360, 70};
        SDL_RenderDrawRect(renderer, &hBorder);
        
        // Grid example border
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue border
        SDL_Rect gBorder = {795, 45, 310, 210};
        SDL_RenderDrawRect(renderer, &gBorder);
        
        // Border example border
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); // Magenta border
        SDL_Rect bBorder = {45, 495, 610, 260};
        SDL_RenderDrawRect(renderer, &bBorder);
        
        // Render all examples
        verticalExample->render();
        horizontalExample->render();
        gridExample->render();
        borderExample->render();
        
        // Present the rendered frame
        SDL_RenderPresent(renderer);
        
        // Small delay to prevent excessive CPU usage
        SDL_Delay(16); // ~60 FPS
    }
    
    return 0;
}
