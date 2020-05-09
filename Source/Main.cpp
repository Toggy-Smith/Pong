// SECTION: Headers
#include "SFML/Graphics.hpp"

#include <string>

// SECTION: Global Variables
	// Paddles
	sf::RectangleShape leftPaddle, rightPaddle;

	int leftPaddleScore, rightPaddleScore;

	// Ball
	sf::RectangleShape ball;

	sf::Vector2f ballVelocity;

	float ballHorizontalSpeed = 800.0f;

	// Text
	sf::Font font;

	sf::Text scoreText;
	sf::Text pressEnterText;
	sf::Text pressBackspaceText;

	// View
	sf::Clock viewShakeClock;
	bool isViewShaking = false;

	// Game
	bool gameStarted = false;

// SECTION: Functions
void initialiseText(sf::RenderWindow& window)
{
	font.loadFromFile("assets/FredokaOne-Regular.ttf");

	scoreText.setFont(font);
	scoreText.setCharacterSize(50);

	pressEnterText.setFont(font);
	pressEnterText.setCharacterSize(30);
	pressEnterText.setString("PRESS ENTER TO START");
	pressEnterText.setPosition(sf::Vector2f(window.getSize().x / 2 + pressEnterText.getLocalBounds().left - pressEnterText.getLocalBounds().width / 2, window.getSize().y * 0.45 + pressEnterText.getLocalBounds().top - pressEnterText.getLocalBounds().height / 2));

	pressBackspaceText.setFont(font);
	pressBackspaceText.setCharacterSize(30);
	pressBackspaceText.setString("PRESS BACKSPACE TO RESTART");
	pressBackspaceText.setPosition(sf::Vector2f(window.getSize().x / 2 + pressBackspaceText.getLocalBounds().left - pressBackspaceText.getLocalBounds().width / 2, window.getSize().y - (window.getSize().y * 0.45) + pressBackspaceText.getLocalBounds().top - pressBackspaceText.getLocalBounds().height / 2));
}

void initialisePaddles(sf::RenderWindow& window)
{
	const sf::Color color = sf::Color::White;
	const sf::Vector2f size = sf::Vector2f(10, 100);
	const int positionOffset = 32;

	leftPaddle.setFillColor(color);
	rightPaddle.setFillColor(color);

	leftPaddle.setSize(size);
	rightPaddle.setSize(size);

	leftPaddle.setPosition(sf::Vector2f(positionOffset, window.getSize().y / 2 - size.y / 2));
	rightPaddle.setPosition(sf::Vector2f(window.getSize().x - size.x - positionOffset, window.getSize().y / 2 - size.y / 2));
}

void initialiseBall(sf::RenderWindow& window)
{
	const int size = 10;

	ball.setFillColor(sf::Color::Blue);
	ball.setSize(sf::Vector2f(size, size));
	ball.setPosition(sf::Vector2f(window.getSize().x / 2 - size / 2, window.getSize().y / 2 - size / 2));

	ballVelocity.x = ballHorizontalSpeed;
	ballVelocity.y = -100.f;
}

void initialise(sf::RenderWindow& window)
{
	initialiseText(window);

	initialisePaddles(window);

	initialiseBall(window);
}

void moveEnemyPaddle(sf::RenderWindow& window, const float elapsedTime)
{
	// Move paddle toward ball
	const float paddleSpeed = 500.0f;

	bool isBallAbovePaddle = ball.getPosition().y + ball.getSize().y / 2 > leftPaddle.getPosition().y + leftPaddle.getSize().y / 2 ? true : false;

	if (isBallAbovePaddle)
	{
		leftPaddle.move(0, paddleSpeed * elapsedTime);
	}
	else
	{
		leftPaddle.move(0, -paddleSpeed * elapsedTime);
	}


	// Ensure paddle remains on screen
	bool isPaddleOffScreenTop = leftPaddle.getPosition().y < 0;
	bool isPaddleOffScreenBottom = leftPaddle.getPosition().y + leftPaddle.getSize().y > window.getSize().y;

	if (isPaddleOffScreenTop)
	{
		leftPaddle.setPosition(sf::Vector2f(leftPaddle.getPosition().x, 0));
	}
	else if (isPaddleOffScreenBottom)
	{
		leftPaddle.setPosition(sf::Vector2f(leftPaddle.getPosition().x, window.getSize().y - leftPaddle.getSize().y));
	}
}

void movePlayerPaddle(sf::RenderWindow& window, const float elapsedTime)
{
	// Let player move paddle
	const float paddleSpeed = 1000.0f;

	bool isUpKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	bool isDownKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);

	if (isUpKeyPressed)
	{
		rightPaddle.move(0, -paddleSpeed * elapsedTime);
	}

	if (isDownKeyPressed)
	{
		rightPaddle.move(0, paddleSpeed * elapsedTime);
	}


	// Ensure paddle remains on screen
	bool isPaddleOffScreenTop = rightPaddle.getPosition().y < 0;
	bool isPaddleOffScreenBottom = rightPaddle.getPosition().y + rightPaddle.getSize().y > window.getSize().y;

	if (isPaddleOffScreenTop)
	{
		rightPaddle.setPosition(sf::Vector2f(rightPaddle.getPosition().x, 0));
	}
	else if (isPaddleOffScreenBottom)
	{
		rightPaddle.setPosition(sf::Vector2f(rightPaddle.getPosition().x, window.getSize().y - rightPaddle.getSize().y));
	}
}

void ensureBallRemainsOnScreen(sf::RenderWindow& window)
{
	bool isBallOffScreenTop = ball.getPosition().y < 0;
	bool isBallOffScreenBottom = ball.getPosition().y + ball.getSize().y > window.getSize().y;

	if (isBallOffScreenTop || isBallOffScreenBottom)
	{
		ballVelocity.y = -ballVelocity.y;
	}
}

void detectBallCollisionWithPaddle(sf::RenderWindow& window)
{
	bool isBallTouchingLeftPaddle = ball.getGlobalBounds().intersects(leftPaddle.getGlobalBounds());
	bool isBallTouchingRightPaddle = ball.getGlobalBounds().intersects(rightPaddle.getGlobalBounds());

	if (isBallTouchingLeftPaddle || isBallTouchingRightPaddle)
	{
		float ballPaddleVerticalOffset;

		bool isBallAbovePaddle;

		if (isBallTouchingLeftPaddle)
		{
			// CLARIFY: Makes the ball move right regardless of starting direction
			ballVelocity.x = ballVelocity.x >= 0 ? ballVelocity.x : -ballVelocity.x;

			ballPaddleVerticalOffset = (ball.getPosition().y + ball.getSize().y / 2) - (leftPaddle.getPosition().y - leftPaddle.getSize().y / 2);

			isBallAbovePaddle = ball.getPosition().y + ball.getSize().y / 2 > leftPaddle.getPosition().y + leftPaddle.getSize().y / 2 ? true : false;

			leftPaddle.setFillColor(sf::Color::Red);
			rightPaddle.setFillColor(sf::Color::White);
		}
		else if (isBallTouchingRightPaddle)
		{
			// CLARIFY: Makes the ball move left regardless of starting direction
			ballVelocity.x = ballVelocity.x >= 0 ? -ballVelocity.x : ballVelocity.x;

			ballPaddleVerticalOffset = (ball.getPosition().y + ball.getSize().y / 2) - (rightPaddle.getPosition().y - rightPaddle.getSize().y / 2);

			isBallAbovePaddle = ball.getPosition().y + ball.getSize().y / 2 > rightPaddle.getPosition().y + rightPaddle.getSize().y / 2 ? true : false;

			leftPaddle.setFillColor(sf::Color::White);
			rightPaddle.setFillColor(sf::Color::Green);
		}

		const float someConstant = 5.0f;

		if (isBallAbovePaddle)
		{
			ballVelocity.y = ballPaddleVerticalOffset * someConstant;
		}
		else
		{
			ballVelocity.y = -(ballPaddleVerticalOffset * someConstant);
		}
	}
}

void detectBallGoal(sf::RenderWindow& window)
{
	bool isBallOffScreenLeft = ball.getPosition().x < 0;
	bool isBallOffScreenRight = ball.getPosition().x + ball.getSize().x > window.getSize().x;

	if (isBallOffScreenLeft)
	{
		rightPaddleScore++;

		ballVelocity.x = -ballVelocity.x;
		ballVelocity.y = 0;

		ball.setPosition(sf::Vector2f(leftPaddle.getPosition().x + leftPaddle.getSize().x, leftPaddle.getPosition().y + leftPaddle.getSize().y / 2 - ball.getSize().y / 2));

		viewShakeClock.restart();

		isViewShaking = true;
	}
	else if (isBallOffScreenRight)
	{
		leftPaddleScore++;

		ballVelocity.x = -ballVelocity.x;
		ballVelocity.y = 0;

		ball.setPosition(sf::Vector2f(rightPaddle.getPosition().x - ball.getSize().x, rightPaddle.getPosition().y + rightPaddle.getSize().y / 2 - ball.getSize().y / 2));

		viewShakeClock.restart();

		isViewShaking = true;
	}
}

void moveBall(sf::RenderWindow& window, const float elapsedTime)
{
	ball.move(ballVelocity * elapsedTime);

	ensureBallRemainsOnScreen(window);
	detectBallCollisionWithPaddle(window);
	detectBallGoal(window);
}

void updateScoreText(sf::RenderWindow& window)
{
	const int positionOffset = 32;

	scoreText.setString(std::to_string(leftPaddleScore) + " : " + std::to_string(rightPaddleScore));
	scoreText.setPosition(sf::Vector2f(window.getSize().x / 2 + scoreText.getLocalBounds().left - scoreText.getLocalBounds().width / 2, positionOffset));
}

void restartGame(sf::RenderWindow& window)
{
	leftPaddleScore = 0;
	rightPaddleScore = 0;

	initialisePaddles(window);
	initialiseBall(window);
}

void shakeView(sf::RenderWindow& window)
{
	if (!isViewShaking)
	{
		return;
	}

	const int shakeOffset = 5;

	sf::View view;

	if (viewShakeClock.getElapsedTime().asMilliseconds() % 4 == 0)
	{
		view = sf::View(sf::FloatRect(-shakeOffset, 0, window.getSize().x, window.getSize().y));
	}
	else if (viewShakeClock.getElapsedTime().asMilliseconds() % 4 == 1)
	{
		view = sf::View(sf::FloatRect(shakeOffset, 0, window.getSize().x, window.getSize().y));
	}
	else if (viewShakeClock.getElapsedTime().asMilliseconds() % 4 == 2)
	{
		view = sf::View(sf::FloatRect(0, -shakeOffset, window.getSize().x, window.getSize().y));
	}
	else if (viewShakeClock.getElapsedTime().asMilliseconds() % 4 == 3)
	{
		view = sf::View(sf::FloatRect(0, shakeOffset, window.getSize().x, window.getSize().y));
	}

	window.setView(view);

	if (viewShakeClock.getElapsedTime().asSeconds() >= 0.25)
	{
		isViewShaking = false;
	}
}

void update(sf::RenderWindow& window)
{
	static sf::Clock updateClock;

	float elapsedTime = updateClock.getElapsedTime().asSeconds();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
	{
		gameStarted = true;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace))
	{
		restartGame(window);

		gameStarted = false;
	}

	if (gameStarted)
	{
		shakeView(window);

		movePlayerPaddle(window, elapsedTime);

		moveEnemyPaddle(window, elapsedTime);

		moveBall(window, elapsedTime);

		updateScoreText(window);
	}

	updateClock.restart();
}

void render(sf::RenderWindow& window)
{
	if (gameStarted)
	{
		window.draw(leftPaddle);
		window.draw(rightPaddle);

		window.draw(ball);

		window.draw(scoreText);
	}
	else
	{
		window.draw(pressEnterText);
		window.draw(pressBackspaceText);
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Pong", sf::Style::Close);

	initialise(window);

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear();

		update(window);

		render(window);

		window.display();
	}

	return 0;
}