//
// Created by x11y45 on 4/3/26.
//

#ifndef PLATFORMER_MAP_H
#define PLATFORMER_MAP_H
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>

enum class TileType {
	EMPTY = 0,
	PLATFORM = 1,
	SPIKE = 2,
	WALL = 3,
	COLLECTIBLE = 4
};

struct TileDefinition {
	int id{0};
	std::string imagePath;
	TileType type;
	bool isSolid{false};
	bool isHarmful{false};
	bool isSpawnMarker{false};
};

struct Tile {
	int id{0};
	TileType type{TileType::EMPTY};
	sf::Sprite sprite;
	int gridX{0};
	int gridY{0};
};

struct ParallaxLayer {
	sf::Texture texture;
	sf::Sprite spriteA;
	sf::Sprite spriteB;
	float parallaxFactor;
	std::string name;
};

class map {
public:
	map();
	
	// Parallax layer management
	bool addParallaxLayer(const std::string& filepath, float parallaxFactor, const std::string& name);
	void updateParallax(const sf::Vector2f& cameraPosition, const sf::Vector2f& cameraViewSize);
	void renderParallax(sf::RenderTarget& target, bool renderBackground = true);

	void init(
		const std::map<int, TileDefinition>& tileDefinitions,
		const std::map<std::string,std::pair<std::string, float>>& layerFiles
	);
	
	// Tile system
	void registerTileType(int id, const std::string& imagePath, TileType type, bool isSolid, bool isHarmful, bool isSpawnMarker = false);
	bool loadFromCSV(const std::string& filepath);
	void renderTiles(sf::RenderTarget& target, const sf::View& cameraView);
	
	// Tile queries
	Tile* getTileAt(float x, float y);
	Tile* getTileAtGrid(int col, int row);
	bool isSolidAt(float x, float y);
	bool isHarmfulAt(float x, float y);
	std::vector<Tile*> getCollidingTiles(const sf::FloatRect& bounds);
	sf::FloatRect getTileWorldBounds(const Tile& tile) const;
	
	// main functions
	void update(const sf::View& cameraView);
	void render(sf::RenderTarget& target, const sf::View& cameraView);
	
	// Utilities
	int getParallaxLayerCount() const;
	void setLayerParallaxFactor(int index, float factor);
	ParallaxLayer getParallaxLayer(int index) const;
	void clear();
	int getTileSize() const;
	sf::Vector2i getGridDimensions() const;
	
private:
	// Parallax layers
	std::vector<ParallaxLayer> parallaxLayers;
	sf::Vector2f lastCameraPos;
	
	// Tile grid
	std::map<int, TileDefinition> tileRegistry;
	std::map<int, sf::Texture> tileTextures;
	std::vector<std::vector<Tile>> tileGrid;
	int tileSize;
	int gridWidth;
	int gridHeight;
	
	// Helper methods
	void buildTileGrid(const std::vector<std::vector<int>>& csvData);
	std::vector<std::vector<int>> parseCSV(const std::string& filepath);
	bool isTileSolid(const Tile& tile) const;
	void configureTileSprite(Tile& tile, int tileId);
	void refreshParallaxTextureBindings();
};


#endif //PLATFORMER_MAP_H
