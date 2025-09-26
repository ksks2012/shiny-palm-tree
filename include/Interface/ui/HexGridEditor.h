#pragma once
#include "UIComponent.h"
#include "HexGrid.h"
#include "HexGridRenderer.h"
#include <memory>
#include <vector>
#include <functional>

/**
 * Editor tools for hexagonal grid editing
 */
enum class HexEditorTool {
    SELECT,         // Selection tool for inspecting tiles
    PAINT,          // Paint terrain on tiles
    FILL,           // Flood fill terrain
    HEIGHT,         // Adjust tile height
    UNIT_PLACE,     // Place units on tiles
    EVENT_EDIT,     // Edit tile events and triggers
    FORMATION,      // Formation placement tool
    MEASURE         // Distance/area measurement
};

/**
 * Editor state and settings
 */
struct HexEditorState {
    HexEditorTool currentTool = HexEditorTool::SELECT;
    TerrainType selectedTerrain = TerrainType::PLAIN;
    int selectedHeight = 0;
    std::string selectedUnitType;
    
    // Paint settings
    int brushSize = 1;
    bool respectExistingUnits = true;
    
    // Formation settings
    std::vector<HexCoordinate> currentFormation;
    std::string formationType = "legion";
    
    // UI state
    bool showToolPanel = true;
    bool showTerrainPanel = true;
    bool showPropertiesPanel = true;
    bool showMinimap = false;
};

/**
 * Action for undo/redo system
 */
struct EditorAction {
    enum Type { TERRAIN_CHANGE, HEIGHT_CHANGE, UNIT_PLACE, UNIT_REMOVE, PROPERTY_CHANGE } type;
    HexCoordinate coordinate;
    
    // Before/after state
    TerrainType oldTerrain, newTerrain;
    int oldHeight, newHeight;
    std::string oldUnit, newUnit;
    TileProperties oldProperties, newProperties;
    
    std::string description;
};

/**
 * Comprehensive hexagonal grid map editor for Roman warfare scenarios
 * Provides intuitive tools for creating detailed tactical battlefields
 */
class HexGridEditor : public UIComponent {
public:
    HexGridEditor(int x, int y, int width, int height, SDLManager& sdlManager);
    ~HexGridEditor() = default;
    
    // UIComponent interface
    void render() override;
    void handleEvent(const SDL_Event& event) override;
    bool canReceiveFocus() const override { return true; }
    
    // Grid management
    void newMap(int width, int height);
    void loadMap(const std::string& filename);
    void saveMap(const std::string& filename) const;
    std::shared_ptr<HexGrid> getGrid() const { return grid_; }
    HexGridRenderer* getRenderer() const { return renderer_.get(); }
    
    // Tool management
    void setTool(HexEditorTool tool) { state_.currentTool = tool; }
    HexEditorTool getCurrentTool() const { return state_.currentTool; }
    
    // Terrain editing
    void setSelectedTerrain(TerrainType terrain) { state_.selectedTerrain = terrain; }
    TerrainType getSelectedTerrain() const { return state_.selectedTerrain; }
    void paintTerrain(const HexCoordinate& coord, TerrainType terrain);
    void fillTerrain(const HexCoordinate& startCoord, TerrainType newTerrain);
    void paintTerrainArea(const std::vector<HexCoordinate>& coords, TerrainType terrain);
    
    // Height editing
    void setSelectedHeight(int height) { state_.selectedHeight = std::max(0, std::min(3, height)); }
    int getSelectedHeight() const { return state_.selectedHeight; }
    void adjustHeight(const HexCoordinate& coord, int height);
    void adjustHeightArea(const std::vector<HexCoordinate>& coords, int height);
    
    // Unit placement
    void setSelectedUnitType(const std::string& unitType) { state_.selectedUnitType = unitType; }
    const std::string& getSelectedUnitType() const { return state_.selectedUnitType; }
    void placeUnit(const HexCoordinate& coord, const std::string& unitType);
    void removeUnit(const HexCoordinate& coord);
    
    // Formation tools
    void startFormationPlacement(const std::string& formationType);
    void addToFormation(const HexCoordinate& coord);
    void finalizeFormation();
    void cancelFormation();
    bool isFormationMode() const { return !state_.currentFormation.empty(); }
    
    // Selection and multi-selection
    void selectTile(const HexCoordinate& coord);
    void addToSelection(const HexCoordinate& coord);
    void clearSelection();
    const std::vector<HexCoordinate>& getSelection() const { return selectedTiles_; }
    bool isSelected(const HexCoordinate& coord) const;
    
    // Copy/paste operations
    void copySelection();
    void pasteAtLocation(const HexCoordinate& targetCoord);
    bool canPaste() const { return !clipboard_.empty(); }
    
    // Undo/redo system
    void executeAction(const EditorAction& action);
    void undo();
    void redo();
    bool canUndo() const { return undoIndex_ > 0; }
    bool canRedo() const { return undoIndex_ < actionHistory_.size(); }
    
    // Historical battlefield templates
    void loadCannaeBattlefield();
    void loadAlesiaBattlefield();
    void loadTeutobergBattlefield();
    
    // Validation and analysis
    struct MapValidation {
        bool isValid = true;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        int unreachableAreas = 0;
        int isolatedSupplyPoints = 0;
    };
    MapValidation validateMap() const;
    
    // Export for game engine
    std::string exportToJSON() const;
    void importFromJSON(const std::string& json);
    
    // Layout update
    void updateLayout();
    
    // UI panels and settings
    const HexEditorState& getState() const { return state_; }
    HexEditorState& getState() { return state_; }
    const HexEditorState& getPanelState() const { return state_; }
    void setPanelsVisible(bool visible);
    void togglePanels();
    
    // Event callbacks for UI integration
    std::function<void(const HexCoordinate&)> onTileSelected;
    std::function<void(const EditorAction&)> onActionExecuted;
    std::function<void(const std::string&)> onMapSaved;
    std::function<void(const std::string&)> onMapLoaded;

private:
    std::shared_ptr<HexGrid> grid_;
    std::unique_ptr<HexGridRenderer> renderer_;
    HexEditorState state_;
    
    // Selection system
    std::vector<HexCoordinate> selectedTiles_;
    HexCoordinate primarySelection_{-9999, -9999, -9999};
    
    // Clipboard for copy/paste
    struct ClipboardEntry {
        HexCoordinate relativeCoord;
        TerrainType terrain;
        int height;
        TileProperties properties;
        std::string unitType;
    };
    std::vector<ClipboardEntry> clipboard_;
    
    // Undo/redo system
    std::vector<EditorAction> actionHistory_;
    size_t undoIndex_ = 0;
    static const size_t MAX_HISTORY_SIZE = 100;
    
    // UI layout
    SDL_Rect toolPanelRect_;
    SDL_Rect terrainPanelRect_;
    SDL_Rect propertiesPanelRect_;
    SDL_Rect rendererRect_;
    
    // Tool-specific helpers
    std::vector<HexCoordinate> getBrushArea(const HexCoordinate& center, int size) const;
    void floodFillRecursive(const HexCoordinate& coord, TerrainType targetTerrain, 
                           TerrainType newTerrain, std::vector<HexCoordinate>& filled) const;
    
    // Formation helpers
    std::vector<HexCoordinate> generateLegionFormation(const HexCoordinate& center) const;
    std::vector<HexCoordinate> generatePhalanxFormation(const HexCoordinate& start, int width, int depth) const;
    std::vector<HexCoordinate> generateCavalryFormation(const HexCoordinate& center) const;
    
    // UI rendering
    void renderToolPanel();
    void renderTerrainPanel();
    void renderPropertiesPanel();
    void renderStatusBar();
    
    // Event handling
    void handleToolSelection(const SDL_Event& event);
    void handleTerrainSelection(const SDL_Event& event);
    void handleGridInteraction(const SDL_Event& event);
    void handleKeyboardShortcuts(const SDL_Event& event);
    
    // Tool implementations
    void executeSelectTool(const HexCoordinate& coord, bool addToSelection = false);
    void executePaintTool(const HexCoordinate& coord);
    void executeFillTool(const HexCoordinate& coord);
    void executeHeightTool(const HexCoordinate& coord);
    void executeUnitPlaceTool(const HexCoordinate& coord);
    void executeFormationTool(const HexCoordinate& coord);
    void executeMeasureTool(const HexCoordinate& coord);
    
    // Layout management
    void calculateLayout();
    void updateRendererView();
    
    // History management
    void addToHistory(const EditorAction& action);
    void clearHistoryAfterIndex();
    
    // Utility functions
    bool isCoordinateInBounds(const HexCoordinate& coord) const;
    void updateSelectionHighlights();
    void updateFormationHighlights();
    
    // Default terrain properties for quick access
    static const std::vector<TerrainType> COMMON_TERRAINS;
    static const std::vector<std::string> ROMAN_UNIT_TYPES;
};
