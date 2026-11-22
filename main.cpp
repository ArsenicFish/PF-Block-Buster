#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 600, 700 }), "Block Buster");
    window.setFramerateLimit(60);

  //const float BRICK_W = 60.f, BRICK_H = 25.f; // one brick is 60 x 25 pixels
    //const float BRICKS_Y = 100.f; // the bricks start at y = 100
    //sf::Color colors[4] = { sf::Color(25, 25, 45), // 0: no brick
    //sf::Color::Green, // 1: one hit left
    //sf::Color::Yellow, // 2: two hits left
    //sf::Color::Red }; // 3: three hits left

    //sf::RectangleShape brick({ BRICK_W - 2, BRICK_H - 2 }); // smaller, so a gap shows
    //// for the brick at row r and column c:
    //brick.setFillColor(colors[bricks[r][c]]);
    //brick.setPosition({ c * BRICK_W + 1, BRICKS_Y + r * BRICK_H + 1 });
    //window.draw(brick);
    //
  
}