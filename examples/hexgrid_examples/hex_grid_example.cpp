#include "Interface/ui/HexGridEditor.h"
#include "Interface/ui/UIManager.h"
#include "Systems/SDLManager.h"
#include "Interface/ui/HexTile.h"
#include <memory>

/**
 * Example application demonstrating the hexagonal grid system
 * Features Roman warfare-themed tactical map editing
 */
class HexGridExample {
public:
    HexGridExample() : running_(false) {}
    
    bool initialize() {
        // Initialize SDL
        if (!sdlManager_.initialize()) {
            return false;
        }
        
        // Create UI manager
        uiManager_ = std::make_unique<UIManager>();
        
        // Create hex grid editor
        hexEditor_ = std::make_shared<HexGridEditor>(0, 0, 1200, 800, sdlManager_);
        
        // Set up event callbacks
        hexEditor_->onTileSelected = [this](const HexCoordinate& coord) {
            // Handle tile selection for UI updates
            selectedCoord_ = coord;
        };
        
        hexEditor_->onActionExecuted = [this](const EditorAction& action) {
            // Handle action for status updates
            lastAction_ = action.description;
        };
        
        // Add editor to UI manager
        uiManager_->addComponent(hexEditor_, true);
        
        // Configure renderer for better visibility  
        if (hexEditor_->getRenderer()) {
            auto& config = hexEditor_->getRenderer()->getRenderConfig();
            config.hexSize = 35.0f;  // Larger hexagons
            config.showGrid = true;
            config.borderColor = {255, 255, 255, 255};  // White borders
            config.borderThickness = 2.0f;
        }
        
        // Initialize with Cannae battlefield
        hexEditor_->loadCannaeBattlefield();
        
        running_ = true;
        return true;
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
    HexCoordinate selectedCoord_{-9999, -9999, -9999};
    std::string lastAction_;
    
    void handleEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_QUIT:
                running_ = false;
                break;
                
            case SDL_KEYDOWN:
                handleKeyPress(event.key.keysym.sym);
                break;
        }
    }
    
    void handleKeyPress(SDL_Keycode key) {
        switch (key) {
            case SDLK_ESCAPE:
                running_ = false;
                break;
                
            case SDLK_F1:
                // Load Cannae battlefield
                hexEditor_->loadCannaeBattlefield();
                break;
                
            case SDLK_F2:
                // Load Alesia battlefield
                hexEditor_->loadAlesiaBattlefield();
                break;
                
            case SDLK_F3:
                // Load Teutoburg battlefield
                hexEditor_->loadTeutobergBattlefield();
                break;
                
            case SDLK_n:
                // New map
                hexEditor_->newMap(30, 20);
                break;
                
            case SDLK_s:
                if (SDL_GetModState() & KMOD_CTRL) {
                    // Save map
                    hexEditor_->saveMap("tactical_map.json");
                }
                break;
                
            case SDLK_l:
                if (SDL_GetModState() & KMOD_CTRL) {
                    // Load map
                    hexEditor_->loadMap("tactical_map.json");
                }
                break;
                
            case SDLK_v:
                // Validate map
                auto validation = hexEditor_->validateMap();
                printf("Map validation: %s\n", validation.isValid ? "VALID" : "INVALID");
                for (const auto& warning : validation.warnings) {
                    printf("Warning: %s\n", warning.c_str());
                }
                for (const auto& error : validation.errors) {
                    printf("Error: %s\n", error.c_str());
                }
                break;
        }
    }
    
    void update(float deltaTime) {
        // Update any animations or time-based effects
        if (hexEditor_ && hexEditor_->getGrid()) {
            // Process any time-based events
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
        
        // Render HUD/status information
        renderHUD();
        
        SDL_RenderPresent(sdlManager_.getRenderer());
    }
    
    void renderHUD() {
        // Render help text and controls
        SDL_Color textColor = {255, 255, 255, 255};
        
        // Controls help
        const char* helpText[] = {
            "Roman Tactical Map Editor",
            "",
            "Controls:",
            "1-5: Select tools",
            "Mouse: Paint/Select tiles", 
            "Scroll: Zoom",
            "Middle-drag: Pan",
            "R: Reset view",
            "G: Toggle grid",
            "Esc: Clear selection",
            "",
            "Hotkeys:",
            "F1: Cannae battlefield",
            "F2: Alesia siege",
            "F3: Teutoburg ambush",
            "Ctrl+S: Save map",
            "Ctrl+L: Load map",
            "V: Validate map",
            "N: New map"
        };
        
        int yPos = 10;
        for (const char* line : helpText) {
            if (strlen(line) > 0) {
                // This would use proper text rendering in a real implementation
                // For now, we'll skip text rendering in this example
            }
            yPos += 15;
        }
        
        // Status information
        if (hexEditor_) {
            auto& state = hexEditor_->getState();
            
            // Current tool
            std::string toolName;
            switch (state.currentTool) {
                case HexEditorTool::SELECT: toolName = "SELECT"; break;
                case HexEditorTool::PAINT: toolName = "PAINT"; break;
                case HexEditorTool::FILL: toolName = "FILL"; break;
                case HexEditorTool::HEIGHT: toolName = "HEIGHT"; break;
                case HexEditorTool::UNIT_PLACE: toolName = "UNITS"; break;
                case HexEditorTool::EVENT_EDIT: toolName = "EVENTS"; break;
                case HexEditorTool::FORMATION: toolName = "FORMATION"; break;
                case HexEditorTool::MEASURE: toolName = "MEASURE"; break;
            }
            
            // Selected terrain
            std::string terrainName = HexTileUtils::getTerrainName(state.selectedTerrain);
        }
    }
};

int main(int argc, char* argv[]) {
    HexGridExample app;
    
    if (!app.initialize()) {
        printf("Failed to initialize hex grid example\n");
        return -1;
    }
    
    printf("Roman Tactical Map Editor\n");
    printf("=========================\n");
    printf("Use the mouse and keyboard to create detailed battlefield maps\n");
    printf("Press F1-F3 for historical battlefield templates\n");
    printf("Press ESC to quit\n\n");
    
    app.run();
    app.shutdown();
    
    return 0;
}
