#include "include/Pipe.h"


Pipe::Pipe()
{
    m_active = false;
    m_passed = false;
}

void Pipe::init(sf::Texture& tex, float x, int gap) {
    float centre = (rand() % 241 + 180);
    float topH   = centre - gap / 2.f;
    float botY   = centre + gap / 2.f;
    float botH   = (WINDOW_H) - GROUND_H - botY;

    float texW = (tex.getSize().x);
    float texH = (tex.getSize().y);

    float scaleX = PIPE_W / texW;

    // ── Top pipe (flipped upside-down) ──
    m_topSprite.setTexture(tex);
    m_topSprite.setScale(scaleX, -(topH / texH));
    m_topSprite.setOrigin(0.f, 0.f);
    m_topSprite.setPosition(x, topH);

    // ── Bottom pipe (normal orientation) ──
    m_bottomSprite.setTexture(tex);
    m_bottomSprite.setScale(scaleX, botH / texH);
    m_bottomSprite.setOrigin(0.f, 0.f);
    m_bottomSprite.setPosition(x, botY);

    m_topHit    = { x, 0.f,  PIPE_W, topH };
    m_bottomHit = { x, botY, PIPE_W, botH };

    m_active = true;
    m_passed = false;
}

void Pipe::update(float dt, float pipeSpeed) {
    if (!m_active) return;

    float dx = -pipeSpeed * dt;
    m_topSprite.move(dx, 0.f);
    m_bottomSprite.move(dx, 0.f);
    m_topHit.left    += dx;
    m_bottomHit.left += dx;

    if (m_topHit.left + PIPE_W < 0.f)
        m_active = false;
}

const sf::Sprite& Pipe::getTopSprite() const {
    return m_topSprite;
}

const sf::Sprite& Pipe::getBottomSprite() const {
    return m_bottomSprite;
}

bool Pipe::checkCollision(sf::FloatRect bird) const {
    // Shrink hitbox slightly for fairness
    bird.left   += 4.f;
    bird.top    += 4.f;
    bird.width  -= 8.f;
    bird.height -= 8.f;
    return bird.intersects(m_topHit) || bird.intersects(m_bottomHit);
}

bool Pipe::birdPassed(float birdX) {
    if (!m_passed && m_topHit.left + PIPE_W < birdX) {
        m_passed = true;
        return true;
    }
    return false;
}

bool Pipe::isActive() const {
    return m_active;
}
