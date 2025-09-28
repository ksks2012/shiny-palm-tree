#include "Systems/SDLManager.h"
#include "Interface/ui/HexGridEditor.h"
#include <iostream>

/**
 * Program for testing the middle mouse button drag-cancel functionality
 */
class MiddleClickTest {
public:
    MiddleClickTest() : running_(false) {}
    
    bool initialize() {
        std::cout << "=== Middle Click Pan Test ===" << std::endl;
        std::cout << "Instructions:" << std::endl;
        std::cout << "1. Hold middle mouse button and drag to pan" << std::endl;
        std::cout << "2. Release middle button to cancel pan" << std::endl;
        std::cout << "3. Press ESC to quit" << std::endl;
        std::cout << "=============================" << std::endl;

        if (!sdlManager_.initialize()) {
            std::cout << "ERROR: Failed to initialize SDL" << std::endl;
            return false;
        }
        std::cout << "SDL initialized" << std::endl;
        
        // Create hex grid editor
        hexEditor_ = std::make_shared<HexGridEditor>(0, 0, 1200, 800, sdlManager_);
        std::cout << "Hex grid editor created" << std::endl;
        
        // Configure for better visibility
        if (hexEditor_->getRenderer()) {
            auto& config = hexEditor_->getRenderer()->getRenderConfig();
            config.hexSize = 40.0f;
            config.showGrid = true;
            config.showCoordinates = true;
            std::cout << "Renderer configured" << std::endl;
        }
        
        return true;
    }
    
    void run() {
        running_ = true;
        SDL_Event event;
        
        std::cout << "Starting event loop..." << std::endl;
        
        while (running_) {
            while (SDL_PollEvent(&event)) {
                handleEvent(event);
                
                if (hexEditor_) {
                    hexEditor_->handleEvent(event);
                }
            }
            
            render();
            SDL_Delay(16); // ~60 FPS
        }
        
        std::cout << "Event loop ended" << std::endl;
    }
    
    void shutdown() {
        hexEditor_.reset();
        sdlManager_.cleanup();
        std::cout << "Cleanup complete" << std::endl;
    }

private:
    SDLManager sdlManager_;
    std::shared_ptr<HexGridEditor> hexEditor_;
    bool running_;
    
    void handleEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_QUIT:
                running_ = false;
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running_ = false;
                }
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                std::cout << "Mouse button " << static_cast<int>(event.button.button) 
                         << " pressed at (" << event.button.x << "," << event.button.y << ")" << std::endl;
                if (event.button.button == SDL_BUTTON_MIDDLE) {
                    std::cout << ">>> MIDDLE CLICK DETECTED - Starting pan" << std::endl;
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                std::cout << "Mouse button " << static_cast<int>(event.button.button) 
                         << " released at (" << event.button.x << "," << event.button.y << ")" << std::endl;
                if (event.button.button == SDL_BUTTON_MIDDLE) {
                    std::cout << ">>> MIDDLE CLICK RELEASED - Pan should cancel" << std::endl;
                }
                break;
                
            case SDL_MOUSEMOTION:
                // Only log motion when middle button is held down
                if (event.motion.state & SDL_BUTTON_MMASK) {
                    std::cout << "Pan motion: (" << event.motion.x << "," << event.motion.y 
                             << ") delta=(" << event.motion.xrel << "," << event.motion.yrel << ")" << std::endl;
                }
                break;
        }
    }
    
    void render() {
        SDL_Renderer* renderer = sdlManager_.getRenderer();
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);
        
        // Render hex grid editor
        if (hexEditor_) {
            hexEditor_->render();
        }
        
        // Present
        SDL_RenderPresent(renderer);
    }
};

int main() {
    MiddleClickTest test;
    
    if (test.initialize()) {
        test.run();
    }
    
    test.shutdown();
    return 0;
}
