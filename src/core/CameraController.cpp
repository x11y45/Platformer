#include "CameraController.hpp"
#include <algorithm>
#include <cmath>

CameraController::CameraController(float viewWidth, float viewHeight)
    : target(nullptr)
    , bounds(0.f, 0.f, 0.f, 0.f)
    , smoothness(5.f)
    , currentZoom(1.f)
    , desiredPosition(0.f, 0.f)
{
    view.setSize(viewWidth, viewHeight);
    view.setCenter(viewWidth / 2.f, viewHeight / 2.f);
}

void CameraController::setTarget(const sf::Vector2f* target) {
    this->target = target;
    if (target) {
        desiredPosition = *target;
        view.setCenter(desiredPosition);
    }
}

void CameraController::setBounds(const sf::FloatRect& bounds) {
    this->bounds = bounds;
    clampToBounds();
}

void CameraController::update(float dt) {
    if (!target) return;

    desiredPosition = *target;

    sf::Vector2f currentCenter = view.getCenter();
    sf::Vector2f difference = desiredPosition - currentCenter;

    float lerpFactor = std::min(1.f, smoothness * dt);
    sf::Vector2f newCenter = currentCenter + difference * lerpFactor;

    view.setCenter(newCenter);
    clampToBounds();


    sf::Vector2f snappedCenter = view.getCenter();
    snappedCenter.x = std::round(snappedCenter.x);
    snappedCenter.y = std::round(snappedCenter.y);
    view.setCenter(snappedCenter);
}

sf::View& CameraController::getView() {
    return view;
}

const sf::View& CameraController::getView() const {
    return view;
}

void CameraController::setSmoothness(float smoothness) {
    this->smoothness = smoothness;
}

void CameraController::setZoom(float zoom) {
    currentZoom = zoom;
    sf::Vector2f size = view.getSize();
    view.setSize(size.x * zoom, size.y * zoom);
}

void CameraController::clampToBounds() {
    if (bounds.width <= 0.f || bounds.height <= 0.f) return;

    sf::Vector2f center = view.getCenter();
    sf::Vector2f halfSize = view.getSize() / 2.f;

    float minX = bounds.left + halfSize.x;
    float maxX = bounds.left + bounds.width - halfSize.x;
    float minY = bounds.top + halfSize.y;
    float maxY = bounds.top + bounds.height - halfSize.y;


    if (halfSize.x * 2.f >= bounds.width) {
        center.x = bounds.left + bounds.width / 2.f;
    } else {
        center.x = std::clamp(center.x, minX, maxX);
    }

    if (halfSize.y * 2.f >= bounds.height) {
        center.y = bounds.top + bounds.height / 2.f;
    } else {
        center.y = std::clamp(center.y, minY, maxY);
    }

    view.setCenter(center);
}
