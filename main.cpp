#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <ctime>
#include <thread>
#include <chrono>

class scoreBoard {
public:
    scoreBoard () {left = 0; right = 0; total = 0; rally = 0;};
    int left, right, total, rally;
    sf::Text imageToDraw (const sf::Font& font, const sf::Color& color, const float& width) const {
        std::string bufferString;
        std::stringstream buffer;
        buffer << left;
        buffer >> bufferString;
        buffer.str("");
        buffer.clear();
        std::string input = bufferString + " | ";
        buffer << right;
        buffer >> bufferString;
        buffer.str("");
        buffer.clear();
        input += bufferString + "\nRally: ";
        buffer << rally;
        buffer >> bufferString;
        buffer.str("");
        buffer.clear();
        input = input + bufferString;
        sf::Text scoreText(input, font, 20);
        scoreText.setColor(color);
        scoreText.setOrigin(scoreText.getLocalBounds().width/2, scoreText.getLocalBounds().height);
        scoreText.setPosition(width/2, 50);
        std::cout << "Input: " << input;
        return scoreText;
    }
} score;

int main () {
    srand(time(NULL));
    //Create window
    const sf::Vector2f windowSize (800, 600);
    sf::RenderWindow window (sf::VideoMode(windowSize.x, windowSize.y), "Pong", sf::Style::Close | sf::Style::Titlebar);

    //Music
    //sf::Music bckMusic;
    //if (!(bckMusic.openFromFile("Earthworm_Jim_Acrophobia_OC_ReMix.mp3")))
    //    return EXIT_FAILURE;

    //Sound buffer (ball hit paddle)
    sf::SoundBuffer bingBuffer;
    if (!(bingBuffer.loadFromFile("bounce.wav")))
        return EXIT_FAILURE;
    //Sound buffer (ball hit wall)
    sf::SoundBuffer bumpBuffer;
    if (!(bumpBuffer.loadFromFile("pinDrop.wav")))
        return EXIT_FAILURE;
    //sound effect
    sf::Sound bump(bumpBuffer);
    sf::Sound bing(bingBuffer);

    //Declare background image
    sf::RectangleShape bckGrnd (windowSize);
    bckGrnd.setFillColor(sf::Color(171, 78, 104));

    //Declare borders for game
    sf::RectangleShape topBounds (sf::Vector2f(windowSize.x, 10.0f));
    sf::RectangleShape botBounds (sf::Vector2f(windowSize.x, 10.0f));
    sf::RectangleShape leftBounds (sf::Vector2f(10, windowSize.y));
    sf::RectangleShape rightBounds (sf::Vector2f(10, windowSize.y));
    //Set fill color for bounds
    sf::Color boundColor (sf::Color(125, 17, 40));
    topBounds.setFillColor(boundColor);
    botBounds.setFillColor(boundColor);
    leftBounds.setFillColor(boundColor);
    rightBounds.setFillColor(boundColor);
    //Set position
    topBounds.setPosition(0, 0);
    botBounds.setPosition(0, (windowSize.y - 10.0f));
    leftBounds.setPosition(0, 0);
    rightBounds.setPosition(windowSize.x-10, 0);

    //Declare paddles
    sf::Vector2f paddleSize (25, 100);
    sf::RectangleShape leftPaddle (paddleSize);
    sf::RectangleShape rightPaddle (paddleSize);
    //color
    const sf::Color paddleColor (sf::Color(82, 25, 69));
    leftPaddle.setFillColor(paddleColor);
    rightPaddle.setFillColor(paddleColor);
    //position
    leftPaddle.setOrigin(paddleSize / 2.0f);
    leftPaddle.setPosition(leftBounds.getGlobalBounds().width+20, windowSize.y / 2);
    rightPaddle.setOrigin(paddleSize / 2.0f);
    rightPaddle.setPosition(rightBounds.getPosition().x-20, windowSize.y / 2);

    //Declare ball
    sf::Vector2f ballSize (20 , 20);
    sf::RectangleShape ball (ballSize);
    //color
    ball.setFillColor(paddleColor);
    //position
    ball.setOrigin(ballSize / 2.0f);
    ball.setPosition(windowSize.x/2-10, windowSize.y/2);

    //Font and text color
    sf::Font font;
    if (!(font.loadFromFile("framd.ttf")))
        return EXIT_FAILURE;
    sf::Color textColor (sf::Color(94, 56, 76));

    //Pause text
    sf::Text pauseText ("Press space to play . . .", font, 50);
    pauseText.setOrigin(pauseText.getGlobalBounds().width /2, pauseText.getGlobalBounds().height /2);
    pauseText.setPosition(windowSize / 2.0f);
    pauseText.setColor(textColor);
    //Wind lose text
    sf::Text winLose ("", font, 50);
    winLose.setOrigin(winLose.getGlobalBounds().width/2, winLose.getGlobalBounds().height/2);
    winLose.setPosition(pauseText.getPosition().x - pauseText.getGlobalBounds().width/2, pauseText.getPosition().y-pauseText.getGlobalBounds().height-55);
    winLose.setColor(textColor);

    //game properties
        //Paddle
    const float paddleSpeed = 400.0f;
    float rightPaddleSpeed  = 0.f;
        //Ball
    float ballSlope;
    sf::Vector2i ballSlopeEq (0.f, 0.f);
        //Other
    bool isPlaying = false;
    bool start = true;
    sf::Clock clock, AIClock;
    const sf::Time AIDelay = sf::seconds(0.f);

    while(window.isOpen()) {
    float ballSpeed = 3.0f + (score.rally/10);

        if (start) {
            do {
                ballSlopeEq.x = rand() % 200;
                ballSlopeEq.y = rand() % 200;
                if (ballSlopeEq.x != 0 && ballSlopeEq.y != 0 && ballSlopeEq.x < 3.5 && ballSlopeEq.x > 2.f
                    && ballSlopeEq.y < 3.5 && ballSlopeEq.y > 2.f)
                        ballSlope = ballSlopeEq.y / ballSlopeEq.x;
            }   while (ballSlope > 1.f && ballSlope < 2.5f);
            ball.setPosition(windowSize.x/2-50, windowSize.y/2);
            winLose.setString("");
            start = false;
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Space) {
                        if (!isPlaying) {
                            isPlaying = true;
                            clock.restart();
                        }
                    }
                    else if (event.key.code == sf::Keyboard::P)
                        isPlaying = false;
                    else if (event.key.code == sf::Keyboard::R)
                        start = true;
            }
        }

        //move objects
        if (isPlaying) {
            float deltaTime = clock.restart().asSeconds();

            //Move paddles
                //left Paddle
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                if (leftPaddle.getPosition().y-paddleSize.y/2 > topBounds.getPosition().y+10) {
                    leftPaddle.move(0.f, -paddleSpeed * deltaTime);
                } else
                    if (bump.getStatus() == sf::Sound::Stopped)
                        bump.play();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                if (leftPaddle.getPosition().y+paddleSize.y/2 < botBounds.getPosition().y) {
                    leftPaddle.move(0.f, paddleSpeed * deltaTime);
                } else
                    if (bump.getStatus() == sf::Sound::Stopped)
                        bump.play();
            }
                //Right Paddle
            if (AIClock.getElapsedTime() > AIDelay) {
                AIClock.restart();
                 if (ball.getPosition().y > rightPaddle.getPosition().y+paddleSize.y/2-5
                    && rightPaddle.getPosition().y+paddleSize.y/2+5 < botBounds.getPosition().y)
                        rightPaddleSpeed = paddleSpeed;
                else if (ball.getPosition().y < rightPaddle.getPosition().y-paddleSize.y/2
                         && rightPaddle.getPosition().y-paddleSize.y/2 > topBounds.getPosition().y)
                            rightPaddleSpeed = (-paddleSpeed);
                else
                    rightPaddleSpeed = 0;
            }

            //move Right paddle if ball on right half of screen
            if ( (ball.getPosition().x >= windowSize.x/2
                  && (ball.getPosition().x > rightPaddle.getPosition().x-(paddleSize.x/2)-80 || ballSlopeEq.x > 0)) )
                rightPaddle.move(0.f, rightPaddleSpeed * deltaTime);

            float factor (deltaTime * ballSpeed);
            ball.move(ballSlopeEq.x * factor, ballSlopeEq.y * factor);

            //check ball collision
            if (ball.getPosition().y - ballSize.y /2 <= topBounds.getPosition().y+topBounds.getGlobalBounds().height) {
                if (bump.getStatus() == sf::Sound::Stopped)
                        bump.play();
                ballSlopeEq.y *= -1;
                ballSlope *= -1;
                ball.setPosition(ball.getPosition().x, (topBounds.getPosition().y+0.1f+topBounds.getGlobalBounds().height+ballSize.y/2));
            }
            else if (ball.getPosition().y + ballSize.y /2 >= botBounds.getPosition().y) {
                if (bump.getStatus() == sf::Sound::Stopped)
                        bump.play();
                ballSlopeEq.y *= -1;
                ballSlope *= -1;
                ball.setPosition(ball.getPosition().x, botBounds.getPosition().y-0.1f-ballSize.y/2);
            } //Right Paddle
            else if (ball.getPosition().x + ballSize.x/2 >= rightPaddle.getPosition().x - paddleSize.x/2
                        && ball.getPosition().y+ballSize.y/2 >= rightPaddle.getPosition().y - paddleSize.y/2
                        && ball.getPosition().y-ballSize.y/2 <= rightPaddle.getPosition().y + paddleSize.y/2) {
                if (bing.getStatus() == sf::Sound::Stopped)
                        bing.play();
                score.rally++;
                ballSlopeEq.x *= -1;
                ballSlope *= -1;
            } //Left Paddle
            else if (ball.getPosition().x - ballSize.x/2 <= leftPaddle.getPosition().x + paddleSize.x/2
                        && ball.getPosition().y+ballSize.y/2 >= leftPaddle.getPosition().y - paddleSize.y/2
                        && ball.getPosition().y-ballSize.y/2 <= leftPaddle.getPosition().y + paddleSize.y/2) {
                if (bing.getStatus() == sf::Sound::Stopped)
                        bing.play();
                std::cout << "Ball hit left paddle";
                score.rally++;
                ballSlopeEq.x *= -1;
                ballSlope *= -1;
            }

            //Check win
                //if right paddle point
            if (ball.getPosition().x < leftPaddle.getPosition().x) {
                std::cout << "Lose";
                winLose.setString("Loser!");
                ball.setPosition(windowSize.x/2-50, windowSize.y/2);
                ballSlopeEq.x *= -1;
                ++score.left; score.rally = 0;     //rest rally and track score
                isPlaying = false;
            }   //if left paddle point
            else if (ball.getPosition().x > rightPaddle.getPosition().x + paddleSize.x/2) {
                std::cout << "Win";
                winLose.setString("Meh :l");
                ball.setPosition(windowSize.x/2-50, windowSize.y/2);
                ++score.right; score.rally = 0;     //rest rally and track score
                isPlaying = false;
            }
        }

        window.clear();
        window.draw(bckGrnd);
        window.draw(topBounds);
        window.draw(botBounds);
        window.draw(leftBounds);
        window.draw(rightBounds);
        window.draw(score.imageToDraw(font, textColor, windowSize.x));
        window.draw(ball);
        window.draw(rightPaddle);
        window.draw(leftPaddle);
        if (!isPlaying) {
            window.draw(pauseText);
            window.draw(winLose);
        }
        window.display();
    }
}
