#include <SFML/Graphics.hpp>
#include <ctime>
#include <sstream>
#include <iostream>

#include "include/Bird.h"
#include "include/Pipe.h"

using namespace sf;

// ─────────────────────── CONSTANTS ───────────────────────
static const int WIDTH = 800;
static const int HEIGHT = 600;
static const int MAX_PIPES = 10;
static const float GROUND_HEIGHT = 50.f;

// ─────────────────────── DIFFICULTY SETTINGS ───────────────────────
struct DifficultySettings
{
    std::string name;
    float pipeSpeed;
    float spawnInterval;
    int gap;
    Color labelColor;
};

static const DifficultySettings DIFFICULTIES[3] = {
    { "EASY",   160.f, 2.2f, 190, Color(100, 220, 100) },
    { "MEDIUM", 220.f, 1.8f, 160, Color(255, 200,  50) },
    { "HARD",   300.f, 1.3f, 125, Color(255,  80,  80) }
};

// ─────────────────────── STATE ───────────────────────
enum class State { Menu, Playing, Dead };

// ─────────────────────── HELPERS ───────────────────────
static std::string toString(int v)
{
    std::ostringstream ss; ss << v; return ss.str();
}
static bool hitGround(Vector2f pos) { return pos.y + 20.f >= HEIGHT - GROUND_HEIGHT; }
static bool hitCeiling(Vector2f pos) { return pos.y - 20.f <= 0.f; }

// ─────────────────────── BACKGROUND ───────────────────────
class Background
{
private:
    static constexpr int NUM_CLOUDS = 6;
    RectangleShape m_sky, m_ground;
    RectangleShape m_clouds[NUM_CLOUDS];
    float m_cloudOffsets[NUM_CLOUDS];

public:
    void init()
    {
        m_sky.setSize({(float)WIDTH, HEIGHT - GROUND_HEIGHT});
        m_sky.setFillColor(Color(135, 206, 235));
        m_ground.setSize({(float)WIDTH, GROUND_HEIGHT});
        m_ground.setPosition(0.f, HEIGHT - GROUND_HEIGHT);
        m_ground.setFillColor(Color(83, 53, 10));
        for (int i = 0; i < NUM_CLOUDS; ++i)
        {
            float w = (float)(rand() % 80 + 60);
            m_clouds[i].setSize({w, w * 0.5f});
            m_clouds[i].setFillColor(Color(255, 255, 255, 180));
            m_clouds[i].setPosition((float)(rand() % WIDTH), (float)(rand() % 150 + 20));
            m_cloudOffsets[i] = (float)(rand() % 30 + 10) * 0.01f;
        }
    }
    void update(float dt, float pipeSpeed)
    {
        for (int i = 0; i < NUM_CLOUDS; ++i)
        {
            m_clouds[i].move(-m_cloudOffsets[i] * pipeSpeed * dt, 0.f);
            if (m_clouds[i].getPosition().x + m_clouds[i].getSize().x < 0.f)
                m_clouds[i].setPosition(WIDTH + 10.f, m_clouds[i].getPosition().y);
        }
    }
    void draw(RenderWindow &window)
    {
        window.draw(m_sky);
        for (auto &c : m_clouds) window.draw(c);
        window.draw(m_ground);
    }
};

// ─────────────────────── TEXT HELPER ───────────────────────
Text makeText(const Font &font, unsigned size, Color fill)
{
    Text t;
    t.setFont(font);
    t.setCharacterSize(size);
    t.setFillColor(fill);
    t.setOutlineColor(Color::Black);
    t.setOutlineThickness(2.f);
    return t;
}

// ─────────────────────── RESET ───────────────────────
void resetGame(Bird &bird, Pipe *pipes, int maxPipes,
               int &pipeIndex, float &spawnTimer,
               int &score, State &state)
{
    bird.reset();
    for (int i = 0; i < maxPipes; ++i) pipes[i] = Pipe();
    pipeIndex  = 0;
    spawnTimer = 0.f;
    score      = 0;
    state      = State::Playing;
}

// ─────────────────────── MAIN ───────────────────────
int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Flappy Bird");
    window.setFramerateLimit(60);

    // ── Textures ──
    Texture birdTex;
    bool hasBirdTex = birdTex.loadFromFile("Assets/graphics/Bird.png");

    Texture pipeTex;
    pipeTex.loadFromFile("Assets/graphics/Pipes.png");
    pipeTex.setRepeated(false);

    // ── Font ──
    Font font;
    font.loadFromFile("Assets/fonts/KOMIKAP_.ttf");

    // ── Bird ──
    Bird bird;
    bird.init(birdTex);

    CircleShape birdCircle(18.f);
    birdCircle.setFillColor(Color(255, 220, 50));
    birdCircle.setOutlineColor(Color(200, 150, 0));
    birdCircle.setOutlineThickness(2.f);
    birdCircle.setOrigin(18.f, 18.f);

    // ── Pipes ──
    Pipe pipes[MAX_PIPES];
    int pipeIndex = 0;
    float spawnTimer = 0.f;

    // ── Background ──
    Background bg;
    bg.init();

    // ── UI Text ──
    Text scoreText    = makeText(font, 36, Color::White);
    Text bestText     = makeText(font, 36, Color::Yellow);
    Text centerText   = makeText(font, 30, Color::White);
    Text diffHudText  = makeText(font, 22, Color::White);

    scoreText.setPosition(10.f, 10.f);

    // ── State ──
    State state   = State::Menu;
    int score     = 0;
    int bestScore = 0;
    int diffIndex = 1; // default: Medium

    Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        const DifficultySettings &diff = DIFFICULTIES[diffIndex];

        // ── Events ──
        Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == Event::Closed)
                window.close();

            // ---- Keyboard events ----
            if (ev.type == Event::KeyPressed)
            {
                // 1 / 2 / 3 — select difficulty on menu screen
                if (state == State::Menu)
                {
                    if (ev.key.code == Keyboard::Num1 || ev.key.code == Keyboard::Numpad1)
                        diffIndex = 0;
                    else if (ev.key.code == Keyboard::Num2 || ev.key.code == Keyboard::Numpad2)
                        diffIndex = 1;
                    else if (ev.key.code == Keyboard::Num3 || ev.key.code == Keyboard::Numpad3)
                        diffIndex = 2;
                }

                // Space — start / jump / restart
                if (ev.key.code == Keyboard::Space)
                {
                    if (state == State::Menu)
                        resetGame(bird, pipes, MAX_PIPES, pipeIndex, spawnTimer, score, state);
                    else if (state == State::Playing)
                        bird.jump();
                    else if (state == State::Dead)
                        resetGame(bird, pipes, MAX_PIPES, pipeIndex, spawnTimer, score, state);
                }
            }

            // Left click — jump / restart (not used for difficulty selection anymore)
            if (ev.type == Event::MouseButtonPressed &&
                ev.mouseButton.button == Mouse::Left)
            {
                if (state == State::Playing)
                    bird.jump();
                else if (state == State::Dead)
                    resetGame(bird, pipes, MAX_PIPES, pipeIndex, spawnTimer, score, state);
            }
        }

        // ── Update ──
        if (state == State::Playing)
        {
            bg.update(dt, diff.pipeSpeed);
            bird.update(dt);

            spawnTimer += dt;
            if (spawnTimer >= diff.spawnInterval)
            {
                pipes[pipeIndex].init(pipeTex, (float)WIDTH, diff.gap);
                pipeIndex = (pipeIndex + 1) % MAX_PIPES;
                spawnTimer = 0.f;
            }

            for (int i = 0; i < MAX_PIPES; ++i)
            {
                if (!pipes[i].isActive()) continue;
                pipes[i].update(dt, diff.pipeSpeed);

                if (pipes[i].birdPassed(bird.getPosition().x))
                    ++score;

                if (pipes[i].checkCollision(bird.getBounds()))
                {
                    bird.kill();
                    state = State::Dead;
                }
            }

            if (hitGround(bird.getPosition()) || hitCeiling(bird.getPosition()))
            {
                bird.kill();
                state = State::Dead;
            }

            if (score > bestScore) bestScore = score;

            scoreText.setString("Score: " + toString(score));
            bestText.setString("Best: " + toString(bestScore));
            FloatRect bsr = bestText.getLocalBounds();
            bestText.setPosition(WIDTH - bsr.width - 10.f, 10.f);

            // Difficulty HUD (bottom-left)
            diffHudText.setFillColor(diff.labelColor);
            diffHudText.setString(diff.name);
            diffHudText.setPosition(10.f, HEIGHT - GROUND_HEIGHT - 30.f);
        }

        // ── Draw ──
        window.clear();
        bg.draw(window);

        for (int i = 0; i < MAX_PIPES; ++i)
            if (pipes[i].isActive())
            {
                window.draw(pipes[i].getTopSprite());
                window.draw(pipes[i].getBottomSprite());
            }

        if (hasBirdTex) window.draw(bird.getSprite());
        else { birdCircle.setPosition(bird.getPosition()); birdCircle.setRotation(bird.getRotation()); window.draw(birdCircle); }

        if (state == State::Playing || state == State::Dead)
        {
            window.draw(scoreText);
            window.draw(bestText);
            window.draw(diffHudText);
        }

        // ── Menu screen ──
        if (state == State::Menu)
        {
            // Title
            centerText.setCharacterSize(42);
            centerText.setFillColor(Color::White);
            centerText.setString("FLAPPY BIRD");
            FloatRect r = centerText.getLocalBounds();
            centerText.setOrigin(r.width / 2.f, r.height / 2.f);
            centerText.setPosition(WIDTH / 2.f, HEIGHT / 2.f - 110.f);
            window.draw(centerText);

            // "Select Difficulty:" label
            centerText.setCharacterSize(24);
            centerText.setFillColor(Color::White);
            centerText.setString("Select Difficulty:");
            r = centerText.getLocalBounds();
            centerText.setOrigin(r.width / 2.f, r.height / 2.f);
            centerText.setPosition(WIDTH / 2.f, HEIGHT / 2.f - 40.f);
            window.draw(centerText);

            // Three key-hint lines, each colored + arrow indicator for selected
            const std::string labels[3] = {
                "[1]  EASY",
                "[2]  MEDIUM",
                "[3]  HARD"
            };
            for (int i = 0; i < 3; ++i)
            {
                bool selected = (i == diffIndex);
                centerText.setCharacterSize(selected ? 28 : 24);
                centerText.setFillColor(selected
                    ? DIFFICULTIES[i].labelColor
                    : Color(180, 180, 180));
                centerText.setString(selected ? "> " + labels[i] + " <" : "  " + labels[i]);
                r = centerText.getLocalBounds();
                centerText.setOrigin(r.width / 2.f, r.height / 2.f);
                centerText.setPosition(WIDTH / 2.f, HEIGHT / 2.f + 10.f + i * 40.f);
                window.draw(centerText);
            }

            // Start hint
            centerText.setCharacterSize(20);
            centerText.setFillColor(Color(220, 220, 220));
            centerText.setString("Press SPACE to Start");
            r = centerText.getLocalBounds();
            centerText.setOrigin(r.width / 2.f, r.height / 2.f);
            centerText.setPosition(WIDTH / 2.f, HEIGHT / 2.f + 155.f);
            window.draw(centerText);
        }
        // ── Dead screen ──
        else if (state == State::Dead)
        {
            centerText.setCharacterSize(30);
            centerText.setString(
                "Game Over!\n\nDifficulty : " + diff.name +
                "\nScore      : " + toString(score) +
                "\nBest       : " + toString(bestScore) +
                "\n\nPress SPACE to Restart");
            FloatRect r = centerText.getLocalBounds();
            centerText.setOrigin(r.width / 2.f, r.height / 2.f);
            centerText.setPosition(WIDTH / 2.f, HEIGHT / 2.f - 20.f);

            RectangleShape backdrop({r.width + 40.f, r.height + 50.f});
            backdrop.setFillColor(Color(0, 0, 0, 150));
            backdrop.setOrigin(backdrop.getSize().x / 2.f, backdrop.getSize().y / 2.f);
            backdrop.setPosition(WIDTH / 2.f, HEIGHT / 2.f);
            window.draw(backdrop);
            window.draw(centerText);
        }

        window.display();
    }

    return 0;
}