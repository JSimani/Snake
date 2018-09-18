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

// Different terminal escape characters for modifying text appearances
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

/* Constructor
 * Purpose: Initialize members of the Game object
 * Parameters: None
 * Returns: Nothing
 */
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

/* Parameterized Constructor
 * Purpose: Initialize members of the Game object with implementer's choice
 *          of game board dimensions.
 * Parameters: y_dimen (desired vertical size of board), x_dimen (desired 
               horizontal size of board)
 * Returns: Nothing
 */
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

/* Copy Constructor
 * Purpose: Initialize members of the Game object based on the values from
 *          another Game object.
 * Parameters: source (Game object to be copied)
 * Returns: Nothing
 */
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

/* Assignment Overload "="
 * Purpose: Overload the assignment operation (=) so that Games are not
 *          shallow copied when assigned to each other.
 * Parameters: source (Game object to be duplicated)
 * Returns: Game (new Game object)
 */
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

/* Destructor
 * Purpose: Frees all heap-allocated memory in the Game object.
 * Parameters: None
 * Returns: Nothing
 */
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
        cout << NORMAL;
        place_cursor(15,0);
}

/* run()
 * Purpose: Runs the Snake game, generating the first "food," retreiving user 
 *          input, and moving the Snake accordingly. Prints the board after
 *          every move and ends the game when the Snake hits the wall.
 * Parameters: None
 * Returns: void
 */
void Game::run()
{
        hide_cursor();
        screen_clear();
        bake_food();
        print();
        cout << "Enter \'w\', \'a\', \'s\', or \'d\' to start!" << endl;

        // Get initial input, do not start until a valid direction is provided
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

/* get_move()
 * Purpose: Gets a move from the user. If no move is provided, direction stays
 *          the same as the previous direction.
 * Parameters: None
 * Returns: void
 */
void Game::get_move()
{
        char temp;
        int wait = speed;
        int opposite_direction;

        /* Sets an opposite direction so that the user can't select to 
         * turn the Snake around */
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

        /* Keeps prompting for input until time runs out. This is the delay
         * in between each of the Snake's moves. */
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

/* move()
 * Purpose: Moves the Snake's head an body in the direction it is supposed to
 *          go. Changes the direction of the body accordingly when it moves.
 * Parameters: None
 * Returns: void
 */
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

/* move_up()
 * Purpose: Moves the Snake's head up one space, and changes its body 
 *          positions and directions accordingly.
 * Parameters: None
 * Returns: void
 */
void Game::move_up()
{
        if (y_head - 1 < 0) {
                // Case 1: Snake hits a wall
                game_over = true;
                return;
        } else if (board[y_head - 1][x_head] == BODY_FROM_UP ||
                   board[y_head - 1][x_head] == BODY_FROM_DOWN ||
                   board[y_head - 1][x_head] == BODY_FROM_LEFT ||
                   board[y_head - 1][x_head] == BODY_FROM_RIGHT) {
                // Case 2: Snake hits its own body
                game_over = true;
                return;
        } else if (board[y_head - 1][x_head] == FOOD) {
                // Case 3: Snake hits food
                carry_body(y_head, x_head, BODY_FROM_UP, true);
                board[y_head--][x_head] = BODY_FROM_UP;
                board[y_head][x_head] = HEAD;
                bake_food();
                snake_size++;
        } else {
                // Case 4: Snake doesn't hit anything
                carry_body(y_head, x_head, BODY_FROM_UP, false);
                board[y_head--][x_head] = BODY_FROM_UP;
                board[y_head][x_head] = HEAD;
        }
}

/* move_down()
 * Purpose: Moves the Snake's head down one space, and changes its body 
 *          positions and directions accordingly.
 * Parameters: None
 * Returns: void
 */
void Game::move_down()
{
        if (y_head + 1 >= y_dimension) {
                // Case 1: Snake hits a wall
                game_over = true;
                return;
        } else if (board[y_head + 1][x_head] == BODY_FROM_UP ||
                   board[y_head + 1][x_head] == BODY_FROM_DOWN ||
                   board[y_head + 1][x_head] == BODY_FROM_LEFT ||
                   board[y_head + 1][x_head] == BODY_FROM_RIGHT) {
                // Case 2: Snake hits its own body
                game_over = true;
                return;
        } else if (board[y_head + 1][x_head] == FOOD) {
                // Case 3: Snake hits food
                carry_body(y_head, x_head, BODY_FROM_DOWN, true);
                board[y_head++][x_head] = BODY_FROM_DOWN;
                board[y_head][x_head] = HEAD;
                bake_food();
                snake_size++;
        } else {
                // Case 4: Snake doesn't hit anything
                carry_body(y_head, x_head, BODY_FROM_DOWN, false);
                board[y_head++][x_head] = BODY_FROM_DOWN;
                board[y_head][x_head] = HEAD;
        }
}

/* move_left()
 * Purpose: Moves the Snake's head left one space, and changes its body 
 *          positions and directions accordingly.
 * Parameters: None
 * Returns: void
 */
void Game::move_left()
{
        if (x_head - 1 < 0) {
                // Case 1: Snake hits a wall
                game_over = true;
                return;
        } else if (board[y_head][x_head - 1] == BODY_FROM_UP ||
                   board[y_head][x_head - 1] == BODY_FROM_DOWN ||
                   board[y_head][x_head - 1] == BODY_FROM_LEFT ||
                   board[y_head][x_head - 1] == BODY_FROM_RIGHT) {
                // Case 2: Snake hits its own body
                game_over = true;
                return;
        } else if (board[y_head][x_head - 1] == FOOD) {
                // Case 3: Snake hits food
                carry_body(y_head, x_head, BODY_FROM_LEFT, true);
                board[y_head][x_head--] = BODY_FROM_LEFT;
                board[y_head][x_head] = HEAD;
                bake_food();
                snake_size++;
        } else {
                // Case 4: Snake doesn't hit anything
                carry_body(y_head, x_head, BODY_FROM_LEFT, false);
                board[y_head][x_head--] = BODY_FROM_LEFT;
                board[y_head][x_head] = HEAD;
        }
}

/* move_right()
 * Purpose: Moves the Snake's head right one space, and changes its body 
 *          positions and directions accordingly.
 * Parameters: None
 * Returns: void
 */
void Game::move_right()
{
        if (x_head + 1 >= x_dimension) {
                // Case 1: Snake hits a wall
                game_over = true;
                return;
        } else if (board[y_head][x_head + 1] == BODY_FROM_UP ||
                   board[y_head][x_head + 1] == BODY_FROM_DOWN ||
                   board[y_head][x_head + 1] == BODY_FROM_LEFT ||
                   board[y_head][x_head + 1] == BODY_FROM_RIGHT) {
                // Case 2: Snake hits its own body
                game_over = true;
                return;
        } else if (board[y_head][x_head + 1] == FOOD) {
                // Case 3: Snake hits food
                carry_body(y_head, x_head, BODY_FROM_RIGHT, true);
                board[y_head][x_head++] = BODY_FROM_RIGHT;
                board[y_head][x_head] = HEAD;
                bake_food();
                snake_size++;
        } else {
                // Case 4: Snake doesn't hit anything
                carry_body(y_head, x_head, BODY_FROM_RIGHT, false);
                board[y_head][x_head++] = BODY_FROM_RIGHT;
                board[y_head][x_head] = HEAD;
        }
}

/* carry_body()
 * Purpose: Recursive function to iterate through the body of the Snake, 
 *          each body part's direction accordingly and moving on to the next
 *          body part.
 * Parameters: y_position (the y coordinate of the body part being changed), 
 *             x_position (the x coordinate of the body part being changed), 
 *             new_direction (indicated direction that the current body part
 *             should now have), food (true if the head ate a food during the
 *             current move)
 * Returns: void
 */
void Game::carry_body(int y_position, int x_position, int new_direction, 
                      bool food)
{
        if (y_position - 1 >= 0 && 
            board[y_position - 1][x_position] == BODY_FROM_DOWN) {
                /* Case 1: The next part of the body is above the current 
                 * location */
                board[y_position][x_position] = new_direction;
                carry_body(y_position - 1, x_position, BODY_FROM_DOWN, food);
                return;
        } 

        if (y_position + 1 < y_dimension && 
            board[y_position + 1][x_position] == BODY_FROM_UP) {
                /* Case 2: The next part of the body is below the current 
                 * location */
                board[y_position][x_position] = new_direction;
                carry_body(y_position + 1, x_position, BODY_FROM_UP, food);
                return;
        }

        if (x_position - 1 >= 0 && 
            board[y_position][x_position - 1] == BODY_FROM_RIGHT) {
                /* Case 3: The next part of the body is left of the current 
                 * location */
                board[y_position][x_position] = new_direction;
                carry_body(y_position, x_position - 1, BODY_FROM_RIGHT, food);
                return;
        }

        if (x_position + 1 < x_dimension && 
            board[y_position][x_position + 1] == BODY_FROM_LEFT) {
                /* Case 4: The next part of the body is right of the current 
                 * location */
                board[y_position][x_position] = new_direction;
                carry_body(y_position, x_position + 1, BODY_FROM_LEFT, food);
                return;
        }

        
        if (food) {
                // Case 4: Last part of the body, Snake ate food
                board[y_position][x_position] = new_direction;
        } else {
                // Case 5: Last part of the body, Snake didn't eat food
                board[y_position][x_position] = EMPTY;
        }
}

/* print()
 * Purpose: Print the board.
 * Parameters: None
 * Returns: void
 */
void Game::print()
{
        screen_home();
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
        cout << NORMAL << endl << "Size: " << snake_size << endl << endl;

}

/* bake_food()
 * Purpose: Generates a food item in a random space on the board, given that 
 *          there is a space to put the food. If there are no spaces to put 
 *          the food and/or the board is full, it does nothing.
 * Parameters: None
 * Returns: void
 */
void Game::bake_food()
{
        int y_rand, x_rand;
        if (check_win() || !empty_spaces()) {
                return;
        }

        /* Keep generating new coordinates on the board until an empty space
         * is found */
        do {
                y_rand = rand() % y_dimension;
                x_rand = rand() % x_dimension;
        } while (board[y_rand][x_rand] != EMPTY);

        board[y_rand][x_rand] = FOOD;
        // Spped up the movement of the snake if it is still above 20
        speed -= (speed > 20 ? 1 : 0);
}

/* check_win()
 * Purpose: Checks to see if the user has won the game
 * Parameters: None
 * Returns: bool (true if the user has won the game)
 */
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

/* empty_spaces()
 * Purpose: Checks to see if the board contains any empty spaces
 * Parameters: None
 * Returns: bool (true if the board contains any empty spaces)
 */
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

/* end_game()
 * Purpose: Prints message to user based on if they won or lost. Prompts user
 *          if they want to play again
 * Parameters: None
 * Returns: void
 */
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
