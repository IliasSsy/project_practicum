#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <iostream>

struct Vehicle {
    sf::RectangleShape body{sf::Vector2f(300.f, 80.f)};
    sf::ConvexShape roof;
    sf::CircleShape wheelLeft{30.f};
    sf::CircleShape wheelRight{30.f};
    sf::CircleShape headlight{12.f};

    sf::Vector2f position{250.f, 320.f};
    float speed = 300.f;

    sf::Vector2f bodyOffset{0.f, 0.f};
    sf::Vector2f roofOffset{60.f, -60.f};
    sf::Vector2f wheelLeftOffset{30.f, 50.f};
    sf::Vector2f wheelRightOffset{220.f, 50.f};
    sf::Vector2f headlightOffset{270.f, 15.f};
};

struct Enemy {
    sf::RectangleShape body{sf::Vector2f(50.f, 25.f)};
    sf::Vector2f position{100.f, 100.f};
    sf::Vector2f velocity{200.f, 150.f};
};

void carInit(Vehicle& car)
{
    car.body.setFillColor(sf::Color(220, 50, 50));
    car.body.setOutlineColor(sf::Color::White);
    car.body.setOutlineThickness(3.f);

    car.roof.setPointCount(4);
    car.roof.setPoint(0, sf::Vector2f(0.f, 0.f));
    car.roof.setPoint(1, sf::Vector2f(140.f, 0.f));
    car.roof.setPoint(2, sf::Vector2f(180.f, 60.f));
    car.roof.setPoint(3, sf::Vector2f(-40.f, 60.f));
    car.roof.setFillColor(sf::Color(100, 180, 240));
    car.roof.setOutlineColor(sf::Color::White);
    car.roof.setOutlineThickness(3.f);

    car.wheelLeft.setFillColor(sf::Color(40, 40, 40));
    car.wheelLeft.setOutlineColor(sf::Color(180, 180, 180));
    car.wheelLeft.setOutlineThickness(4.f);

    car.wheelRight.setFillColor(sf::Color(40, 40, 40));
    car.wheelRight.setOutlineColor(sf::Color(180, 180, 180));
    car.wheelRight.setOutlineThickness(4.f);

    car.headlight.setFillColor(sf::Color::Yellow);
    car.headlight.setOutlineColor(sf::Color::Blue);
    car.headlight.setOutlineThickness(2.f);
}

void enemyInit(Enemy& enemy)
{
    enemy.body.setFillColor(sf::Color(255, 140, 0));
    enemy.body.setOutlineColor(sf::Color::Red);
    enemy.body.setOutlineThickness(2.f);
}

void carUpdate(Vehicle& car, float dt)
{
    sf::Vector2f movement(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        movement.y -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        movement.y += 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        movement.x -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        movement.x += 1.f;
    }
    float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
    if (length != 0.f) {
        movement.x /= length;
        movement.y /= length;
    }
    car.position += movement * car.speed * dt;
    if (car.position.x < 0.f) car.position.x = 0.f;
    if (car.position.x > 800.f - 300.f) car.position.x = 800.f - 300.f;
    if (car.position.y < 60.f) car.position.y = 60.f;
    if (car.position.y > 600.f - 100.f) car.position.y = 600.f - 100.f;

    car.body.setPosition(car.position + car.bodyOffset);
    car.roof.setPosition(car.position + car.roofOffset);
    car.wheelLeft.setPosition(car.position + car.wheelLeftOffset);
    car.wheelRight.setPosition(car.position + car.wheelRightOffset);
    car.headlight.setPosition(car.position + car.headlightOffset);
}

void enemyUpdate(Enemy& enemy, float dt)
{
    enemy.position += enemy.velocity * dt;

    if (enemy.position.x < 0.f) {
        enemy.position.x = 0.f;
        enemy.velocity.x = -enemy.velocity.x;
    }
    if (enemy.position.x + 50.f > 800.f) {
        enemy.position.x = 800.f - 50.f;
        enemy.velocity.x = -enemy.velocity.x;
    }
    if (enemy.position.y < 0.f) {
        enemy.position.y = 0.f;
        enemy.velocity.y = -enemy.velocity.y;
    }
    if (enemy.position.y + 25.f > 600.f) {
        enemy.position.y = 600.f - 25.f;
        enemy.velocity.y = -enemy.velocity.y;
    }
    enemy.body.setPosition(enemy.position);
}

void carDraw(sf::RenderWindow& window, const Vehicle& car)
{
    window.draw(car.body);
    window.draw(car.roof);
    window.draw(car.wheelLeft);
    window.draw(car.wheelRight);
    window.draw(car.headlight);
}

void enemyDraw(sf::RenderWindow& window, const Enemy& enemy)
{
    window.draw(enemy.body);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Car vs Rocket");
    window.setFramerateLimit(60);
    sf::Color curColor = sf::Color::Green;
    Vehicle car;
    carInit(car);
    Enemy enemy;
    enemyInit(enemy);
    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                window.close();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            {
                curColor = sf::Color(30, 30, 40);
            }
        }

        carUpdate(car, dt);
        enemyUpdate(enemy, dt);

        bool collisionX = car.position.x < enemy.position.x + 50.f && car.position.x + 300.f > enemy.position.x;
        bool collisionY = car.position.y < enemy.position.y + 25.f && car.position.y + 80.f > enemy.position.y;

        if (collisionX && collisionY)
        {
            std::cout << "COLLISION DETECTED!" << std::endl;
            car.position = sf::Vector2f(250.f, 320.f);
            enemy.body.setFillColor(sf::Color::Red);
        }
        else
        {
            enemy.body.setFillColor(sf::Color(255, 140, 0));
        }

        window.clear(curColor);
        carDraw(window, car);
        enemyDraw(window, enemy);
        window.display();
    }
    return 0;
}