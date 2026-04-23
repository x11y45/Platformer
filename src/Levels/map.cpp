



#include "map.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

map::map() : lastCameraPos(0.f, 0.f), tileSize(16), gridWidth(0), gridHeight(0) {

}

void map::init(
	const std::map<int, TileDefinition>& tileDefinitions,
	const std::map<int,std::pair<std::string, float>>& layerFiles
) {
	for (const auto& tileEntry : tileDefinitions) {
		registerTileType(
			tileEntry.second.id,
			tileEntry.second.imagePath,
			tileEntry.second.type,
			tileEntry.second.isSolid,
			tileEntry.second.isHarmful,
			tileEntry.second.isSpawnMarker
		);
	}

	for (const auto& layerEntry : layerFiles) {
		const int& num = layerEntry.first;
		const std::string& filepath = layerEntry.second.first;
		const float parallaxFactor = layerEntry.second.second;
		if (addParallaxLayer(filepath, parallaxFactor, num)) {
			std::cout << "Loaded parallax layer: " << num << " with factor " << parallaxFactor << std::endl;
		} else {
			std::cerr << "Failed to load parallax layer: " << num << std::endl;
		}
	}
}

bool map::addParallaxLayer(const std::string& filepath, float parallaxFactor, const int& num) {

	ParallaxLayer layer;
	layer.parallaxFactor = parallaxFactor;
	layer.num = num;

	if (!layer.texture.loadFromFile(filepath)) {
		std::cerr << "Failed to load parallax layer: " << filepath << std::endl;
		return false;
	}

	parallaxLayers.push_back(layer);
	refreshParallaxTextureBindings();

	return true;
}

void map::refreshParallaxTextureBindings() {
	for (auto& layer : parallaxLayers) {
		layer.spriteA.setTexture(layer.texture);
		layer.spriteB.setTexture(layer.texture);
		layer.spriteA.setPosition(0.f, 0.f);
		layer.spriteB.setPosition(layer.spriteA.getGlobalBounds().width, 0.f);
	}
}

void map::updateParallax(const sf::Vector2f& cameraPosition, const sf::Vector2f& cameraViewSize) {
	for (auto& layer : parallaxLayers) {
		const sf::Vector2u textureSize = layer.texture.getSize();
		if (textureSize.x == 0 || textureSize.y == 0) {
			continue;
		}

		const auto textureWidth = static_cast<float>(textureSize.x);
		const auto textureHeight = static_cast<float>(textureSize.y);
		const float targetWidth = cameraViewSize.x * 1.1f;
		const float targetHeight = cameraViewSize.y * 1.1f;
		const float scale = std::max(targetWidth / textureWidth, targetHeight / textureHeight);
		layer.spriteA.setScale(scale, scale);
		layer.spriteB.setScale(scale, scale);

		const float spriteWidth = textureWidth * scale;
		const float spriteHeight = textureHeight * scale;
		if (spriteWidth <= 0.f) {
			continue;
		}

		const float viewLeft = cameraPosition.x - cameraViewSize.x * 0.5f;
		const float viewTop = cameraPosition.y - cameraViewSize.y * 0.5f;
		const float parallaxOffsetX = cameraPosition.x * layer.parallaxFactor;
		float wrappedOffsetX = std::fmod(parallaxOffsetX, spriteWidth);
		if (wrappedOffsetX < 0.f) {
			wrappedOffsetX += spriteWidth;
		}

		const float baseX = viewLeft - (spriteWidth - cameraViewSize.x) * 0.5f - wrappedOffsetX;
		const float baseY = viewTop - (spriteHeight - cameraViewSize.y) * 0.5f;
		layer.spriteA.setPosition(baseX, baseY);
		layer.spriteB.setPosition(baseX + spriteWidth, baseY);
	}
}

void map::renderParallax(sf::RenderTarget& target, bool renderBackground) {
	for (const auto& layer : parallaxLayers) {
		if (layer.spriteA.getTexture()) {
			target.draw(layer.spriteA);
		}
		if (layer.spriteB.getTexture()) {
			target.draw(layer.spriteB);
		}
	}
}

void map::registerTileType(int id, const std::string& imagePath, TileType type, bool isSolid, bool isHarmful, bool isSpawnMarker) {
	TileDefinition def;
	def.id = id;
	def.imagePath = imagePath;
	def.type = type;
	def.isSolid = isSolid;
	def.isHarmful = isHarmful;
	def.isSpawnMarker = isSpawnMarker;

	tileRegistry[id] = def;


	if (id != 0 && !imagePath.empty()) {
		sf::Texture texture;
		if (texture.loadFromFile(imagePath)) {
			texture.setSmooth(false);
			tileTextures[id] = texture;
		} else {
			std::cerr << "Failed to load tile texture: " << imagePath << std::endl;
		}
	}
}

void map::configureTileSprite(Tile& tile, const int tileId) {
	if (tileTextures.count(tileId) == 0) {
		return;
	}

	const sf::Texture& texture = tileTextures[tileId];
	tile.sprite.setTexture(texture);
	tile.sprite.setOrigin(0.f, 0.f);

	const sf::FloatRect worldBounds = getTileWorldBounds(tile);
	tile.sprite.setPosition(worldBounds.left, worldBounds.top);

	const sf::Vector2u textureSize = texture.getSize();
	if (textureSize.x == 0 || textureSize.y == 0) {
		return;
	}

	const int sourceWidth = static_cast<int>(textureSize.x);
	const int sourceHeight = static_cast<int>(textureSize.y);
	tile.sprite.setTextureRect({0, 0, sourceWidth, sourceHeight});
	tile.sprite.setScale(
		worldBounds.width / static_cast<float>(sourceWidth),
		worldBounds.height / static_cast<float>(sourceHeight)
	);
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
				row.push_back(0);
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

	const int csvHeight = static_cast<int>(csvData.size());
	const int csvWidth = static_cast<int>(csvData[0].size());
	gridHeight = csvHeight;
	gridWidth = csvWidth;


	tileGrid.assign(gridHeight, std::vector<Tile>(gridWidth));
	for (int row = 0; row < gridHeight; ++row) {
		for (int col = 0; col < gridWidth; ++col) {
			Tile& tile = tileGrid[row][col];
			tile.gridX = col;
			tile.gridY = row;
		}
	}


	for (int row = 0; row < csvHeight; ++row) {
		for (int col = 0; col < csvWidth; ++col) {
			int tileId = csvData[row][col];

			Tile& tile = tileGrid[row][col];
			tile.id = tileId;

			if (tileId == 0) {
				tile.type = TileType::EMPTY;
			} else if (tileRegistry.count(tileId) > 0) {
				const TileDefinition& def = tileRegistry[tileId];
				tile.type = def.type;
				configureTileSprite(tile, tileId);
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

	tileGrid.clear();
	buildTileGrid(csvData);
	return true;
}

void map::renderTiles(sf::RenderTarget& target, const sf::View& cameraView) {

	const sf::Vector2f viewCenter = cameraView.getCenter();
	const sf::Vector2f viewSize = cameraView.getSize();

	const float left = viewCenter.x - viewSize.x / 2.f;
	const float right = viewCenter.x + viewSize.x / 2.f;
	const float top = viewCenter.y - viewSize.y / 2.f;
	const float bottom = viewCenter.y + viewSize.y / 2.f;


	const int startCol = std::max(0, static_cast<int>(std::floor(left / static_cast<float>(tileSize))) - 1);
	const int endCol = std::min(gridWidth, static_cast<int>(std::ceil(right / static_cast<float>(tileSize))) + 1);
	const int startRow = std::max(0, static_cast<int>(std::floor(top / static_cast<float>(tileSize))) - 1);
	const int endRow = std::min(gridHeight, static_cast<int>(std::ceil(bottom / static_cast<float>(tileSize))) + 1);


	for (int row = startRow; row < endRow; ++row) {
		for (int col = startCol; col < endCol; ++col) {
			const Tile& tile = tileGrid[row][col];

			if (tile.type != TileType::EMPTY && tile.sprite.getTexture()) {
				target.draw(tile.sprite);
			}
		}
	}
}

Tile* map::getTileAt(float x, float y) {
	const int col = static_cast<int>(x / tileSize);
	const int row = static_cast<int>(y / tileSize);

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
	return tile && isTileSolid(*tile);
}
bool map::isHarmfulAt(float x, float y) {
	Tile* tile = getTileAt(x, y);
	if (tile && tile->type != TileType::EMPTY) {

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


	int startCol = static_cast<int>(bounds.left / tileSize);
	int endCol = static_cast<int>((bounds.left + bounds.width) / tileSize) + 1;
	int startRow = static_cast<int>(bounds.top / tileSize);
	int endRow = static_cast<int>((bounds.top + bounds.height) / tileSize) + 1;


	startCol = std::max(0, startCol);
	endCol = std::min(gridWidth, endCol);
	startRow = std::max(0, startRow);
	endRow = std::min(gridHeight, endRow);


	for (int row = startRow; row < endRow; ++row) {
		for (int col = startCol; col < endCol; ++col) {
			Tile* tile = getTileAtGrid(col, row);
			if (tile && isTileSolid(*tile)) {
				collidingTiles.push_back(tile);
			}
		}
	}

	return collidingTiles;
}

sf::FloatRect map::getTileWorldBounds(const Tile& tile) const {
	const auto worldX = static_cast<float>(tile.gridX * tileSize);
	const auto worldY = static_cast<float>(tile.gridY * tileSize);
	const auto size = static_cast<float>(tileSize);
	return {worldX, worldY, size, size};
}

bool map::isTileSolid(const Tile& tile) const {
	if (tile.type == TileType::EMPTY) {
		return false;
	}
	for (const auto& tileEntry : tileRegistry) {
		if (tileEntry.second.type == tile.type) {
			return tileEntry.second.isSolid;
		}
	}
	return false;
}

void map::update(const sf::View &cameraView) {
	updateParallax(cameraView.getCenter(), cameraView.getSize());
}

void map::render(sf::RenderTarget& target, const sf::View& cameraView) {
	updateParallax(cameraView.getCenter(), cameraView.getSize());
	renderParallax(target);
	renderTiles(target, cameraView);
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

ParallaxLayer map::getParallaxLayer(int index) const {
	if (index >= 0 && index < static_cast<int>(parallaxLayers.size())) {
		return parallaxLayers[index];
	}
	std::cout << "Warning: Requested parallax layer index " << index << " is out of bounds." << std::endl;
	return {};
}

sf::Vector2i map::getGridDimensions() const {
	return {gridWidth, gridHeight};
}
