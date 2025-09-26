#pragma once
#include <cmath>
#include <vector>

/**
 * Hexagonal coordinate system using cubic coordinates (x, y, z) where x + y + z = 0
 * This system simplifies distance calculation and neighbor finding
 * Supports conversion between cubic and offset coordinates for display
 */
class HexCoordinate {
public:
    int x, y, z;
    
    HexCoordinate(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}
    
    // Create from offset coordinates (for compatibility with existing grid systems)
    static HexCoordinate fromOffset(int col, int row);
    
    // Convert to offset coordinates for display purposes
    void toOffset(int& col, int& row) const;
    
    // Distance between two hex coordinates (Manhattan distance in hex space)
    int distanceTo(const HexCoordinate& other) const;
    
    // Get all 6 neighboring coordinates
    std::vector<HexCoordinate> getNeighbors() const;
    
    // Get neighbor in specific direction (0-5, clockwise from right)
    HexCoordinate getNeighbor(int direction) const;
    
    // Check if coordinates are valid (x + y + z = 0)
    bool isValid() const { return (x + y + z) == 0; }
    
    // Operators
    HexCoordinate operator+(const HexCoordinate& other) const;
    HexCoordinate operator-(const HexCoordinate& other) const;
    bool operator==(const HexCoordinate& other) const;
    bool operator!=(const HexCoordinate& other) const;
    bool operator<(const HexCoordinate& other) const; // for use in std::map
    
    // Linear interpolation between two coordinates (useful for smooth movement)
    HexCoordinate lerp(const HexCoordinate& target, float t) const;
    
    // Get all coordinates within a given distance (used for movement/attack range)
    std::vector<HexCoordinate> getCoordinatesInRange(int range) const;
    
    // Convert to screen coordinates for rendering
    void toScreenCoords(float& screenX, float& screenY, float hexSize) const;
    
    // Convert from screen coordinates to hex coordinates
    static HexCoordinate fromScreenCoords(float screenX, float screenY, float hexSize);

private:
    static const std::vector<HexCoordinate> DIRECTIONS;
};

// Hash function for HexCoordinate to use in unordered_map
namespace std {
    template<>
    struct hash<HexCoordinate> {
        size_t operator()(const HexCoordinate& coord) const {
            return hash<int>()(coord.x) ^ (hash<int>()(coord.y) << 1) ^ (hash<int>()(coord.z) << 2);
        }
    };
}
