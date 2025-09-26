#include "Interface/ui/HexGridRenderer.h"
#include "Interface/ui/HexTile.h"
#include "Systems/SDLManager.h"
#include <cmath>
#include <algorithm>

HexGridRenderer::HexGridRenderer(int x, int y, int width, int height, SDLManager& sdlManager, 
                                std::shared_ptr<HexGrid> grid)
    : UIComponent(x, y, width, height, sdlManager), grid_(grid) {
    // Default configuration suitable for tactical maps
    config_.hexSize = 40.0f;  // Larger hexagons for better visibility
    config_.showGrid = true;
    config_.showHighlights = true;
    config_.enableCulling = true;
    config_.showCoordinates = true;  // Show coordinates for better visibility
    
    // Make borders more visible
    config_.borderColor = {255, 255, 255, 255};  // White borders
    config_.borderThickness = 2.0f;
    
    // Brighter grid lines
    config_.gridColor = {128, 128, 128, 255};
}

void HexGridRenderer::render() {
    if (!grid_) return;
    
    // Set viewport clipping
    SDL_Rect clipRect = {x_, y_, width_, height_};
    SDL_RenderSetClipRect(sdlManager_.getRenderer(), &clipRect);
    
    // Render background
    renderBackground({40, 30, 20, 255}); // Roman parchment-like background
    
    // Render in order: grid -> tiles -> highlights -> selection -> hover -> animations -> overlays
    if (config_.showGrid) {
        renderGrid();
    }
    
    renderTiles();
    
    if (config_.showHighlights) {
        renderHighlights();
    }
    
    renderSelection();
    renderHover();
    renderAnimations();
    renderOverlays();
    
    if (debugMode_) {
        renderDebugInfo();
    }
    
    // Clear clipping
    SDL_RenderSetClipRect(sdlManager_.getRenderer(), nullptr);
}

void HexGridRenderer::handleEvent(const SDL_Event& event) {
    if (!grid_ || !enabled_) return;
    
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (isPointInside(event.button.x, event.button.y)) {
                // Convert absolute coordinates to relative coordinates for the renderer
                int relativeX = event.button.x - x_;
                int relativeY = event.button.y - y_;
                handleMouseClick(relativeX, relativeY, event.button.button);
            }
            break;
            
        case SDL_MOUSEMOTION:
            if (isPointInside(event.motion.x, event.motion.y)) {
                // Convert absolute coordinates to relative coordinates for the renderer
                int relativeX = event.motion.x - x_;
                int relativeY = event.motion.y - y_;
                handleMouseMotion(relativeX, relativeY);
            }
            break;
            
        case SDL_MOUSEWHEEL:
            if (hasFocus_) {
                handleMouseWheel(event.wheel.y);
            }
            break;
            
        case SDL_KEYDOWN:
            if (hasFocus_) {
                handleKeyPress(event.key.keysym.sym);
            }
            break;
    }
}

void HexGridRenderer::setZoom(float zoom) {
    config_.zoomLevel = std::max(0.1f, std::min(5.0f, zoom));
}

void HexGridRenderer::setPan(float x, float y) {
    config_.panX = x;
    config_.panY = y;
}

void HexGridRenderer::centerOnCoordinate(const HexCoordinate& coord) {
    float screenX, screenY;
    coord.toScreenCoords(screenX, screenY, config_.hexSize);
    
    config_.panX = width_ / 2.0f - screenX * config_.zoomLevel;
    config_.panY = height_ / 2.0f - screenY * config_.zoomLevel;
}

void HexGridRenderer::resetView() {
    config_.zoomLevel = 1.0f;
    config_.panX = width_ / 2.0f;  // Center horizontally
    config_.panY = height_ / 2.0f; // Center vertically
}

HexCoordinate HexGridRenderer::screenToHex(int relativeX, int relativeY) const {
    // Start with coordinates relative to the renderer component
    float worldX = relativeX;
    float worldY = relativeY;
    
    // Reverse pan and zoom to get world coordinates
    reverseViewTransform(worldX, worldY);
    
    // Convert world coordinates to logical hex coordinates
    return HexCoordinate::fromScreenCoords(worldX, worldY, config_.hexSize);
}

void HexGridRenderer::hexToWorld(const HexCoordinate& coord, float& worldX, float& worldY) const {
    coord.toScreenCoords(worldX, worldY, config_.hexSize);
}

void HexGridRenderer::hexToScreen(const HexCoordinate& coord, float& screenX, float& screenY) const {
    // Convert to world first, then apply view transform
    hexToWorld(coord, screenX, screenY);
    applyViewTransform(screenX, screenY);
}

void HexGridRenderer::hexToWindowCoords(const HexCoordinate& coord, float& windowX, float& windowY) const {
    hexToScreen(coord, windowX, windowY);
    windowX += x_;
    windowY += y_;
}

void HexGridRenderer::highlightTile(const HexCoordinate& coord, const SDL_Color& color) {
    if (auto* tile = grid_->getTile(coord)) {
        tile->setHighlighted(true);
        tile->setHighlightColor(color);
    }
}

void HexGridRenderer::highlightTiles(const std::vector<HexCoordinate>& coords, const SDL_Color& color) {
    for (const auto& coord : coords) {
        highlightTile(coord, color);
    }
}

void HexGridRenderer::highlightMovementRange(const HexCoordinate& start, int movementPoints, const SDL_Color& color) {
    auto range = grid_->calculateMovementRange(start, movementPoints);
    
    for (const auto& [coord, cost] : range.reachableTiles) {
        if (coord != start) { // Don't highlight starting position
            highlightTile(coord, color);
        }
    }
}

void HexGridRenderer::highlightAttackRange(const HexCoordinate& attacker, int range, const SDL_Color& color) {
    auto attackRange = grid_->calculateAttackRange(attacker, range);
    highlightTiles(attackRange, color);
}

void HexGridRenderer::highlightPath(const std::vector<HexCoordinate>& path, const SDL_Color& color) {
    SDL_Color pathColor = color;
    for (size_t i = 0; i < path.size(); ++i) {
        // Fade path color from start to end
        float alpha = 0.3f + 0.7f * (1.0f - static_cast<float>(i) / path.size());
        pathColor.a = static_cast<Uint8>(255 * alpha);
        highlightTile(path[i], pathColor);
    }
}

void HexGridRenderer::clearHighlights() {
    if (grid_) {
        grid_->clearHighlights();
    }
}

void HexGridRenderer::setSelectedTile(const HexCoordinate& coord) {
    selectedTile_ = coord;
    hasSelection_ = true;
}

void HexGridRenderer::clearSelection() {
    hasSelection_ = false;
}

void HexGridRenderer::setHoveredTile(const HexCoordinate& coord) {
    hoveredTile_ = coord;
    hasHover_ = true;
}

void HexGridRenderer::clearHover() {
    hasHover_ = false;
}

std::vector<HexCoordinate> HexGridRenderer::getVisibleTiles() const {
    std::vector<HexCoordinate> visibleTiles;
    
    if (!grid_) return visibleTiles;
    
    // Calculate screen bounds in world coordinates
    float minX = 0, minY = 0;
    float maxX = width_, maxY = height_;
    reverseViewTransform(minX, minY);
    reverseViewTransform(maxX, maxY);
    
    // Expand bounds to include partially visible tiles
    float padding = config_.hexSize * 2.0f;
    minX -= padding; minY -= padding;
    maxX += padding; maxY += padding;
    
    // Check all tiles in grid (optimization: could use spatial indexing)
    for (const HexCoordinate& coord : grid_->getAllCoordinates()) {
        float tileX, tileY;
        hexToScreen(coord, tileX, tileY);  // Use hexToScreen which applies transforms
        
        if (tileX >= 0 && tileX <= width_ && tileY >= 0 && tileY <= height_) {
            visibleTiles.push_back(coord);
        }
    }
    
    return visibleTiles;
}

void HexGridRenderer::renderHexagon(const HexCoordinate& coord, const SDL_Color& fillColor, 
                                   const SDL_Color& borderColor, float borderThickness) {
    auto points = getHexagonPointsF(coord);
    
    // Fill hexagon
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    
    // Simple polygon fill using triangulation
    SDL_FPoint center;
    hexToScreen(coord, center.x, center.y);
    
    for (size_t i = 0; i < points.size(); ++i) {
        size_t next = (i + 1) % points.size();
        
        SDL_Vertex vertices[3] = {
            {{center.x, center.y}, {fillColor.r, fillColor.g, fillColor.b, fillColor.a}},
            {{points[i].x, points[i].y}, {fillColor.r, fillColor.g, fillColor.b, fillColor.a}},
            {{points[next].x, points[next].y}, {fillColor.r, fillColor.g, fillColor.b, fillColor.a}}
        };
        
        SDL_RenderGeometry(sdlManager_.getRenderer(), nullptr, vertices, 3, nullptr, 0);
    }
    
    // Draw border
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    
    for (size_t i = 0; i < points.size(); ++i) {
        size_t next = (i + 1) % points.size();
        SDL_RenderDrawLineF(sdlManager_.getRenderer(), points[i].x, points[i].y, points[next].x, points[next].y);
    }
}

void HexGridRenderer::renderTiles() {
    if (!grid_) return;
    
    auto visibleTiles = getVisibleTiles();
    
    // Debug: render at least some tiles if we have any coordinates
    if (visibleTiles.empty() && !grid_->getAllCoordinates().empty()) {
        // Fallback: render a few tiles around origin
        for (int x = -2; x <= 2; x++) {
            for (int z = -2; z <= 2; z++) {
                int y = -x - z;
                HexCoordinate coord(x, y, z);
                if (grid_->isValidCoordinate(coord)) {
                    visibleTiles.push_back(coord);
                }
            }
        }
    }
    
    for (const HexCoordinate& coord : visibleTiles) {
        const HexTile* tile = grid_->getTile(coord);
        
        SDL_Color tileColor;
        if (tile) {
            // Get terrain color from existing tile
            tileColor = HexTileUtils::getTerrainColor(tile->getTerrainType());
            
            // Adjust color based on height (darker for higher elevations)
            int heightFactor = tile->getHeight() * 20;
            tileColor.r = std::max(0, static_cast<int>(tileColor.r) - heightFactor);
            tileColor.g = std::max(0, static_cast<int>(tileColor.g) - heightFactor);
            tileColor.b = std::max(0, static_cast<int>(tileColor.b) - heightFactor);
        } else {
            // Default color for empty tiles (light gray)
            tileColor = {180, 180, 180, 255};
        }
        
        renderHexagon(coord, tileColor, config_.borderColor);
        
        // Render tile-specific content (only if tile exists)
        if (tile) {
            renderTileContent(coord);
        }
        
        // Show coordinates if enabled
        if (config_.showCoordinates) {
            std::string coordText = std::to_string(coord.x) + "," + std::to_string(coord.y) + "," + std::to_string(coord.z);
            renderTileText(coord, coordText, config_.coordinateTextColor);
        }
    }
}

void HexGridRenderer::renderTileContent(const HexCoordinate& coord) {
    const HexTile* tile = grid_->getTile(coord);
    if (!tile) return;
    
    float centerX, centerY;
    hexToScreen(coord, centerX, centerY);
    
    // Render occupation indicator
    if (tile->isOccupied()) {
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 255, 0, 0, 128);
        float radius = config_.hexSize * config_.zoomLevel * 0.3f;
        
        // Simple circle approximation
        for (int angle = 0; angle < 360; angle += 10) {
            float rad = angle * M_PI / 180.0f;
            int x = static_cast<int>(centerX + radius * cos(rad));
            int y = static_cast<int>(centerY + radius * sin(rad));
            SDL_RenderDrawPoint(sdlManager_.getRenderer(), x, y);
        }
    }
    
    // Render special markers
    if (tile->isSupplyPoint()) {
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 255, 215, 0, 255); // Gold
        SDL_Rect rect = {
            static_cast<int>(centerX - 3),
            static_cast<int>(centerY - 3),
            6, 6
        };
        SDL_RenderFillRect(sdlManager_.getRenderer(), &rect);
    }
    
    if (tile->isFormationTile()) {
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 128, 0, 128, 128); // Purple
        renderHexagonOutline(coord, {128, 0, 128, 128}, 2.0f);
    }
}

void HexGridRenderer::renderHighlights() {
    if (!grid_) return;
    
    auto visibleTiles = getVisibleTiles();
    
    for (const HexCoordinate& coord : visibleTiles) {
        const HexTile* tile = grid_->getTile(coord);
        if (!tile || !tile->isHighlighted()) continue;
        
        SDL_Color highlightColor = tile->getHighlightColor();
        renderHexagonOutline(coord, highlightColor, 3.0f);
    }
}

void HexGridRenderer::renderSelection() {
    if (!hasSelection_) return;
    
    SDL_Color selectionColor = {255, 255, 0, 255}; // Bright yellow
    renderHexagonOutline(selectedTile_, selectionColor, 4.0f);
}

void HexGridRenderer::renderHover() {
    if (!hasHover_) return;
    
    SDL_Color hoverColor = {255, 255, 255, 128}; // Semi-transparent white
    renderHexagonOutline(hoveredTile_, hoverColor, 2.0f);
}

void HexGridRenderer::renderHexagonOutline(const HexCoordinate& coord, const SDL_Color& color, float thickness) {
    auto points = getHexagonPointsF(coord);
    
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), color.r, color.g, color.b, color.a);
    
    for (size_t i = 0; i < points.size(); ++i) {
        size_t next = (i + 1) % points.size();
        
        // Draw thick line by drawing multiple parallel lines
        for (int offset = -static_cast<int>(thickness/2); offset <= static_cast<int>(thickness/2); ++offset) {
            SDL_RenderDrawLineF(sdlManager_.getRenderer(), 
                               points[i].x + offset, points[i].y + offset,
                               points[next].x + offset, points[next].y + offset);
        }
    }
}

std::vector<SDL_FPoint> HexGridRenderer::getHexagonPointsF(const HexCoordinate& coord) const {
    std::vector<SDL_FPoint> points;
    points.reserve(6);
    
    float centerX, centerY;
    hexToScreen(coord, centerX, centerY);
    float radius = getHexagonRadius();
    
    for (int i = 0; i < 6; ++i) {
        float angle = (i * 60.0f - 30.0f) * M_PI / 180.0f; // Flat-top hexagon
        points.push_back({
            centerX + radius * cos(angle),
            centerY + radius * sin(angle)
        });
    }
    
    return points;
}

void HexGridRenderer::applyViewTransform(float& x, float& y) const {
    x = x * config_.zoomLevel + config_.panX;
    y = y * config_.zoomLevel + config_.panY;
}

void HexGridRenderer::reverseViewTransform(float& x, float& y) const {
    x = (x - config_.panX) / config_.zoomLevel;
    y = (y - config_.panY) / config_.zoomLevel;
}

void HexGridRenderer::handleMouseClick(int x, int y, Uint8 button) {
    HexCoordinate clickedCoord = screenToHex(x, y);
    
    if (button == SDL_BUTTON_LEFT) {
        if (grid_->isValidCoordinate(clickedCoord)) {
            setSelectedTile(clickedCoord);
        } else {
            clearSelection();
        }
    } else if (button == SDL_BUTTON_RIGHT) {
        // Right-click could trigger context menu or special actions
        clearSelection();
    } else if (button == SDL_BUTTON_MIDDLE) {
        // Middle-click for panning
        isDragging_ = true;
        lastMouseX_ = x;
        lastMouseY_ = y;
    }
}

void HexGridRenderer::handleMouseMotion(int x, int y) {
    if (isDragging_) {
        float deltaX = x - lastMouseX_;
        float deltaY = y - lastMouseY_;
        config_.panX += deltaX;
        config_.panY += deltaY;
        
        lastMouseX_ = x;
        lastMouseY_ = y;
    } else {
        // Update hover
        HexCoordinate hoveredCoord = screenToHex(x, y);
        if (grid_->isValidCoordinate(hoveredCoord)) {
            setHoveredTile(hoveredCoord);
        } else {
            clearHover();
        }
    }
}

void HexGridRenderer::handleMouseWheel(int deltaY) {
    float zoomFactor = (deltaY > 0) ? 1.1f : 0.9f;
    setZoom(config_.zoomLevel * zoomFactor);
}

void HexGridRenderer::handleKeyPress(SDL_Keycode key) {
    switch (key) {
        case SDLK_r:
            resetView();
            break;
        case SDLK_g:
            config_.showGrid = !config_.showGrid;
            break;
        case SDLK_c:
            config_.showCoordinates = !config_.showCoordinates;
            break;
        case SDLK_d:
            setDebugMode(!debugMode_);
            break;
        case SDLK_ESCAPE:
            clearSelection();
            clearHighlights();
            break;
    }
}

void HexGridRenderer::renderGrid() {
    if (!grid_) return;
    
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 
                          config_.gridColor.r, config_.gridColor.g, 
                          config_.gridColor.b, config_.gridColor.a);
    
    auto visibleTiles = getVisibleTiles();
    
    for (const HexCoordinate& coord : visibleTiles) {
        auto points = getHexagonPointsF(coord);
        
        for (size_t i = 0; i < points.size(); ++i) {
            size_t next = (i + 1) % points.size();
            SDL_RenderDrawLineF(sdlManager_.getRenderer(), 
                               points[i].x, points[i].y, 
                               points[next].x, points[next].y);
        }
    }
}

void HexGridRenderer::renderTileText(const HexCoordinate& coord, const std::string& text, const SDL_Color& color) {
    // Use the same coordinate system as hexagon rendering for consistency
    float centerX, centerY;
    hexToScreen(coord, centerX, centerY);
    
    // Convert to absolute window coordinates for text rendering
    float windowX = centerX + x_;
    float windowY = centerY + y_;
    
    // Get actual text dimensions for proper centering
    int textWidth, textHeight;
    getTextSize(text, textWidth, textHeight);
    
    // Center the text in the hexagon
    int textX = static_cast<int>(windowX - textWidth / 2);
    int textY = static_cast<int>(windowY - textHeight / 2);
    
    renderText(text, textX, textY, color);
}

void HexGridRenderer::renderAnimations() {
    // Update and render animations
    for (auto it = animations_.begin(); it != animations_.end(); ) {
        auto& anim = *it;
        
        if (anim.elapsed >= anim.duration) {
            it = animations_.erase(it);
        } else {
            // Render animation frame based on type
            float progress = anim.elapsed / anim.duration;
            SDL_Color animColor = anim.color;
            
            switch (anim.type) {
                case TileAnimation::PULSE:
                    animColor.a = static_cast<Uint8>(128 + 127 * sin(progress * M_PI * 4));
                    break;
                case TileAnimation::FADE:
                    animColor.a = static_cast<Uint8>(255 * (1.0f - progress));
                    break;
                default:
                    break;
            }
            
            renderHexagonOutline(anim.coord, animColor, 2.0f);
            ++it;
        }
    }
}

void HexGridRenderer::renderOverlays() {
    // Render any special overlays (formations, siege lines, etc.)
    // This would be expanded based on specific needs
}

void HexGridRenderer::renderDebugInfo() {
    if (!grid_) return;
    
    // Render debug information
    SDL_Color debugColor = {255, 255, 0, 255};
    
    if (hasSelection_) {
        const HexTile* tile = grid_->getTile(selectedTile_);
        if (tile) {
            std::string debugText = "Selected: " + HexTileUtils::getTerrainName(tile->getTerrainType()) +
                                   " Height: " + std::to_string(tile->getHeight()) +
                                   " Cost: " + std::to_string(tile->getMovementCost());
            renderText(debugText, 10, height_ - 30, debugColor);
        }
    }
    
    // Show grid statistics
    auto stats = grid_->getStatistics();
    std::string statsText = "Tiles: " + std::to_string(stats.totalTiles) + 
                           " Zoom: " + std::to_string(config_.zoomLevel);
    renderText(statsText, 10, height_ - 50, debugColor);
}
