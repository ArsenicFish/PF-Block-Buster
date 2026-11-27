
#include <SFML/Graphics.hpp>
#include <iostream>
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

const int MENU = 0;           // Menu screen
const int PLAYING = 1;        // Game screen
const int GAME_OVER = 2;      // Game over screen
int gameState = MENU;
bool playerWon = false;

KeyAction pressedKey = NONE;
bool ballLaunched = false;

bool checkBricks(float ballX, float ballY, int bricks[Rows][Cols], int& score);

// Function to restart the game and reset all values
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

    // Load brick values from the file
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

// Function to update the game movements and collisions
void updateGame(float& paddleX, float& ballX, float& ballY, int bricks[Rows][Cols], int& score)
{
    if (gameOver)
        return;

    // Move paddle using arrow keys or A and D
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        paddleX = paddleX - 8;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        paddleX = paddleX + 8;

    // Keep paddle inside the window boundaries
    if (paddleX < 0)
        paddleX = 0;

    if (paddleX > 500)
        paddleX = 500;

    // Keep ball on the paddle before launching
    if (!ballLaunched)
    {
        ballX = paddleX + 44.f;
        ballY = 630.f;
    }

    // Launch ball when Space is pressed
    if (pressedKey == LAUNCH && !ballLaunched)
    {
        ballLaunched = true;
        ballSpeedY = -5;

        // Give ball a random horizontal direction
        if (rand() % 2)
            ballSpeedX = 3;
        else
            ballSpeedX = -3;
    }

    // Move ball and check brick collisions
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

    // Right and left boundary limits for ball
    if (ballX <= 0.f || ballX >= 586)
        ballSpeedX = -ballSpeedX;

    // Top boundary for ball
    if (ballY <= 0)
        ballSpeedY = -ballSpeedY;

    // Ball collision with paddle
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

    // Ball falls below the screen
    if (ballY > 700 && ballLaunched)
    {
        lives--;

        if (lives <= 0)
        {
            gameOver = true;
        }
        else
        {
            // Reset ball position after losing a life
            ballLaunched = false;
            ballSpeedX = 0;
            ballSpeedY = 0;

            ballX = paddleX + 44.f;
            ballY = 630.f;
        }
    }
}

const int BALL_SIZE = 16.f;

// Function to check collision between ball and bricks
bool checkBricks(float ballX, float ballY, int bricks[Rows][Cols], int& score)
{
    bool touched = false;
    bool checked[Rows][Cols] = {};

    // Store the four corners of the ball
    float cx[4] = { ballX, ballX + BALL_SIZE, ballX, ballX + BALL_SIZE };
    float cy[4] = { ballY, ballY, ballY + BALL_SIZE, ballY + BALL_SIZE };

    for (int i = 0; i < 4; i++)
    {
        // Check if corner is inside the brick area
        if (cy[i] >= 100 && cy[i] < 300 &&
            cx[i] >= 0 && cx[i] < 600)
        {
            int r = (cy[i] - 100) / 25;
            int c = cx[i] / 60;

            // Avoid hitting the same brick twice
            if (!checked[r][c])
            {
                checked[r][c] = true;

                if (bricks[r][c] > 0)
                {
                    bricks[r][c]--;
                    touched = true;

                    // Update score according to brick condition
                    if (bricks[r][c] == 0)
                        score += 50;
                    else
                        score += 10;
                }
            }
        }
    }

    return touched;
}

// Function to save the current game progress
void saveGame(int bricks[Rows][Cols], int score, float paddleX, float ballX, float ballY)
{
    std::ofstream out("savegame.txt");

    out << score << " " << lives << " " << paddleX << " "
        << ballX << " " << ballY << " " << ballSpeedX << " "
        << ballSpeedY << " " << ballLaunched << "\n";

    // Save all brick values
    for (int r = 0; r < Rows; r++)
    {
        for (int c = 0; c < Cols; c++)
            out << bricks[r][c] << " ";

        out << "\n";
    }
}

// Function to load previously saved game progress
bool loadGame(int bricks[Rows][Cols], int& score, float& paddleX, float& ballX, float& ballY)
{
    std::ifstream in("savegame.txt");

    if (!(in >> score >> lives >> paddleX >> ballX >> ballY
        >> ballSpeedX >> ballSpeedY >> ballLaunched))
        return false;

    // Load all brick values
    for (int r = 0; r < Rows; r++)
        for (int c = 0; c < Cols; c++)
            in >> bricks[r][c];

    gameOver = false;
    playerWon = false;

    return true;
}

// Empty savegame.txt by opening and closing it
void clearSave()
{
    std::ofstream out("savegame.txt");
}

// Main function to display and control the game
int drawGame()
{
    sf::RenderWindow window(sf::VideoMode({ 600, 700 }), "Block Buster");

    window.setFramerateLimit(60);

    // Background image
    sf::Texture backgroundTexture;

    if (!backgroundTexture.loadFromFile("background.png"))
    {
        std::cout << "Background image not found!";
        return 0;
    }

    sf::Sprite background(backgroundTexture);

    // Adjust background to full window size
    sf::Vector2u imageSize = backgroundTexture.getSize();
    sf::Vector2u windowSize = window.getSize();

    background.setScale({
        (float)windowSize.x / imageSize.x,
        (float)windowSize.y / imageSize.y
        });

    // Create paddle and ball shapes
    sf::RectangleShape paddle({ 100.f, 14.f });
    sf::CircleShape ball(8.f);

    // Font for displaying game information
    sf::Font font;

    if (!font.openFromFile("arial.ttf"))
    {
        std::cout << "Font not found!";
        return 0;
    }

    // Display lives
    sf::Text livesText(font, "Lives: 3", 22);
    livesText.setFillColor(sf::Color::Cyan);
    livesText.setPosition({ 45.f, 10.f });

    // Display game over message
    sf::Text gameOverText(font, "GAME OVER", 45);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition({ 160.f, 300.f });

    // Display total score
    sf::Text Totalscore(font, "Total Score: 0", 45);
    Totalscore.setFillColor(sf::Color::Yellow);
    Totalscore.setPosition({ 140.f, 360.f });

    // Display win message
    sf::Text winText(font, "YOU WIN!", 60);
    winText.setFillColor(sf::Color::Green);
    winText.setPosition({ 150.f, 280.f });

    sf::Text restartText(font, "Press Enter", 22);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition({ 190.f, 430.f });

    // Display current score
    sf::Text scoreText(font, "Score: 0", 22);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition({ 400.f, 10.f });

    // Game title
    sf::Text titleText(font, "BLOCK BUSTER", 50);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setPosition({ 100.f, 120.f });

    // Menu options
    std::string menuItems[3] = { "New Game", "Continue", "Exit" };
    int menuChoice = 0;
    std::string menuMessage = "";

    // Brick dimensions and starting position
    const float BRICK_W = 60.f;
    const float BRICK_H = 25.f;
    const float BRICKS_Y = 100.f;

    // Different colors for bricks
    sf::Color colors[4] =
    {
        sf::Color(0, 255, 255),
        sf::Color(0, 150, 255),
        sf::Color(180, 0, 255),
        sf::Color(255, 50, 100)
    };

    // Store brick values
    int Bricks[Rows][Cols] = {};

    int score = 0;

    sf::RectangleShape brick({ BRICK_W - 2, BRICK_H - 2 });

    // Initial paddle and ball positions
    float paddleX = 250.f;
    float ballX = 200.f;
    float ballY = 630.f;

    // Main game loop
    while (window.isOpen())
    {
        pressedKey = NONE;

        // Handle window closing and keyboard input
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Space)
                    pressedKey = LAUNCH;

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

        // Control different game screens
        switch (gameState)
        {
        case MENU:

            // Move selection up
            if (pressedKey == UP)
            {
                menuChoice--;

                if (menuChoice < 0)
                    menuChoice = 2;

                menuMessage = "";
            }

            // Move selection down
            if (pressedKey == DOWN)
            {
                menuChoice = (menuChoice + 1) % 3;
                menuMessage = "";
            }

            // Select menu option
            if (pressedKey == ENTER)
            {
                if (menuChoice == 0)
                {
                    if (restartGame(Bricks, score, paddleX, ballX, ballY))
                        gameState = PLAYING;
                    else
                        menuMessage = "Bricks.txt not found!";
                }
                else if (menuChoice == 1)
                {
                    if (loadGame(Bricks, score, paddleX, ballX, ballY))
                        gameState = PLAYING;
                    else
                        menuMessage = "No saved game";
                }
                else
                    window.close();
            }

            break;

        case PLAYING:

            // Save game and return to menu when Escape is pressed
            if (pressedKey == ESCAPE)
            {
                saveGame(Bricks, score, paddleX, ballX, ballY);
                gameState = MENU;
                menuMessage = "";
                break;
            }

            // Update game movements
            updateGame(paddleX, ballX, ballY, Bricks, score);

            // Check if any bricks are left
            if (!gameOver)
            {
                bool anyBrickLeft = false;

                for (int r = 0; r < Rows; r++)
                    for (int c = 0; c < Cols; c++)
                        if (Bricks[r][c] > 0)
                            anyBrickLeft = true;

                // Player wins when all bricks are destroyed
                if (!anyBrickLeft)
                {
                    playerWon = true;
                    gameOver = true;
                }
            }

            // Move to game over screen
            if (gameOver)
            {
                clearSave();
                gameState = GAME_OVER;
            }

            break;

        case GAME_OVER:

            // Return to menu when Enter is pressed
            if (pressedKey == ENTER)
            {
                gameState = MENU;
                menuMessage = "";
            }

            break;
        }

        // Update lives and score
        livesText.setString("Lives: " + std::to_string(lives));
        scoreText.setString("Score: " + std::to_string(score));
        Totalscore.setString("Total Score: " + std::to_string(score));

        // Draw background first
        window.clear();
        window.draw(background);

        // Draw menu screen
        if (gameState == MENU)
        {
            window.draw(titleText);

            // Display menu options
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

            // Display menu error message
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
            // Draw bricks
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

            // Draw paddle and ball
            if (!gameOver)
            {
                paddle.setPosition({ paddleX, 650.f });
                ball.setPosition({ ballX, ballY });

                window.draw(paddle);
                window.draw(ball);
            }

            // Display lives and score
            window.draw(livesText);
            window.draw(scoreText);

            // Display game over screen
            if (gameOver)
            {
                if (playerWon)
                    window.draw(winText);
                else
                    window.draw(gameOverText);

                window.draw(Totalscore);
                window.draw(restartText);
            }
        }

        window.display();
    }

    return 0;
}

// Main function
int main()
{
    drawGame();
    return 0;
}
