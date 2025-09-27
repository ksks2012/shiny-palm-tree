#include "Interface/ui/HexGridEditor.h"
#include "Interface/ui/UIManager.h"
#include "Systems/SDLManager.h"
#include "Interface/ui/HexTile.h"
#include <memory>
#include <iostream>

/**
 * Enhanced hex grid example with improved visibility
 */
class EnhancedHexGridExample {
public:
    EnhancedHexGridExample() : running_(false) {}
    
    bool initialize() {
        std::cout << "=== Enhanced Roman Tactical Map Editor ===" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  Mouse: Click and paint terrain" << std::endl;
        std::cout << "  1-8: Select tools" << std::endl;
        std::cout << "  F1-F3: Load historical battlefields" << std::endl;
        std::cout << "  G: Toggle grid lines" << std::endl;
        std::cout << "  C: Toggle coordinates" << std::endl;
        std::cout << "  R: Reset view" << std::endl;
        std::cout << "  ESC: Quit" << std::endl;
        std::cout << "======================================" << std::endl;
        
        // Initialize SDL
        if (!sdlManager_.initialize()) {
            std::cout << "ERROR: Failed to initialize SDL" << std::endl;
            return false;
        }
        std::cout << "SDL initialized successfully" << std::endl;
        
        // Create UI manager
        uiManager_ = std::make_unique<UIManager>();
        
        // Create hex grid editor
        hexEditor_ = std::make_shared<HexGridEditor>(0, 0, 1200, 800, sdlManager_);
        std::cout << "Hex grid editor created (1200x800)" << std::endl;
        
        // Configure renderer for better visibility
        if (hexEditor_->getRenderer()) {
            auto& config = hexEditor_->getRenderer()->getRenderConfig();
            config.hexSize = 35.0f;  // Slightly larger hexagons
            config.showGrid = true;
            config.showCoordinates = false;  // Start without coordinates
            config.borderColor = {255, 255, 255, 255};  // White borders
            config.borderThickness = 2.0f;
            config.gridColor = {128, 128, 128, 255};  // Gray grid lines
            
            std::cout << "Renderer configured: hexSize=" << config.hexSize << std::endl;
        }
        
        // Set up event callbacks
        hexEditor_->onTileSelected = [this](const HexCoordinate& coord) {
            selectedCoord_ = coord;
            std::cout << "Selected tile: (" << coord.x << "," << coord.y << "," << coord.z << ")" << std::endl;
        };
        
        hexEditor_->onActionExecuted = [this](const EditorAction& action) {
            lastAction_ = action.description;
            std::cout << "Action: " << action.description << std::endl;
        };
        
        // Add editor to UI manager
        uiManager_->addComponent(hexEditor_, true);
        
        // Create a more obvious initial pattern
        createTestPattern();
        
        running_ = true;
        std::cout << "Initialization complete!" << std::endl;
        return true;
    }
    
    void createTestPattern() {
        std::cout << "Creating test pattern..." << std::endl;
        
        if (!hexEditor_ || !hexEditor_->getGrid()) {
            std::cout << "ERROR: Grid not available" << std::endl;
            return;
        }
        
        auto grid = hexEditor_->getGrid();
        int patternCount = 0;
        
        // Create a visible pattern in the center
        for (int x = -4; x <= 4; x++) {
            for (int z = -3; z <= 3; z++) {
                int y = -x - z;
                HexCoordinate coord(x, y, z);
                
                if (grid->isValidCoordinate(coord)) {
                    TerrainType terrain;
                    
                    // Create concentric circles pattern
                    int distance = std::abs(x) + std::abs(y) + std::abs(z);
                    distance /= 2;  // Hex distance
                    
                    switch (distance) {
                        case 0: terrain = TerrainType::CITY_WALL; break;  // Center
                        case 1: terrain = TerrainType::ROAD; break;
                        case 2: terrain = TerrainType::FOREST; break;
                        case 3: terrain = TerrainType::MOUNTAIN; break;
                        default: terrain = TerrainType::PLAIN; break;
                    }
                    
                    grid->setTerrain(coord, terrain);
                    patternCount++;
                }
            }
        }
        
        std::cout << "Test pattern created with " << patternCount << " tiles" << std::endl;
        
        // Center view on the pattern
        if (hexEditor_->getRenderer()) {
            hexEditor_->getRenderer()->resetView();
            hexEditor_->getRenderer()->centerOnCoordinate(HexCoordinate(0, 0, 0));
            std::cout << "View centered on origin" << std::endl;
        }
    }
    
    void run() {
        if (!running_) return;
        
        SDL_Event event;
        Uint32 lastTime = SDL_GetTicks();
        
        while (running_) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;
            
            // Handle events
            while (SDL_PollEvent(&event)) {
                handleEvent(event);
                uiManager_->handleEvent(event);
            }
            
            // Update
            update(deltaTime);
            
            // Render
            render();
            
            // Cap framerate
            SDL_Delay(16); // ~60 FPS
        }
        
        std::cout << "Application ended" << std::endl;
    }
    
    void shutdown() {
        std::cout << "Shutting down..." << std::endl;
        uiManager_.reset();
        sdlManager_.cleanup();
        std::cout << "Shutdown complete" << std::endl;
    }

private:
    SDLManager sdlManager_;
    std::unique_ptr<UIManager> uiManager_;
    std::shared_ptr<HexGridEditor> hexEditor_;
    
    bool running_;
    HexCoordinate selectedCoord_{-9999, -9999, -9999};
    std::string lastAction_;
    
    void handleEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_QUIT:
                std::cout << "Quit requested" << std::endl;
                running_ = false;
                break;
                
            case SDL_KEYDOWN:
                handleKeyPress(event.key.keysym.sym);
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                std::cout << "Mouse " << (int)event.button.button 
                          << " clicked at (" << event.button.x << "," << event.button.y << ")" << std::endl;
                break;
        }
    }
    
    void handleKeyPress(SDL_Keycode key) {
        std::cout << "Key pressed: " << SDL_GetKeyName(key) << std::endl;
        
        switch (key) {
            case SDLK_ESCAPE:
                running_ = false;
                break;
                
            case SDLK_F1:
                std::cout << "Loading Cannae battlefield..." << std::endl;
                hexEditor_->loadCannaeBattlefield();
                break;
                
            case SDLK_F2:
                std::cout << "Loading Alesia battlefield..." << std::endl;
                hexEditor_->loadAlesiaBattlefield();
                break;
                
            case SDLK_F3:
                std::cout << "Loading Teutoburg battlefield..." << std::endl;
                hexEditor_->loadTeutobergBattlefield();
                break;
                
            case SDLK_n:
                std::cout << "Creating new map..." << std::endl;
                hexEditor_->newMap(20, 15);
                createTestPattern();
                break;
                
            case SDLK_g:
                if (hexEditor_->getRenderer()) {
                    auto& config = hexEditor_->getRenderer()->getRenderConfig();
                    config.showGrid = !config.showGrid;
                    std::cout << "Grid " << (config.showGrid ? "enabled" : "disabled") << std::endl;
                }
                break;
                
            case SDLK_c:
                if (hexEditor_->getRenderer()) {
                    auto& config = hexEditor_->getRenderer()->getRenderConfig();
                    config.showCoordinates = !config.showCoordinates;
                    std::cout << "Coordinates " << (config.showCoordinates ? "enabled" : "disabled") << std::endl;
                }
                break;
                
            case SDLK_r:
                std::cout << "Resetting view..." << std::endl;
                if (hexEditor_->getRenderer()) {
                    hexEditor_->getRenderer()->resetView();
                    hexEditor_->getRenderer()->centerOnCoordinate(HexCoordinate(0, 0, 0));
                }
                break;
                
            case SDLK_t:
                std::cout << "Recreating test pattern..." << std::endl;
                createTestPattern();
                break;
        }
    }
    
    void update(float deltaTime) {
        // Update any animations or time-based effects
        if (hexEditor_ && hexEditor_->getGrid()) {
            std::unordered_map<std::string, std::string> context;
            context["deltaTime"] = std::to_string(deltaTime);
            hexEditor_->getGrid()->processEvents("time_update", context);
        }
    }
    
    void render() {
        // Clear screen with Roman-themed background
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 40, 30, 20, 255);
        SDL_RenderClear(sdlManager_.getRenderer());
        
        // Render UI
        uiManager_->renderAll();
        
        SDL_RenderPresent(sdlManager_.getRenderer());
    }
};

int main(int argc, char* argv[]) {
    EnhancedHexGridExample app;
    
    if (!app.initialize()) {
        std::cout << "Failed to initialize application" << std::endl;
        return -1;
    }
    
    app.run();
    app.shutdown();
    
    return 0;
}
