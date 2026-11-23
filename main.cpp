#include <SFML/Graphics.hpp>
#include<iostream>
#include <fstream>
float ballSpeedX = 0;
float ballSpeedY = 0;
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        paddleX = paddleX - 8; // true for every frame the key is held down
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        paddleX = paddleX + 8; // true for every frame the key is held down

    if (paddleX < 0)                       //left bounadry limit for paddle
    {
        paddleX = 0;
    }
    if (paddleX > 500)                     //right boundary limit for paddle
    {
        paddleX = 500;
    }

    if (!ballLaunched)                                  //the ball stays on top of paddle before launch
    {
         ballX = paddleX + 44.f;
         ballY = 630.f;
    }


    if (pressedKey == LAUNCH && !ballLaunched)             //for launching ball only once
    {
        ballLaunched = true;
        ballSpeedY = -5;
        if (rand() % 2)                                    // for deciding direction randomly
        {
            ballSpeedX = 3;
        }
        else
        {
            ballSpeedX = -3;
        }

    }

    if (ballLaunched)                                   // for motion of ball
    {
        ballX += ballSpeedX;
        ballY += ballSpeedY;
    }
    if (ballX <= 0 || ballX >= 590)                     // right and left boundary limits for ball 
    {
        ballSpeedX = -ballSpeedX;
    }
    if (ballY <= 0)                                     // top boundary for ball
    {
        ballSpeedY = -ballSpeedY;
    }
    //ball collision with paddle
    if (ballY + 16 >= 650 && ballY <= 664 && ballX + 16 >= paddleX && ballX <= paddleX + 100 && ballSpeedY > 0)
    {
        float paddlewidth = 100;                                       
        float part = (ballX + 8.f - paddleX) * 4.f / paddlewidth;       //differnt direction and speed depending on which part of paddle ball hits
        if (part < 0)
            part = 0;
        if (part > 4)
            part = 4;



        ballSpeedX = (part - 2) * 3;
        ballSpeedY = -5;
    }
}

int drawGame()
{
    sf::RenderWindow window(sf::VideoMode({ 600, 700 }), "Block Buster");

    sf::RectangleShape paddle({ 100.f, 14.f });
    sf::CircleShape ball(8.f); // radius 6, so it fits in a 12 x 12 box
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

    std::ifstream file("Bricks.txt");                           //if txt file not found 
    if (!file.is_open())
    {
        std::cout << "File not found!";
        return 0;
    }
    int fileRows, fileCols;

    file >> fileRows >> fileCols;
    for (int r = 0; r < Rows; r++)                              //taking bricks from file
    {
        for (int c = 0; c < Cols; c++)
        {
            file >> Bricks[r][c];
        }
    }

    file.close();

    sf::RectangleShape brick({ BRICK_W - 2, BRICK_H - 2 }); // smaller, so a gap shows
    // for the brick at row r and column c:
    float paddleX = 250.f;
    float ballX = 200.f;
    float ballY = 630.f;


    while (window.isOpen())                                           // actual display
    {
       
        pressedKey = NONE;
        
        // Handle window closing
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
       
            
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Space)
                    pressedKey = LAUNCH; // the key the player pressed
                // ... Enter, Escape, Up, Down in the same way
                
            }
        }

        updateGame(paddleX,ballX,ballY);

        window.clear(sf::Color::Black);

        for (int r = 0; r < Rows; r++)                            //loop for every brick
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
        
       
        paddle.setPosition({ (float)paddleX, 650.f });
        ball.setPosition({ (float)ballX, (float)ballY });
        window.draw(paddle);
        window.draw(ball);
        window.display();
    }
    

    

}
int main()
{
    drawGame();
    
}
