
#include <SFML/Graphics.hpp>                  
#include<iostream>
#include <fstream>
#include <cstdlib>
#include <optional>
#include <string>                              

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

const int MENU = 0;                            
const int PLAYING = 1;                         
const int GAME_OVER = 2;                       
int gameState = MENU;                          
bool playerWon = false;                        

KeyAction pressedKey = NONE;
bool ballLaunched = false;
bool checkBricks(float ballX, float ballY, int bricks[Rows][Cols], int& score);


bool restartGame(int bricks[Rows][Cols], int& score, float& paddleX, float& ballX, float& ballY)
{
    lives = 3;
    gameOver = false;
    playerWon = false;                         
    ballLaunched = false;
    ballSpeedX = 0;
    ballSpeedY = 0;
    score = 0;

    paddleX = 250.f;
    ballX = paddleX + 44.f;
    ballY = 630.f;

    std::ifstream file("Bricks.txt");

    if (!file.is_open())                       
        return false;                          

    int fileRows, fileCols;
    file >> fileRows >> fileCols;

    for (int r = 0; r < Rows; r++)
    {
        for (int c = 0; c < Cols; c++)
        {
            file >> bricks[r][c];
        }
    }

    file.close();
    return true;                               
}

void updateGame(float& paddleX, float& ballX, float& ballY, int bricks[Rows][Cols], int& score)
{
    if (gameOver)
        return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        paddleX = paddleX - 8; 

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        paddleX = paddleX + 8;

    if (paddleX < 0)                       
    {
        paddleX = 0;
    }

    if (paddleX > 500)                     
    {
        paddleX = 500;
    }

    if (!ballLaunched)                     
    {
        ballX = paddleX + 44.f;
        ballY = 630.f;
    }

    if (pressedKey == LAUNCH && !ballLaunched) 
    {
        ballLaunched = true;
        ballSpeedY = -5;

        if (rand() % 2)                    
        {
            ballSpeedX = 3;
        }
        else
        {
            ballSpeedX = -3;
        }
    }

    if (ballLaunched)                      
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

bool checkBricks(float ballX, float ballY, int bricks[Rows][Cols], int& score)
{
    bool touched = false;
    bool checked[Rows][Cols] = {};

    float cx[4] = { ballX,ballX + BALL_SIZE,ballX,ballX + BALL_SIZE };

    float cy[4] = { ballY,ballY,ballY + BALL_SIZE,ballY + BALL_SIZE };

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


void saveGame(int bricks[Rows][Cols], int score)
{
    std::ofstream out("savegame.txt");
    out << score << " " << lives << "\n";
    for (int r = 0; r < Rows; r++)
    {
        for (int c = 0; c < Cols; c++)
            out << bricks[r][c] << " ";
        out << "\n";
    }
}


bool loadGame(int bricks[Rows][Cols], int& score, float& paddleX, float& ballX, float& ballY)
{
    std::ifstream in("savegame.txt");
    int s, l;
    if (!(in >> s >> l))
        return false;

    score = s;
    lives = l;
    for (int r = 0; r < Rows; r++)
        for (int c = 0; c < Cols; c++)
            in >> bricks[r][c];

    gameOver = false;
    playerWon = false;
    ballLaunched = false;
    ballSpeedX = 0;
    ballSpeedY = 0;
    paddleX = 250.f;
    ballX = paddleX + 44.f;
    ballY = 630.f;
    return true;
}

//  empties savegame.txt (open and close straight away)
void clearSave()
{
    std::ofstream out("savegame.txt");
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

    sf::Text restartText(font, "Press Enter", 22);     
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition({ 190.f, 430.f });

    sf::Text scoreText(font, "Score: 0", 22);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition({ 400.f, 10.f });

    
    sf::Text titleText(font, "BLOCK BUSTER", 50);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setPosition({ 100.f, 120.f });

    std::string menuItems[3] = { "New Game", "Continue", "Exit" };
    int menuChoice = 0;                        // which option is highlighted
    std::string menuMessage = "";              // error line shown on the menu

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
    int Bricks[Rows][Cols] = {};               

    
    

    int score = 0;

    sf::RectangleShape brick({ BRICK_W - 2, BRICK_H - 2 });

    //for the brick at row r and column c:
    float paddleX = 250.f;
    float ballX = 200.f;
    float ballY = 630.f;

    while (window.isOpen())                 
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

                

                if (key->code == sf::Keyboard::Key::Enter)     
                    pressedKey = ENTER;                        
                if (key->code == sf::Keyboard::Key::Escape)    
                    pressedKey = ESCAPE;                       
                if (key->code == sf::Keyboard::Key::Up)       
                    pressedKey = UP;                           
                if (key->code == sf::Keyboard::Key::Down)      
                    pressedKey = DOWN;                         
            }
        }

        
        switch (gameState)
        {
        case MENU:
            if (pressedKey == UP)
            {
                menuChoice--;
                if (menuChoice < 0)
                    menuChoice = 2;                 
                menuMessage = "";
            }
            if (pressedKey == DOWN)
            {
                menuChoice = (menuChoice + 1) % 3;  
                menuMessage = "";
            }
            if (pressedKey == ENTER)
            {
                if (menuChoice == 0)                
                {
                    if (restartGame(Bricks, score, paddleX, ballX, ballY))
                        gameState = PLAYING;
                    else
                        menuMessage = "Bricks.txt not found!";
                }
                else if (menuChoice == 1)           // Continue
                {
                    if (loadGame(Bricks, score, paddleX, ballX, ballY))
                        gameState = PLAYING;
                    else
                        menuMessage = "No saved game";
                }
                else                                // Exit
                    window.close();
            }
            break;

        case PLAYING:
            if (pressedKey == ESCAPE)               // save and go back to menu
            {
                saveGame(Bricks, score);
                gameState = MENU;
                menuMessage = "";
                break;
            }

            updateGame(paddleX, ballX, ballY, Bricks, score);   

            if (!gameOver)                          
            {
                bool anyBrickLeft = false;
                for (int r = 0; r < Rows; r++)
                    for (int c = 0; c < Cols; c++)
                        if (Bricks[r][c] > 0)
                            anyBrickLeft = true;

                if (!anyBrickLeft)
                {
                    playerWon = true;
                    gameOver = true;                
                }
            }

            if (gameOver)                           
            {
                clearSave();
                gameState = GAME_OVER;
            }
            break;

        case GAME_OVER:
            if (pressedKey == ENTER)
            {
                gameState = MENU;
                menuMessage = "";
            }
            break;
        }

        //update lives
        livesText.setString("Lives: " + std::to_string(lives));
        //update score
        scoreText.setString("Score: " + std::to_string(score));
        //ipdate total score
        Totalscore.setString("Total Score: " + std::to_string(score));
        //  "YOU WIN" or "GAME OVER"
        gameOverText.setString(playerWon ? "YOU WIN" : "GAME OVER");

        window.clear(sf::Color::Black);

        if (gameState == MENU)                      
        {
            window.draw(titleText);
            for (int i = 0; i < 3; i++)
            {
                sf::Text item(font, menuItems[i], 32);
                item.setPosition({ 210.f, 260.f + i * 60.f });
                if (i == menuChoice)
                {
                    item.setString("> " + menuItems[i]);   
                    item.setFillColor(sf::Color::Yellow);
                }
                else
                    item.setFillColor(sf::Color::White);
                window.draw(item);
            }
            if (menuMessage != "")
            {
                sf::Text msg(font, menuMessage, 24);
                msg.setFillColor(sf::Color::Red);
                msg.setPosition({ 200.f, 460.f });
                window.draw(msg);
            }
        }
        else                                        
        {
            for (int r = 0; r < Rows; r++)      
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
                window.draw(restartText);
            }
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
