#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"

void saveTrainToFile(Train *train)
{
    FILE *file = fopen("train_details.csv", "w");
    if (file == NULL)
    {
        printf("Error saving train details!\n");
        return;
    }

    for (int i = 0; i < train->trainCount; i++)
    {
        fprintf(file, "%s,%s,%s,%s,%s,%d\n",
                train->trains[i].trainNumber,
                train->trains[i].trainName,
                train->trains[i].source,
                train->trains[i].destination,
                train->trains[i].time,
                train->trains[i].compartments);
    }

    fclose(file);
}

void loadTrainFromFile(Train *train)
{
    FILE *file = fopen("train_details.csv", "r");
    if (file == NULL)
    {
        printf("train_details.csv not found. Starting fresh.\n");
        return;
    }

    train->trainCount = 0;

    while (fscanf(file, "%[^,],%[^,],%[^,],%[^,],%[^,],%d\n",
                  train->trains[train->trainCount].trainNumber,
                  train->trains[train->trainCount].trainName,
                  train->trains[train->trainCount].source,
                  train->trains[train->trainCount].destination,
                  train->trains[train->trainCount].time,
                  &train->trains[train->trainCount].compartments) == 6)
    {
        for (int i = 0; i < train->trains[train->trainCount].compartments; i++)
        for (int j = 0; j < ROWS; j++)
            for (int k = 0; k < COLS; k++)
                train->trains[train->trainCount].seatLayout[i][j][k] = 0;

    // Set initial total available seats
    train->trains[train->trainCount].seats = train->trains[train->trainCount].compartments * ROWS * COLS;

    train->trainCount++;
    }

    fclose(file);

    // Load seat bookings
    FILE *book = fopen("bookings.csv", "r");
    if (book == NULL)
    {
        printf("bookings.csv not found. No prior bookings loaded.\n");
        return;
    }

    char username[50];
    char trainNumber[20];
    char trainName[20];
    int comp, seat;

    while (fscanf(book, "%[^,],%[^,],%[^,],%d,%d\n",
                  username, trainNumber, trainName, &comp, &seat) == 5)
    {
        for (int i = 0; i < train->trainCount; i++)
        {
            if (strcmp(train->trains[i].trainNumber, trainNumber) == 0)
            {
                int row = (seat - 1) / COLS;
                int col = (seat - 1) % COLS;

                if (comp > 0 && comp <= train->trains[i].compartments)
                    train->trains[i].seatLayout[comp - 1][row][col] = 1;
                    train->trains[i].seats--;
            }
        }
    }

    fclose(book);
}
