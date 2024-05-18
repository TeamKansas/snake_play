/*
 * Play Snake!
 * Simple snake game, eat the apples, try to grow your snake
 * Have fun!
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define HEIGHT 32
#define WIDTH  32

void disableRaw();              // reset terminal input to normal
void enableRaw(int min);        // enable return of read() after (int) min characters read. (don't wait for newline)
void printField(int map[WIDTH][HEIGHT], int apple[2]);                      // display map, snake, and apple
void reset(int map[WIDTH][HEIGHT], int apple[2], int pos[2], int dir[2]);   // reset map, snake, and apple

int main() {
    int map[WIDTH][HEIGHT];
    int apple[2] = {35,25};
    int pos[2] = {10,25};
    int dir[2] = {1,0};

    int x, y;

    int score = 0;
    int hiscore = 0;
    int delay = 50000;
    int update = 1;

    char c;
    int running = 1;
    clock_t timer;

    running = 1;
    reset(map,apple,pos,dir);

    memset(map,0,WIDTH*HEIGHT*sizeof(int));
    map[pos[0]][pos[1]] = 1;

    enableRaw(1);
    // instructions and wait for player ready
    printf("\033cWASD to move. Don't Crash into yourself or into the wall!\nPress any key to continue.\n\n");
    printField(map,apple);
    read(0,NULL,1);

    enableRaw(0);
    timer = clock();
    while(running) {
        if(clock() - timer > delay) {   // timer expired
            timer = clock();
            update = 1;
        }
        if(update) { // if update requested, update only happens when timer expires or player gives input
            printf("\033cScore: %d\t\tHigh Score: %d\n\n\n",score,hiscore);
            printField(map,apple);
            x = pos[0] + dir[0];
            y = pos[1] + dir[1];
            if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || map[x][y]) // if crashed
                running = 0;
            else { // otherwise, move!
                pos[0] = x;
                pos[1] = y;
                if(x == apple[0] && y == apple[1]) {
                    apple[1] = rand()%HEIGHT;
                    apple[0] = rand()%WIDTH;
                    map[x][y] = score + 2;
                    ++score;
                    if(score > hiscore)
                        hiscore = score;
                }
                else { // if player just scored, don't move snake tail
                    for(x = 0; x < WIDTH; ++x) {
                        for(y = 0; y < HEIGHT; ++y) {
                            if(map[x][y])
                                --map[x][y];
                        }
                    }
                    map[pos[0]][pos[1]] = score + 1;
                }
                
            }
            update = 0;
        }
        if(read(0,&c,1)) { // user input
            update = 1;
            switch(c) {
                case 'w':
                    if(dir[1] == 0) {
                        dir[1] = 1;
                        dir[0] = 0;
                    }
                    break;
                case 's':
                    if(dir[1] == 0) {
                        dir[1] = -1;
                        dir[0] = 0;
                    }
                    break;
                case 'a':
                    if(dir[0] == 0) {
                        dir[0] = -1;
                        dir[1] = 0;
                    }
                    break;
                case 'd':
                    if(dir[0] == 0) {
                        dir[0] = 1;
                        dir[1] = 0;
                    }
                    break;
            }
        }
        if(!running) { // you crashed!
            enableRaw(1);
            // ask to play again, wait for input
            printf("You lose );\nWould you like to play again? (y/n)\n");
            while(read(0,&c,1) && c != 'y' && c != 'n');
            if(c == 'y') { // if affirmed, reset board, play again.
                running = 1;
                update = 1;
                reset(map,apple,pos,dir);
                score = 0;
                timer = clock();
                enableRaw(0);
            }

        }
    }
    disableRaw();
}


void disableRaw() {
    struct termios t;
    tcgetattr(0,&t);
    t.c_lflag |= (ECHO|ICANON);
    tcsetattr(0,0,&t);
}

void enableRaw(int min) {
    struct termios t;
    tcgetattr(0,&t);
    t.c_cc[VTIME] = 0;
    t.c_cc[VMIN] = min;
    t.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(0,0,&t);
}

void printField(int map[WIDTH][HEIGHT], int apple[2]) {
    for(int y = WIDTH; y >= -1; --y) {
        for(int x = -1; x <= WIDTH; ++x) {
            if(x == -1 || x == WIDTH || y == -1 || y == HEIGHT) // draw walls!
                printf("//");
            else if(map[x][y]) // draw snake!
                printf("\033[34m[]\033[0m");
            else if(x == apple[0] && y == apple[1]) // draw apple!
                printf("\033[31m@\033[32m<\033[0m");
            else
                printf("  ");
        }
        printf("\n");
    }
}

void reset(int map[WIDTH][HEIGHT], int apple[2], int pos[2], int dir[2]) {
    apple[0] = 25;
    apple[1] = 16;
    pos[0] = 5;
    pos[1] = 16;
    dir[0] = 1;
    dir[1] = 0;
    memset(map,0,WIDTH*HEIGHT*sizeof(int));
    map[5][16] = 1;
}