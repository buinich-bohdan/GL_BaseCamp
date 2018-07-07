#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


double timespec_diff(struct timespec *stop, struct timespec *start)
{
    double diff = difftime(stop->tv_sec, start->tv_sec);
    diff *= 1000;
    diff += (stop->tv_nsec - start->tv_nsec) / 1e6;
    return diff;
}


#define WIDTH 9
#define HEIGHT 7

int gridArray[WIDTH][HEIGHT] = {
        { 0, -1, -1, -1, -2, -1, -2 },
        { -2, -1, -1, -1, -1, -2, -2 },
        { -2, -1, -1, -1, -1, -2, -2 },
        { -2, -2, -1, -1, -1, -2, -1 },
        { -1, -2, -2, -2, -1, -1, -2 },
        { -2, -1, -1, -1, -1, -2, -2 },
        { -2, -2, -1, -1, -1, -2, -2 },
        { -2, -2, -1, -1, -1, -2, -2 },
        { -2, -2, -1, -3, -1, -2, -2 }

};

void DrawGrid()
{
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            char symbol[2] = {};

            if (gridArray[x][y] == 0) //start point
                symbol[0] = 'S';
            else if (gridArray[x][y] == -3) //stop point
                symbol[0] = 'E';
            else if (gridArray[x][y] == -2)  //wall
                symbol[0] = '#';
            else if (gridArray[x][y] == -1) //Maze
                symbol[0] = '.';
            else
                sprintf(symbol, "%d", gridArray[x][y] );
            printf("%3s", symbol);
        }
        printf("\n\n");
    }
    printf("\n\n");
}

void SolveMaze()
{
    bool foundEnd = false;
    int it = 0; //next point
    while (!foundEnd) {
        bool foundEmpty = false;
        for (int x = 0; x < WIDTH && !foundEnd; ++x) {
            for (int y = 0; y < HEIGHT - 1; ++y) {
                if (gridArray[x][y] == it) {
                    /*
                                                 N
                                                 |
                                             W-------E
                                                 |
                                                 S                                               S
                   */
                    if (y < HEIGHT - 1) {
                        // check north cell
                        int* north = &(gridArray[x][y + 1]); //ptr to grid

                        if (*north == -3) { //stop point
                            foundEnd = true;
                            break;
                        }
                        else if (*north == -1) { //next step+1
                            *north = it + 1;
                            foundEmpty = true;
                        }
                    }

                    // check south cell
                    if (y > 0) {
                        int* south = &(gridArray[x][y - 1]); //ptr to grid
                        if (*south == -3) {
                            foundEnd = true;
                            break;
                        }
                        else if (*south == -1) {
                            *south = it + 1;
                            foundEmpty = true;
                        }
                        // check west cell
                        if (x < WIDTH - 1) {
                            int* west = &gridArray[x + 1][y]; //ptr to grid
                            if (*west == -3) {
                                foundEnd = true;
                                break;
                            }
                            else if (*west == -1) {
                                *west = it + 1;
                                foundEmpty = true;
                            }
                        }

                        // check east cell
                        if (x > 0) {
                            int* east = &gridArray[x - 1][y]; //ptr to grid
                            if (*east == -3) {
                                foundEnd = true;
                                break;
                            }
                            else if (*east == -1) {
                                *east = it + 1;
                                foundEmpty = true;
                            }
                        }
                    }
                }
            }
        }
        if (!foundEnd && !foundEmpty) { //no solution
            printf("This maze has no solution!\n");
            break;
        }

        it++;
    }
}

int main()
{
    /* Sort array */
    struct timespec time_now, time_after;
    DrawGrid();
    SolveMaze();
    DrawGrid();
    /* Print measured sorting time */
    printf("%g\n", timespec_diff(&time_after, &time_now));
    return 0;
}