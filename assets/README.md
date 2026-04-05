# Map System Assets

## Tile ID Mapping

The CSV level files use the following tile IDs:

| ID | Tile Type    | Image File     | Solid | Harmful | Description                |
|----|--------------|----------------|-------|---------|----------------------------|
| 0  | EMPTY        | (none)         | No    | No      | Empty space / air          |
| 1  | PLATFORM     | platform.png   | Yes   | No      | Brown platform tile        |
| 2  | SPIKE        | spike.png      | No    | Yes     | Red spikes - damages player|
| 3  | WALL         | wall.png       | Yes   | No      | Gray stone wall            |
| 4  | COLLECTIBLE  | coin.png       | No    | No      | Golden coin collectible    |

## Tile Properties

- **Tile Size**: 64x64 pixels
- **Format**: PNG with transparency support for coins
- **Grid**: 20 columns × 12 rows (1280×768 pixels)

## Level Files

### level1.csv
A simple test level with:
- Ground platform layer (row 9)
- Solid wall foundation (rows 10-11)
- Floating platforms for jumping
- Spikes as hazards (row 8)
- Collectible coins (rows 2 and 4)

## Parallax Layers

Existing assets for parallax scrolling:
- `background.png` - Far background layer (parallax factor: 0.3-0.5)
- `middleground.png` - Mid background layer (parallax factor: 0.6-0.8)

## Directory Structure

```
assets/
├── background.png          # Parallax background
├── middleground.png        # Parallax middleground
├── levels/
│   └── level1.csv         # Test level layout
└── tiles/
    ├── platform.png       # Brown platform
    ├── spike.png          # Red spikes
    ├── wall.png           # Gray wall
    └── coin.png           # Gold coin
```

## Usage in Code

```cpp
// Register tile types
map.registerTileType(0, "", TileType::EMPTY, false, false);
map.registerTileType(1, "assets/tiles/platform.png", TileType::PLATFORM, true, false);
map.registerTileType(2, "assets/tiles/spike.png", TileType::SPIKE, false, true);
map.registerTileType(3, "assets/tiles/wall.png", TileType::WALL, true, false);
map.registerTileType(4, "assets/tiles/coin.png", TileType::COLLECTIBLE, false, false);

// Load level
map.loadFromCSV("assets/levels/level1.csv");

// Add parallax layers
map.addParallaxLayer("assets/background.png", 0.3f, "far_background");
map.addParallaxLayer("assets/middleground.png", 0.7f, "mid_background");
```
