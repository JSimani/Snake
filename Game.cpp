#include <iostream>
#include <cstdlib>
#include "Game.h"
#include "termfuncs.h"
#include <unistd.h>
using namespace std;

#define UP 'w' 
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'

#define NORMAL "\033[0m"
#define BOLD "\033[1m"

#define RED_TEXT "\033[31m"
#define GREEN_TEXT "\033[32m"
#define YELLOW_TEXT "\033[33m"
#define BLUE_TEXT "\033[34m"
#define MAGENTA_TEXT "\033[35m"
#define CYAN_TEXT "\033[36m"
#define WHITE_TEXT "\033[37m"

#define RED_BACKGROUND "\033[41m"
#define GREEN_BACKGROUND "\033[42m"
#define YELLOW_BACKGROUND "\033[43m"
#define BLUE_BACKGROUND "\033[44m"
#define MAGENTA_BACKGROUND "\033[45m"
#define CYAN_BACKGROUND "\033[46m"
#define WHITE_BACKGROUND "\033[47m"

typedef enum Space {
        HEAD = 0, BODY_FROM_UP, BODY_FROM_RIGHT, BODY_FROM_DOWN, 
        BODY_FROM_LEFT, EMPTY, FOOD
} Space;

Game::Game()
{
        srand(time(NULL));
        y_dimension = 0;
        x_dimension = 0;
        y_head = 0;
        x_head = 0;
        snake_size = 1;
        direction = UP;
        speed = 50;
        game_over = false; 
        won = false;
        board = NULL;
}

Game::Game(int y_dimen, int x_dimen)
{
        srand(time(NULL));
        y_dimension = y_dimen;
        x_dimension = x_dimen;
        y_head = y_dimension / 2;
        x_head = x_dimension / 2;
        snake_size = 1;
        direction = UP;
        speed = 50;
        game_over = false;
        won = false;

        if (y_dimension < 2 || x_dimension < 2) {
                cerr << "Invalid Dimensions. Please choose dimensions "
                     << "of size 2 or greater.\n";
                exit(EXIT_FAILURE);
        }

        board = new int*[y_dimension];
        for (int i = 0; i < y_dimension; i++) {
                board[i] = new int[x_dimension];
        }

        for (int i = 0; i < y_dimension; i++) {
                for (int j = 0; j < x_dimension; j++) {
                        board[i][j] = EMPTY;
                }
        }

        board[y_head][x_head] = HEAD;
}

Game::Game(const Game &source)
{
        srand(time(NULL));
        y_dimension = source.y_dimension;
        x_dimension = source.x_dimension;
        y_head = source.y_head;
        x_head = source.x_head;
        snake_size = source.snake_size;
        direction = source.direction;
        speed = source.speed;
        game_over = source.game_over;
        won = source.won;

        board = new int*[y_dimension];
        for (int i = 0; i < y_dimension; i++) {
                board[i] = new int[x_dimension];

                for (int j = 0; j < x_dimension; j++) {
                        board[i][j] = source.board[i][j];
                }
        }
}

Game &Game::operator=(const Game &source)
{
        if (this == &source) {
                return *this;
        }


        if (this->board != NULL) {
                for (int i = 0; i < this->y_dimension; i++) {
                        if (this->board[i] != NULL) {
                                delete[] this->board[i];
                        }
                }

                delete[] this->board;
        }

        this->y_dimension = source.y_dimension;
        this->x_dimension = source.x_dimension;
        this->y_head = source.y_head;
        this->x_head = source.x_head;
        this->snake_size = source.snake_size;
        this->direction = source.direction;
        this->speed = source.speed;
        this->game_over = source.game_over;
        this->won = source.won;

        this->board = new int*[this->y_dimension];
        for (int i = 0; i < this->y_dimension; i++) {
                this->board[i] = new int[x_dimension];

                for (int j = 0; j < this->x_dimension; j++) {
                        this->board[i][j] = source.board[i][j];
                }
        }

        return *this;
}

Game::~Game()
{
        if (board != NULL) {
                for (int i = 0; i < y_dimension; i++) {
                        if (board[i] != NULL) {
                                delete[] board[i];
                        }
                }

                delete[] board;
        }
        show_cursor();
}

void Game::run()
{
        hide_cursor();
        bake_food();
        print();
        cout << "Enter \'w\', \'a\', \'s\', or \'d\' to start!" << endl;

        do {
                direction = getachar();
        } while (direction != UP && direction != DOWN && direction != LEFT &&
                 direction != RIGHT);

        move();
        print();
        while (!game_over) {
                get_move();
                move();
                print();
        }

        end_game();
        
}

void Game::get_move()
{
        char temp;
        int wait = speed;
        int opposite_direction = DOWN;

        switch (direction) {
                case UP:
                        opposite_direction = DOWN;
                        break;
                case DOWN: 
                        opposite_direction = UP;
                        break;
                case LEFT:
                        opposite_direction = RIGHT;
                        break;
                case RIGHT:
                        opposite_direction = LEFT;
                        break;
                default:
                        break;
        }

        while (wait > 0) {
                temp = getacharnow(0);
                if (temp != '\0' && (temp == UP || temp == DOWN || 
                                     temp == LEFT || temp == RIGHT)
                                 && temp != direction
                                 && temp != opposite_direction) {
                        direction = temp;
                        break;
                }
                usleep(10000);
                wait -= 1;
        }
}

void Game::move()
{
        switch (direction) {
                case UP: 
                        move_up();
                        break;
                case DOWN:
                        move_down();
                        break;
                case LEFT:
                        move_left();
                        break;
                case RIGHT:
                        move_right();
                        break;
                default: 
                        break;
        }
}

void Game::move_up()
{
        if (y_head - 1 < 0) {
                game_over = true;
                return;
        } else if (board[y_head - 1][x_head] == BODY_FROM_UP ||
                   board[y_head - 1][x_head] == BODY_FROM_DOWN ||
                   board[y_head - 1][x_head] == BODY_FROM_LEFT ||
                   board[y_head - 1][x_head] == BODY_FROM_RIGHT) {
                game_over = true;
                return;
        } else if (board[y_head - 1][x_head] == FOOD) {
                carry_body(y_head, x_head, BODY_FROM_UP, true);
                board[y_head--][x_head] = BODY_FROM_UP;
                board[y_head][x_head] = HEAD;
                bake_food();
                snake_size++;
        } else {
                carry_body(y_head, x_head, BODY_FROM_UP, false);
                board[y_head--][x_head] = BODY_FROM_UP;
                board[y_head][x_head] = HEAD;
        }
}

void Game::move_down()
{
        if (y_head + 1 >= y_dimension) {
                game_over = true;
                return;
        } else if (board[y_head + 1][x_head] == BODY_FROM_UP ||
                   board[y_head + 1][x_head] == BODY_FROM_DOWN ||
                   board[y_head + 1][x_head] == BODY_FROM_LEFT ||
                   board[y_head + 1][x_head] == BODY_FROM_RIGHT) {
                game_over = true;
                return;
        } else if (board[y_head + 1][x_head] == FOOD) {
                carry_body(y_head, x_head, BODY_FROM_DOWN, true);
                board[y_head++][x_head] = BODY_FROM_DOWN;
                board[y_head][x_head] = HEAD;
                bake_food();
                snake_size++;
        } else {
                carry_body(y_head, x_head, BODY_FROM_DOWN, false);
                board[y_head++][x_head] = BODY_FROM_DOWN;
                board[y_head][x_head] = HEAD;
        }
}

void Game::move_left()
{
        if (x_head - 1 < 0) {
                game_over = true;
                return;
        } else if (board[y_head][x_head - 1] == BODY_FROM_UP ||
                   board[y_head][x_head - 1] == BODY_FROM_DOWN ||
                   board[y_head][x_head - 1] == BODY_FROM_LEFT ||
                   board[y_head][x_head - 1] == BODY_FROM_RIGHT) {
                game_over = true;
                return;
        } else if (board[y_head][x_head - 1] == FOOD) {
                carry_body(y_head, x_head, BODY_FROM_LEFT, true);
                board[y_head][x_head--] = BODY_FROM_LEFT;
                board[y_head][x_head] = HEAD;
                bake_food();
                snake_size++;
        } else {
                carry_body(y_head, x_head, BODY_FROM_LEFT, false);
                board[y_head][x_head--] = BODY_FROM_LEFT;
                board[y_head][x_head] = HEAD;
        }
}

void Game::move_right()
{
        if (x_head + 1 >= x_dimension) {
                game_over = true;
                return;
        } else if (board[y_head][x_head + 1] == BODY_FROM_UP ||
                   board[y_head][x_head + 1] == BODY_FROM_DOWN ||
                   board[y_head][x_head + 1] == BODY_FROM_LEFT ||
                   board[y_head][x_head + 1] == BODY_FROM_RIGHT) {
                game_over = true;
                return;
        } else if (board[y_head][x_head + 1] == FOOD) {
                carry_body(y_head, x_head, BODY_FROM_RIGHT, true);
                board[y_head][x_head++] = BODY_FROM_RIGHT;
                board[y_head][x_head] = HEAD;
                bake_food();
                snake_size++;
        } else {
                carry_body(y_head, x_head, BODY_FROM_RIGHT, false);
                board[y_head][x_head++] = BODY_FROM_RIGHT;
                board[y_head][x_head] = HEAD;
        }
}

void Game::carry_body(int y_position, int x_position, int new_direction, 
                      bool food)
{
        if (y_position - 1 >= 0 && 
            board[y_position - 1][x_position] == BODY_FROM_DOWN) {
                board[y_position][x_position] = new_direction;
                carry_body(y_position - 1, x_position, BODY_FROM_DOWN, food);
                return;
        } 

        if (y_position + 1 < y_dimension && 
            board[y_position + 1][x_position] == BODY_FROM_UP) {
                board[y_position][x_position] = new_direction;
                carry_body(y_position + 1, x_position, BODY_FROM_UP, food);
                return;
        }

        if (x_position - 1 >= 0 && 
            board[y_position][x_position - 1] == BODY_FROM_RIGHT) {
                board[y_position][x_position] = new_direction;
                carry_body(y_position, x_position - 1, BODY_FROM_RIGHT, food);
                return;
        }

        if (x_position + 1 < x_dimension && 
            board[y_position][x_position + 1] == BODY_FROM_LEFT) {
                board[y_position][x_position] = new_direction;
                carry_body(y_position, x_position + 1, BODY_FROM_LEFT, food);
                return;
        }

        if (food) {
                board[y_position][x_position] = new_direction;
        } else {
                board[y_position][x_position] = EMPTY;
        }
}

void Game::print()
{
        screen_clear();
        for (int i = 0; i < x_dimension + 2; i++) {
                cout << RED_TEXT << BOLD << '_';
        }
        cout << NORMAL << endl;

        for (int i = 0; i < y_dimension; i++) {
                cout << RED_TEXT << BOLD << '|' << NORMAL;
                for (int j = 0; j < x_dimension; j++) {
                        switch (board[i][j]) {
                                case EMPTY: 
                                        cout << ' ';
                                        break;
                                case HEAD: 
                                        cout << YELLOW_TEXT << 'O' << NORMAL;
                                        break;
                                case BODY_FROM_UP: 
                                        cout << BLUE_TEXT << BOLD << '|' 
                                             << NORMAL;
                                        break;
                                case BODY_FROM_RIGHT: 
                                        cout << BLUE_TEXT << BOLD << '-' 
                                             << NORMAL;
                                        break;
                                case BODY_FROM_DOWN: 
                                        cout << BLUE_TEXT << BOLD << '|' 
                                             << NORMAL;
                                        break;
                                case BODY_FROM_LEFT: 
                                        cout << BLUE_TEXT << BOLD << '-' 
                                             << NORMAL;
                                        break;
                                case FOOD: 
                                        cout << GREEN_BACKGROUND << BOLD 
                                             <<'.' << NORMAL;
                                        break;
                                default:
                                        break;
                        }
                }
                cout << RED_TEXT << BOLD << '|' << NORMAL << endl;
        }

        for (int i = 0; i < x_dimension + 2; i++) {
                cout << RED_TEXT << BOLD << '-';
        }
        cout << NORMAL << endl << "Size: " << snake_size << endl;

}

void Game::bake_food()
{
        int y_rand, x_rand;
        if (check_win() || !empty_spaces()) {
                return;
        }

        do {
                y_rand = rand() % y_dimension;
                x_rand = rand() % x_dimension;
        } while (board[y_rand][x_rand] != EMPTY);

        board[y_rand][x_rand] = FOOD;
        speed -= (speed > 20 ? 1 : 0);
}

bool Game::check_win()
{
        bool done = true;

        for (int i = 0; i < y_dimension; i++) {
                for (int j = 0; j < x_dimension; j++) {
                        if (board[i][j] == EMPTY || board[i][j] == FOOD) {
                                done = false;
                        }
                }
        }

        if (done) {
                game_over = done;
                won = done;
        }

        return won;
}

bool Game::empty_spaces()
{
        bool empty = false;

        for (int i = 0; i < y_dimension; i++) {
                for (int j = 0; j < x_dimension; j++) {
                        if (board[i][j] == EMPTY) {
                                empty = true;
                        }
                }
        }

        return empty;
}

void Game::end_game()
{
        char response;
        if (won) {
                cout << "Congratulations, you won!" << endl;
        } else {
                cout << "Game Over!" << endl;
        }

        cout << "Would you like to play again? (Y/N) ";
        response = getachar();
        while (toupper(response) != 'Y' && toupper(response) != 'N') {
                cerr << "\nInvalid Reponse. Please answer with \'Y\' or "
                     << "\'N\' ";
                response = getachar();
        }

        if (toupper(response) == 'Y') {
                Game new_game(y_dimension, x_dimension);
                new_game.run();
        }

        cout << endl;
}

#undef UP
#undef LEFT
#undef DOWN
#undef RIGHT

#undef NORMAL 
#undef BOLD 

#undef RED_TEXT 
#undef GREEN_TEXT 
#undef YELLOW_TEXT 
#undef BLUE_TEXT 
#undef MAGENTA_TEXT 
#undef CYAN_TEXT 
#undef WHITE_TEXT

#undef RED_BACKGROUND 
#undef GREEN_BACKGROUND 
#undef YELLOW_BACKGROUND
#undef BLUE_BACKGROUND 
#undef MAGENTA_BACKGROUND
#undef CYAN_BACKGROUND 
#undef WHITE_BACKGROUND
