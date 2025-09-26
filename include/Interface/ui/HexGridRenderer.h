#pragma once
#include "UIComponent.h"
#include "HexGrid.h"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

/**
 * Rendering configuration for hexagonal grid display
 */
struct HexRenderConfig {
    float hexSize = 32.0f;           // Radius of hexagon in pixels
    float borderThickness = 1.0f;    // Border line thickness
    bool showCoordinates = false;    // Display hex coordinates as text
    bool showGrid = true;            // Show grid lines
    bool showHighlights = true;      // Show tile highlights
    
    // Colors
    SDL_Color gridColor = {64, 64, 64, 255};        // Grid line color
    SDL_Color borderColor = {0, 0, 0, 255};         // Tile border color
    SDL_Color coordinateTextColor = {255, 255, 255, 255}; // Coordinate text color
    
    // Zoom and pan
    float zoomLevel = 1.0f;          // Zoom factor
    float panX = 0.0f;               // Pan offset X
    float panY = 0.0f;               // Pan offset Y
    
    // Rendering optimizations
    bool enableCulling = true;       // Only render visible tiles
    int maxRenderDistance = 20;     // Maximum tiles to render from view center
};

/**
 * UI component for rendering hexagonal grids with Roman warfare theme
 * Supports interactive map editing, unit visualization, and tactical overlays
 */
class HexGridRenderer : public UIComponent {
public:
    HexGridRenderer(int x, int y, int width, int height, SDLManager& sdlManager, 
                   std::shared_ptr<HexGrid> grid = nullptr);
    ~HexGridRenderer() = default;
    
    // UIComponent interface
    void render() override;
    void handleEvent(const SDL_Event& event) override;
    bool canReceiveFocus() const override { return true; }
    
    // Grid management
    void setGrid(std::shared_ptr<HexGrid> grid) { grid_ = grid; }
    std::shared_ptr<HexGrid> getGrid() const { return grid_; }
    
    // Rendering configuration
    const HexRenderConfig& getRenderConfig() const { return config_; }
    HexRenderConfig& getRenderConfig() { return config_; }
    void setRenderConfig(const HexRenderConfig& config) { config_ = config; }
    
    // View manipulation
    void setZoom(float zoom);
    void setPan(float x, float y);
    void centerOnCoordinate(const HexCoordinate& coord);
    void resetView();
    
    // Screen/world coordinate conversion
    // Converts coordinates relative to the renderer component to a hex coordinate
    HexCoordinate screenToHex(int relativeX, int relativeY) const;
    // Converts a hex coordinate to world coordinates (relative to 0,0 of the grid, before pan/zoom)
    void hexToWorld(const HexCoordinate& coord, float& worldX, float& worldY) const;
    // Converts a hex coordinate to renderer-space coordinates (with pan/zoom applied)
    void hexToScreen(const HexCoordinate& coord, float& screenX, float& screenY) const;
    // Converts a hex coordinate to absolute window coordinates
    void hexToWindowCoords(const HexCoordinate& coord, float& windowX, float& windowY) const;
    
    // Highlighting and visual feedback
    void highlightTile(const HexCoordinate& coord, const SDL_Color& color);
    void highlightTiles(const std::vector<HexCoordinate>& coords, const SDL_Color& color);
    void highlightMovementRange(const HexCoordinate& start, int movementPoints, const SDL_Color& color);
    void highlightAttackRange(const HexCoordinate& attacker, int range, const SDL_Color& color);
    void highlightPath(const std::vector<HexCoordinate>& path, const SDL_Color& color);
    void clearHighlights();
    
    // Selection system
    void setSelectedTile(const HexCoordinate& coord);
    void clearSelection();
    const HexCoordinate& getSelectedTile() const { return selectedTile_; }
    bool hasSelection() const { return hasSelection_; }
    
    // Hover system for tooltips
    void setHoveredTile(const HexCoordinate& coord);
    void clearHover();
    const HexCoordinate& getHoveredTile() const { return hoveredTile_; }
    bool hasHover() const { return hasHover_; }
    
    // Visibility culling
    std::vector<HexCoordinate> getVisibleTiles() const;
    bool isTileVisible(const HexCoordinate& coord) const;
    
    // Animation support (for unit movement, effects)
    void animateTileHighlight(const HexCoordinate& coord, const SDL_Color& color, float duration);
    void animatePathTrace(const std::vector<HexCoordinate>& path, float duration);
    void updateAnimations(float deltaTime);
    
    // Roman-themed overlays
    void renderFormationOverlay(const std::vector<HexCoordinate>& formation, const std::string& formationType);
    void renderSiegeOverlay(const HexCoordinate& center, int radius);
    void renderSupplyLineOverlay(const std::vector<HexCoordinate>& supplyRoute);
    
    // Debug and development tools
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    bool isDebugMode() const { return debugMode_; }
    void renderDebugInfo();

private:
    std::shared_ptr<HexGrid> grid_;
    HexRenderConfig config_;
    
    // Selection and interaction state
    HexCoordinate selectedTile_{-9999, -9999, -9999};  // Invalid coordinate as null
    HexCoordinate hoveredTile_{-9999, -9999, -9999};   // Invalid coordinate as null
    bool hasSelection_ = false;
    bool hasHover_ = false;
    
    // Input handling state
    bool isDragging_ = false;
    int lastMouseX_ = 0;
    int lastMouseY_ = 0;
    
    // Animation system
    struct TileAnimation {
        HexCoordinate coord;
        SDL_Color color;
        float duration;
        float elapsed;
        enum Type { HIGHLIGHT, PULSE, FADE } type = HIGHLIGHT;
    };
    std::vector<TileAnimation> animations_;
    
    // Debug mode
    bool debugMode_ = false;
    
    // Rendering helpers
    void renderHexagon(const HexCoordinate& coord, const SDL_Color& fillColor, 
                      const SDL_Color& borderColor, float borderThickness = 1.0f);
    void renderHexagonOutline(const HexCoordinate& coord, const SDL_Color& color, float thickness = 1.0f);
    void renderTileContent(const HexCoordinate& coord);
    void renderTileText(const HexCoordinate& coord, const std::string& text, const SDL_Color& color);
    
    // Geometry calculations
    std::vector<SDL_Point> getHexagonPoints(const HexCoordinate& coord) const;
    std::vector<SDL_FPoint> getHexagonPointsF(const HexCoordinate& coord) const;
    SDL_Rect getTileBounds(const HexCoordinate& coord) const;
    
    // View transformation
    void applyViewTransform(float& x, float& y) const;
    void reverseViewTransform(float& x, float& y) const;
    
    // Culling and optimization
    bool shouldRenderTile(const HexCoordinate& coord) const;
    
    // Event handlers
    void handleMouseClick(int x, int y, Uint8 button);
    void handleMouseMotion(int x, int y);
    void handleMouseWheel(int deltaY);
    void handleKeyPress(SDL_Keycode key);
    
    // Rendering passes
    void renderGrid();
    void renderTiles();
    void renderHighlights();
    void renderSelection();
    void renderHover();
    void renderAnimations();
    void renderOverlays();
    
    // Utility functions
    float getHexagonRadius() const { return config_.hexSize * config_.zoomLevel; }
    bool isValidHexCoordinate(const HexCoordinate& coord) const;
};
