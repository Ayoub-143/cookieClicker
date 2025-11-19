#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Player {
public:
    sf::Sprite sprite;
    double speed = 280.0;

    Player(const sf::Texture& texture);
    void move(double dx, double dy, float dt);
    void triggerBong();
    void updateBong(float dt);

private:
    sf::Clock animClock;
    bool animating = false;
    float animTime = 0.0f;
    const float ANIM_DURATION = 0.12f;
};

class Background {
public:
    sf::Sprite sprite;
    Background(const sf::Texture& texture);
};

class UpgradeButton {
public:
    sf::Sprite sprite;
    int cost = 100;
    int cpsIncrease = 1;
    bool purchased = false;

    UpgradeButton(const sf::Texture& texture, int c, int increase)
        : cost(c), cpsIncrease(increase) {
        sprite.setTexture(texture);
        sprite.scale(1.0f, 1.0f);
    }
    void scaling(const sf::Texture& texture,float scaleX,float scaleY);
};

class Game {
public:
    Game();
    void run();

private:
    void handleEvents();
    void update(float dt);
    void render();
    void loadGame();
    void saveGame();

    sf::RenderWindow window{sf::VideoMode(1920, 1080), "Cookie Clicker Pro Maximum Ultra Deluxe Edition"};
    
    // Textures (must be loaded BEFORE objects that use them)
    sf::Texture backTexture;
    sf::Texture playerTexture;
    sf::Texture upgradeTexture;
    sf::Texture grandpaTexture;

    // These use placement new → will be constructed after textures load
    Background bg{backTexture};
    Player player{playerTexture};
    UpgradeButton upgradeButton{upgradeTexture, 100, 1};
    UpgradeButton Grandpa{upgradeTexture, 300, 1};

    sf::Font font;
    sf::Text cookieText;
    sf::Text fpsText;
    sf::Text upText;
    sf::SoundBuffer clickBuffer;
    sf::Sound clickSound;

    int cookies = 0;
    int cookiesPerClick = 1;
    int costIncrement = 10;

    sf::Clock clock;
    sf::Clock saveTimer;
    int frameCount = 0;
    float fpsTimer = 0.0f;
};