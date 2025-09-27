#include "Interface/ui/HexCoordinate.h"
#include <algorithm>
#include <cmath>

// Directions for hexagonal grid (clockwise from right)
const std::vector<HexCoordinate> HexCoordinate::DIRECTIONS = {
    {1, -1, 0},   // Right
    {1, 0, -1},   // Bottom-right
    {0, 1, -1},   // Bottom-left
    {-1, 1, 0},   // Left
    {-1, 0, 1},   // Top-left
    {0, -1, 1}    // Top-right
};

HexCoordinate HexCoordinate::fromOffset(int col, int row) {
    int x = col - (row - (row & 1)) / 2;
    int z = row;
    int y = -x - z;
    return HexCoordinate(x, y, z);
}

void HexCoordinate::toOffset(int& col, int& row) const {
    col = x + (z - (z & 1)) / 2;
    row = z;
}

int HexCoordinate::distanceTo(const HexCoordinate& other) const {
    return (std::abs(x - other.x) + std::abs(y - other.y) + std::abs(z - other.z)) / 2;
}

std::vector<HexCoordinate> HexCoordinate::getNeighbors() const {
    std::vector<HexCoordinate> neighbors;
    neighbors.reserve(6);
    for (const auto& dir : DIRECTIONS) {
        neighbors.push_back(*this + dir);
    }
    return neighbors;
}

HexCoordinate HexCoordinate::getNeighbor(int direction) const {
    if (direction < 0 || direction >= 6) return *this;
    return *this + DIRECTIONS[direction];
}

HexCoordinate HexCoordinate::operator+(const HexCoordinate& other) const {
    return HexCoordinate(x + other.x, y + other.y, z + other.z);
}

HexCoordinate HexCoordinate::operator-(const HexCoordinate& other) const {
    return HexCoordinate(x - other.x, y - other.y, z - other.z);
}

bool HexCoordinate::operator==(const HexCoordinate& other) const {
    return x == other.x && y == other.y && z == other.z;
}

bool HexCoordinate::operator!=(const HexCoordinate& other) const {
    return !(*this == other);
}

bool HexCoordinate::operator<(const HexCoordinate& other) const {
    if (x != other.x) return x < other.x;
    if (y != other.y) return y < other.y;
    return z < other.z;
}

HexCoordinate HexCoordinate::lerp(const HexCoordinate& target, float t) const {
    float newX = x + t * (target.x - x);
    float newY = y + t * (target.y - y);
    float newZ = z + t * (target.z - z);
    
    // Round to nearest valid hex coordinate
    int roundedX = static_cast<int>(std::round(newX));
    int roundedY = static_cast<int>(std::round(newY));
    int roundedZ = static_cast<int>(std::round(newZ));
    
    // Ensure x + y + z = 0 by adjusting the coordinate with the largest rounding error
    float xDiff = std::abs(roundedX - newX);
    float yDiff = std::abs(roundedY - newY);
    float zDiff = std::abs(roundedZ - newZ);
    
    if (xDiff > yDiff && xDiff > zDiff) {
        roundedX = -roundedY - roundedZ;
    } else if (yDiff > zDiff) {
        roundedY = -roundedX - roundedZ;
    } else {
        roundedZ = -roundedX - roundedY;
    }
    
    return HexCoordinate(roundedX, roundedY, roundedZ);
}

std::vector<HexCoordinate> HexCoordinate::getCoordinatesInRange(int range) const {
    std::vector<HexCoordinate> coordinates;
    
    for (int dx = -range; dx <= range; ++dx) {
        int minY = std::max(-range, -dx - range);
        int maxY = std::min(range, -dx + range);
        for (int dy = minY; dy <= maxY; ++dy) {
            int dz = -dx - dy;
            coordinates.emplace_back(x + dx, y + dy, z + dz);
        }
    }
    
    return coordinates;
}

void HexCoordinate::toScreenCoords(float& screenX, float& screenY, float hexSize) const {
    // Flat-top hexagon coordinate conversion
    screenX = hexSize * (std::sqrt(3.0f) * x + std::sqrt(3.0f) / 2.0f * z);
    screenY = hexSize * (3.0f / 2.0f * z);
}

HexCoordinate HexCoordinate::fromScreenCoords(float screenX, float screenY, float hexSize) {
    // Inverse of flat-top hexagon toScreenCoords formulas:
    // screenX = hexSize * (√3 * x + √3/2 * z)
    // screenY = hexSize * (3/2 * z)
    
    float z = (2.0f / 3.0f * screenY) / hexSize;
    float x = (screenX / hexSize - std::sqrt(3.0f) / 2.0f * z) / std::sqrt(3.0f);
    float y = -x - z;
    
    // Round to nearest hex coordinate
    HexCoordinate temp(static_cast<int>(std::round(x)), 
                      static_cast<int>(std::round(y)), 
                      static_cast<int>(std::round(z)));
    
    // Ensure coordinate is valid
    float xDiff = std::abs(temp.x - x);
    float yDiff = std::abs(temp.y - y);
    float zDiff = std::abs(temp.z - z);
    
    if (xDiff > yDiff && xDiff > zDiff) {
        temp.x = -temp.y - temp.z;
    } else if (yDiff > zDiff) {
        temp.y = -temp.x - temp.z;
    } else {
        temp.z = -temp.x - temp.y;
    }
    
    return temp;
}
