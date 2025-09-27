#include "Interface/ui/HexGridRenderer.h"
#include "Interface/ui/HexGrid.h"
#include "Systems/SDLManager.h"
#include <iostream>
#include <iomanip>
#include <cmath>

/**
 * Hexagon alignment test to check for gaps between tiles
 */
int main() {
    std::cout << "=== Hexagon Alignment Test ===" << std::endl;
    
    SDLManager sdlManager;
    if (!sdlManager.initialize()) {
        std::cout << "Failed to initialize SDL" << std::endl;
        return 1;
    }
    
    auto grid = std::make_shared<HexGrid>(20, 20);
    HexGridRenderer renderer(0, 0, 1200, 800, sdlManager, grid);
    
    // Set configuration
    auto& config = renderer.getRenderConfig();
    config.hexSize = 50.0f;
    config.zoomLevel = 1.0f;
    config.panX = 600.0f;
    config.panY = 400.0f;
    
    std::cout << "Testing hexagon alignment with hexSize = " << config.hexSize << std::endl;
    std::cout << "Expected distance between adjacent hex centers:" << std::endl;
    
    // Calculate theoretical distances for flat-top hexagons
    float hexRadius = config.hexSize;
    float expectedHorizontalDistance = hexRadius * std::sqrt(3.0f);  // Distance between horizontal neighbors
    float expectedDiagonalDistance = hexRadius * std::sqrt(3.0f);    // Distance between diagonal neighbors
    
    std::cout << "  Horizontal neighbors: " << expectedHorizontalDistance << " pixels" << std::endl;
    std::cout << "  Diagonal neighbors: " << expectedDiagonalDistance << " pixels" << std::endl;
    
    // Test actual distances
    std::cout << "\n=== Testing Adjacent Hexagon Distances ===" << std::endl;
    
    struct TestPair {
        HexCoordinate coord1, coord2;
        const char* description;
        float expectedDistance;
    };
    
    TestPair testPairs[] = {
        {{0,0,0}, {1,-1,0}, "Right neighbor", expectedHorizontalDistance},
        {{0,0,0}, {-1,1,0}, "Left neighbor", expectedHorizontalDistance},
        {{0,0,0}, {0,-1,1}, "Top-right neighbor", expectedDiagonalDistance},
        {{0,0,0}, {0,1,-1}, "Bottom-left neighbor", expectedDiagonalDistance},
        {{0,0,0}, {1,0,-1}, "Bottom-right neighbor", expectedDiagonalDistance},
        {{0,0,0}, {-1,0,1}, "Top-left neighbor", expectedDiagonalDistance}
    };
    
    bool allAligned = true;
    
    for (const auto& pair : testPairs) {
        float x1, y1, x2, y2;
        renderer.hexToScreen(pair.coord1, x1, y1);
        renderer.hexToScreen(pair.coord2, x2, y2);
        
        float actualDistance = std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
        float error = std::abs(actualDistance - pair.expectedDistance);
        bool aligned = error < 0.1f;
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << pair.description << ":" << std::endl;
        std::cout << "  From (" << pair.coord1.x << "," << pair.coord1.y << "," << pair.coord1.z << ") ";
        std::cout << "at (" << x1 << "," << y1 << ")" << std::endl;
        std::cout << "  To   (" << pair.coord2.x << "," << pair.coord2.y << "," << pair.coord2.z << ") ";
        std::cout << "at (" << x2 << "," << y2 << ")" << std::endl;
        std::cout << "  Expected distance: " << pair.expectedDistance << std::endl;
        std::cout << "  Actual distance:   " << actualDistance << std::endl;
        std::cout << "  Error: " << error << " pixels ";
        std::cout << (aligned ? "✅" : "❌") << std::endl << std::endl;
        
        if (!aligned) {
            allAligned = false;
        }
    }
    
    std::cout << "=== Angle Analysis ===" << std::endl;
    
    // Check if angles between neighbors are correct (should be 60 degrees)
    float centerX, centerY;
    renderer.hexToScreen({0,0,0}, centerX, centerY);
    
    std::vector<HexCoordinate> neighbors = {
        {1,-1,0}, {1,0,-1}, {0,1,-1}, {-1,1,0}, {-1,0,1}, {0,-1,1}
    };
    
    // Expected angles for flat-top hexagon layout (clockwise from right)
    std::vector<float> expectedAngles = {0.0f, 300.0f, 240.0f, 180.0f, 120.0f, 60.0f};
    
    for (size_t i = 0; i < neighbors.size(); ++i) {
        float x, y;
        renderer.hexToScreen(neighbors[i], x, y);
        
        float angle = std::atan2(y - centerY, x - centerX) * 180.0f / M_PI;
        if (angle < 0) angle += 360.0f;
        
        float expectedAngle = expectedAngles[i];
        float angleError = std::abs(angle - expectedAngle);
        if (angleError > 180.0f) angleError = 360.0f - angleError;  // Handle wrap-around
        
        bool correctAngle = angleError < 1.0f;
        
        std::cout << "Neighbor " << i << " (" << neighbors[i].x << "," << neighbors[i].y << "," << neighbors[i].z << "): ";
        std::cout << "angle = " << std::fixed << std::setprecision(1) << angle << "°, ";
        std::cout << "expected = " << expectedAngle << "°, ";
        std::cout << "error = " << angleError << "° ";
        std::cout << (correctAngle ? "✅" : "❌") << std::endl;
        
        if (!correctAngle) {
            allAligned = false;
        }
    }
    
    std::cout << "\n=== Summary ===" << std::endl;
    if (allAligned) {
        std::cout << "✅ All hexagons are properly aligned with no gaps!" << std::endl;
    } else {
        std::cout << "❌ Hexagon alignment issues detected." << std::endl;
        std::cout << "This suggests the coordinate system or hexagon drawing needs adjustment." << std::endl;
    }
    
    sdlManager.cleanup();
    return allAligned ? 0 : 1;
}
