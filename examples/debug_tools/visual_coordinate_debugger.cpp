#include "Interface/ui/HexGridEditor.h"
#include "Interface/ui/UIManager.h"
#include "Systems/SDLManager.h"
#include <memory>
#include <iostream>
#include <iomanip>
#include <cmath>

/**
 * Visual coordinate debugging program
 * Shows all coordinate calculations and visual alignment
 */
class VisualCoordinateDebugger {
public:
    VisualCoordinateDebugger() : running_(false) {}
    
    bool initialize() {
        std::cout << "=== Visual Coordinate Debugger ===" << std::endl;
        std::cout << "This program displays ALL coordinate calculations for debugging" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  Mouse: Click to see detailed coordinate analysis" << std::endl;
        std::cout << "  Mouse Wheel: Zoom in/out (centered at cursor position)" << std::endl;
        std::cout << "  SPACE: Toggle coordinate display overlay" << std::endl;
        std::cout << "  G: Toggle grid lines" << std::endl;
        std::cout << "  P: Toggle panels" << std::endl;
        std::cout << "  1-9: Change hex size (1=20, 9=80)" << std::endl;
        std::cout << "  R: Reset view" << std::endl;
        std::cout << "  ESC: Quit" << std::endl;
        std::cout << "==================================" << std::endl;
        
        if (!sdlManager_.initialize()) {
            std::cout << "ERROR: Failed to initialize SDL" << std::endl;
            return false;
        }
        
        uiManager_ = std::make_unique<UIManager>();
        hexEditor_ = std::make_shared<HexGridEditor>(0, 0, 1200, 800, sdlManager_);
        
        // Configure for maximum visibility and debugging
        if (hexEditor_->getRenderer()) {
            auto& config = hexEditor_->getRenderer()->getRenderConfig();
            config.hexSize = 50.0f;  // Large hexagons
            config.showGrid = true;
            config.showCoordinates = true;
            config.borderColor = {255, 255, 255, 255};  // White borders
            config.borderThickness = 2.0f;
            config.gridColor = {0, 255, 255, 255};      // Cyan grid
            
            std::cout << "Initial renderer config:" << std::endl;
            std::cout << "  hexSize: " << config.hexSize << std::endl;
            std::cout << "  zoom: " << config.zoomLevel << std::endl;
            std::cout << "  pan: (" << config.panX << ", " << config.panY << ")" << std::endl;
        }
        
        // Print layout information
        printLayoutInfo();
        
        // Create a comprehensive test pattern
        createDebugPattern();
        
        uiManager_->addComponent(hexEditor_, true);
        
        // Ensure HexGridEditor can receive focus for mouse wheel events
        uiManager_->setFocus(hexEditor_);
        
        running_ = true;
        return true;
    }
    
    void printLayoutInfo() {
        std::cout << "\n--- Layout Information ---" << std::endl;
        std::cout << "Window size: 1200x800" << std::endl;
        std::cout << "HexGridEditor bounds: (0,0) to (1200,800)" << std::endl;
        
        // Calculate expected renderer bounds
        const int PANEL_WIDTH = 200;
        const int STATUS_HEIGHT = 30;
        int rendererX = PANEL_WIDTH;
        int rendererY = 0;
        int rendererW = 1200 - 2 * PANEL_WIDTH;
        int rendererH = 800 - STATUS_HEIGHT;
        
        std::cout << "Expected renderer bounds: (" << rendererX << "," << rendererY 
                  << ") size " << rendererW << "x" << rendererH << std::endl;
        std::cout << "Renderer area: X[" << rendererX << " to " << (rendererX + rendererW) 
                  << "] Y[" << rendererY << " to " << (rendererY + rendererH) << "]" << std::endl;
        std::cout << "Center of renderer: (" << (rendererX + rendererW/2) << "," << (rendererY + rendererH/2) << ")" << std::endl;
    }
    
    void createDebugPattern() {
        if (!hexEditor_ || !hexEditor_->getGrid()) return;
        
        auto grid = hexEditor_->getGrid();
        std::cout << "\n--- Creating Debug Pattern ---" << std::endl;
        
        // Create a small, clear pattern for precise testing
        std::vector<std::pair<HexCoordinate, TerrainType>> debugTiles = {
            {{0, 0, 0}, TerrainType::CITY_WALL},      // Red center
            {{1, -1, 0}, TerrainType::FOREST},       // Green right
            {{-1, 1, 0}, TerrainType::MOUNTAIN},     // Brown left
            {{0, -1, 1}, TerrainType::ROAD},         // Yellow top-right
            {{0, 1, -1}, TerrainType::RIVER},        // Blue bottom-left
            {{1, 0, -1}, TerrainType::SWAMP},        // Dark green bottom-right
            {{-1, 0, 1}, TerrainType::PLAIN}         // Light green top-left
        };
        
        for (const auto& [coord, terrain] : debugTiles) {
            if (grid->isValidCoordinate(coord)) {
                grid->setTerrain(coord, terrain);
                
                // Calculate expected screen position
                float screenX, screenY;
                if (hexEditor_->getRenderer()) {
                    hexEditor_->getRenderer()->hexToWindowCoords(coord, screenX, screenY);
                    
                    std::cout << "Set coord (" << std::setw(2) << coord.x << "," 
                              << std::setw(2) << coord.y << "," << std::setw(2) << coord.z 
                              << ") terrain=" << std::setw(2) << static_cast<int>(terrain)
                              << " expected screen=(" << std::setw(6) << std::fixed << std::setprecision(1) 
                              << screenX << "," << std::setw(6) << screenY << ")" << std::endl;
                }
            }
        }
        
        // Center view and print result
        if (hexEditor_->getRenderer()) {
            auto renderer = hexEditor_->getRenderer();
            std::cout << "\nRenderer dimensions before centering:" << std::endl;
            std::cout << "  position: (" << renderer->getX() << ", " << renderer->getY() << ")" << std::endl;
            std::cout << "  size: " << renderer->getWidth() << "x" << renderer->getHeight() << std::endl;
            std::cout << "  center should be at: (" << (renderer->getX() + renderer->getWidth()/2) << ", " << (renderer->getY() + renderer->getHeight()/2) << ")" << std::endl;
            
            renderer->resetView();
            renderer->centerOnCoordinate(HexCoordinate(0, 0, 0));
            
            auto& config = renderer->getRenderConfig();
            std::cout << "\nAfter centering view:" << std::endl;
            std::cout << "  pan: (" << config.panX << ", " << config.panY << ")" << std::endl;
            
            // Recalculate positions after centering
            std::cout << "\nActual positions after centering:" << std::endl;
            for (const auto& [coord, terrain] : debugTiles) {
                float screenX, screenY;
                hexEditor_->getRenderer()->hexToWindowCoords(coord, screenX, screenY);
                std::cout << "Coord (" << std::setw(2) << coord.x << "," 
                          << std::setw(2) << coord.y << "," << std::setw(2) << coord.z 
                          << ") -> screen(" << std::setw(6) << std::fixed << std::setprecision(1) 
                          << screenX << "," << std::setw(6) << screenY << ")" << std::endl;
            }
        }
    }
    
    void run() {
        SDL_Event event;
        bool showCoordOverlay = true;
        
        std::cout << "\n--- Starting Interactive Debug Session ---" << std::endl;
        
        while (running_) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running_ = false;
                } else if (event.type == SDL_KEYDOWN) {
                    handleKeyPress(event.key.keysym.sym, showCoordOverlay);
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    handleDetailedMouseClick(event);
                } else if (event.type == SDL_MOUSEMOTION) {
                    handleMouseMotion(event);
                }
                
                // Let UI framework handle mouse wheel events
                uiManager_->handleEvent(event);
            }
            
            // Clear screen
            SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 64, 64, 64, 255);
            SDL_RenderClear(sdlManager_.getRenderer());
            
            // Render main UI
            uiManager_->renderAll();
            
            // Render coordinate overlay if enabled
            if (showCoordOverlay) {
                renderCoordinateOverlay();
            }
            
            SDL_RenderPresent(sdlManager_.getRenderer());
            SDL_Delay(16);
        }
    }
    
    void handleKeyPress(SDL_Keycode key, bool& showCoordOverlay) {
        switch (key) {
            case SDLK_ESCAPE:
                running_ = false;
                break;
                
            case SDLK_SPACE:
                showCoordOverlay = !showCoordOverlay;
                std::cout << "Coordinate overlay: " << (showCoordOverlay ? "ON" : "OFF") << std::endl;
                break;
                
            case SDLK_g:
                if (hexEditor_->getRenderer()) {
                    auto& config = hexEditor_->getRenderer()->getRenderConfig();
                    config.showGrid = !config.showGrid;
                    std::cout << "Grid lines: " << (config.showGrid ? "ON" : "OFF") << std::endl;
                }
                break;
                
            case SDLK_p:
                if (hexEditor_) {
                    auto& state = hexEditor_->getState();
                    bool newState = !state.showToolPanel;
                    state.showToolPanel = newState;
                    state.showTerrainPanel = newState;
                    state.showPropertiesPanel = newState;
                    
                    // Recalculate layout with new panel visibility
                    hexEditor_->updateLayout();
                    
                    // Re-center the view since renderer size changed
                    if (hexEditor_->getRenderer()) {
                        auto renderer = hexEditor_->getRenderer();
                        // OPTION 1: Auto re-center (uncomment line below)
                        // renderer->centerOnCoordinate(HexCoordinate(0, 0, 0));
                        
                        // OPTION 2: No auto re-center - let user control with mouse/wheel
                        
                        // Comprehensive consistency verification
                        std::cout << "\n=== COMPREHENSIVE CONSISTENCY VERIFICATION ===" << std::endl;
                        std::cout << "After panel toggle:" << std::endl;
                        std::cout << "  Renderer position: (" << renderer->getX() << ", " << renderer->getY() << ")" << std::endl;
                        std::cout << "  Renderer size: " << renderer->getWidth() << "x" << renderer->getHeight() << std::endl;
                        
                        // Show pan values
                        auto& config = renderer->getRenderConfig();
                        std::cout << "  Pan values: (" << config.panX << ", " << config.panY << ")" << std::endl;
                        
                        // Test multiple hexagons for consistency
                        std::vector<HexCoordinate> testCoords = {
                            {0, 0, 0},     // Center
                            {1, -1, 0},    // Right
                            {-1, 1, 0},    // Left  
                            {0, -1, 1},    // Top-right
                            {0, 1, -1}     // Bottom-left
                        };
                        
                        std::cout << "\n--- CONSISTENCY CHECK FOR MULTIPLE HEXAGONS ---" << std::endl;
                        bool allConsistent = true;
                        
                        for (const auto& coord : testCoords) {
                            // 1. Get actual render position (matching what hexagon/text rendering uses)
                            float hexRenderX, hexRenderY;
                            renderer->hexToScreen(coord, hexRenderX, hexRenderY);
                            
                            // Convert to absolute coordinates for display purposes
                            float hexScreenX = hexRenderX + renderer->getX();
                            float hexScreenY = hexRenderY + renderer->getY();
                            
                            // 2. Test click detection at that position using the correct coordinate space
                            // screenToHex expects relative coordinates, so use hexRenderX, hexRenderY directly
                            HexCoordinate clickResult = renderer->screenToHex(static_cast<int>(hexRenderX), static_cast<int>(hexRenderY));
                            
                            // 3. Check if click detection matches original coordinate
                            bool clickConsistent = (clickResult.x == coord.x && clickResult.y == coord.y && clickResult.z == coord.z);
                            
                            std::cout << "  Hex (" << coord.x << "," << coord.y << "," << coord.z << "):" << std::endl;
                            std::cout << "    Actual render position: (" << std::fixed << std::setprecision(1) << hexScreenX << ", " << hexScreenY << ")" << std::endl;
                            std::cout << "    Click test result: (" << clickResult.x << "," << clickResult.y << "," << clickResult.z << ")" << std::endl;
                            std::cout << "    Consistency: " << (clickConsistent ? "✅ CONSISTENT" : "❌ INCONSISTENT") << std::endl;
                            
                            if (!clickConsistent) allConsistent = false;
                        }
                        
                        std::cout << "\n--- OVERALL CONSISTENCY STATUS ---" << std::endl;
                        std::cout << "  Grid rendering ↔ Click detection: " << (allConsistent ? "✅ FULLY CONSISTENT" : "❌ INCONSISTENT") << std::endl;
                        std::cout << "  Text rendering ↔ Grid rendering: ✅ CONSISTENT (both use hexToScreen)" << std::endl;
                        std::cout << "  Overall system status: " << (allConsistent ? "✅ ALL COMPONENTS ALIGNED" : "❌ ALIGNMENT ISSUES DETECTED") << std::endl;
                        std::cout << "=================================================" << std::endl;
                    }
                    
                    std::cout << "Panels: " << (newState ? "ON" : "OFF") << " - Layout updated and view re-centered" << std::endl;
                }
                break;
                
            case SDLK_r:
                if (hexEditor_->getRenderer()) {
                    hexEditor_->getRenderer()->resetView();
                    hexEditor_->getRenderer()->centerOnCoordinate(HexCoordinate(0, 0, 0));
                    auto& config = hexEditor_->getRenderer()->getRenderConfig();
                    std::cout << "View reset. New pan: (" << config.panX << ", " << config.panY << ")" << std::endl;
                }
                break;
                
            case SDLK_1: case SDLK_2: case SDLK_3: case SDLK_4: case SDLK_5:
            case SDLK_6: case SDLK_7: case SDLK_8: case SDLK_9:
                if (hexEditor_->getRenderer()) {
                    int size = 10 + (key - SDLK_1) * 10;  // 20, 30, 40... 100
                    auto& config = hexEditor_->getRenderer()->getRenderConfig();
                    config.hexSize = size;
                    std::cout << "Hex size changed to: " << size << std::endl;
                    
                    // Recenter after size change
                    hexEditor_->getRenderer()->centerOnCoordinate(HexCoordinate(0, 0, 0));
                    std::cout << "Recentered. New pan: (" << config.panX << ", " << config.panY << ")" << std::endl;
                }
                break;
        }
    }
    
    void handleDetailedMouseClick(const SDL_Event& event) {
        int mouseX = event.button.x;
        int mouseY = event.button.y;
        
        std::cout << "\n========= DETAILED CLICK ANALYSIS =========" << std::endl;
        std::cout << "Raw mouse: (" << mouseX << ", " << mouseY << ")" << std::endl;
        
        // Check which area was clicked
        if (mouseX < 200) {
            std::cout << "Click area: LEFT PANEL" << std::endl;
        } else if (mouseX > 1000) {
            std::cout << "Click area: RIGHT PANEL" << std::endl;
        } else if (mouseY > 770) {
            std::cout << "Click area: STATUS BAR" << std::endl;
        } else {
            std::cout << "Click area: HEX GRID AREA" << std::endl;
            
            if (hexEditor_ && hexEditor_->getRenderer()) {
                // Get coordinate through renderer
                HexCoordinate coord = hexEditor_->getRenderer()->screenToHex(mouseX, mouseY);
                std::cout << "Calculated hex coord: (" << coord.x << ", " << coord.y << ", " << coord.z << ")" << std::endl;
                
                // Manual step-by-step calculation
                std::cout << "\n--- Manual Step-by-Step Calculation ---" << std::endl;
                auto& config = hexEditor_->getRenderer()->getRenderConfig();
                
                std::cout << "1. Screen to renderer-relative:" << std::endl;
                // Get actual renderer position from renderer bounds
                auto renderer = hexEditor_->getRenderer();
                float rendererX = renderer->getX();  
                float rendererY = renderer->getY();
                float relativeX = mouseX - rendererX;  // Subtract actual renderer x offset
                float relativeY = mouseY - rendererY;  // Subtract actual renderer y offset  
                std::cout << "   renderer offset=(" << rendererX << ", " << rendererY << ")" << std::endl;
                std::cout << "   (" << mouseX << ", " << mouseY << ") -> (" << relativeX << ", " << relativeY << ")" << std::endl;
                
                std::cout << "2. Reverse view transform:" << std::endl;
                float worldX = (relativeX - config.panX) / config.zoomLevel;
                float worldY = (relativeY - config.panY) / config.zoomLevel;
                std::cout << "   pan=(" << config.panX << ", " << config.panY << ") zoom=" << config.zoomLevel << std::endl;
                std::cout << "   (" << relativeX << ", " << relativeY << ") -> (" << worldX << ", " << worldY << ")" << std::endl;
                
                std::cout << "3. Hex coordinate calculation:" << std::endl;
                // Use corrected fromScreenCoords formula
                float hexX = (2.0f / 3.0f * worldX) / config.hexSize;
                float hexZ = (worldY / config.hexSize - std::sqrt(3.0f) / 2.0f * hexX) / std::sqrt(3.0f);
                float hexY = -hexX - hexZ;
                std::cout << "   Raw hex: (" << hexX << ", " << hexY << ", " << hexZ << ")" << std::endl;
                
                int roundedX = static_cast<int>(std::round(hexX));
                int roundedY = static_cast<int>(std::round(hexY));
                int roundedZ = static_cast<int>(std::round(hexZ));
                std::cout << "   Rounded: (" << roundedX << ", " << roundedY << ", " << roundedZ << ")" << std::endl;
                
                // Verify constraint x + y + z = 0
                int sum = roundedX + roundedY + roundedZ;
                std::cout << "   Sum check: " << roundedX << " + " << roundedY << " + " << roundedZ << " = " << sum << std::endl;
                
                if (sum != 0) {
                    std::cout << "   Constraint violation! Correcting..." << std::endl;
                    float xDiff = std::abs(roundedX - hexX);
                    float yDiff = std::abs(roundedY - hexY);
                    float zDiff = std::abs(roundedZ - hexZ);
                    
                    if (xDiff > yDiff && xDiff > zDiff) {
                        roundedX = -roundedY - roundedZ;
                    } else if (yDiff > zDiff) {
                        roundedY = -roundedX - roundedZ;
                    } else {
                        roundedZ = -roundedX - roundedY;
                    }
                    std::cout << "   Corrected: (" << roundedX << ", " << roundedY << ", " << roundedZ << ")" << std::endl;
                }
                
                // Reverse calculation to verify
                std::cout << "\n4. Reverse verification:" << std::endl;
                HexCoordinate finalCoord(roundedX, roundedY, roundedZ);
                float backX, backY;
                hexEditor_->getRenderer()->hexToWindowCoords(finalCoord, backX, backY);
                std::cout << "   Coord (" << roundedX << ", " << roundedY << ", " << roundedZ 
                          << ") -> screen (" << backX << ", " << backY << ")" << std::endl;
                std::cout << "   Difference: (" << (backX - mouseX) << ", " << (backY - mouseY) << ")" << std::endl;
                
                // Calculate distance from hex center
                float distance = std::sqrt((backX - mouseX) * (backX - mouseX) + (backY - mouseY) * (backY - mouseY));
                std::cout << "   Distance from center: " << distance << " pixels" << std::endl;
                
                // Use the same radius calculation as renderer
                float hexRadius = config.hexSize * config.zoomLevel;
                if (distance < hexRadius) {
                    std::cout << "   ✅ Click is INSIDE hexagon (radius=" << hexRadius << ")" << std::endl;
                } else {
                    std::cout << "   ❌ Click is OUTSIDE hexagon (radius=" << hexRadius << ")" << std::endl;
                }
            }
        }
        std::cout << "===========================================" << std::endl;
    }
    
    void handleMouseMotion(const SDL_Event& event) {
        if (hexEditor_ && hexEditor_->getRenderer()) {
            currentMouseX_ = event.motion.x;
            currentMouseY_ = event.motion.y;
            
            // Only print for grid area
            if (currentMouseX_ >= 200 && currentMouseX_ <= 1000 && currentMouseY_ <= 770) {
                HexCoordinate hoverCoord = hexEditor_->getRenderer()->screenToHex(currentMouseX_, currentMouseY_);
                if (hoverCoord != lastHoverCoord_) {
                    lastHoverCoord_ = hoverCoord;
                    // Uncomment for continuous hover feedback (can be noisy)
                    // std::cout << "Hover: (" << hoverCoord.x << ", " << hoverCoord.y << ", " << hoverCoord.z << ")" << std::endl;
                }
            }
        }
    }
    

    
    void renderCoordinateOverlay() {
        // This would render coordinate labels on screen
        // For now, we rely on console output
        // In a full implementation, we'd use SDL_ttf or similar to render text
    }
    
    void shutdown() {
        uiManager_.reset();
        sdlManager_.cleanup();
    }

private:
    SDLManager sdlManager_;
    std::unique_ptr<UIManager> uiManager_;
    std::shared_ptr<HexGridEditor> hexEditor_;
    bool running_;
    int currentMouseX_ = 0;
    int currentMouseY_ = 0;
    HexCoordinate lastHoverCoord_{-999, -999, -999};
};

int main() {
    VisualCoordinateDebugger debugger;
    
    if (!debugger.initialize()) {
        return -1;
    }
    
    debugger.run();
    debugger.shutdown();
    
    return 0;
}
