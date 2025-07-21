#ifndef TRAIN_H
#define TRAIN_H

#define MAX_TRAINS 100
#define MAX_COMPARTMENTS 10
#define ROWS 5
#define COLS 4

typedef struct {
    char trainNumber[20];
    char trainName[50];
    char source[30];
    char destination[30];
    char time[10];
    int compartments;
    int seatLayout[MAX_COMPARTMENTS][ROWS][COLS];
    int seats;
} TrainDetails;

typedef struct {
    TrainDetails trains[MAX_TRAINS];
    int trainCount;
} Train;

#endif
