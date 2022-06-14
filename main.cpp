#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <thread>

using namespace std;

#define COL 24 //column equal height.
#define ROW 32 //row equal width.
#define DEFAULT_LENGTH 30 // len the snake
#define WALL '#' // wall symbol
#define SNAKE_HEAD 'S' // snake head symbol
#define SNAKE_BODY 's' // snake body symbol
#define MEDICINE 'M'// medicine symbol
#define FOOD 'F' // food symbol
#define GOAL 1 // (length of snake)'s goal
#define EAT_FOOD_TO_OVER_TIMES 3 // eat more than 3 times to go to die

#define GOALDONE { \
"OH MY GOD! ARE YOU GOD?", \
"That so Crazy!",  \
"Now your weight is (kg)", \
to_string(snake.getSnakeLength() * 50) \
}

#define EATEN_TOO_MORE_FOOD { \
"HAHA! GAME OVER!",           \
"YOU ARE A LOSER!",           \
"YOU EATEN FOOD MORE THAN",   \
to_string(EAT_FOOD_TO_OVER_TIMES), \
"times" \
}

#define GAME_OVER {"Game OVER!" , \
"YOU SUCK!",                      \
"Weight: (kg)",                   \
to_string(snake.getSnakeLength() * 50) \
}

enum DIRECTION {
    UP,
    DOWN,
    RIGHT,
    LEFT,
    STOP,
};

enum LEVEL {
    MONKEY,
    NORMAL,
    HARD,
    HOLYSHIT,
};

enum FOOD_TYPE {
    NORMAL_FOOD,
    MEDICINE_,
};

class Food {
private:
    COORD foodPosition;
protected:
    FOOD_TYPE foodType;
public:
    Food() {
        foodType = FOOD_TYPE::NORMAL_FOOD;
        spawnFood();
    }

    void spawnFood() {
        foodPosition = {0,0};
        while (foodPosition.X <= 1 || foodPosition.X >= COL) {
            foodPosition.X = (rand() % COL);
        }

        while (foodPosition.Y <= 1 || foodPosition.Y >= ROW) {
            foodPosition.Y = (rand() % ROW);
        }
    }

    COORD getFoodPosition() {
        return this->foodPosition;
    }

    FOOD_TYPE getFoodType() {
        return foodType;
    }
};

class Medicine : public Food {
public:
    Medicine() {
        foodType = FOOD_TYPE::MEDICINE_;
    }
};

class Snake {
private:
    COORD position;
    int moveValue;
    DIRECTION direction;
    int snakeLength;
    vector<COORD> bodyPosition;
public:
    Snake() {
        this->position = {COL / 2, ROW /2};
        this->moveValue = 1;
        this->direction = DIRECTION::STOP;
        this->snakeLength = DEFAULT_LENGTH;
        this->bodyPosition.push_back(this->position);
    }

    void setDirection(DIRECTION direction) {
        this->direction = direction;
    }

    void growReverse() {
        this->snakeLength--;
    }

    void grow() {
        this->snakeLength++;
    }

    COORD getPosition() {
        return this->position;
    }

    int getSnakeLength() {
        return  this->snakeLength;
    }

    vector<COORD> getBodyPosition() {
        return this->bodyPosition;
    }

    void move() {
        switch (this->direction) {
            case DIRECTION::UP:
                this->position.X -= this->moveValue;
                break;
            case DIRECTION::DOWN:
                this->position.X += this->moveValue;
                break;
            case DIRECTION::LEFT:
                this->position.Y -= this->moveValue;
                break;
            case DIRECTION::RIGHT:
                this->position.Y += this->moveValue;
                break;
        }

        this->bodyPosition.push_back(this->position);

        if (this->bodyPosition.size() > this->snakeLength) {
            this->bodyPosition.erase(this->bodyPosition.begin());
        }
    }

    bool collided() {
        if (this->getPosition().X >= COL ||
            this->getPosition().X <= 1 ||
            this->getPosition().Y >= ROW ||
            this->getPosition().Y <= 0) return true;
        else {
            return false;
        }
    }

    bool eaten(COORD foodPosition) {
        if(this->position.X == foodPosition.X && this->position.Y == foodPosition.Y) {
            return true;
        } else {
            return false;
        }
    }
};

class Frame {
public:
    Frame(vector<string> msg, int times = 3000) {
        clear(times);
        for (string message : msg) {
            cout << message << endl;
        }
    }

    Frame(Snake snake, vector<Food> food) {
        clear();
        print(snake, food);
    }

    void print(Snake snake, vector<Food> food) {
        for (int c = 0; c < COL; c++) {
            cout << "#";
            for (int r = 0; r < ROW; r++) {
                bool  isPrint = false;

                for (int i=0; i < food.size(); i++) {
                    if (r == food[i].getFoodPosition().Y && c+1 == food[i].getFoodPosition().X) {
                        if (food[i].getFoodType() == FOOD_TYPE::NORMAL_FOOD) {
                            cout << FOOD;
                        } else if (food[i].getFoodType() == FOOD_TYPE::MEDICINE_) {
                            cout << MEDICINE;
                        }
                        isPrint = true;
                    }
                }

                if (isPrint) continue;

                if (c == 0 || c == (COL - 1)) {
                    cout << WALL;
                } else if (r == snake.getPosition().Y && c+1 == snake.getPosition().X) {
                    cout << SNAKE_HEAD;
                } else {
                    bool  isBody = false;
                    for (int i=0; i < snake.getBodyPosition().size() - 1; i++) {
                        if (r == snake.getBodyPosition()[i].Y && c+1 == snake.getBodyPosition()[i].X) {
                            cout << SNAKE_BODY;
                            isBody = true;
                            break;
                        }
                    }

                    if (!isBody){
                        cout << " ";
                    }
                }
            }
            cout << "#\n";
        }
        cout << "Weight: " << snake.getSnakeLength() * 50 << "kg \n";
    }

    void goTo(short x = 0, short y = 0) {
        COORD coord = {x,y};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void clear(int times = 100) {
        goTo();
        for (int i=0; i < times; i++) {
            cout << " ";
        }
        goTo();
    }
};

int getLevel(LEVEL level) {
    switch (level) {
        case LEVEL::MONKEY:
            return 100;
        case LEVEL::NORMAL:
            return 40;
        case LEVEL::HARD:
            return 10;
        case LEVEL::HOLYSHIT:
            return 0;
    }
}

void stopGame(vector<string> reason) {
    (Frame(reason));
    Sleep(2000);
    system("pause");
}

void startGame(LEVEL level = LEVEL::HOLYSHIT) {
    int speed = getLevel(level);
    bool isGameOver = false;
    int eatFoodTimes = 0;

    Snake snake;
    vector<Food> food;
    food.push_back(Food());
    food.push_back(Medicine());

    while (!isGameOver) {
        Sleep(speed);
        Frame(snake, food);

        if (kbhit()) {
            switch (getch()) {
                case 'w':
                    snake.setDirection(DIRECTION::UP);
                    break;
                case 'a':
                    snake.setDirection(DIRECTION::LEFT);
                    break;
                case 's':
                    snake.setDirection(DIRECTION::DOWN);
                    break;
                case 'd':
                    snake.setDirection(DIRECTION::RIGHT);
                    break;
            }
        }

        if (snake.collided()) {
            isGameOver = true;
        }

        for (int i=0; i < food.size(); i++) {
            if (snake.eaten(food[i].getFoodPosition())) {
                if (i == 0) {
                    food[i].spawnFood();
                    snake.grow();
                    eatFoodTimes++;
                } else {
                    food[i].spawnFood();
                    snake.growReverse();
                }
            }
        }

        if (snake.getSnakeLength() <= GOAL) {
            stopGame(GOALDONE);
            isGameOver = true;
            return;
        }

        if (eatFoodTimes >= EAT_FOOD_TO_OVER_TIMES) {
            stopGame(EATEN_TOO_MORE_FOOD);
            isGameOver = true;
            return;
        }

        snake.move();
    }

    stopGame(GAME_OVER);
}

int main() {
    cout << "enter difficulty level. (0)MONKEY (1)NORMAL (2)HARD (3)HOLYSHIT: ";
    int d;
    for (;;) {
        if (cin.peek() == '\n' || cin >> d) {
            break;
        }
    }

    LEVEL level;
    switch (d) {
        case 0:
            level = LEVEL::MONKEY;
            break;
        case 1:
            level = LEVEL::NORMAL;
            break;
        case 2:
            level = LEVEL::HARD;
            break;
        case 3:
            level = LEVEL::HOLYSHIT;
            break;
        default:
            level = LEVEL::HOLYSHIT;
            break;
    }

    srand(time(0));
    startGame();
}