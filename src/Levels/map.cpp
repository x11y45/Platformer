//
// Created by x11y45 on 4/3/26.
//

#include "map.h"
#include <fstream>
#include <sstream>
#include <iostream>

map::map() : tileSize(64), gridWidth(0), gridHeight(0), lastCameraPos(0.f, 0.f) {}

bool map::addParallaxLayer(const std::string& filepath, float parallaxFactor, const std::string& name) {
	ParallaxLayer layer;
	layer.parallaxFactor = parallaxFactor;
	layer.name = name;
	
	if (!layer.texture.loadFromFile(filepath)) {
		std::cerr << "Failed to load parallax layer: " << filepath << std::endl;
		return false;
	}
	
	layer.sprite.setTexture(layer.texture);
	parallaxLayers.push_back(layer);
	
	return true;
}

void map::updateParallax(const sf::Vector2f& cameraPosition) {
	sf::Vector2f cameraDelta = cameraPosition - lastCameraPos;
	
	for (auto& layer : parallaxLayers) {
		sf::Vector2f currentPos = layer.sprite.getPosition();
		layer.sprite.setPosition(
			currentPos.x - cameraDelta.x * layer.parallaxFactor,
			currentPos.y - cameraDelta.y * layer.parallaxFactor
		);
	}
	
	lastCameraPos = cameraPosition;
}

void map::renderParallax(sf::RenderTarget& target, bool renderBackground) {
	for (const auto& layer : parallaxLayers) {
		// Background layers have factor < 1.0
		// Foreground layers have factor >= 1.0
		bool isBackground = layer.parallaxFactor < 1.0f;
		
		if (isBackground == renderBackground) {
			target.draw(layer.sprite);
		}
	}
}

void map::registerTileType(int id, const std::string& imagePath, TileType type, bool isSolid, bool isHarmful) {
	TileDefinition def;
	def.id = id;
	def.imagePath = imagePath;
	def.type = type;
	def.isSolid = isSolid;
	def.isHarmful = isHarmful;
	
	tileRegistry[id] = def;
	
	// Load texture for this tile type
	if (id != 0) { // Don't load texture for empty tiles
		sf::Texture texture;
		if (texture.loadFromFile(imagePath)) {
			tileTextures[id] = std::move(texture);
		} else {
			std::cerr << "Failed to load tile texture: " << imagePath << std::endl;
		}
	}
}

std::vector<std::vector<int>> map::parseCSV(const std::string& filepath) {
	std::vector<std::vector<int>> result;
	std::ifstream file(filepath);
	
	if (!file.is_open()) {
		std::cerr << "Failed to open CSV file: " << filepath << std::endl;
		return result;
	}
	
	std::string line;
	while (std::getline(file, line)) {
		std::vector<int> row;
		std::stringstream ss(line);
		std::string cell;
		
		while (std::getline(ss, cell, ',')) {
			try {
				row.push_back(std::stoi(cell));
			} catch (const std::exception& e) {
				std::cerr << "Error parsing CSV cell: " << cell << std::endl;
				row.push_back(0); // Default to empty
			}
		}
		
		if (!row.empty()) {
			result.push_back(row);
		}
	}
	
	file.close();
	return result;
}

void map::buildTileGrid(const std::vector<std::vector<int>>& csvData) {
	if (csvData.empty()) {
		return;
	}
	
	gridHeight = csvData.size();
	gridWidth = csvData[0].size();
	
	// Initialize grid
	tileGrid.resize(gridHeight);
	for (int row = 0; row < gridHeight; ++row) {
		tileGrid[row].resize(gridWidth);
	}
	
	// Build tiles from CSV data
	for (int row = 0; row < gridHeight; ++row) {
		for (int col = 0; col < gridWidth && col < static_cast<int>(csvData[row].size()); ++col) {
			int tileId = csvData[row][col];
			
			Tile& tile = tileGrid[row][col];
			tile.gridX = col;
			tile.gridY = row;
			
			if (tileId == 0) {
				tile.type = TileType::EMPTY;
			} else if (tileRegistry.count(tileId) > 0) {
				const TileDefinition& def = tileRegistry[tileId];
				tile.type = def.type;
				
				// Set up sprite if texture exists
				if (tileTextures.count(tileId) > 0) {
					tile.sprite.setTexture(tileTextures[tileId]);
					tile.sprite.setPosition(
						static_cast<float>(col * tileSize),
						static_cast<float>(row * tileSize)
					);
				}
			} else {
				std::cerr << "Unknown tile ID: " << tileId << " at (" << col << ", " << row << ")" << std::endl;
				tile.type = TileType::EMPTY;
			}
		}
	}
}

bool map::loadFromCSV(const std::string& filepath) {
	std::vector<std::vector<int>> csvData = parseCSV(filepath);
	
	if (csvData.empty()) {
		std::cerr << "CSV data is empty or failed to parse" << std::endl;
		return false;
	}
	
	buildTileGrid(csvData);
	return true;
}

void map::renderTiles(sf::RenderTarget& target, const sf::View& cameraView) {
	// Get camera bounds for culling
	sf::Vector2f viewCenter = cameraView.getCenter();
	sf::Vector2f viewSize = cameraView.getSize();
	
	float left = viewCenter.x - viewSize.x / 2.f;
	float right = viewCenter.x + viewSize.x / 2.f;
	float top = viewCenter.y - viewSize.y / 2.f;
	float bottom = viewCenter.y + viewSize.y / 2.f;
	
	// Convert to grid coordinates with margin
	int startCol = std::max(0, static_cast<int>(left / tileSize) - 1);
	int endCol = std::min(gridWidth, static_cast<int>(right / tileSize) + 2);
	int startRow = std::max(0, static_cast<int>(top / tileSize) - 1);
	int endRow = std::min(gridHeight, static_cast<int>(bottom / tileSize) + 2);
	
	// Render visible tiles only
	for (int row = startRow; row < endRow; ++row) {
		for (int col = startCol; col < endCol; ++col) {
			const Tile& tile = tileGrid[row][col];
			
			if (tile.type != TileType::EMPTY) {
				target.draw(tile.sprite);
			}
		}
	}
}

Tile* map::getTileAt(float x, float y) {
	int col = static_cast<int>(x / tileSize);
	int row = static_cast<int>(y / tileSize);
	
	return getTileAtGrid(col, row);
}

Tile* map::getTileAtGrid(int col, int row) {
	if (row >= 0 && row < gridHeight && col >= 0 && col < gridWidth) {
		return &tileGrid[row][col];
	}
	return nullptr;
}

bool map::isSolidAt(float x, float y) {
	Tile* tile = getTileAt(x, y);
	if (tile && tile->type != TileType::EMPTY) {
		// Check tile registry for solid property
		for (const auto& pair : tileRegistry) {
			if (pair.second.type == tile->type) {
				return pair.second.isSolid;
			}
		}
	}
	return false;
}

bool map::isHarmfulAt(float x, float y) {
	Tile* tile = getTileAt(x, y);
	if (tile && tile->type != TileType::EMPTY) {
		// Check tile registry for harmful property
		for (const auto& pair : tileRegistry) {
			if (pair.second.type == tile->type) {
				return pair.second.isHarmful;
			}
		}
	}
	return false;
}

std::vector<Tile*> map::getCollidingTiles(const sf::FloatRect& bounds) {
	std::vector<Tile*> collidingTiles;
	
	// Convert bounds to grid coordinates
	int startCol = static_cast<int>(bounds.left / tileSize);
	int endCol = static_cast<int>((bounds.left + bounds.width) / tileSize) + 1;
	int startRow = static_cast<int>(bounds.top / tileSize);
	int endRow = static_cast<int>((bounds.top + bounds.height) / tileSize) + 1;
	
	// Clamp to grid bounds
	startCol = std::max(0, startCol);
	endCol = std::min(gridWidth, endCol);
	startRow = std::max(0, startRow);
	endRow = std::min(gridHeight, endRow);
	
	// Collect tiles in bounds
	for (int row = startRow; row < endRow; ++row) {
		for (int col = startCol; col < endCol; ++col) {
			Tile* tile = getTileAtGrid(col, row);
			if (tile && tile->type != TileType::EMPTY) {
				collidingTiles.push_back(tile);
			}
		}
	}
	
	return collidingTiles;
}

void map::render(sf::RenderTarget& target, const sf::View& cameraView) {
	// Render background parallax layers (factor < 1.0)
	renderParallax(target, true);
	
	// Render tile grid
	renderTiles(target, cameraView);
	
	// Render foreground parallax layers (factor >= 1.0)
	renderParallax(target, false);
}

int map::getParallaxLayerCount() const {
	return parallaxLayers.size();
}

void map::setLayerParallaxFactor(int index, float factor) {
	if (index >= 0 && index < static_cast<int>(parallaxLayers.size())) {
		parallaxLayers[index].parallaxFactor = factor;
	}
}

void map::clear() {
	parallaxLayers.clear();
	tileRegistry.clear();
	tileTextures.clear();
	tileGrid.clear();
	gridWidth = 0;
	gridHeight = 0;
	lastCameraPos = sf::Vector2f(0.f, 0.f);
}

int map::getTileSize() const {
	return tileSize;
}

sf::Vector2i map::getGridDimensions() const {
	return sf::Vector2i(gridWidth, gridHeight);
}
