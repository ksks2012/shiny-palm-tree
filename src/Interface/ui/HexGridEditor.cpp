#include "Interface/ui/HexGridEditor.h"
#include "Systems/SDLManager.h"
#include <algorithm>
#include <fstream>
#include <sstream>

const std::vector<TerrainType> HexGridEditor::COMMON_TERRAINS = {
    TerrainType::PLAIN, TerrainType::FOREST, TerrainType::MOUNTAIN,
    TerrainType::RIVER, TerrainType::SWAMP, TerrainType::ROAD,
    TerrainType::CITY_WALL, TerrainType::CAMP, TerrainType::FORTIFICATION
};

const std::vector<std::string> HexGridEditor::ROMAN_UNIT_TYPES = {
    "Legionnaire", "Centurion", "Archer", "Cavalry", "Engineer",
    "Ballista", "Catapult", "General", "Barbarian Warrior", "War Elephant"
};

HexGridEditor::HexGridEditor(int x, int y, int width, int height, SDLManager& sdlManager)
    : UIComponent(x, y, width, height, sdlManager) {
    
    // Initialize with a default grid
    grid_ = std::make_shared<HexGrid>(20, 15);
    
    // Calculate UI layout
    calculateLayout();
    
    // Create renderer for the main grid area
    renderer_ = std::make_unique<HexGridRenderer>(
        rendererRect_.x, rendererRect_.y, 
        rendererRect_.w, rendererRect_.h, 
        sdlManager, grid_
    );
    
    // Initialize view to center of grid
    if (renderer_ && grid_) {
        renderer_->resetView();
        renderer_->centerOnCoordinate(HexCoordinate(0, 0, 0));
    }
    
    // Set up default state
    state_.currentTool = HexEditorTool::SELECT;
    state_.selectedTerrain = TerrainType::PLAIN;
    state_.brushSize = 1;
}

void HexGridEditor::render() {
    // Render background for panel areas only (not over the grid area)
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_SetRenderDrawColor(renderer, 50, 40, 30, 255); // Dark parchment color
    
    // Fill left panel area
    if (state_.showToolPanel || state_.showTerrainPanel) {
        SDL_Rect leftPanelArea = {x_, y_, 200, height_};
        SDL_RenderFillRect(renderer, &leftPanelArea);
    }
    
    // Fill right panel area  
    if (state_.showPropertiesPanel) {
        SDL_Rect rightPanelArea = {x_ + width_ - 200, y_, 200, height_};
        SDL_RenderFillRect(renderer, &rightPanelArea);
    }
    
    // Fill status bar area
    SDL_Rect statusArea = {x_, y_ + height_ - 30, width_, 30};
    SDL_RenderFillRect(renderer, &statusArea);
    
    // Render main grid (now it won't be covered by background)
    if (renderer_) {
        renderer_->render();
    }
    
    // Render UI panels
    if (state_.showToolPanel) {
        renderToolPanel();
    }
    
    if (state_.showTerrainPanel) {
        renderTerrainPanel();
    }
    
    if (state_.showPropertiesPanel) {
        renderPropertiesPanel();
    }
    
    renderStatusBar();
    
    // Render selection highlights
    updateSelectionHighlights();
    
    // Render formation highlights if in formation mode
    if (isFormationMode()) {
        updateFormationHighlights();
    }
}

void HexGridEditor::handleEvent(const SDL_Event& event) {
    if (!enabled_) return;
    
    switch (event.type) {
        case SDL_KEYDOWN:
            handleKeyboardShortcuts(event);
            break;
            
        case SDL_MOUSEBUTTONDOWN:
            if (isPointInside(event.button.x, event.button.y)) {
                // Let renderer handle its own events for view control (pan, zoom, etc.)
                if (renderer_ && renderer_->isPointInside(event.button.x, event.button.y)) {
                    renderer_->handleEvent(event);
                }
                
                // Handle editor-specific interactions
                handleGridInteraction(event);
                handleToolSelection(event);
                handleTerrainSelection(event);
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            // Let renderer handle button release events (for pan cancellation, etc.)
            if (renderer_) {
                renderer_->handleEvent(event);
            }
            break;
            
        case SDL_MOUSEMOTION:
            // Let renderer handle motion events for hover, panning, etc.
            if (renderer_ && renderer_->isPointInside(event.motion.x, event.motion.y)) {
                renderer_->handleEvent(event);
            }
            
            // Handle drag operations for paint tool
            if (state_.currentTool == HexEditorTool::PAINT && (event.motion.state & SDL_BUTTON_LMASK)) {
                // Check if the drag is within the renderer's bounds
                if (renderer_ && renderer_->isPointInside(event.motion.x, event.motion.y)) {
                    // Convert absolute screen coordinates to relative coordinates for the renderer
                    int relativeX = event.motion.x - renderer_->getX();
                    int relativeY = event.motion.y - renderer_->getY();
                    
                    HexCoordinate coord = renderer_->screenToHex(relativeX, relativeY);
                    if (isCoordinateInBounds(coord)) {
                        executePaintTool(coord);
                    }
                }
            }
            break;
            
        case SDL_MOUSEWHEEL:
            // Forward mouse wheel events to renderer for zoom functionality
            if (renderer_) {
                renderer_->handleEvent(event);
            }
            break;
    }
}

void HexGridEditor::newMap(int width, int height) {
    grid_ = std::make_shared<HexGrid>(width, height);
    renderer_->setGrid(grid_);
    clearSelection();
    
    // Clear history
    actionHistory_.clear();
    undoIndex_ = 0;
}

void HexGridEditor::saveMap(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << exportToJSON();
        file.close();
        
        if (onMapSaved) {
            onMapSaved(filename);
        }
    }
}

void HexGridEditor::loadMap(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        importFromJSON(buffer.str());
        file.close();
        
        if (onMapLoaded) {
            onMapLoaded(filename);
        }
    }
}

void HexGridEditor::paintTerrain(const HexCoordinate& coord, TerrainType terrain) {
    if (!isCoordinateInBounds(coord)) return;
    
    HexTile* tile = grid_->getTile(coord);
    if (!tile) return;
    
    TerrainType oldTerrain = tile->getTerrainType();
    if (oldTerrain == terrain) return; // No change needed
    
    // Create undo action
    EditorAction action;
    action.type = EditorAction::TERRAIN_CHANGE;
    action.coordinate = coord;
    action.oldTerrain = oldTerrain;
    action.newTerrain = terrain;
    action.description = "Paint " + HexTileUtils::getTerrainName(terrain);
    
    // Execute change
    tile->setTerrainType(terrain);
    
    addToHistory(action);
    
    if (onActionExecuted) {
        onActionExecuted(action);
    }
}

void HexGridEditor::fillTerrain(const HexCoordinate& startCoord, TerrainType newTerrain) {
    if (!isCoordinateInBounds(startCoord)) return;
    
    HexTile* startTile = grid_->getTile(startCoord);
    if (!startTile) return;
    
    TerrainType targetTerrain = startTile->getTerrainType();
    if (targetTerrain == newTerrain) return; // No change needed
    
    std::vector<HexCoordinate> filled;
    floodFillRecursive(startCoord, targetTerrain, newTerrain, filled);
    
    if (!filled.empty()) {
        // Create compound action for undo
        EditorAction action;
        action.type = EditorAction::TERRAIN_CHANGE;
        action.coordinate = startCoord; // Use start coordinate as reference
        action.oldTerrain = targetTerrain;
        action.newTerrain = newTerrain;
        action.description = "Fill " + std::to_string(filled.size()) + " tiles with " + 
                           HexTileUtils::getTerrainName(newTerrain);
        
        addToHistory(action);
        
        if (onActionExecuted) {
            onActionExecuted(action);
        }
    }
}

void HexGridEditor::floodFillRecursive(const HexCoordinate& coord, TerrainType targetTerrain, 
                                      TerrainType newTerrain, std::vector<HexCoordinate>& filled) const {
    if (!isCoordinateInBounds(coord)) return;
    
    HexTile* tile = grid_->getTile(coord);
    if (!tile || tile->getTerrainType() != targetTerrain) return;
    
    // Check if already filled
    if (std::find(filled.begin(), filled.end(), coord) != filled.end()) return;
    
    // Fill this tile
    tile->setTerrainType(newTerrain);
    filled.push_back(coord);
    
    // Recursively fill neighbors
    for (const HexCoordinate& neighbor : grid_->getNeighbors(coord)) {
        floodFillRecursive(neighbor, targetTerrain, newTerrain, filled);
    }
}

void HexGridEditor::adjustHeight(const HexCoordinate& coord, int height) {
    if (!isCoordinateInBounds(coord)) return;
    
    HexTile* tile = grid_->getTile(coord);
    if (!tile) return;
    
    int oldHeight = tile->getHeight();
    if (oldHeight == height) return;
    
    EditorAction action;
    action.type = EditorAction::HEIGHT_CHANGE;
    action.coordinate = coord;
    action.oldHeight = oldHeight;
    action.newHeight = height;
    action.description = "Set height to " + std::to_string(height);
    
    tile->setHeight(height);
    addToHistory(action);
    
    if (onActionExecuted) {
        onActionExecuted(action);
    }
}

void HexGridEditor::placeUnit(const HexCoordinate& coord, const std::string& unitType) {
    if (!isCoordinateInBounds(coord)) return;
    
    HexTile* tile = grid_->getTile(coord);
    if (!tile || !tile->isPassable()) return;
    
    std::string oldUnit = tile->getOccupantId();
    
    EditorAction action;
    action.type = (oldUnit.empty()) ? EditorAction::UNIT_PLACE : EditorAction::UNIT_REMOVE;
    action.coordinate = coord;
    action.oldUnit = oldUnit;
    action.newUnit = unitType;
    action.description = "Place " + unitType;
    
    tile->setOccupant(unitType);
    addToHistory(action);
    
    if (onActionExecuted) {
        onActionExecuted(action);
    }
}

void HexGridEditor::removeUnit(const HexCoordinate& coord) {
    if (!isCoordinateInBounds(coord)) return;
    
    HexTile* tile = grid_->getTile(coord);
    if (!tile || !tile->isOccupied()) return;
    
    std::string oldUnit = tile->getOccupantId();
    
    EditorAction action;
    action.type = EditorAction::UNIT_REMOVE;
    action.coordinate = coord;
    action.oldUnit = oldUnit;
    action.newUnit = "";
    action.description = "Remove " + oldUnit;
    
    tile->setOccupant("");
    addToHistory(action);
    
    if (onActionExecuted) {
        onActionExecuted(action);
    }
}

void HexGridEditor::selectTile(const HexCoordinate& coord) {
    clearSelection();
    if (isCoordinateInBounds(coord)) {
        selectedTiles_.push_back(coord);
        primarySelection_ = coord;
        
        if (onTileSelected) {
            onTileSelected(coord);
        }
    }
}

void HexGridEditor::addToSelection(const HexCoordinate& coord) {
    if (isCoordinateInBounds(coord) && !isSelected(coord)) {
        selectedTiles_.push_back(coord);
    }
}

void HexGridEditor::clearSelection() {
    selectedTiles_.clear();
    primarySelection_ = HexCoordinate(-9999, -9999, -9999);
}

bool HexGridEditor::isSelected(const HexCoordinate& coord) const {
    return std::find(selectedTiles_.begin(), selectedTiles_.end(), coord) != selectedTiles_.end();
}

void HexGridEditor::executeSelectTool(const HexCoordinate& coord, bool addToSelection) {
    if (addToSelection) {
        this->addToSelection(coord);
    } else {
        selectTile(coord);
    }
}

void HexGridEditor::executePaintTool(const HexCoordinate& coord) {
    auto brushArea = getBrushArea(coord, state_.brushSize);
    
    for (const HexCoordinate& brushCoord : brushArea) {
        paintTerrain(brushCoord, state_.selectedTerrain);
    }
}

void HexGridEditor::executeFillTool(const HexCoordinate& coord) {
    fillTerrain(coord, state_.selectedTerrain);
}

void HexGridEditor::executeHeightTool(const HexCoordinate& coord) {
    auto brushArea = getBrushArea(coord, state_.brushSize);
    
    for (const HexCoordinate& brushCoord : brushArea) {
        adjustHeight(brushCoord, state_.selectedHeight);
    }
}

void HexGridEditor::executeUnitPlaceTool(const HexCoordinate& coord) {
    if (!state_.selectedUnitType.empty()) {
        placeUnit(coord, state_.selectedUnitType);
    }
}

void HexGridEditor::executeFormationTool(const HexCoordinate& coord) {
    addToFormation(coord);
}

void HexGridEditor::executeMeasureTool(const HexCoordinate& coord) {
    if (primarySelection_.isValid()) {
        int distance = primarySelection_.distanceTo(coord);
        // Display distance in UI (implementation depends on UI system)
    }
}

std::vector<HexCoordinate> HexGridEditor::getBrushArea(const HexCoordinate& center, int size) const {
    return center.getCoordinatesInRange(size - 1);
}

void HexGridEditor::addToFormation(const HexCoordinate& coord) {
    if (isCoordinateInBounds(coord)) {
        state_.currentFormation.push_back(coord);
    }
}

void HexGridEditor::finalizeFormation() {
    if (!state_.currentFormation.empty()) {
        // Mark all tiles in formation
        for (const HexCoordinate& coord : state_.currentFormation) {
            HexTile* tile = grid_->getTile(coord);
            if (tile) {
                tile->setFormationTile(true);
            }
        }
        
        state_.currentFormation.clear();
    }
}

void HexGridEditor::cancelFormation() {
    state_.currentFormation.clear();
}

void HexGridEditor::undo() {
    if (!canUndo()) return;
    
    --undoIndex_;
    const EditorAction& action = actionHistory_[undoIndex_];
    
    // Reverse the action
    switch (action.type) {
        case EditorAction::TERRAIN_CHANGE: {
            HexTile* tile = grid_->getTile(action.coordinate);
            if (tile) {
                tile->setTerrainType(action.oldTerrain);
            }
            break;
        }
        case EditorAction::HEIGHT_CHANGE: {
            HexTile* tile = grid_->getTile(action.coordinate);
            if (tile) {
                tile->setHeight(action.oldHeight);
            }
            break;
        }
        case EditorAction::UNIT_PLACE:
        case EditorAction::UNIT_REMOVE: {
            HexTile* tile = grid_->getTile(action.coordinate);
            if (tile) {
                tile->setOccupant(action.oldUnit);
            }
            break;
        }
    }
}

void HexGridEditor::redo() {
    if (!canRedo()) return;
    
    const EditorAction& action = actionHistory_[undoIndex_];
    ++undoIndex_;
    
    // Re-execute the action
    switch (action.type) {
        case EditorAction::TERRAIN_CHANGE: {
            HexTile* tile = grid_->getTile(action.coordinate);
            if (tile) {
                tile->setTerrainType(action.newTerrain);
            }
            break;
        }
        case EditorAction::HEIGHT_CHANGE: {
            HexTile* tile = grid_->getTile(action.coordinate);
            if (tile) {
                tile->setHeight(action.newHeight);
            }
            break;
        }
        case EditorAction::UNIT_PLACE:
        case EditorAction::UNIT_REMOVE: {
            HexTile* tile = grid_->getTile(action.coordinate);
            if (tile) {
                tile->setOccupant(action.newUnit);
            }
            break;
        }
    }
}

void HexGridEditor::addToHistory(const EditorAction& action) {
    // Remove any actions after current position
    clearHistoryAfterIndex();
    
    // Add new action
    actionHistory_.push_back(action);
    ++undoIndex_;
    
    // Limit history size
    if (actionHistory_.size() > MAX_HISTORY_SIZE) {
        actionHistory_.erase(actionHistory_.begin());
        --undoIndex_;
    }
}

void HexGridEditor::clearHistoryAfterIndex() {
    if (undoIndex_ < actionHistory_.size()) {
        actionHistory_.erase(actionHistory_.begin() + undoIndex_, actionHistory_.end());
    }
}

void HexGridEditor::calculateLayout() {
    const int PANEL_WIDTH = 200;
    const int STATUS_HEIGHT = 30;
    
    // Tool panel on the left
    toolPanelRect_ = {x_, y_, PANEL_WIDTH, height_ / 2};
    
    // Terrain panel below tool panel
    terrainPanelRect_ = {x_, y_ + height_ / 2, PANEL_WIDTH, height_ / 2 - STATUS_HEIGHT};
    
    // Properties panel on the right
    propertiesPanelRect_ = {x_ + width_ - PANEL_WIDTH, y_, PANEL_WIDTH, height_ - STATUS_HEIGHT};
    
    // Keep renderer position fixed regardless of panel state
    // This ensures hexagon grid, text, and click events don't move when panels are toggled
    // Use panel-off state as the standard (full width without panel offsets)
    
    // Main renderer uses full window area (panel-off state as standard)
    rendererRect_ = {
        x_, 
        y_, 
        width_, 
        height_ - STATUS_HEIGHT
    };
    
    // Update renderer size if it exists
    if (renderer_) {
        renderer_->setPosition(rendererRect_.x, rendererRect_.y);
        renderer_->setSize(rendererRect_.w, rendererRect_.h);
    }
}

void HexGridEditor::renderToolPanel() {
    SDL_Rect panelRect = toolPanelRect_;
    
    // Render panel background in correct area
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_SetRenderDrawColor(renderer, 30, 25, 20, 255);
    SDL_RenderFillRect(renderer, &panelRect);
    
    // Render border around panel
    SDL_SetRenderDrawColor(renderer, 100, 80, 60, 255);
    SDL_RenderDrawRect(renderer, &panelRect);
    
    // Tool buttons (simplified rendering)
    const char* toolNames[] = {"Select", "Paint", "Fill", "Height", "Units", "Events", "Formation", "Measure"};
    
    for (int i = 0; i < 8; ++i) {
        SDL_Rect buttonRect = {
            panelRect.x + 10,
            panelRect.y + 10 + i * 30,
            panelRect.w - 20,
            25
        };
        
        SDL_Color buttonColor = (static_cast<int>(state_.currentTool) == i) ? 
            SDL_Color{100, 150, 200, 255} : SDL_Color{80, 80, 80, 255};
        
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(sdlManager_.getRenderer(), &buttonRect);
        
        renderText(toolNames[i], buttonRect.x + 5, buttonRect.y + 5, {255, 255, 255, 255});
    }
}

void HexGridEditor::renderTerrainPanel() {
    SDL_Rect panelRect = terrainPanelRect_;
    
    // Render panel background in correct area
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_SetRenderDrawColor(renderer, 30, 25, 20, 255);
    SDL_RenderFillRect(renderer, &panelRect);
    
    // Render border around panel
    SDL_SetRenderDrawColor(renderer, 100, 80, 60, 255);
    SDL_RenderDrawRect(renderer, &panelRect);
    
    // Terrain type buttons
    int buttonIndex = 0;
    for (TerrainType terrain : COMMON_TERRAINS) {
        SDL_Rect buttonRect = {
            panelRect.x + 10 + (buttonIndex % 2) * 90,
            panelRect.y + 10 + (buttonIndex / 2) * 30,
            85,
            25
        };
        
        SDL_Color terrainColor = HexTileUtils::getTerrainColor(terrain);
        if (terrain == state_.selectedTerrain) {
            // Highlight selected terrain
            terrainColor.r = std::min(255, terrainColor.r + 50);
            terrainColor.g = std::min(255, terrainColor.g + 50);
            terrainColor.b = std::min(255, terrainColor.b + 50);
        }
        
        SDL_SetRenderDrawColor(sdlManager_.getRenderer(), terrainColor.r, terrainColor.g, terrainColor.b, 255);
        SDL_RenderFillRect(sdlManager_.getRenderer(), &buttonRect);
        
        std::string terrainName = HexTileUtils::getTerrainName(terrain);
        if (terrainName.length() > 8) {
            terrainName = terrainName.substr(0, 8);
        }
        
        renderText(terrainName, buttonRect.x + 2, buttonRect.y + 5, {255, 255, 255, 255});
        ++buttonIndex;
    }
}

void HexGridEditor::renderPropertiesPanel() {
    SDL_Rect panelRect = propertiesPanelRect_;
    
    // Render panel background in correct area
    SDL_Renderer* renderer = sdlManager_.getRenderer();
    SDL_SetRenderDrawColor(renderer, 30, 25, 20, 255);
    SDL_RenderFillRect(renderer, &panelRect);
    
    // Render border around panel
    SDL_SetRenderDrawColor(renderer, 100, 80, 60, 255);
    SDL_RenderDrawRect(renderer, &panelRect);
    
    // Show properties of selected tile
    if (!selectedTiles_.empty() && grid_) {
        const HexTile* tile = grid_->getTile(selectedTiles_[0]);
        if (tile) {
            int yOffset = panelRect.y + 10;
            
            renderText("Selected Tile:", panelRect.x + 5, yOffset, {255, 255, 255, 255});
            yOffset += 20;
            
            std::string terrainName = HexTileUtils::getTerrainName(tile->getTerrainType());
            renderText("Terrain: " + terrainName, panelRect.x + 5, yOffset, {200, 200, 200, 255});
            yOffset += 15;
            
            renderText("Height: " + std::to_string(tile->getHeight()), panelRect.x + 5, yOffset, {200, 200, 200, 255});
            yOffset += 15;
            
            renderText("Movement Cost: " + std::to_string(tile->getMovementCost()), panelRect.x + 5, yOffset, {200, 200, 200, 255});
            yOffset += 15;
            
            if (tile->isOccupied()) {
                renderText("Unit: " + tile->getOccupantId(), panelRect.x + 5, yOffset, {255, 200, 100, 255});
                yOffset += 15;
            }
            
            if (tile->getDefensiveBonus() > 0) {
                renderText("Defense: +" + std::to_string(tile->getDefensiveBonus()) + "%", 
                          panelRect.x + 5, yOffset, {100, 255, 100, 255});
            }
        }
    }
}

void HexGridEditor::renderStatusBar() {
    SDL_Rect statusRect = {x_, y_ + height_ - 30, width_, 30};
    SDL_SetRenderDrawColor(sdlManager_.getRenderer(), 20, 15, 10, 255);
    SDL_RenderFillRect(sdlManager_.getRenderer(), &statusRect);
    
    std::string status = "Tool: ";
    switch (state_.currentTool) {
        case HexEditorTool::SELECT: status += "Select"; break;
        case HexEditorTool::PAINT: status += "Paint"; break;
        case HexEditorTool::FILL: status += "Fill"; break;
        case HexEditorTool::HEIGHT: status += "Height"; break;
        case HexEditorTool::UNIT_PLACE: status += "Units"; break;
        case HexEditorTool::EVENT_EDIT: status += "Events"; break;
        case HexEditorTool::FORMATION: status += "Formation"; break;
        case HexEditorTool::MEASURE: status += "Measure"; break;
    }
    
    if (grid_) {
        status += " | Grid: " + std::to_string(grid_->getWidth()) + "x" + std::to_string(grid_->getHeight());
        
        if (!selectedTiles_.empty()) {
            status += " | Selected: " + std::to_string(selectedTiles_.size()) + " tiles";
        }
    }
    
    renderText(status, statusRect.x + 5, statusRect.y + 8, {255, 255, 255, 255});
}

void HexGridEditor::handleGridInteraction(const SDL_Event& event) {
    if (event.type != SDL_MOUSEBUTTONDOWN || !renderer_) return;

    // Check if the click is within the renderer's bounds first
    if (!renderer_->isPointInside(event.button.x, event.button.y)) {
        return;
    }
    
    // Get the selected coordinate from the renderer (which already handles coordinate conversion)
    HexCoordinate coord = renderer_->getSelectedTile();
    
    // If no selection, try to get the coordinate directly
    if (!renderer_->hasSelection()) {
        // Convert absolute screen coordinates to coordinates relative to the renderer component
        int relativeX = event.button.x - renderer_->getX();
        int relativeY = event.button.y - renderer_->getY();
        coord = renderer_->screenToHex(relativeX, relativeY);
    }
    
    if (!isCoordinateInBounds(coord)) return;
    
    bool shiftPressed = (SDL_GetModState() & KMOD_SHIFT) != 0;
    
    switch (state_.currentTool) {
        case HexEditorTool::SELECT:
            executeSelectTool(coord, shiftPressed);
            break;
        case HexEditorTool::PAINT:
            executePaintTool(coord);
            break;
        case HexEditorTool::FILL:
            executeFillTool(coord);
            break;
        case HexEditorTool::HEIGHT:
            executeHeightTool(coord);
            break;
        case HexEditorTool::UNIT_PLACE:
            executeUnitPlaceTool(coord);
            break;
        case HexEditorTool::FORMATION:
            executeFormationTool(coord);
            break;
        case HexEditorTool::MEASURE:
            executeMeasureTool(coord);
            break;
    }
}

void HexGridEditor::handleKeyboardShortcuts(const SDL_Event& event) {
    if (event.type != SDL_KEYDOWN) return;
    
    switch (event.key.keysym.sym) {
        case SDLK_1: setTool(HexEditorTool::SELECT); break;
        case SDLK_2: setTool(HexEditorTool::PAINT); break;
        case SDLK_3: setTool(HexEditorTool::FILL); break;
        case SDLK_4: setTool(HexEditorTool::HEIGHT); break;
        case SDLK_5: setTool(HexEditorTool::UNIT_PLACE); break;
        
        case SDLK_z:
            if (SDL_GetModState() & KMOD_CTRL) {
                if (SDL_GetModState() & KMOD_SHIFT) {
                    redo();
                } else {
                    undo();
                }
            }
            break;
            
        case SDLK_ESCAPE:
            clearSelection();
            if (isFormationMode()) {
                cancelFormation();
            }
            break;
            
        case SDLK_DELETE:
            // Delete units from selected tiles
            for (const HexCoordinate& coord : selectedTiles_) {
                removeUnit(coord);
            }
            break;
    }
}

bool HexGridEditor::isCoordinateInBounds(const HexCoordinate& coord) const {
    return grid_ && grid_->isValidCoordinate(coord);
}

void HexGridEditor::updateSelectionHighlights() {
    if (!renderer_) return;
    
    // Clear existing selection highlights
    renderer_->clearHighlights();
    
    // Highlight selected tiles
    for (const HexCoordinate& coord : selectedTiles_) {
        renderer_->highlightTile(coord, {255, 255, 0, 128}); // Yellow highlight
    }
}

void HexGridEditor::updateFormationHighlights() {
    if (!renderer_ || state_.currentFormation.empty()) return;
    
    // Highlight formation tiles
    for (const HexCoordinate& coord : state_.currentFormation) {
        renderer_->highlightTile(coord, {128, 0, 255, 128}); // Purple highlight
    }
}

std::string HexGridEditor::exportToJSON() const {
    if (!grid_) return "{}";
    return grid_->toJSON();
}

void HexGridEditor::importFromJSON(const std::string& json) {
    if (grid_) {
        grid_->fromJSON(json);
        renderer_->setGrid(grid_);
    }
}

// Historical battlefield templates
void HexGridEditor::loadCannaeBattlefield() {
    if (grid_) {
        grid_->setupCannaeBattlefield();
        // Center view on the battlefield
        if (renderer_) {
            renderer_->resetView();
            renderer_->centerOnCoordinate(HexCoordinate(0, 0, 0));
        }
    }
}

void HexGridEditor::loadAlesiaBattlefield() {
    if (grid_) {
        grid_->setupAlesiaBattlefield();
        // Center view on the battlefield
        if (renderer_) {
            renderer_->resetView();
            renderer_->centerOnCoordinate(HexCoordinate(0, 0, 0));
        }
    }
}

void HexGridEditor::loadTeutobergBattlefield() {
    if (grid_) {
        grid_->setupTeutobergBattlefield();
        // Center view on the battlefield
        if (renderer_) {
            renderer_->resetView();
            renderer_->centerOnCoordinate(HexCoordinate(0, 0, 0));
        }
    }
}

void HexGridEditor::updateLayout() {
    calculateLayout();
}

void HexGridEditor::handleToolSelection(const SDL_Event& event) {
    if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT) return;
    
    // Check if click is within tool panel
    if (!isPointInside(event.button.x, event.button.y)) return;
    
    int relativeX = event.button.x - toolPanelRect_.x;
    int relativeY = event.button.y - toolPanelRect_.y;
    
    // Simple tool selection based on Y position
    if (relativeX >= 10 && relativeX <= toolPanelRect_.w - 20) {
        int toolIndex = (relativeY - 10) / 30;
        if (toolIndex >= 0 && toolIndex < 8) {
            setTool(static_cast<HexEditorTool>(toolIndex));
        }
    }
}

void HexGridEditor::handleTerrainSelection(const SDL_Event& event) {
    if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT) return;
    
    // Check if click is within terrain panel
    SDL_Point point = {event.button.x, event.button.y};
    if (SDL_PointInRect(&point, &terrainPanelRect_)) {
        int relativeX = event.button.x - terrainPanelRect_.x;
        int relativeY = event.button.y - terrainPanelRect_.y;
        
        // Calculate which terrain button was clicked (2 columns)
        int buttonIndex = (relativeY - 10) / 30 * 2 + (relativeX - 10) / 90;
        
        if (buttonIndex >= 0 && buttonIndex < static_cast<int>(COMMON_TERRAINS.size())) {
            setSelectedTerrain(COMMON_TERRAINS[buttonIndex]);
        }
    }
}

HexGridEditor::MapValidation HexGridEditor::validateMap() const {
    MapValidation validation;
    
    if (!grid_) {
        validation.isValid = false;
        validation.errors.push_back("No grid loaded");
        return validation;
    }
    
    auto stats = grid_->getStatistics();
    
    // Check for isolated areas
    if (stats.impassableTiles > stats.totalTiles * 0.8) {
        validation.warnings.push_back("Map has too many impassable tiles (>80%)");
    }
    
    // Check for supply points
    if (stats.supplyPoints == 0) {
        validation.warnings.push_back("No supply points found - units cannot resupply");
    }
    
    // Check for buildable tiles
    if (stats.buildableTiles == 0) {
        validation.warnings.push_back("No buildable tiles - engineers cannot construct fortifications");
    }
    
    return validation;
}

void HexGridEditor::setPanelsVisible(bool visible) {
    state_.showToolPanel = visible;
    state_.showTerrainPanel = visible;
    state_.showPropertiesPanel = visible;
    calculateLayout();
}

void HexGridEditor::togglePanels() {
    bool currentState = state_.showToolPanel;
    setPanelsVisible(!currentState);
}
