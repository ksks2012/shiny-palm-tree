#pragma once
#include "HexCoordinate.h"
#include "HexTile.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

/**
 * Pathfinding result containing path and cost information
 */
struct PathfindingResult {
    std::vector<HexCoordinate> path;
    int totalCost = 0;
    bool pathFound = false;
    
    PathfindingResult() = default;
    PathfindingResult(const std::vector<HexCoordinate>& p, int cost) 
        : path(p), totalCost(cost), pathFound(true) {}
};

/**
 * Movement range calculation result
 */
struct MovementRange {
    std::unordered_map<HexCoordinate, int> reachableTiles; // coordinate -> movement cost
    int maxMovementPoints;
    
    MovementRange(int maxPoints) : maxMovementPoints(maxPoints) {}
    
    bool canReach(const HexCoordinate& coord) const {
        auto it = reachableTiles.find(coord);
        return it != reachableTiles.end() && it->second <= maxMovementPoints;
    }
    
    int getCostToReach(const HexCoordinate& coord) const {
        auto it = reachableTiles.find(coord);
        return (it != reachableTiles.end()) ? it->second : -1;
    }
};

/**
 * Hexagonal grid manager supporting Roman warfare tactics
 * Manages the collection of hex tiles and provides pathfinding, 
 * movement calculation, and battle scenario support
 */
class HexGrid {
public:
    HexGrid(int width, int height);
    ~HexGrid() = default;
    
    // Grid management
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    void resize(int width, int height);
    void clear();
    
    // Tile access and modification
    HexTile* getTile(const HexCoordinate& coord);
    const HexTile* getTile(const HexCoordinate& coord) const;
    HexTile* getTile(int x, int y, int z) { return getTile(HexCoordinate(x, y, z)); }
    const HexTile* getTile(int x, int y, int z) const { return getTile(HexCoordinate(x, y, z)); }
    
    void setTile(const HexCoordinate& coord, const HexTile& tile);
    void setTerrain(const HexCoordinate& coord, TerrainType terrain);
    
    bool isValidCoordinate(const HexCoordinate& coord) const;
    std::vector<HexCoordinate> getAllCoordinates() const;
    
    // Pathfinding using A* algorithm
    PathfindingResult findPath(const HexCoordinate& start, const HexCoordinate& goal,
                              std::function<bool(const HexTile&)> isPassable = nullptr) const;
    
    // Movement range calculation (Dijkstra-based)
    MovementRange calculateMovementRange(const HexCoordinate& start, int movementPoints,
                                       std::function<bool(const HexTile&)> isPassable = nullptr) const;
    
    // Attack range calculation considering line of sight
    std::vector<HexCoordinate> calculateAttackRange(const HexCoordinate& attacker, int range,
                                                   bool requireLineOfSight = true) const;
    
    // Line of sight calculation
    bool hasLineOfSight(const HexCoordinate& from, const HexCoordinate& to) const;
    std::vector<HexCoordinate> getLineOfSightPath(const HexCoordinate& from, const HexCoordinate& to) const;
    
    // Neighbors and adjacency
    std::vector<HexCoordinate> getNeighbors(const HexCoordinate& coord) const;
    std::vector<HexTile*> getNeighborTiles(const HexCoordinate& coord);
    
    // Area operations
    std::vector<HexCoordinate> getCoordinatesInRadius(const HexCoordinate& center, int radius) const;
    std::vector<HexTile*> getTilesInRadius(const HexCoordinate& center, int radius);
    
    // Formation and group operations (Roman military tactics)
    bool canPlaceFormation(const std::vector<HexCoordinate>& formation) const;
    void highlightFormation(const std::vector<HexCoordinate>& formation, const SDL_Color& color);
    void clearHighlights();
    
    // Engineering operations (Roman military engineering)
    bool canBuildBridge(const HexCoordinate& coord) const;
    void buildBridge(const HexCoordinate& coord);
    bool canBuildFortification(const HexCoordinate& coord) const;
    void buildFortification(const HexCoordinate& coord);
    void destroyStructure(const HexCoordinate& coord);
    
    // Event system for dynamic scenarios
    void processEvents(const std::string& triggerCondition, 
                      const std::unordered_map<std::string, std::string>& context);
    
    // Serialization for map editor
    std::string toJSON() const;
    void fromJSON(const std::string& json);
    
    // Statistics and analysis
    struct GridStatistics {
        int totalTiles = 0;
        std::unordered_map<TerrainType, int> terrainCounts;
        int impassableTiles = 0;
        int buildableTiles = 0;
        int supplyPoints = 0;
    };
    GridStatistics getStatistics() const;
    
    // Batch operations for performance
    void setTerrainArea(const std::vector<HexCoordinate>& coords, TerrainType terrain);
    void setHeightArea(const std::vector<HexCoordinate>& coords, int height);
    
    // Historical battle setup helpers
    void setupCannaeBattlefield();      // Cannae battle terrain setup
    void setupAlesiaBattlefield();     // Alesia siege terrain setup
    void setupTeutobergBattlefield();  // Teutoburg Forest ambush setup

private:
    int width_, height_;
    std::unordered_map<HexCoordinate, std::unique_ptr<HexTile>> tiles_;
    
    // Pathfinding helpers
    struct PathNode {
        HexCoordinate coord;
        int gCost = 0;      // Cost from start
        int hCost = 0;      // Heuristic cost to goal
        int fCost() const { return gCost + hCost; }
        HexCoordinate parent{-9999, -9999, -9999}; // Invalid coordinate as null
        
        bool operator>(const PathNode& other) const { return fCost() > other.fCost(); }
    };
    
    int calculateHeuristic(const HexCoordinate& from, const HexCoordinate& to) const;
    bool isDefaultPassable(const HexTile& tile) const;
    
    // Grid initialization helpers
    void initializeGrid();
    HexCoordinate offsetToHex(int col, int row) const;
    void offsetToHex(int col, int row, int& x, int& y, int& z) const;
};
