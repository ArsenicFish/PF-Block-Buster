#include <SFML/Graphics.hpp>
#include<iostream>
#include <fstream>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 600, 700 }), "Block Buster");
    window.setFramerateLimit(60);

  const float BRICK_W = 60.f, BRICK_H = 25.f; // one brick is 60 x 25 pixels
    const float BRICKS_Y = 100.f; // the bricks start at y = 100
    sf::Color colors[4] = { sf::Color(25, 25, 45), // 0: no brick
    sf::Color::Cyan, // 1: one hit left
    sf::Color::Magenta, // 2: two hits left
    sf::Color::Red }; // 3: three hits left
    
    const int Rows = 8;
    const int Cols = 10;
    int Bricks[Rows][Cols];

    std::ifstream file("Bricks.txt");
    if (!file.is_open())
    {
        std::cout << "File not found!";
        return 0;
    }
    int fileRows, fileCols;

    file >> fileRows >> fileCols;
    for (int r = 0; r < Rows; r++)
    {
        for (int c = 0; c < Cols; c++)
        {
            file >> Bricks[r][c];
        }
    }

    file.close();

    sf::RectangleShape brick({ BRICK_W - 2, BRICK_H - 2 }); // smaller, so a gap shows
    // for the brick at row r and column c:
    while (window.isOpen())
    {
        // Handle window closing
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);

        for (int r = 0; r < Rows; r++)
        {
            for (int c = 0; c < Cols; c++)
            {
                if (Bricks[r][c] == 0)
                    continue;

                brick.setFillColor(colors[Bricks[r][c]]);
                brick.setPosition({ c * BRICK_W + 1, BRICKS_Y + r * BRICK_H + 1 });
               /* brick.setPosition({
            c * 60.f,
            100.f + r * 25.f
                    });*/
                window.draw(brick);
            }
        }
        window.display();
    }
    
    //sf::RectangleShape paddle({ 100.f, 14.f });
    //sf::CircleShape ball(6.f); // radius 6, so it fits in a 12 x 12 box
    //paddle.setPosition({ (float)paddleX, 650.f });
    //ball.setPosition({ (float)ballX, (float)ballY });
    //window.draw(paddle);
    //window.draw(ball);

    //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
    //    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    //    paddleX = paddleX - 8; // true for every frame the key is held down
    //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
    //    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    //    paddleX = paddleX - 8; // true for every frame the key is held down
  
}
