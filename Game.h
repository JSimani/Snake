#ifndef GAME_H_
#define GAME_H_ 

class Game
{
        private:
                int y_dimension;
                int x_dimension;
                int y_head;
                int x_head;
                int snake_size;
                int direction;
                int speed;
                int **board;

                bool game_over;
                bool won;

                void print();
                void get_move();
                void move();
                void move_up();
                void move_down();
                void move_left();
                void move_right();
                void carry_body(int y_position, int x_dimension, 
                                int new_direction, bool food);
                void bake_food();
                bool check_win();
                bool empty_spaces();
                void end_game();

        public:
                Game();
                Game(int y_dimen, int x_dimen);
                Game(const Game &source);
                Game &operator=(const Game &source);
                ~Game();

                void run();
};


#endif
