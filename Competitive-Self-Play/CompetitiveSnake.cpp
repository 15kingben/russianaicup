#include <cmath>
#include <ctime>
#include <vector>
#include <thread>
#include <random>
#include <cstdlib>
#include <iostream>

// #define PIPEWITHGUI
// #define GUI
#ifdef GUI
#include <SFML/Graphics.hpp>
#define W 1000
#endif

#define FOOD_MIN 0.25
#define FOOD_MAX 0.75
#define SNAKE_MIN 0.15
#define SNAKE_MAX 0.85

#define FRAME_RATE 100
#define BLOB_SIZE 0.01
#define FOOD_SIZE 0.01
#define INITIAL_LENGTH 10
#define BLOB_DISTANCE 0.01
#define MAX_SNAKE_LENGTH 10
#define MAX_ACCELERATION 0.0003

#define REWARD 1000.0
#define FOOD_REWARD 500.0
#define TIMEOUT_REWARD 100.0

#define DEFAULT_SPEED 0.003
#define MAX_IDLE_STEPS 1000
#define FOOD_TIMER_STEPS 300

#define FOOD_COLOR sf::Color(248, 249, 145)
#define SNAKE1_COLOR sf::Color(100, 200, 100)
#define SNAKE2_COLOR sf::Color(200, 100, 100)
#define BACKGROUND_COLOR sf::Color(30, 29, 53)
#define TRANSPARENT_FOOD_COLOR sf::Color(248, 249, 145, 100)

class Point
{
public:
    double x, y;
};

double magnitude(Point a)
{
    return sqrt(a.x*a.x + a.y*a.y);
}

double unitRandom()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(mt);
}

double uniformRandom(double min, double max)
{
    return min + unitRandom() * (max - min);
}

double timeInSeconds()
{
    return std::clock() / (double)CLOCKS_PER_SEC;
}

class Food
{
public:
    int timer;
    Point position;

    void reset()
    {
        this->timer = FOOD_TIMER_STEPS;
        this->position = Point{
            uniformRandom(FOOD_MIN, FOOD_MAX),
            uniformRandom(FOOD_MIN, FOOD_MAX)
        };
    }

    bool available()
    {
        return this->timer == 0;
    }

    void step()
    {
        if (this->timer != 0)
            this->timer -= 1;
    }

#ifdef GUI
    void render(sf::RenderWindow& window)
    {
        sf::CircleShape blob((int)(W * FOOD_SIZE));
        blob.setPosition(
          (int)(W * this->position.x),
          (int)(W * this->position.y)
        );
        if (this->available())
            blob.setFillColor(FOOD_COLOR);
        else
            blob.setFillColor(TRANSPARENT_FOOD_COLOR);
        window.draw(blob);
    }
#endif
};

class Snake
{
public:
    Point velocity;
    std::vector<Point> blobs;

    void reset()
    {
        this->velocity = Point{DEFAULT_SPEED, 0};
        double initialX = uniformRandom(SNAKE_MIN, SNAKE_MAX);
        double initialY = uniformRandom(SNAKE_MIN, SNAKE_MAX);
        this->blobs.clear();
        for (int i = 0; i < INITIAL_LENGTH; i++)
        {
            this->blobs.push_back(
                Point{
                    initialX,
                    initialY + i * BLOB_DISTANCE
                }
            );
        }
    }

    void addBlob()
    {
        int length = this->blobs.size();
        double dx = this->blobs[length-1].x - this->blobs[length-2].x;
        double dy = this->blobs[length-1].y - this->blobs[length-2].y;
        this->blobs.push_back(
            Point {
                this->blobs[length-1].x + dx,
                this->blobs[length-1].y + dy
            }
        );
    }

    bool isHeadOutside()
    {
        return this->blobs[0].x < 0 || this->blobs[0].x + 2 * BLOB_SIZE > 1 ||
               this->blobs[0].y < 0 || this->blobs[0].y + 2 * BLOB_SIZE > 1;
    }

    bool isHeadCollidingWithBody()
    {
        for (int i = 3; i < this->blobs.size(); i++)
        {
            double dx = this->blobs[i].x - this->blobs[0].x;
            double dy = this->blobs[i].y - this->blobs[0].y;
            if (magnitude(Point{dx, dy}) < 2 * BLOB_SIZE)
            {
                return true;
            }
        }
        return false;
    }

    void step(float angle)
    {
        this->velocity.x += MAX_ACCELERATION * cos(angle);
        this->velocity.y += MAX_ACCELERATION * sin(angle);

        double speed = magnitude(this->velocity);
        this->velocity.x /= speed / DEFAULT_SPEED;
        this->velocity.y /= speed / DEFAULT_SPEED;

        this->blobs[0].x += this->velocity.x;
        this->blobs[0].y += this->velocity.y;

        for (int i = 1; i < this->blobs.size(); i++)
        {
            double dx = this->blobs[i-1].x - this->blobs[i].x;
            double dy = this->blobs[i-1].y - this->blobs[i].y;

            double distance = sqrt(dx*dx + dy*dy);
            double ratio = BLOB_DISTANCE / distance;

            this->blobs[i].x += (1 - ratio) * dx;
            this->blobs[i].y += (1 - ratio) * dy;
        }
    }

#ifdef GUI
    void render(sf::RenderWindow& window, sf::Color color)
    {
        for (int i = 0; i < this->blobs.size(); i++)
        {
            sf::CircleShape blob((int)(W * BLOB_SIZE));
            blob.setPosition(
                (int)(W * this->blobs[i].x),
                (int)(W * this->blobs[i].y)
            );
            blob.setFillColor(color);
            window.draw(blob);
        }
    }
#endif
};

class SnakeGame
{
public:
    Food food;
    Snake snake1;
    Snake snake2;
    int numIdleSteps;

    void reset()
    {
        this->food.reset();
        this->snake1.reset();
        this->snake2.reset();
        this->numIdleSteps = 0;
    }

    float getHeadAngle(Point action)
    {
        double dx = action.x - this->snake1.blobs[0].x;
        double dy = action.y - this->snake1.blobs[0].y;
        return atan2(dy, dx);
    }

    static bool doesCollide(Point& head, Snake& snake)
    {
        for (int i = 0; i < snake.blobs.size(); i++)
        {
            double dx = head.x - snake.blobs[i].x;
            double dy = head.y - snake.blobs[i].y;
            if (magnitude(Point{dx, dy}) < 2 * BLOB_SIZE)
            {
                return true;
            }
        }
        return false;
    }

    std::vector<float> state(float reward1, float reward2)
    {
        std::vector<float> state1(4 * MAX_SNAKE_LENGTH + 2 + 1 + 1, 0.0);
        std::vector<float> state2(4 * MAX_SNAKE_LENGTH + 2 + 1 + 1, 0.0);

        for (int i = 0; i < this->snake1.blobs.size(); i++)
        {
            state1[2*i  ] = this->snake1.blobs[i].x;
            state1[2*i+1] = this->snake1.blobs[i].y;

            state2[2 * MAX_SNAKE_LENGTH + 2*i    ] = this->snake1.blobs[i].x;
            state2[2 * MAX_SNAKE_LENGTH + 2*i + 1] = this->snake1.blobs[i].y;
        }

        for (int i = 0; i < this->snake2.blobs.size(); i++)
        {
            state2[2*i  ] = this->snake2.blobs[i].x;
            state2[2*i+1] = this->snake2.blobs[i].y;

            state1[2 * MAX_SNAKE_LENGTH + 2*i    ] = this->snake2.blobs[i].x;
            state1[2 * MAX_SNAKE_LENGTH + 2*i + 1] = this->snake2.blobs[i].y;
        }

        state1[4 * MAX_SNAKE_LENGTH    ] = this->food.position.x;
        state2[4 * MAX_SNAKE_LENGTH    ] = this->food.position.x;
        state1[4 * MAX_SNAKE_LENGTH + 1] = this->food.position.y;
        state2[4 * MAX_SNAKE_LENGTH + 1] = this->food.position.y;
        state1[4 * MAX_SNAKE_LENGTH + 2] = this->numIdleSteps / (float)MAX_IDLE_STEPS;
        state2[4 * MAX_SNAKE_LENGTH + 2] = this->numIdleSteps / (float)MAX_IDLE_STEPS;
        state1[4 * MAX_SNAKE_LENGTH + 3] = reward1;
        state2[4 * MAX_SNAKE_LENGTH + 3] = reward2;

        std::vector<float> state(state1.size() + state2.size());

        for (int i = 0; i < state1.size(); i++)
            state[i] = state1[i];

        for (int i = 0; i < state2.size(); i++)
            state[state1.size() + i] = state2[i];

        return state;
    }

    std::vector<float> step(float action1, float action2)
    {
        this->numIdleSteps += 1;
        this->food.step();
        this->snake1.step(action1);
        this->snake2.step(action2);

        bool done = false;
        bool snake1Collided = false;
        bool snake2Collided = false;
        float reward1 = 0;
        float reward2 = 0;

        Point head1 = this->snake1.blobs[0];
        Point head2 = this->snake2.blobs[0];

        snake1Collided = snake1Collided || snake1.isHeadOutside();
        snake1Collided = snake1Collided || snake1.isHeadCollidingWithBody();
        snake1Collided = snake1Collided || SnakeGame::doesCollide(head1, snake2);

        snake2Collided = snake2Collided || snake2.isHeadOutside();
        snake2Collided = snake2Collided || snake2.isHeadCollidingWithBody();
        snake2Collided = snake2Collided || SnakeGame::doesCollide(head2, snake1);

        done = snake1Collided || snake2Collided;

        if (snake1Collided && !snake2Collided)
        {
            reward1 = -REWARD;
            reward2 = +REWARD;
        }

        if (snake2Collided && !snake1Collided)
        {
            reward1 = +REWARD;
            reward2 = -REWARD;
        }

        if (snake1Collided && snake2Collided)
        {
            reward1 = -REWARD;
            reward2 = -REWARD;
        }

        if (!done && this->food.available())
        {
            double dx1 = head1.x - food.position.x;
            double dy1 = head1.y - food.position.y;
            double dx2 = head2.x - food.position.x;
            double dy2 = head2.y - food.position.y;

            bool snake1AteFood = magnitude(Point{dx1, dy1}) < BLOB_SIZE + FOOD_SIZE;
            bool snake2AteFood = magnitude(Point{dx2, dy2}) < BLOB_SIZE + FOOD_SIZE;

            done = snake1AteFood || snake2AteFood;

            if (snake1AteFood && !snake2AteFood)
            {
                reward1 = +FOOD_REWARD;
                reward2 = -FOOD_REWARD;
            }

            if (snake2AteFood && !snake1AteFood)
            {
                reward1 = -FOOD_REWARD;
                reward2 = +FOOD_REWARD;
            }

            if (snake1AteFood && snake2AteFood)
            {
                reward1 = -FOOD_REWARD;
                reward2 = -FOOD_REWARD;
            }
        }

        if (this->numIdleSteps > MAX_IDLE_STEPS)
        {
            reward1 = -TIMEOUT_REWARD;
            reward2 = -TIMEOUT_REWARD;
            done = true;
        }

        if (done) this->reset();

        std::vector<float> state = this->state(reward1, reward2);
        state.push_back((float)done);
        return state;
    }

#ifdef GUI
    void render(sf::RenderWindow& window)
    {
        this->food.render(window);
        this->snake2.render(window, SNAKE2_COLOR);
        this->snake1.render(window, SNAKE1_COLOR);
    }
#endif
};

int main()
{
#ifdef INTERACTIVE
    sf::RenderWindow window(sf::VideoMode(W, W), "Snake");

    float mouseX = 1, mouseY = 1;
    SnakeGame game = SnakeGame();
    game.reset();

    while (window.isOpen())
    {
        double start = timeInSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseMoved)
            {
                mouseX = event.mouseMove.x;
                mouseY = event.mouseMove.y;
            }
        }
        game.step(game.getHeadAngle(Point {mouseX / W, mouseY / W}), uniformRandom(0, 2*M_PI));
        window.clear(BACKGROUND_COLOR);
        game.render(window);
        window.display();

        double timeElapsed = timeInSeconds() - start;
        int timeToSleep = (int)((1.0 / FRAME_RATE - timeElapsed) * 1000);
        if (timeToSleep < 0)
            std::cout << "Frame rate is too high!" << std::endl;
        else
            std::this_thread::sleep_for(std::chrono::milliseconds((timeToSleep)));
    }
#endif
#ifdef PIPEWITHGUI
    sf::RenderWindow window(sf::VideoMode(W, W), "Snake");

    SnakeGame game = SnakeGame();
    game.reset();

    while (window.isOpen())
    {
        double start = timeInSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        float action1;
        float action2;
        fread(&action1, 4, 1, stdin);
        fread(&action2, 4, 1, stdin);
        std::vector<float> state = game.step(action1, action2);
        fwrite(&state[0], 4, state.size(), stdout);
        fflush(stdout);

        window.clear(BACKGROUND_COLOR);
        game.render(window);
        window.display();

        double timeElapsed = timeInSeconds() - start;
        int timeToSleep = (int)((1.0 / FRAME_RATE - timeElapsed) * 1000);
        if (timeToSleep < 0)
            continue;
        else
            std::this_thread::sleep_for(std::chrono::milliseconds((timeToSleep)));
    }
#endif
#ifndef GUI
    SnakeGame game = SnakeGame();
    game.reset();
    while(true)
    {
        float action1;
        float action2;
        fread(&action1, 4, 1, stdin);
        fread(&action2, 4, 1, stdin);
        std::vector<float> state = game.step(action1, action2);
        fwrite(&state[0], 4, state.size(), stdout);
        fflush(stdout);
    }
#endif
    return 0;
}
