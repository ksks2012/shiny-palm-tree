#include "Interface/ui/HexGrid.h"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <sstream>

HexGrid::HexGrid(int width, int height) : width_(width), height_(height) {
    initializeGrid();
}

void HexGrid::resize(int width, int height) {
    width_ = width;
    height_ = height;
    tiles_.clear();
    initializeGrid();
}

void HexGrid::clear() {
    for (auto& [coord, tile] : tiles_) {
        tile->setTerrainType(TerrainType::PLAIN);
        tile->setHeight(0);
        tile->setOccupied(false);
        tile->setHighlighted(false);
    }
}

void HexGrid::initializeGrid() {
    tiles_.clear();
    
    // Initialize grid using offset coordinates and convert to hex coordinates
    for (int row = 0; row < height_; ++row) {
        for (int col = 0; col < width_; ++col) {
            HexCoordinate coord = HexCoordinate::fromOffset(col, row);
            tiles_[coord] = std::make_unique<HexTile>(coord, TerrainType::PLAIN);
        }
    }
}

HexCoordinate HexGrid::offsetToHex(int col, int row) const {
    return HexCoordinate::fromOffset(col, row);
}

void HexGrid::offsetToHex(int col, int row, int& x, int& y, int& z) const {
    HexCoordinate coord = HexCoordinate::fromOffset(col, row);
    x = coord.x;
    y = coord.y;
    z = coord.z;
}

HexTile* HexGrid::getTile(const HexCoordinate& coord) {
    auto it = tiles_.find(coord);
    return (it != tiles_.end()) ? it->second.get() : nullptr;
}

const HexTile* HexGrid::getTile(const HexCoordinate& coord) const {
    auto it = tiles_.find(coord);
    return (it != tiles_.end()) ? it->second.get() : nullptr;
}

void HexGrid::setTile(const HexCoordinate& coord, const HexTile& tile) {
    if (isValidCoordinate(coord)) {
        tiles_[coord] = std::make_unique<HexTile>(tile);
    }
}

void HexGrid::setTerrain(const HexCoordinate& coord, TerrainType terrain) {
    auto* tile = getTile(coord);
    if (tile) {
        tile->setTerrainType(terrain);
    }
}

bool HexGrid::isValidCoordinate(const HexCoordinate& coord) const {
    return tiles_.find(coord) != tiles_.end();
}

std::vector<HexCoordinate> HexGrid::getAllCoordinates() const {
    std::vector<HexCoordinate> coords;
    coords.reserve(tiles_.size());
    
    for (const auto& [coord, tile] : tiles_) {
        coords.push_back(coord);
    }
    
    return coords;
}

PathfindingResult HexGrid::findPath(const HexCoordinate& start, const HexCoordinate& goal,
                                   std::function<bool(const HexTile&)> isPassable) const {
    if (!isValidCoordinate(start) || !isValidCoordinate(goal)) {
        return PathfindingResult();
    }
    
    if (!isPassable) {
        isPassable = [this](const HexTile& tile) { return isDefaultPassable(tile); };
    }
    
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openSet;
    std::unordered_set<HexCoordinate> closedSet;
    std::unordered_map<HexCoordinate, PathNode> nodeMap;
    
    PathNode startNode;
    startNode.coord = start;
    startNode.gCost = 0;
    startNode.hCost = calculateHeuristic(start, goal);
    
    openSet.push(startNode);
    nodeMap[start] = startNode;
    
    while (!openSet.empty()) {
        PathNode current = openSet.top();
        openSet.pop();
        
        if (current.coord == goal) {
            // Reconstruct path
            std::vector<HexCoordinate> path;
            HexCoordinate pathCoord = goal;
            int totalCost = current.gCost;
            
            while (pathCoord != start) {
                path.push_back(pathCoord);
                pathCoord = nodeMap[pathCoord].parent;
            }
            path.push_back(start);
            
            std::reverse(path.begin(), path.end());
            return PathfindingResult(path, totalCost);
        }
        
        closedSet.insert(current.coord);
        
        // Check all neighbors
        for (const HexCoordinate& neighbor : current.coord.getNeighbors()) {
            if (!isValidCoordinate(neighbor) || closedSet.count(neighbor)) {
                continue;
            }
            
            const HexTile* neighborTile = getTile(neighbor);
            if (!neighborTile || !isPassable(*neighborTile)) {
                continue;
            }
            
            const HexTile* currentTile = getTile(current.coord);
            int tentativeGCost = current.gCost + 
                HexTileUtils::calculateMovementCost(*currentTile, *neighborTile);
            
            auto neighborIt = nodeMap.find(neighbor);
            if (neighborIt == nodeMap.end() || tentativeGCost < neighborIt->second.gCost) {
                PathNode neighborNode;
                neighborNode.coord = neighbor;
                neighborNode.gCost = tentativeGCost;
                neighborNode.hCost = calculateHeuristic(neighbor, goal);
                neighborNode.parent = current.coord;
                
                nodeMap[neighbor] = neighborNode;
                openSet.push(neighborNode);
            }
        }
    }
    
    return PathfindingResult(); // No path found
}

MovementRange HexGrid::calculateMovementRange(const HexCoordinate& start, int movementPoints,
                                            std::function<bool(const HexTile&)> isPassable) const {
    MovementRange range(movementPoints);
    
    if (!isValidCoordinate(start)) {
        return range;
    }
    
    if (!isPassable) {
        isPassable = [this](const HexTile& tile) { return isDefaultPassable(tile); };
    }
    
    std::priority_queue<std::pair<int, HexCoordinate>, 
                       std::vector<std::pair<int, HexCoordinate>>,
                       std::greater<std::pair<int, HexCoordinate>>> queue;
    
    range.reachableTiles[start] = 0;
    queue.push({0, start});
    
    while (!queue.empty()) {
        auto [currentCost, currentCoord] = queue.top();
        queue.pop();
        
        if (currentCost > movementPoints) {
            continue;
        }
        
        if (currentCost > range.reachableTiles[currentCoord]) {
            continue; // Already found a better path to this tile
        }
        
        const HexTile* currentTile = getTile(currentCoord);
        if (!currentTile) continue;
        
        for (const HexCoordinate& neighbor : currentCoord.getNeighbors()) {
            if (!isValidCoordinate(neighbor)) {
                continue;
            }
            
            const HexTile* neighborTile = getTile(neighbor);
            if (!neighborTile || !isPassable(*neighborTile)) {
                continue;
            }
            
            int newCost = currentCost + HexTileUtils::calculateMovementCost(*currentTile, *neighborTile);
            
            if (newCost <= movementPoints) {
                auto it = range.reachableTiles.find(neighbor);
                if (it == range.reachableTiles.end() || newCost < it->second) {
                    range.reachableTiles[neighbor] = newCost;
                    queue.push({newCost, neighbor});
                }
            }
        }
    }
    
    return range;
}

std::vector<HexCoordinate> HexGrid::calculateAttackRange(const HexCoordinate& attacker, int range,
                                                        bool requireLineOfSight) const {
    std::vector<HexCoordinate> attackRange;
    
    if (!isValidCoordinate(attacker)) {
        return attackRange;
    }
    
    auto coordinatesInRange = attacker.getCoordinatesInRange(range);
    
    for (const HexCoordinate& coord : coordinatesInRange) {
        if (!isValidCoordinate(coord) || coord == attacker) {
            continue;
        }
        
        if (!requireLineOfSight || hasLineOfSight(attacker, coord)) {
            attackRange.push_back(coord);
        }
    }
    
    return attackRange;
}

bool HexGrid::hasLineOfSight(const HexCoordinate& from, const HexCoordinate& to) const {
    auto path = getLineOfSightPath(from, to);
    
    // Check if any tile in the path (except start and end) blocks line of sight
    for (size_t i = 1; i < path.size() - 1; ++i) {
        const HexTile* tile = getTile(path[i]);
        if (tile && tile->blocksLineOfSight()) {
            return false;
        }
    }
    
    return true;
}

std::vector<HexCoordinate> HexGrid::getLineOfSightPath(const HexCoordinate& from, const HexCoordinate& to) const {
    std::vector<HexCoordinate> path;
    
    int distance = from.distanceTo(to);
    if (distance == 0) {
        path.push_back(from);
        return path;
    }
    
    // Use linear interpolation to get path
    for (int i = 0; i <= distance; ++i) {
        float t = (distance == 0) ? 0.0f : static_cast<float>(i) / static_cast<float>(distance);
        HexCoordinate coord = from.lerp(to, t);
        path.push_back(coord);
    }
    
    return path;
}

std::vector<HexCoordinate> HexGrid::getNeighbors(const HexCoordinate& coord) const {
    std::vector<HexCoordinate> neighbors;
    
    for (const HexCoordinate& neighbor : coord.getNeighbors()) {
        if (isValidCoordinate(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }
    
    return neighbors;
}

std::vector<HexTile*> HexGrid::getNeighborTiles(const HexCoordinate& coord) {
    std::vector<HexTile*> neighborTiles;
    
    for (const HexCoordinate& neighbor : getNeighbors(coord)) {
        if (auto* tile = getTile(neighbor)) {
            neighborTiles.push_back(tile);
        }
    }
    
    return neighborTiles;
}

std::vector<HexCoordinate> HexGrid::getCoordinatesInRadius(const HexCoordinate& center, int radius) const {
    std::vector<HexCoordinate> coordinates;
    
    auto allCoords = center.getCoordinatesInRange(radius);
    for (const HexCoordinate& coord : allCoords) {
        if (isValidCoordinate(coord)) {
            coordinates.push_back(coord);
        }
    }
    
    return coordinates;
}

std::vector<HexTile*> HexGrid::getTilesInRadius(const HexCoordinate& center, int radius) {
    std::vector<HexTile*> tiles;
    
    for (const HexCoordinate& coord : getCoordinatesInRadius(center, radius)) {
        if (auto* tile = getTile(coord)) {
            tiles.push_back(tile);
        }
    }
    
    return tiles;
}

bool HexGrid::canPlaceFormation(const std::vector<HexCoordinate>& formation) const {
    for (const HexCoordinate& coord : formation) {
        const HexTile* tile = getTile(coord);
        if (!tile || !tile->isPassable() || tile->isOccupied()) {
            return false;
        }
    }
    return true;
}

void HexGrid::highlightFormation(const std::vector<HexCoordinate>& formation, const SDL_Color& color) {
    for (const HexCoordinate& coord : formation) {
        if (auto* tile = getTile(coord)) {
            tile->setHighlighted(true);
            tile->setHighlightColor(color);
        }
    }
}

void HexGrid::clearHighlights() {
    for (auto& [coord, tile] : tiles_) {
        tile->setHighlighted(false);
    }
}

bool HexGrid::canBuildBridge(const HexCoordinate& coord) const {
    const HexTile* tile = getTile(coord);
    return tile && tile->getTerrainType() == TerrainType::RIVER;
}

void HexGrid::buildBridge(const HexCoordinate& coord) {
    auto* tile = getTile(coord);
    if (tile && canBuildBridge(coord)) {
        tile->buildBridge();
    }
}

bool HexGrid::canBuildFortification(const HexCoordinate& coord) const {
    const HexTile* tile = getTile(coord);
    return tile && HexTileUtils::canBuildSiegeWorks(*tile);
}

void HexGrid::buildFortification(const HexCoordinate& coord) {
    auto* tile = getTile(coord);
    if (tile && canBuildFortification(coord)) {
        tile->buildFortification();
    }
}

void HexGrid::destroyStructure(const HexCoordinate& coord) {
    auto* tile = getTile(coord);
    if (tile) {
        tile->destroy();
    }
}

void HexGrid::processEvents(const std::string& triggerCondition, 
                           const std::unordered_map<std::string, std::string>& context) {
    for (auto& [coord, tile] : tiles_) {
        auto events = tile->checkTriggeredEvents(triggerCondition, context);
        // Process triggered events (implementation depends on game engine)
        for (const auto& event : events) {
            // Handle event actions based on event.action
            // This would typically interface with the main game system
        }
    }
}

int HexGrid::calculateHeuristic(const HexCoordinate& from, const HexCoordinate& to) const {
    return from.distanceTo(to);
}

bool HexGrid::isDefaultPassable(const HexTile& tile) const {
    return tile.isPassable() && !tile.isOccupied();
}

HexGrid::GridStatistics HexGrid::getStatistics() const {
    GridStatistics stats;
    stats.totalTiles = tiles_.size();
    
    for (const auto& [coord, tile] : tiles_) {
        stats.terrainCounts[tile->getTerrainType()]++;
        
        if (!tile->isPassable()) {
            stats.impassableTiles++;
        }
        
        if (tile->canBuild()) {
            stats.buildableTiles++;
        }
        
        if (tile->isSupplyPoint()) {
            stats.supplyPoints++;
        }
    }
    
    return stats;
}

void HexGrid::setTerrainArea(const std::vector<HexCoordinate>& coords, TerrainType terrain) {
    for (const HexCoordinate& coord : coords) {
        setTerrain(coord, terrain);
    }
}

void HexGrid::setHeightArea(const std::vector<HexCoordinate>& coords, int height) {
    for (const HexCoordinate& coord : coords) {
        auto* tile = getTile(coord);
        if (tile) {
            tile->setHeight(height);
        }
    }
}

// Historical battle setups
void HexGrid::setupCannaeBattlefield() {
    // Cannae (216 BC) - mostly flat plains with Aufidus river
    clear();
    
    // Create river along one edge
    for (int row = 0; row < height_; ++row) {
        HexCoordinate coord = offsetToHex(0, row);
        setTerrain(coord, TerrainType::RIVER);
    }
    
    // Set up gentle hills in center for Roman camp
    HexCoordinate center = offsetToHex(width_/2, height_/2);
    auto centerArea = getCoordinatesInRadius(center, 2);
    setHeightArea(centerArea, 1);
}

void HexGrid::setupAlesiaBattlefield() {
    // Alesia (52 BC) - hilltop fortress with siege works
    clear();
    
    // Create central hill for Alesia
    HexCoordinate center = offsetToHex(width_/2, height_/2);
    auto hillArea = getCoordinatesInRadius(center, 3);
    setHeightArea(hillArea, 2);
    setTerrainArea(hillArea, TerrainType::CITY_WALL);
    
    // Surrounding plains for Roman siege works
    auto siegeArea = getCoordinatesInRadius(center, 6);
    for (const HexCoordinate& coord : siegeArea) {
        if (center.distanceTo(coord) > 3) {
            setTerrain(coord, TerrainType::PLAIN);
        }
    }
}

void HexGrid::setupTeutobergBattlefield() {
    // Teutoburg Forest (9 AD) - dense forest with Roman road
    clear();
    
    // Fill most of map with forest
    for (auto& [coord, tile] : tiles_) {
        tile->setTerrainType(TerrainType::FOREST);
    }
    
    // Create Roman road through center
    for (int col = 0; col < width_; ++col) {
        HexCoordinate coord = offsetToHex(col, height_/2);
        setTerrain(coord, TerrainType::ROAD);
    }
    
    // Clear some areas for ambush positions
    for (int i = 0; i < 3; ++i) {
        HexCoordinate ambushPoint = offsetToHex(width_/4 + i*width_/4, height_/2 + (i%2 ? 2 : -2));
        auto ambushArea = getCoordinatesInRadius(ambushPoint, 2);
        setTerrainArea(ambushArea, TerrainType::PLAIN);
    }
}

std::string HexGrid::toJSON() const {
    std::stringstream ss;
    ss << "{";
    ss << "\"width\":" << width_ << ",";
    ss << "\"height\":" << height_ << ",";
    ss << "\"tiles\":[";
    
    bool first = true;
    for (const auto& [coord, tile] : tiles_) {
        if (!first) ss << ",";
        ss << tile->toJSON();
        first = false;
    }
    
    ss << "]}";
    return ss.str();
}

void HexGrid::fromJSON(const std::string& json) {
    // Simple JSON parsing - in a real implementation, you would use nlohmann/json
    // For now, we'll implement a basic parser
    
    // Clear existing grid
    clear();
    
    // For this basic implementation, let's just create a default grid
    // In a full implementation, you would parse the JSON properly
    initializeGrid();
}
