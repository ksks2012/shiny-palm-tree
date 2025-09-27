#include "Interface/ui/HexCoordinate.h"
#include "Interface/ui/HexGridRenderer.h"
#include "Interface/ui/HexGridEditor.h"
#include "Interface/ui/UIManager.h"
#include "Systems/SDLManager.h"
#include <iostream>
#include <vector>
#include <cmath>

/**
 * Comprehensive coordinate testing suite
 * Combines multiple coordinate tests into a single program
 */

class CoordinateTestSuite {
public:
    CoordinateTestSuite() {}
    
    void runAllTests() {
        std::cout << "=== UI Framework Coordinate Test Suite ===" << std::endl;
        std::cout << "Running comprehensive coordinate system tests..." << std::endl;
        
        // Test 1: Basic coordinate conversion
        testBasicCoordinateConversion();
        
        // Test 2: Center coordinate accuracy
        testCenterCoordinateAccuracy();
        
        // Test 3: Direction calculations
        testDirectionCalculations();
        
        // Test 4: Edge case handling
        testEdgeCases();
        
        // Test 5: Coordinate offset analysis (integrated from backup)
        testCoordinateOffsetAnalysis();
        
        // Summary
        printTestSummary();
    }

private:
    int testsRun = 0;
    int testsPassed = 0;
    
    void testBasicCoordinateConversion() {
        std::cout << "\n--- Test 1: Basic Coordinate Conversion ---" << std::endl;
        testsRun++;
        
        // Test basic coordinate math
        HexCoordinate coord(0, 0, 0);
        if (coord.x + coord.y + coord.z == 0) {
            std::cout << "✅ Origin coordinate constraint satisfied" << std::endl;
        } else {
            std::cout << "❌ Origin coordinate constraint failed" << std::endl;
            return;
        }
        
        // Test neighbor calculations
        auto neighbors = coord.getNeighbors();
        if (neighbors.size() == 6) {
            std::cout << "✅ Six neighbors generated correctly" << std::endl;
            
            bool allValid = true;
            for (const auto& neighbor : neighbors) {
                if (neighbor.x + neighbor.y + neighbor.z != 0) {
                    allValid = false;
                    break;
                }
            }
            
            if (allValid) {
                std::cout << "✅ All neighbor coordinates satisfy constraint" << std::endl;
                testsPassed++;
            } else {
                std::cout << "❌ Some neighbor coordinates invalid" << std::endl;
            }
        } else {
            std::cout << "❌ Wrong number of neighbors: " << neighbors.size() << std::endl;
        }
    }
    
    void testCenterCoordinateAccuracy() {
        std::cout << "\n--- Test 2: Center Coordinate Accuracy ---" << std::endl;
        testsRun++;
        
        // Test center coordinate with actual renderer
        SDLManager sdlManager;
        if (!sdlManager.initialize()) {
            std::cout << "❌ Failed to initialize SDL for accuracy test" << std::endl;
            return;
        }
        
        auto uiManager = std::make_unique<UIManager>();
        auto hexEditor = std::make_shared<HexGridEditor>(0, 0, 800, 600, sdlManager);
        
        // Test center coordinate (0,0,0)
        HexCoordinate center(0, 0, 0);
        float windowX, windowY;
        hexEditor->getRenderer()->hexToWindowCoords(center, windowX, windowY);
        
        // Convert back
        HexCoordinate reversed = hexEditor->getRenderer()->screenToHex(windowX, windowY);
        
        if (reversed == center) {
            std::cout << "✅ Center coordinate round-trip conversion accurate" << std::endl;
            testsPassed++;
        } else {
            std::cout << "❌ Center coordinate conversion failed: (" 
                      << reversed.x << "," << reversed.y << "," << reversed.z << ")" << std::endl;
        }
        
        sdlManager.cleanup();
    }
    
    void testDirectionCalculations() {
        std::cout << "\n--- Test 3: Direction Calculations ---" << std::endl;
        testsRun++;
        
        HexCoordinate center(0, 0, 0);
        
        // Test all 6 directions
        std::vector<HexCoordinate> expectedDirections = {
            HexCoordinate(1, -1, 0),  // East
            HexCoordinate(1, 0, -1),  // Northeast  
            HexCoordinate(0, 1, -1),  // Northwest
            HexCoordinate(-1, 1, 0),  // West
            HexCoordinate(-1, 0, 1),  // Southwest
            HexCoordinate(0, -1, 1)   // Southeast
        };
        
        auto neighbors = center.getNeighbors();
        bool directionsMatch = true;
        
        if (neighbors.size() != 6) {
            std::cout << "❌ Expected 6 directions, got " << neighbors.size() << std::endl;
            return;
        }
        
        // Check if all expected directions are present
        for (const auto& expected : expectedDirections) {
            bool found = false;
            for (const auto& neighbor : neighbors) {
                if (neighbor == expected) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << "❌ Missing expected direction: (" 
                          << expected.x << "," << expected.y << "," << expected.z << ")" << std::endl;
                directionsMatch = false;
            }
        }
        
        if (directionsMatch) {
            std::cout << "✅ All 6 hexagonal directions calculated correctly" << std::endl;
            testsPassed++;
        }
    }
    
    void testEdgeCases() {
        std::cout << "\n--- Test 4: Edge Cases ---" << std::endl;
        testsRun++;
        
        // Test large coordinates
        HexCoordinate large(100, -50, -50);
        if (large.x + large.y + large.z == 0) {
            std::cout << "✅ Large coordinate constraint satisfied" << std::endl;
        } else {
            std::cout << "❌ Large coordinate constraint failed" << std::endl;
            return;
        }
        
        // Test distance calculation
        HexCoordinate origin(0, 0, 0);
        HexCoordinate distant(3, -1, -2);
        int distance = origin.distanceTo(distant);
        
        if (distance == 3) {  // Expected distance
            std::cout << "✅ Distance calculation correct" << std::endl;
        } else {
            std::cout << "❌ Distance calculation wrong: expected 3, got " << distance << std::endl;
            return;
        }
        
        // Test equality and comparison
        HexCoordinate coord1(1, -1, 0);
        HexCoordinate coord2(1, -1, 0);
        HexCoordinate coord3(0, 1, -1);
        
        if (coord1 == coord2 && !(coord1 == coord3)) {
            std::cout << "✅ Coordinate equality working correctly" << std::endl;
            testsPassed++;
        } else {
            std::cout << "❌ Coordinate equality failed" << std::endl;
        }
    }
    
    void testCoordinateOffsetAnalysis() {
        std::cout << "\n--- Test 5: Coordinate Offset Analysis ---" << std::endl;
        testsRun++;
        
        // Initialize SDL for renderer testing
        SDLManager sdlManager;
        if (!sdlManager.initialize()) {
            std::cout << "❌ Failed to initialize SDL for offset analysis" << std::endl;
            return;
        }
        
        auto grid = std::make_shared<HexGrid>(20, 20);
        HexGridRenderer renderer(200, 0, 800, 770, sdlManager, grid);
        
        // Set test configuration
        auto& config = renderer.getRenderConfig();
        config.hexSize = 50.0f;
        config.zoomLevel = 1.0f;
        config.panX = 400.0f;
        config.panY = 385.0f;
        
        std::cout << "Renderer config: hexSize=" << config.hexSize 
                  << " pan=(" << config.panX << "," << config.panY << ")" 
                  << " zoom=" << config.zoomLevel << std::endl;
        
        HexCoordinate testCoord(0, 0, 0);
        
        std::cout << "\n=== Step-by-Step Coordinate Analysis ===" << std::endl;
        
        // Step 1: Basic hex to screen coordinate conversion
        float rawScreenX, rawScreenY;
        testCoord.toScreenCoords(rawScreenX, rawScreenY, config.hexSize);
        std::cout << "1. Raw coordinate (0,0,0) -> screen: (" << rawScreenX << ", " << rawScreenY << ")" << std::endl;
        
        // Step 2: Use hexToScreen (includes view transform)
        float hexToScreenX, hexToScreenY;
        renderer.hexToScreen(testCoord, hexToScreenX, hexToScreenY);
        std::cout << "2. hexToScreen result: (" << hexToScreenX << ", " << hexToScreenY << ")" << std::endl;
        
        // Step 3: Use hexToWindowCoords (includes renderer offset)
        float hexToWindowX, hexToWindowY;
        renderer.hexToWindowCoords(testCoord, hexToWindowX, hexToWindowY);
        std::cout << "3. hexToWindowCoords result: (" << hexToWindowX << ", " << hexToWindowY << ")" << std::endl;
        
        // Calculate expected window coordinates manually
        float expectedWindowX = hexToScreenX + renderer.getX();
        float expectedWindowY = hexToScreenY + renderer.getY();
        std::cout << "4. Expected window coords: (" << expectedWindowX << ", " << expectedWindowY << ")" << std::endl;
        
        // Check consistency
        bool windowConsistent = (std::abs(hexToWindowX - expectedWindowX) < 0.01f && 
                               std::abs(hexToWindowY - expectedWindowY) < 0.01f);
        
        std::cout << "\n=== Consistency Check ===" << std::endl;
        std::cout << "hexToWindowCoords consistent: " << (windowConsistent ? "✅" : "❌") << std::endl;
        
        if (!windowConsistent) {
            std::cout << "Window difference: (" 
                      << (hexToWindowX - expectedWindowX) << ", " 
                      << (hexToWindowY - expectedWindowY) << ")" << std::endl;
        }
        
        // Test coordinate validation using screenToHex reverse conversion
        // screenToHex expects renderer-relative coordinates, not window coordinates
        std::cout << "\n=== Reverse Conversion Test ===" << std::endl;
        HexCoordinate reverseCoord = renderer.screenToHex(hexToScreenX, hexToScreenY);
        bool reverseConsistent = (reverseCoord.x == testCoord.x && 
                                reverseCoord.y == testCoord.y && 
                                reverseCoord.z == testCoord.z);
        
        std::cout << "Reverse conversion (0,0,0): (" << reverseCoord.x << "," << reverseCoord.y << "," << reverseCoord.z << ")" << std::endl;
        std::cout << "Reverse conversion consistent: " << (reverseConsistent ? "✅" : "❌") << std::endl;
        
        if (windowConsistent && reverseConsistent) {
            testsPassed++;
            std::cout << "✅ Coordinate offset analysis passed" << std::endl;
        } else {
            std::cout << "❌ Coordinate offset analysis failed" << std::endl;
        }
        
        sdlManager.cleanup();
    }
    
    void printTestSummary() {
        std::cout << "\n=== Test Suite Summary ===" << std::endl;
        std::cout << "Tests run: " << testsRun << std::endl;
        std::cout << "Tests passed: " << testsPassed << std::endl;
        std::cout << "Success rate: " << (testsPassed * 100 / testsRun) << "%" << std::endl;
        
        if (testsPassed == testsRun) {
            std::cout << "🎉 All tests passed! Coordinate system is working correctly." << std::endl;
        } else {
            std::cout << "⚠️  Some tests failed. Please review coordinate implementation." << std::endl;
        }
    }
};

int main() {
    CoordinateTestSuite suite;
    suite.runAllTests();
    return 0;
}
