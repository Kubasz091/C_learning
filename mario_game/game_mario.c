#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define WIDTH 120
#define HEIGHT 20

#define FPS 60

const char STANDING = 'O';
const char CRUNCHING = 'o';

const char OBSTACLE = '#';

const int WAIT_TIME = 1000 / FPS;
const int SPEED_INCREASE_TIME = 5000; // 10s

struct termios original_termios;

// Methods
long currentMillis(void);
void set_raw_mode(void);
int getKeyPressed(void);
void printBoard(char board[HEIGHT][WIDTH]);
void unset_raw_mode(void);
void handleInput(long *idle_jump_time, long current_time);
void updatePlayer(void);

struct timeval tp;
struct timespec ts;

char board[HEIGHT][WIDTH];

int player_x = 1;
int player_y = HEIGHT - 2;
int velocity_x = 0;
int velocity_y = 0;
int on_ground = 1;
int is_crounching = 0;

int obstacle_destroyed = 1;
int obstacle_x = WIDTH - 1;
int obstacle_height = 0;

double score = 0.0;
float speed = 5.0;

int main(void) {
    srand(time(NULL));
    set_raw_mode();

    // initialize an empty board
    for (int i = 0; i < HEIGHT-1; i++) {
        for (int j = 0; j < WIDTH; j++){
            board[i][j] = ' ';
        }
    }

    for (int j = 0; j < WIDTH; j++){
        board[HEIGHT-1][j] = 'G';
    }

    board[player_y][player_x] = STANDING;

    long idle_jump_time = currentMillis();

    long screen_update_time = currentMillis();
    long obstacle_update_time = currentMillis();
    long obstacle_spawn_time = currentMillis() + 1000; // 1s
    long speed_increase_time = currentMillis() + SPEED_INCREASE_TIME;

    while (1) {
        long current_time = currentMillis();

        handleInput(&idle_jump_time, current_time);

        if (board[player_y][player_x] == OBSTACLE) {
            system("clear");

            printf("Controls: A/D=move, W/Space=jump, S=crounch, Q=quit\n");

            if (is_crounching) {
                board[player_y][player_x] = CRUNCHING;
            }
            else {
                board[player_y][player_x] = STANDING;
            }

            printBoard(board);

            printf("Score: %.2f\n", score / 100.0);

            printf("Game Over! You hit an obstacle!\n");
            unset_raw_mode();
            exit(0);
        }


        if (current_time >= idle_jump_time && on_ground) {
            velocity_x = 0;
        }

        if (current_time >= screen_update_time){

            updatePlayer();
            system("clear");

            printf("Controls: A/D=move, W/Space=jump, S=crounch, Q=quit\n");

            printBoard(board);

            printf("Score: %.2f\n", score / 100.0);

            screen_update_time = current_time + WAIT_TIME;

        }

        if (current_time >= speed_increase_time) {
            speed *= 1.5;
            speed_increase_time = current_time + SPEED_INCREASE_TIME;
        }

        if (current_time >= obstacle_update_time) {
            score += speed;
            if (!obstacle_destroyed) {
                if (obstacle_x == 0) {
                    obstacle_destroyed = 1;
                    for (int i = HEIGHT - 2; i >= HEIGHT - 2 - obstacle_height; i--) {
                        board[i][0] = ' ';
                    }
                }
                else {
                    for (int i = HEIGHT - 2; i >= HEIGHT - 2 - obstacle_height; i--) {
                        board[i][obstacle_x] = ' ';
                    }
                    obstacle_x--;
                    for (int i = HEIGHT - 2; i >= HEIGHT - 2 - obstacle_height; i--) {
                        board[i][obstacle_x] = OBSTACLE;
                    }
                }
            }
            else if (current_time >= obstacle_spawn_time) {
                obstacle_x = WIDTH - 1;
                obstacle_height = 1 + (rand() % (8));
                obstacle_destroyed = 0;
                for (int i = HEIGHT - 2; i >= HEIGHT - 2 - obstacle_height; i--) {
                        board[i][obstacle_x] = OBSTACLE;
                }
                obstacle_spawn_time = current_time + (int)(5000.0 / speed) + 500 + (rand() % (1000));
            }
            obstacle_update_time = current_time + (int)(250.0 / speed) + 10;
        }

    }
    unset_raw_mode();
    return 0;
}


long currentMillis(void) {
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

void printBoard(char board[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        char row[WIDTH + 1];
        memcpy(row, board[i], WIDTH);
        row[WIDTH] = '\0';
        printf("%s\n", row);
    }
}

void updatePlayer(void) {
    board[player_y][player_x] = ' ';

    if (!on_ground) {
        velocity_y += 1;
        player_y += velocity_y;

        if (player_y >= HEIGHT - 2) {
            player_y = HEIGHT - 2;
            velocity_y = 0;
            on_ground = 1;
        }

        if (player_x > 0 && player_x < WIDTH - 1) {
            player_x += velocity_x;
            if (player_x < 0) {
                player_x = 0;
            }
            else if (player_x > WIDTH - 1) {
                player_x = WIDTH - 1;
            }
        }
    }
    if (is_crounching) {
        board[player_y][player_x] = CRUNCHING;
    }
    else {
        board[player_y][player_x] = STANDING;
    }
}


void handleInput(long *idle_jump_time, long current_time) {
    int key = getKeyPressed();
    if (key != 0) {
        switch(key) {
            case 'a':
            case 'A':
                if (player_x > 0) {
                    board[player_y][player_x] = ' ';
                    player_x--;
                    velocity_x = -1;
                    *idle_jump_time = current_time + 250;
                }
                break;
            case 'd':
            case 'D':
                if (player_x < WIDTH - 1) {
                    board[player_y][player_x] = ' ';
                    player_x++;
                    velocity_x = 1;
                    *idle_jump_time = current_time + 250;
                }
                break;
            case 's':
            case 'S':
                if (on_ground) {
                    is_crounching = 1;
                }
                break;
            case 'w':
            case 'W':
            case ' ':
                if (on_ground) {
                    velocity_y = -5;
                    is_crounching = 0;
                    on_ground = 0;
                }
                break;
            case 'q':
            case 'Q':
            case 27:
                unset_raw_mode();
                exit(0);
        }
    }
}

// Function to set terminal to raw mode
void set_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &original_termios);
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void unset_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

// Function to check if a key has been pressed
int getKeyPressed(void) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    int ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        return ch;
    }
    return 0;
}