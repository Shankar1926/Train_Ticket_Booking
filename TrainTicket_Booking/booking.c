#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "booking.h"
#include "admin.h"
#include "file.h"

void viewMyBookings(const char *username)
{
    FILE *fp = fopen("bookings.csv", "r");
    if (fp == NULL)
    {
        printf("No bookings found.\n");
        return;
    }

    typedef struct {
        char trainNum[20];
        char trainName[50];
        int compartment;
        char seatList[100];
    } BookingGroup;

    BookingGroup bookings[100];
    int groupCount = 0;

    char line[200];
    while (fgets(line, sizeof(line), fp))
    {
        char fileUser[20], trainNum[20], trainName[50];
        int comp, seat;

        if (sscanf(line, "%[^,],%[^,],%[^,],%d,%d", fileUser, trainNum, trainName, &comp, &seat) == 5)
        {
            if (strcmp(fileUser, username) == 0)
            {
                // Check if same train, train name, compartment already exists
                int found = 0;
                for (int i = 0; i < groupCount; i++)
                {
                    if (strcmp(bookings[i].trainNum, trainNum) == 0 &&
                        strcmp(bookings[i].trainName, trainName) == 0 &&
                        bookings[i].compartment == comp)
                    {
                        char seatStr[10];
                        sprintf(seatStr, ",%d", seat);
                        strcat(bookings[i].seatList, seatStr);
                        found = 1;
                        break;
                    }
                }

                if (!found)
                {
                    strcpy(bookings[groupCount].trainNum, trainNum);
                    strcpy(bookings[groupCount].trainName, trainName);
                    bookings[groupCount].compartment = comp;
                    sprintf(bookings[groupCount].seatList, "%d", seat);
                    groupCount++;
                }
            }
        }
    }
    fclose(fp);

    printf("\n------------------------- Your Booked Tickets -------------------------\n");
    printf("%-16s %-12s %-20s %-12s %-s\n", "Username", "Train No", "Train Name", "Compartment", "Seat No(s)");
    printf("-------------------------------------------------------------------------\n");

    if (groupCount == 0)
    {
        printf("No bookings found for %s.\n", username);
        return;
    }

    char prevTrainNum[20] = "";
    char prevTrainName[50] = "";

    for (int i = 0; i < groupCount; i++)
    {
        int isNewTrain = strcmp(prevTrainNum, bookings[i].trainNum) != 0 ||
                         strcmp(prevTrainName, bookings[i].trainName) != 0;

        if (isNewTrain)
        {
            printf("%-16s %-12s %-20s C%d          %-s\n",
                   username,
                   bookings[i].trainNum,
                   bookings[i].trainName,
                   bookings[i].compartment,
                   bookings[i].seatList);
        }
        else
        {
            printf("%-50s C%d          %-s\n",
                   "",
                   bookings[i].compartment,
                   bookings[i].seatList);
        }

        strcpy(prevTrainNum, bookings[i].trainNum);
        strcpy(prevTrainName, bookings[i].trainName);
    }

    printf("-------------------------------------------------------------------------\n");
}

void displaySeats(const TrainDetails *t)
{
    printf("\nTrain: %s - %s\n", t->trainNumber, t->trainName);
    printf("Source: %s\tDestination: %s\tTime: %s\n", t->source, t->destination, t->time);
    printf("Available Seats: %d / %d\n", t->seats, t->compartments * ROWS * COLS);
    printf("\n");

    // Center-aligned compartment headers
    printf("     ");
    for (int c = 0; c < t->compartments; c++)
        printf("    Compartment %-2d         ", c + 1);
    printf("\n");

    // Top borders
    printf("     ");
    for (int c = 0; c < t->compartments; c++)
        printf("+----------------------+ ");
    printf("\n");

    // Seat rows (centered)
    for (int r = 0; r < ROWS; r++)
    {
        printf("     ");
        for (int c = 0; c < t->compartments; c++)
        {
            printf("    ");  // Padding for left edge inside compartment

            for (int col = 0; col < COLS; col++)
            {
                int seatNum = r * COLS + col + 1;
                if (t->seatLayout[c][r][col] == 0)
                    printf("%2d  ", seatNum);
                else
                    printf(" X  ");
            }
            printf("   ");  // Padding after seat row
        }
        printf("\n");
    }

    // Bottom borders
    printf("     ");
    for (int c = 0; c < t->compartments; c++)
        printf("+----------------------+ ");
    printf("\n");
}


void bookTicket(const char *username, Train *train)
{
    viewAllTrains(train);
    char trainNumber[20];

    printf("Enter Train Number to book: ");
    scanf("%s", trainNumber);

    for (int i = 0; i < train->trainCount; i++)
    {
        TrainDetails *t = &train->trains[i];

        if (strcmp(t->trainNumber, trainNumber) == 0)
        {
            int totalSeats = ROWS * COLS;

            int comp;
            displaySeats(t);
            printf("Enter compartment number (1 to %d): ", t->compartments);
            scanf("%d", &comp);

            if (comp < 1 || comp > t->compartments)
            {
                printf("Invalid compartment number.\n");
                return;
            }


            int seatNum;
            printf("Enter seat number to book (1-%d): ", totalSeats);
            scanf("%d", &seatNum);

            if (seatNum < 1 || seatNum > totalSeats)
            {
                printf("Invalid seat number.\n");
                return;
            }

            int row = (seatNum - 1) / COLS;
            int col = (seatNum - 1) % COLS;

            if (t->seatLayout[comp - 1][row][col] == 1)
            {
                printf("Seat already booked. Choose another.\n");
                return;
            }

            t->seatLayout[comp - 1][row][col] = 1;
            t->seats--;

            FILE *fp = fopen("bookings.csv", "a");
            if (fp == NULL)
            {
                printf("Error opening booking file!\n");
                return;
            }

            fprintf(fp, "%s,%s,%s,%d,%d\n", username, t->trainNumber, t->trainName, comp, seatNum);
            fclose(fp);

            printf("✅ Ticket booked: %s - %s | Compartment %d | Seat %d\n",
                   t->trainNumber, t->trainName, comp, seatNum);
            return;
        }
    }

    printf("Train number not found.\n");
}

void cancelTicket(const char *username, Train *train)
{
    FILE *fp = fopen("bookings.csv", "r");
    if (fp == NULL)
    {
        printf("No bookings found.\n");
        return;
    }

    typedef struct {
        char trainNum[20];
        char trainName[50];
        int compartment;
        int seatNum;
    } Ticket;

    Ticket userTickets[100];
    int ticketCount = 0;

    char line[200], fileUser[20], trainNum[20], trainName[50];
    int compNum, seatNum;

    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%[^,],%[^,],%[^,],%d,%d", fileUser, trainNum, trainName, &compNum, &seatNum) == 5)
        {
            if (strcmp(fileUser, username) == 0)
            {
                strcpy(userTickets[ticketCount].trainNum, trainNum);
                strcpy(userTickets[ticketCount].trainName, trainName);
                userTickets[ticketCount].compartment = compNum;
                userTickets[ticketCount].seatNum = seatNum;
                ticketCount++;
            }
        }
    }
    fclose(fp);

    if (ticketCount == 0)
    {
        printf("❌ No bookings found under your name.\n");
        return;
    }

    // Show bookings with numbers
    printf("\n---------------------------- Your Bookings ----------------------------\n");
    printf("%-5s %-12s %-20s %-12s\n", "ID", "Train No", "Train Name", "Comp-S.No");
    printf("------------------------------------------------------------------------\n");
    for (int i = 0; i < ticketCount; i++)
    {
        printf("%-5d %-12s %-20s C%d-S%d\n", i + 1,
               userTickets[i].trainNum,
               userTickets[i].trainName,
               userTickets[i].compartment,
               userTickets[i].seatNum);
    }
    printf("------------------------------------------------------------------------\n");

    int choice;
    printf("Enter the ID of the ticket you want to cancel: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > ticketCount)
    {
        printf("❌ Invalid choice.\n");
        return;
    }

    // Extract details of the ticket to cancel
    Ticket toCancel = userTickets[choice - 1];

    // Rewrite bookings.csv excluding the canceled ticket
    fp = fopen("bookings.csv", "r");
    FILE *temp = fopen("temp.csv", "w");

    while (fgets(line, sizeof(line), fp))
    {
        char u[20], tNo[20], tName[50];
        int c, s;
        if (sscanf(line, "%[^,],%[^,],%[^,],%d,%d", u, tNo, tName, &c, &s) == 5)
        {
            if (strcmp(u, username) == 0 &&
                strcmp(tNo, toCancel.trainNum) == 0 &&
                strcmp(tName, toCancel.trainName) == 0 &&
                c == toCancel.compartment &&
                s == toCancel.seatNum)
            {
                continue; // Skip the canceled ticket
            }
            fprintf(temp, "%s,%s,%s,%d,%d\n", u, tNo, tName, c, s);
        }
    }
    fclose(fp);
    fclose(temp);
    remove("bookings.csv");
    rename("temp.csv", "bookings.csv");

    // Mark seat as unbooked in memory
    for (int i = 0; i < train->trainCount; i++)
    {
        if (strcmp(train->trains[i].trainNumber, toCancel.trainNum) == 0)
        {
            int row = (toCancel.seatNum - 1) / COLS;
            int col = (toCancel.seatNum - 1) % COLS;
            train->trains[i].seatLayout[toCancel.compartment - 1][row][col] = 0;
            train->trains[i].seats++;
            break;
        }
    }

    saveTrainToFile(train);
    printf("✅ Ticket successfully cancelled!\n");
}

