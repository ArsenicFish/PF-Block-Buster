# This is the First semester project of Programming Fundamentals.

The three contributers are::

1. Zayd Abdullah (26L-0522) (ArsenicFish)
2. Abdullah Tanveer (26L-0515)
3. Muhammad Hassan (26L-0535)
#include <SFML/Graphics.hpp>
#include<iostream>
#include <fstream>
#include <cstdlib>

float ballSpeedX = 0;
float ballSpeedY = 0;

int lives = 3;
bool gameOver = false;

enum KeyAction
{
    NONE,
    LAUNCH,
    ENTER,
    ESCAPE,
    UP,
    DOWN
};

KeyAction pressedKey = NONE;
bool ballLaunched = false;

void updateGame(float& paddleX, float& ballX, float& ballY)
{
    if (gameOver)
        return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        paddleX = paddleX - 8; // true for every frame the key is held down

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        paddleX = paddleX + 8;

    if (paddleX < 0)                       //left boundary limit for paddle
    {
        paddleX = 0;
    }

    if (paddleX > 500)                     //right boundary limit for paddle
    {
        paddleX = 500;
    }

    if (!ballLaunched)                     //the ball stays on top of paddle before launch
    {
        ballX = paddleX + 44.f;
        ballY = 630.f;
    }

    if (pressedKey == LAUNCH && !ballLaunched) //for launching ball only once
    {
        ballLaunched = true;
        ballSpeedY = -5;

        if (rand() % 2)                    //for deciding direction randomly
        {
            ballSpeedX = 3;
        }
        else
        {
            ballSpeedX = -3;
        }
    }

    if (ballLaunched)                      //for motion of ball
    {
        ballX += ballSpeedX;
        ballY += ballSpeedY;
    }

    if (ballX <= 0 || ballX >= 590)        //right and left boundary limits for ball
    {
        ballSpeedX = -ballSpeedX;
    }

    if (ballY <= 0)                        //top boundary for ball
    {
        ballSpeedY = -ballSpeedY;
    }

    //ball collision with paddle
    if (ballY + 16 >= 650 && ballY <= 664 &&
        ballX + 16 >= paddleX && ballX <= paddleX + 100 &&
        ballSpeedY > 0)
    {
        float paddlewidth = 100;
        float part = (ballX + 8.f - paddleX) * 4.f / paddlewidth;

        if (part < 0)
            part = 0;

        if (part > 4)
            part = 4;

        ballSpeedX = (part - 2) * 3;
        ballSpeedY = -5;
    }

    //ball falls below the screen
    if (ballY > 700 && ballLaunched)
    {
        lives--;

        if (lives <= 0)
        {
            gameOver = true;
        }
        else
        {
            ballLaunched = false;
            ballSpeedX = 0;
            ballSpeedY = 0;

            ballX = paddleX + 44.f;
            ballY = 630.f;
        }
    }
}

int drawGame()
{
    sf::RenderWindow window(sf::VideoMode({ 600, 700 }), "Block Buster");

    sf::RectangleShape paddle({ 100.f, 14.f });
    sf::CircleShape ball(8.f);

    window.setFramerateLimit(60);

    //font for displaying lives
    sf::Font font;

    if (!font.openFromFile("arial.ttf"))
    {
        std::cout << "Font not found!";
        return 0;
    }

    sf::Text livesText(font, "Lives: 3", 22);
    livesText.setFillColor(sf::Color::Cyan);
    livesText.setPosition({ 45.f, 10.f });

    sf::Text gameOverText(font, "GAME OVER", 45);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition({ 160.f, 300.f });

    const float BRICK_W = 60.f;
    const float BRICK_H = 25.f;
    const float BRICKS_Y = 100.f;

    sf::Color colors[4] =
    {
        sf::Color(0, 255, 255),    // Cyan
        sf::Color(0, 150, 255),    // Blue
        sf::Color(180, 0, 255),   // Purple
        sf::Color(255, 50, 100)   // Pink
    };

    const int Rows = 8;
    const int Cols = 10;
    int Bricks[Rows][Cols];

    std::ifstream file("Bricks.txt");       //if txt file not found

    if (!file.is_open())
    {
        std::cout << "File not found!";
        return 0;
    }

    int fileRows, fileCols;

    file >> fileRows >> fileCols;

    for (int r = 0; r < Rows; r++)          //taking bricks from file
    {
        for (int c = 0; c < Cols; c++)
        {
            file >> Bricks[r][c];
        }
    }

    file.close();

    sf::RectangleShape brick({ BRICK_W - 2, BRICK_H - 2 });

    //for the brick at row r and column c:
    float paddleX = 250.f;
    float ballX = 200.f;
    float ballY = 630.f;

    while (window.isOpen())                 //actual display
    {
        pressedKey = NONE;

        //Handle window closing
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Space)
                    pressedKey = LAUNCH;    //the key the player pressed
            }
        }

        updateGame(paddleX, ballX, ballY);

        //update lives display
        livesText.setString("Lives: " + std::to_string(lives));

        window.clear(sf::Color::Black);

        for (int r = 0; r < Rows; r++)      //loop for every brick
        {
            for (int c = 0; c < Cols; c++)
            {
                if (Bricks[r][c] == 0)
                    continue;

                brick.setFillColor(colors[Bricks[r][c]]);
                brick.setPosition({ c * BRICK_W + 1, BRICKS_Y + r * BRICK_H + 1 });

                window.draw(brick);
            }
        }

        //draw paddle and ball
        if (!gameOver)
        {
            paddle.setPosition({ paddleX, 650.f });
            ball.setPosition({ ballX, ballY });

            window.draw(paddle);
            window.draw(ball);
        }

        //display lives
        window.draw(livesText);

        //display game over
        if (gameOver)
        {
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}

int main()
{
    drawGame();
    return 0;
}
