#include "Interface/ui/HexTile.h"
#include <sstream>
#include <algorithm>

HexTile::HexTile(const HexCoordinate& coord, TerrainType terrain) 
    : coordinate_(coord), terrainType_(terrain) {
    initializeTerrainProperties();
}

void HexTile::setTerrainType(TerrainType terrain) {
    terrainType_ = terrain;
    initializeTerrainProperties();
}

void HexTile::initializeTerrainProperties() {
    properties_ = HexTileUtils::getDefaultProperties(terrainType_);
}

void HexTile::addEvent(const TileEvent& event) {
    // Remove existing event with same ID
    removeEvent(event.eventId);
    events_.push_back(event);
}

void HexTile::removeEvent(const std::string& eventId) {
    events_.erase(
        std::remove_if(events_.begin(), events_.end(),
                      [&eventId](const TileEvent& event) {
                          return event.eventId == eventId;
                      }),
        events_.end()
    );
}

std::vector<TileEvent> HexTile::checkTriggeredEvents(const std::string& condition, 
                                                    const std::unordered_map<std::string, std::string>& context) {
    std::vector<TileEvent> triggeredEvents;
    
    for (auto& event : events_) {
        if (!event.triggered && event.triggerCondition == condition) {
            // Check additional context parameters
            bool shouldTrigger = true;
            for (const auto& param : event.parameters) {
                auto contextIt = context.find(param.first);
                if (contextIt == context.end() || contextIt->second != param.second) {
                    shouldTrigger = false;
                    break;
                }
            }
            
            if (shouldTrigger) {
                event.triggered = true;
                triggeredEvents.push_back(event);
            }
        }
    }
    
    return triggeredEvents;
}

void HexTile::buildFortification() {
    if (canBuild()) {
        terrainType_ = TerrainType::FORTIFICATION;
        initializeTerrainProperties();
        properties_.buildable = false; // Can't build on top of fortification
    }
}

void HexTile::buildBridge() {
    if (terrainType_ == TerrainType::RIVER) {
        terrainType_ = TerrainType::BRIDGE;
        initializeTerrainProperties();
        properties_.destructible = true;
    }
}

void HexTile::destroy() {
    if (properties_.destructible) {
        if (terrainType_ == TerrainType::BRIDGE) {
            terrainType_ = TerrainType::RIVER;
        } else if (terrainType_ == TerrainType::FORTIFICATION || terrainType_ == TerrainType::CITY_WALL) {
            terrainType_ = TerrainType::PLAIN;
        }
        initializeTerrainProperties();
    }
}

std::string HexTile::toJSON() const {
    std::stringstream ss;
    ss << "{";
    ss << "\"x\":" << coordinate_.x << ",";
    ss << "\"y\":" << coordinate_.y << ",";
    ss << "\"z\":" << coordinate_.z << ",";
    ss << "\"terrain\":" << static_cast<int>(terrainType_) << ",";
    ss << "\"height\":" << height_ << ",";
    ss << "\"passable\":" << (properties_.passable ? "true" : "false") << ",";
    ss << "\"buildable\":" << (properties_.buildable ? "true" : "false") << ",";
    ss << "\"hidden\":" << (properties_.hidden ? "true" : "false") << ",";
    ss << "\"movementCost\":" << properties_.movementCost << ",";
    ss << "\"defensiveBonus\":" << properties_.defensiveBonus << ",";
    ss << "\"specialTag\":\"" << properties_.specialTag << "\"";
    ss << "}";
    return ss.str();
}

// Utility functions implementation
TileProperties HexTileUtils::getDefaultProperties(TerrainType terrain) {
    TileProperties props;
    
    switch (terrain) {
        case TerrainType::PLAIN:
            props.movementCost = 1;
            props.buildable = true;
            break;
            
        case TerrainType::FOREST:
            props.movementCost = 2;
            props.evasionBonus = 20;
            props.rangedAccuracyPenalty = 10;
            props.hidden = true;
            break;
            
        case TerrainType::MOUNTAIN:
            props.movementCost = 3;
            props.defensiveBonus = 30;
            break;
            
        case TerrainType::RIVER:
            props.passable = false;
            props.movementCost = 99; // Effectively impassable
            break;
            
        case TerrainType::SWAMP:
            props.movementCost = 3;
            props.evasionBonus = -20; // Harder to evade in swamp
            break;
            
        case TerrainType::CITY_WALL:
            props.passable = false;
            props.defensiveBonus = 50;
            props.destructible = true;
            break;
            
        case TerrainType::ROAD:
            props.movementCost = 1; // Actually 0.5, but we'll handle fractional elsewhere
            props.buildable = true;
            break;
            
        case TerrainType::BRIDGE:
            props.movementCost = 1;
            props.destructible = true;
            break;
            
        case TerrainType::CAMP:
            props.movementCost = 1;
            props.supplyPoint = true;
            props.buildable = false;
            break;
            
        case TerrainType::FORTIFICATION:
            props.movementCost = 1;
            props.defensiveBonus = 40;
            props.destructible = true;
            break;
    }
    
    return props;
}

SDL_Color HexTileUtils::getTerrainColor(TerrainType terrain) {
    switch (terrain) {
        case TerrainType::PLAIN:        return {144, 238, 144, 255}; // Light green
        case TerrainType::FOREST:       return {34, 139, 34, 255};   // Forest green
        case TerrainType::MOUNTAIN:     return {139, 137, 137, 255}; // Gray
        case TerrainType::RIVER:        return {30, 144, 255, 255};  // Blue
        case TerrainType::SWAMP:        return {107, 142, 35, 255};  // Olive drab
        case TerrainType::CITY_WALL:    return {105, 105, 105, 255}; // Dim gray
        case TerrainType::ROAD:         return {210, 180, 140, 255}; // Tan
        case TerrainType::BRIDGE:       return {160, 82, 45, 255};   // Saddle brown
        case TerrainType::CAMP:         return {255, 215, 0, 255};   // Gold
        case TerrainType::FORTIFICATION: return {128, 128, 128, 255}; // Gray
        default:                        return {255, 255, 255, 255}; // White
    }
}

std::string HexTileUtils::getTerrainName(TerrainType terrain) {
    switch (terrain) {
        case TerrainType::PLAIN:        return "Plain";
        case TerrainType::FOREST:       return "Forest";
        case TerrainType::MOUNTAIN:     return "Mountain";
        case TerrainType::RIVER:        return "River";
        case TerrainType::SWAMP:        return "Swamp";
        case TerrainType::CITY_WALL:    return "City Wall";
        case TerrainType::ROAD:         return "Road";
        case TerrainType::BRIDGE:       return "Bridge";
        case TerrainType::CAMP:         return "Military Camp";
        case TerrainType::FORTIFICATION: return "Fortification";
        default:                        return "Unknown";
    }
}

std::string HexTileUtils::getTerrainDescription(TerrainType terrain) {
    switch (terrain) {
        case TerrainType::PLAIN:
            return "Open terrain suitable for formation fighting. Movement cost: 1";
        case TerrainType::FOREST:
            return "Dense woods providing cover but hindering movement. +20% evasion, -10% ranged accuracy";
        case TerrainType::MOUNTAIN:
            return "High ground providing defensive advantage. +30% defense, movement cost: 3";
        case TerrainType::RIVER:
            return "Swift flowing water impassable to infantry";
        case TerrainType::SWAMP:
            return "Treacherous wetlands slowing movement. Movement cost: 3";
        case TerrainType::CITY_WALL:
            return "Fortified walls providing excellent defense. +50% defense";
        case TerrainType::ROAD:
            return "Well-maintained Roman road allowing swift movement";
        case TerrainType::BRIDGE:
            return "Engineered crossing over water. Can be destroyed";
        case TerrainType::CAMP:
            return "Military encampment providing supplies and rest";
        case TerrainType::FORTIFICATION:
            return "Field fortification built by engineers. +40% defense";
        default:
            return "Unknown terrain type";
    }
}

int HexTileUtils::calculateMovementCost(const HexTile& from, const HexTile& to) {
    int baseCost = to.getMovementCost();
    
    // Height difference penalty
    int heightDiff = std::abs(to.getHeight() - from.getHeight());
    if (heightDiff > 0) {
        baseCost += heightDiff; // +1 movement cost per height level
    }
    
    return baseCost;
}

int HexTileUtils::calculateDefenseModifier(const HexTile& tile, int attackerHeight) {
    int modifier = tile.getDefensiveBonus();
    
    // Height advantage
    int heightAdvantage = tile.getHeight() - attackerHeight;
    if (heightAdvantage > 0) {
        modifier += heightAdvantage * 10; // +10% defense per height level above attacker
    }
    
    return modifier;
}

bool HexTileUtils::canSeeThrough(const HexTile& from, const HexTile& to, 
                                const std::vector<HexTile>& path) {
    // Simple line of sight check - if any tile in path blocks LOS, return false
    for (const auto& tile : path) {
        if (tile.blocksLineOfSight()) {
            return false;
        }
    }
    return true;
}

bool HexTileUtils::canFormTestudo(const std::vector<HexCoordinate>& formation) {
    // Testudo formation requires at least 9 units in a compact formation
    if (formation.size() < 9) return false;
    
    // Check if all coordinates are within a 3x3 hex area
    // This is a simplified check - in reality would be more complex
    if (formation.empty()) return false;
    
    HexCoordinate center = formation[0];
    for (const auto& coord : formation) {
        if (center.distanceTo(coord) > 2) {
            return false; // Too spread out for testudo
        }
    }
    
    return true;
}

std::vector<HexCoordinate> HexTileUtils::getLegionFormation(const HexCoordinate& center) {
    // Roman legion formation - 3x3 hex pattern around center
    std::vector<HexCoordinate> formation;
    formation.push_back(center); // Center
    
    // Add all coordinates within distance 2 of center
    auto coords = center.getCoordinatesInRange(2);
    for (const auto& coord : coords) {
        if (coord != center && center.distanceTo(coord) <= 2) {
            formation.push_back(coord);
        }
    }
    
    return formation;
}

bool HexTileUtils::canBuildSiegeWorks(const HexTile& tile) {
    return tile.canBuild() && 
           tile.getTerrainType() == TerrainType::PLAIN &&
           !tile.isOccupied();
}
