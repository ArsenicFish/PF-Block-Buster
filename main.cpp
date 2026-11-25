
#include <SFML/Graphics.hpp>
#include<iostream>
#include <fstream>
#include <cstdlib>
#include <optional>

float ballSpeedX = 0;
float ballSpeedY = 0;
const int Rows = 8;
const int Cols = 10;
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
bool checkBricks(float ballX, float ballY,int bricks[Rows][Cols], int& score);
void updateGame(float& paddleX, float& ballX, float& ballY, int bricks[Rows][Cols], int& score)
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
        if (checkBricks(ballX, ballY, bricks, score))
        {
            ballX -= ballSpeedX;       
            ballSpeedX = -ballSpeedX;  
        }
        ballY += ballSpeedY;
        if (checkBricks(ballX, ballY, bricks, score))
        {
            ballY -= ballSpeedY;       
            ballSpeedY = -ballSpeedY;  
        }
    }

    if (ballX <= 0 || ballX >= 586)        //right and left boundary limits for ball
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
const int BALL_SIZE = 16.f;

bool checkBricks(float ballX, float ballY,int bricks[Rows][Cols], int& score)
{
    bool touched = false;
    bool checked[Rows][Cols] = {};

    float cx[4] = {ballX,ballX + BALL_SIZE,ballX,ballX + BALL_SIZE};

    float cy[4] = {ballY,ballY,ballY + BALL_SIZE,ballY + BALL_SIZE};

    for (int i = 0; i < 4; i++)
    {
        // Check if corner is inside the brick area
        if (cy[i] >= 100 && cy[i] < 300 &&
            cx[i] >= 0 && cx[i] < 600)
        {
            int r = (cy[i] - 100) / 25;
            int c = cx[i] / 60;

            // to avoid hitting the same brick twice
            if (!checked[r][c])
            {
                checked[r][c] = true;

                if (bricks[r][c] > 0)
                {
                    bricks[r][c]--;

                    touched = true;

                    if (bricks[r][c] == 0)
                    {
                        score += 50;
                    }
                    else
                        score += 10;
                }
            }
        }
    }

    return touched;
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

    sf::Text Totalscore(font, "Total Score: 0", 45);
    Totalscore.setFillColor(sf::Color::Yellow);
    Totalscore.setPosition({ 140.f, 360.f });

    sf::Text scoreText(font, "Score: 0", 22);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition({ 400.f, 10.f });

    const float BRICK_W = 60.f;
    const float BRICK_H = 25.f;
    const float BRICKS_Y = 100.f;

    sf::Color colors[4] =
    {
        sf::Color(0, 255, 255), 
        sf::Color(0, 150, 255),   
        sf::Color(180, 0, 255),   
        sf::Color(255, 50, 100)   
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
    int score = 0;

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
        
        

        updateGame(paddleX, ballX, ballY,Bricks,score);
        

        //update lives
        livesText.setString("Lives: " + std::to_string(lives));
        //update score
        scoreText.setString("Score: " + std::to_string(score));
        //ipdate total score
        Totalscore.setString("Total Score: " + std::to_string(score));

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
        //display score
        window.draw(scoreText);

        //display game over
        if (gameOver)
        {
            window.draw(gameOverText);
            window.draw(Totalscore);
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
