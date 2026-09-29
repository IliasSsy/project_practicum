#include <SFML/Graphics.hpp>
#include <optional>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML Test");
    sf::Color curColor = sf::Color::Black;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                window.close();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                curColor = sf::Color::Yellow;
            }
        }
        window.clear(curColor);
        window.display();
    }
    return 0;
}