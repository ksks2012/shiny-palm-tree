# Debug Tools

Interactive debugging and analysis tools for the UI Framework.

## Files

- `visual_coordinate_debugger.cpp` - Interactive coordinate debugging tool

## Visual Coordinate Debugger

A comprehensive tool for debugging hexagonal coordinate system issues. Features:

### Controls
- **Mouse**: Click to see detailed coordinate analysis
- **SPACE**: Toggle coordinate display overlay
- **G**: Toggle grid lines
- **P**: Toggle panels
- **1-9**: Change hex size (1=20, 9=80)
- **R**: Reset view
- **ESC**: Quit

### Features
- Real-time coordinate conversion analysis
- Step-by-step calculation breakdown
- Distance measurement from hex centers
- Inside/outside hexagon detection
- Visual feedback for debugging

## Building

```bash
cd build
make VisualCoordinateDebugger
```

## Usage

This is the primary tool for debugging coordinate-related issues. Use it when:
- Investigating coordinate conversion problems
- Verifying fix implementations
- Understanding coordinate system behavior
- Testing new coordinate features

The tool provides detailed console output for every mouse click, showing the complete calculation process from screen coordinates to hex coordinates and back.
