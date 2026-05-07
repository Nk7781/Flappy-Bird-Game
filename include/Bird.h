#pragma once
#include <SFML/Graphics.hpp>

class Bird
{
private:
    sf::Sprite m_sprite;
    float m_velocity;
    bool m_alive;

    float GRAVITY = 0.5f;
    static constexpr float JUMP_FORCE = -8.0f;
    static constexpr float BIRD_X = 150.f;
    static constexpr float WINDOW_H = 600;
    static constexpr float GROUND_H = 50.f;

public:
    Bird();

    void init(sf::Texture &tex);
    void reset();
    void update(float dt);
    void jump();
    const sf::Sprite &getSprite() const;
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    float getRotation() const;
    bool isAlive() const;
    void kill();
    void setGravity(float g);
};
