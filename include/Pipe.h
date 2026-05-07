#pragma once
#include <SFML/Graphics.hpp>

class Pipe {
public:
    Pipe();

    void init(sf::Texture& tex, float x, int gap);
    void update(float dt, float pipeSpeed);
    const sf::Sprite& getTopSprite()    const;
    const sf::Sprite& getBottomSprite() const;

    bool checkCollision(sf::FloatRect birdBounds) const;
    bool birdPassed(float birdX);
    bool isActive() const;

    static constexpr float PIPE_W = 70.f;

private:
    sf::Sprite    m_topSprite;
    sf::Sprite    m_bottomSprite;
    sf::FloatRect m_topHit;
    sf::FloatRect m_bottomHit;
    bool          m_active;
    bool          m_passed;

    static constexpr int   WINDOW_H  = 600;
    static constexpr float GROUND_H  = 50.f;
};
