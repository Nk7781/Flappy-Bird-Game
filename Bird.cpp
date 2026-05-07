#include "include/Bird.h"

Bird::Bird()
    : m_velocity(0.f)
    , m_alive(true)
{}

void Bird::init(sf::Texture& tex) {
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.2f, 0.2f);
    sf::FloatRect b = m_sprite.getLocalBounds();
    m_sprite.setOrigin(b.width / 2.f, b.height / 2.f);
    reset();
}

void Bird::reset() {
    m_sprite.setPosition(BIRD_X, (WINDOW_H) / 2.f);
    m_velocity = 0.f;
    m_alive    = true;
}

void Bird::update(float dt) {
    if (!m_alive) return;

    m_velocity += GRAVITY;
    m_sprite.move(0.f, m_velocity);

    float angle = m_velocity * 3.f;
    if (angle >  90.f) angle =  90.f;
    if (angle < -30.f) angle = -30.f;
    m_sprite.setRotation(angle);
}

void Bird::jump() {
    if (!m_alive) return;
    m_velocity = JUMP_FORCE;
}

const sf::Sprite& Bird::getSprite() const {
    return m_sprite;
}

sf::FloatRect Bird::getBounds() const {
    return m_sprite.getGlobalBounds();
}

sf::Vector2f Bird::getPosition() const {
    return m_sprite.getPosition();
}

float Bird::getRotation() const {
    return m_sprite.getRotation();
}

bool Bird::isAlive() const {
    return m_alive;
}

void Bird::kill() {
    m_alive = false;
}

void Bird::setGravity(float g){
    GRAVITY = g;
}
