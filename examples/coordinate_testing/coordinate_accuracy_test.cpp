#include "Interface/ui/HexGridEditor.h"
#include "Interface/ui/UIManager.h"
#include "Systems/SDLManager.h"
#include <memory>
#include <iostream>
#include <iomanip>

/**
 * Coordinate accuracy test program
 */
class CoordinateAccuracyTest {
public:
    CoordinateAccuracyTest() : running_(false) {}
    
    bool initialize() {
        std::cout << "=== Coordinate Accuracy Test ===" << std::endl;
        std::cout << "Testing hexagon position, coordinate display, and click accuracy" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  Mouse: Click to test coordinates" << std::endl;
        std::cout << "  C: Toggle coordinate display" << std::endl;
        std::cout << "  G: Toggle grid lines" << std::endl;
        std::cout << "  D: Toggle debug mode" << std::endl;
        std::cout << "  R: Reset view" << std::endl;
        std::cout << "  ESC: Quit" << std::endl;
        std::cout << "=================================" << std::endl;
        
        if (!sdlManager_.initialize()) {
            std::cout << "ERROR: Failed to initialize SDL" << std::endl;
            return false;
        }
        
        uiManager_ = std::make_unique<UIManager>();
        hexEditor_ = std::make_shared<HexGridEditor>(0, 0, 1200, 800, sdlManager_);
        
        // Print layout information
        if (hexEditor_->getRenderer()) {
            std::cout << "HexGridEditor layout:" << std::endl;
            std::cout << "  Editor position: (0, 0)" << std::endl;
            std::cout << "  Editor size: 1200x800" << std::endl;
            
            // Get renderer position from the editor
            auto& config = hexEditor_->getRenderer()->getRenderConfig();
            std::cout << "  Renderer config: hexSize=" << config.hexSize 
                      << ", zoom=" << config.zoomLevel 
                      << ", pan=(" << config.panX << "," << config.panY << ")" << std::endl;
                      
            config.hexSize = 50.0f;  // Large hexagons for easier testing
            config.showGrid = true;
            config.showCoordinates = true;
            config.borderColor = {255, 255, 255, 255};
            config.borderThickness = 3.0f;
            config.gridColor = {255, 255, 0, 255};
        }
        
        // Create a simple test pattern
        createTestPattern();
        
        uiManager_->addComponent(hexEditor_, true);
        
        running_ = true;
        return true;
    }
    
    void createTestPattern() {
        if (!hexEditor_ || !hexEditor_->getGrid()) return;
        
        auto grid = hexEditor_->getGrid();
        std::cout << "Creating coordinate test pattern..." << std::endl;
        
        // Create a cross pattern centered at origin
        std::vector<HexCoordinate> testCoords = {
            HexCoordinate(0, 0, 0),     // Center
            HexCoordinate(1, -1, 0),    // Right
            HexCoordinate(-1, 1, 0),    // Left  
            HexCoordinate(0, -1, 1),    // Top-right
            HexCoordinate(0, 1, -1),    // Bottom-left
            HexCoordinate(1, 0, -1),    // Bottom-right
            HexCoordinate(-1, 0, 1)     // Top-left
        };
        
        for (size_t i = 0; i < testCoords.size(); ++i) {
            const auto& coord = testCoords[i];
            if (grid->isValidCoordinate(coord)) {
                TerrainType terrain = static_cast<TerrainType>(i % 6);
                grid->setTerrain(coord, terrain);
                
                std::cout << "  Set test coord " << i << ": (" 
                          << coord.x << "," << coord.y << "," << coord.z 
                          << ") = terrain " << static_cast<int>(terrain) << std::endl;
            }
        }
        
        // Center view
        if (hexEditor_->getRenderer()) {
            hexEditor_->getRenderer()->resetView();
            hexEditor_->getRenderer()->centerOnCoordinate(HexCoordinate(0, 0, 0));
            std::cout << "View centered on origin (0,0,0)" << std::endl;
        }
    }
    
    void run() {
        SDL_Event event;
        
        while (running_) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running_ = false;
                } else if (event.type == SDL_KEYDOWN) {
                    handleKeyPress(event.key.keysym.sym);
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    handleMouseClick(event);
                }
                
                uiManager_->handleEvent(event);
            }
            
            // Clear with dark background
            SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 32, 32, 32, 255);
            SDL_RenderClear(sdlManager_.getRenderer());
            
            uiManager_->renderAll();
            
            // Overlay coordinate information
            renderCoordinateInfo();
            
            SDL_RenderPresent(sdlManager_.getRenderer());
            SDL_Delay(16);
        }
    }
    
    void handleKeyPress(SDL_Keycode key) {
        switch (key) {
            case SDLK_ESCAPE:
                running_ = false;
                break;
                
            case SDLK_c:
                if (hexEditor_->getRenderer()) {
                    auto& config = hexEditor_->getRenderer()->getRenderConfig();
                    config.showCoordinates = !config.showCoordinates;
                    std::cout << "Coordinates " << (config.showCoordinates ? "ON" : "OFF") << std::endl;
                }
                break;
                
            case SDLK_g:
                if (hexEditor_->getRenderer()) {
                    auto& config = hexEditor_->getRenderer()->getRenderConfig();
                    config.showGrid = !config.showGrid;
                    std::cout << "Grid " << (config.showGrid ? "ON" : "OFF") << std::endl;
                }
                break;
                
            case SDLK_d:
                if (hexEditor_->getRenderer()) {
                    bool debug = !hexEditor_->getRenderer()->isDebugMode();
                    hexEditor_->getRenderer()->setDebugMode(debug);
                    std::cout << "Debug mode " << (debug ? "ON" : "OFF") << std::endl;
                }
                break;
                
            case SDLK_r:
                if (hexEditor_->getRenderer()) {
                    hexEditor_->getRenderer()->resetView();
                    hexEditor_->getRenderer()->centerOnCoordinate(HexCoordinate(0, 0, 0));
                    std::cout << "View reset to center" << std::endl;
                }
                break;
        }
    }
    
    void handleMouseClick(const SDL_Event& event) {
        int mouseX = event.button.x;
        int mouseY = event.button.y;
        
        std::cout << "\n--- Mouse Click Analysis ---" << std::endl;
        std::cout << "Raw mouse position: (" << mouseX << ", " << mouseY << ")" << std::endl;
        
        if (hexEditor_ && hexEditor_->getRenderer()) {
            // Test coordinate conversion through HexGridEditor (using window coordinates)
            HexCoordinate editorCoord = hexEditor_->getRenderer()->screenToHex(mouseX, mouseY);
            
            std::cout << "Editor coord conversion: (" 
                      << editorCoord.x << ", " << editorCoord.y << ", " << editorCoord.z << ")" << std::endl;
            
            // Test direct renderer conversion  
            HexCoordinate rendererCoord = hexEditor_->getRenderer()->screenToHex(mouseX, mouseY);
            std::cout << "Renderer coord conversion: (" 
                      << rendererCoord.x << ", " << rendererCoord.y << ", " << rendererCoord.z << ")" << std::endl;
            
            // Test manual coordinate calculation
            float worldX = mouseX - 200;  // Left panel offset
            float worldY = mouseY;
            
            auto& config = hexEditor_->getRenderer()->getRenderConfig();
            worldX = (worldX - config.panX) / config.zoomLevel;
            worldY = (worldY - config.panY) / config.zoomLevel;
            
            HexCoordinate manualCoord = HexCoordinate::fromScreenCoords(worldX, worldY, config.hexSize);
            std::cout << "Manual calculation: (" 
                      << manualCoord.x << ", " << manualCoord.y << ", " << manualCoord.z << ")" << std::endl;
            
            // Test reverse conversion using hexToWindowCoords
            float backX, backY;
            hexEditor_->getRenderer()->hexToWindowCoords(editorCoord, backX, backY);
            
            std::cout << "Reverse conversion: (" << backX << ", " << backY << ")" << std::endl;
            std::cout << "Position difference: (" << (backX - mouseX) << ", " << (backY - mouseY) << ")" << std::endl;
            
            lastClickCoord_ = editorCoord;
            hasLastClick_ = true;
        }
    }
    
    void renderCoordinateInfo() {
        // Simple coordinate info display
        if (hasLastClick_) {
            std::cout.flush();  // Ensure console output is visible
        }
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
    HexCoordinate lastClickCoord_{0, 0, 0};
    bool hasLastClick_ = false;
};

int main() {
    CoordinateAccuracyTest test;
    
    if (!test.initialize()) {
        return -1;
    }
    
    test.run();
    test.shutdown();
    
    return 0;
}
