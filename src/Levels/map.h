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
	int id;
	std::string imagePath;
	TileType type;
	bool isSolid;
	bool isHarmful;
};

struct Tile {
	TileType type;
	sf::Sprite sprite;
	int gridX;
	int gridY;
};

struct ParallaxLayer {
	sf::Texture texture;
	sf::Sprite sprite;
	float parallaxFactor;
	std::string name;
};

class map {
public:
	map();
	
	// Parallax layer management
	bool addParallaxLayer(const std::string& filepath, float parallaxFactor, const std::string& name);
	void updateParallax(const sf::Vector2f& cameraPosition);
	void renderParallax(sf::RenderTarget& target, bool renderBackground = true);
	
	// Tile system
	void registerTileType(int id, const std::string& imagePath, TileType type, bool isSolid, bool isHarmful);
	bool loadFromCSV(const std::string& filepath);
	void renderTiles(sf::RenderTarget& target, const sf::View& cameraView);
	
	// Tile queries
	Tile* getTileAt(float x, float y);
	Tile* getTileAtGrid(int col, int row);
	bool isSolidAt(float x, float y);
	bool isHarmfulAt(float x, float y);
	std::vector<Tile*> getCollidingTiles(const sf::FloatRect& bounds);
	
	// Complete render
	void render(sf::RenderTarget& target, const sf::View& cameraView);
	
	// Utilities
	int getParallaxLayerCount() const;
	void setLayerParallaxFactor(int index, float factor);
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
};


#endif //PLATFORMER_MAP_H
