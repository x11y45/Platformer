#ifndef CAMERACONTROLLER_HPP
#define CAMERACONTROLLER_HPP

#include <SFML/Graphics.hpp>

class CameraController {
public:
    CameraController(float viewWidth, float viewHeight);
    
    void setTarget(const sf::Vector2f* target);
    void setBounds(const sf::FloatRect& bounds);
    void update(float dt);
    
    sf::View& getView();
    const sf::View& getView() const;
    
    void setSmoothness(float smoothness);
    void setZoom(float zoom);

private:
    // this is the view, the visible part;
    sf::View view;
    // this is the target the view should follow, the player for example;
    const sf::Vector2f* target;
    sf::FloatRect bounds;
    
    float smoothness;
    float currentZoom;
    sf::Vector2f desiredPosition;
    
    void clampToBounds();
};

#endif // CAMERACONTROLLER_HPP
