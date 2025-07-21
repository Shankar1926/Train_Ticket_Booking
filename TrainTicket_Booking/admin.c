#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "admin.h"
#include "train.h"
#include "file.h"

int isAlphaOnly(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isalpha(str[i]) && !isspace(str[i]))
            return 0;
    }
    return 1;
}

void trainDisplayHeader()
{
    printf("\n--------------------------------------------------------------------------------------\n");
    printf("%-10s %-20s %-15s %-15s %-15s %-10s\n", 
           "Train No", "Train Name", "Source", "Destination", "Time", "Seats");
    printf("--------------------------------------------------------------------------------------\n");
}

int adminLogin(Train *train)
{
    char username[20], password[20];
    printf("------------------------------\n");
    printf("Enter Username : ");
    scanf(" %[^\n]", username);
    printf("Enter Password : ");
    scanf(" %[^\n]", password);

    if (strcmp(username, "admin") == 0 && strcmp(password, "123") == 0)
    {
        printf("------------------------------\n");
        printf("✅ Admin Login Successful\n");
        printf("------------------------------\n");

        int choice;
        do
        {
            printf("\n----------------------------\n");
            printf("      Admin Dashboard\n");
            printf("----------------------------\n");
            printf("1. Add Train\n");
            printf("2. View All Trains\n");
            printf("3. View All Booked Tickets\n");
            printf("4. Edit Train\n");
            printf("5. Delete Train\n");
            printf("6. Logout\n");

            printf("Enter your choice: ");
            scanf("%d", &choice);
            switch (choice)
            {
                case 1: addTrain(train); break;
                case 2: viewAllTrains(train); break;
                case 3: viewBookedTickets(); break;
                case 4: editTrain(train); break;
                case 5: deleteTrain(train); break;
                case 6: printf("\n---------------------------------\n"); 
                        printf("        Logging out...\n");
                        printf("---------------------------------\n"); break;
                default: printf("Invalid choice!\n");
            } 
        }while (choice != 6);
    }
    else
    {
        printf("❌ Invalid admin credentials\n");
    }
    return 0;
}

void addTrain(Train *train)
{
    if (train->trainCount >= 100)
    {
        printf("\nTrain limit reached. Cannot add more trains.\n");
        return;
    }
    printf("------------------------------\n");
    printf("  Enter the Train Details\n");
    printf("------------------------------\n");
    char trainNumber[20];
    printf("Enter Train Number                   : ");
    scanf(" %[^\n]", trainNumber);

    for (int i = 0; i < train->trainCount; i++)
    {
        if (strcmp(train->trains[i].trainNumber, trainNumber) == 0)
        {
            printf("----------------------------------------------------------\n");
            printf("    Train number already exists. Try a different one.\n");
            printf("----------------------------------------------------------\n");
            return;
        }
    }

    TrainDetails *t = &train->trains[train->trainCount];
    strcpy(t->trainNumber, trainNumber);

    printf("Enter Train Name                     : ");
    scanf(" %[^\n]", t->trainName);

    while (1)
    {
        printf("Enter Source Station                 : ");
        scanf(" %[^\n]", t->source);
        if (!isAlphaOnly(t->source))
            printf("Source must contain only letters. Try again.\n");
        else
            break;
    }

    while (1)
    {
        printf("Enter Destination Station            : ");
        scanf(" %[^\n]", t->destination);
        if (!isAlphaOnly(t->destination))
            printf("Destination must contain only letters. Try again.\n");
        else if (strcmp(t->source, t->destination) == 0)
            printf("Source and Destination cannot be the same. Try again.\n");
        else
            break;
    }

    char timeStr[20];
    int hh, mm;
    while (1)
    {
        printf("Enter Departure Time (HH:MM)         : ");
        scanf(" %[^\n]", timeStr);
        if (sscanf(timeStr, "%2d:%2d", &hh, &mm) == 2 && hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59)
        {
            strcpy(t->time, timeStr);
            break;
        }
        else
        {
            printf("Invalid time format. Use HH:MM (24-hour).\n");
        }
    }

    while (1)
    {
        printf("Enter number of compartments (max %d): ", MAX_COMPARTMENTS);
        scanf("%d", &t->compartments);
        if (t->compartments > 0 && t->compartments <= MAX_COMPARTMENTS)
            break;
        else
            printf("Invalid compartment count. Must be between 1 and %d.\n", MAX_COMPARTMENTS);
    }

    t->seats = t->compartments * ROWS * COLS;

    for (int c = 0; c < t->compartments; c++)
        for (int i = 0; i < ROWS; i++)
            for (int j = 0; j < COLS; j++)
                t->seatLayout[c][i][j] = 0;

    train->trainCount++;
    printf("---------------------------------------------------------------------\n");
    printf("   Train added successfully with %d compartments and %d total seats.\n", t->compartments, t->seats);
    printf("---------------------------------------------------------------------\n");
}

void viewAllTrains(Train *train)
{
    if (train->trainCount == 0)
    {
        printf("No trains available.\n");
        return;
    }

    trainDisplayHeader();

    for (int i = 0; i < train->trainCount; i++)
    {
        TrainDetails t = train->trains[i];
        printf("%-10s %-20s %-15s %-15s %-15s %-10d\n",
               t.trainNumber, t.trainName, t.source, t.destination, t.time, t.seats);
    }
    printf("--------------------------------------------------------------------------------------\n");
}

void viewBookedTickets()
{
    FILE *fp = fopen("bookings.csv", "r");
    if (fp == NULL)
    {
        printf("No booking records found.\n");
        return;
    }

    typedef struct {
        char username[20];
        char trainNum[20];
        char trainName[50];
        int compartment;
        char seatList[100];
    } BookingGroup;

    BookingGroup bookings[1000];
    int groupCount = 0;

    char line[200];
    while (fgets(line, sizeof(line), fp))
    {
        char fileUser[20], trainNum[20], trainName[50];
        int comp, seat;

        if (sscanf(line, "%[^,],%[^,],%[^,],%d,%d", fileUser, trainNum, trainName, &comp, &seat) == 5)
        {
            // Check for existing group match
            int found = 0;
            for (int i = 0; i < groupCount; i++)
            {
                if (strcmp(bookings[i].username, fileUser) == 0 &&
                    strcmp(bookings[i].trainNum, trainNum) == 0 &&
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
                strcpy(bookings[groupCount].username, fileUser);
                strcpy(bookings[groupCount].trainNum, trainNum);
                strcpy(bookings[groupCount].trainName, trainName);
                bookings[groupCount].compartment = comp;
                sprintf(bookings[groupCount].seatList, "%d", seat);
                groupCount++;
            }
        }
    }

    fclose(fp);

    printf("\n------------------------- All Booked Tickets -------------------------\n");
    printf("%-16s %-12s %-20s %-12s %-s\n", "Username", "Train No", "Train Name", "Compartment", "Seat No(s)");
    printf("-------------------------------------------------------------------------\n");

    if (groupCount == 0)
    {
        printf("No booking records found.\n");
        return;
    }

    char prevUser[20] = "";
    char prevTrain[20] = "";
    char prevTrainName[50] = "";

    for (int i = 0; i < groupCount; i++)
    {
        int isNewGroup =
            strcmp(prevUser, bookings[i].username) != 0 ||
            strcmp(prevTrain, bookings[i].trainNum) != 0 ||
            strcmp(prevTrainName, bookings[i].trainName) != 0;

        if (isNewGroup)
        {
            printf("%-16s %-12s %-20s C%d          %-s\n",
                   bookings[i].username,
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

        strcpy(prevUser, bookings[i].username);
        strcpy(prevTrain, bookings[i].trainNum);
        strcpy(prevTrainName, bookings[i].trainName);
    }

    printf("-------------------------------------------------------------------------\n");
}

void editTrain(Train *train)
{
    if (train->trainCount == 0)
    {
        printf("No trains available to edit.\n");
        return;
    }

    viewAllTrains(train);

    char editNumber[20];
    printf("Enter the Train Number to edit       : ");
    scanf(" %[^\n]", editNumber);

    int found = 0;
    for (int i = 0; i < train->trainCount; i++)
    {
        if (strcmp(train->trains[i].trainNumber, editNumber) == 0)
        {
            found = 1;
            TrainDetails *t = &train->trains[i];

            printf("\nEditing Train: %s - %s\n", t->trainNumber, t->trainName);
            printf("Leave input empty to keep existing value.\n");

            char temp[50];

            printf("Train Name (%s):                       ", t->trainName);
            scanf(" %[^\n]", temp);
            if (strlen(temp) > 0)
                strcpy(t->trainName, temp);

            printf("Source (%s)                          : ", t->source);
            scanf(" %[^\n]", temp);
            if (strlen(temp) > 0)
                strcpy(t->source, temp);

            printf("Destination (%s)                     : ", t->destination);
            scanf(" %[^\n]", temp);
            if (strlen(temp) > 0)
                strcpy(t->destination, temp);

            printf("Departure Time (%s)                  : ", t->time);
            scanf(" %[^\n]", temp);
            if (strlen(temp) > 0)
                strcpy(t->time, temp);

            printf("Enter new number of compartments (current: %d, max: %d): ", t->compartments, MAX_COMPARTMENTS);
            int newComp;
            scanf("%d", &newComp);

            if (newComp > t->compartments && newComp <= MAX_COMPARTMENTS)
            {
                // Increase compartments: initialize new ones
                for (int c = t->compartments; c < newComp; c++)
                    for (int r = 0; r < ROWS; r++)
                        for (int col = 0; col < COLS; col++)
                            t->seatLayout[c][r][col] = 0;

                t->compartments = newComp;
                printf("✅ Compartments increased to %d\n", newComp);
            }
            else if (newComp < t->compartments)
            {
                // Check if any bookings exist in compartments to be removed
                int blocked = 0;
                for (int c = newComp; c < t->compartments; c++)
                    for (int r = 0; r < ROWS; r++)
                        for (int col = 0; col < COLS; col++)
                            if (t->seatLayout[c][r][col] == 1)
                                blocked = 1;

                if (blocked)
                {
                    printf("❌ Cannot reduce compartments. Booked seats exist in compartments beyond %d.\n", newComp);
                }
                else
                {
                    t->compartments = newComp;
                    printf("✅ Compartments reduced to %d\n", newComp);
                }
            }

            // Recalculate seat count
            t->seats = 0;
            for (int c = 0; c < t->compartments; c++)
                for (int r = 0; r < ROWS; r++)
                    for (int col = 0; col < COLS; col++)
                        if (t->seatLayout[c][r][col] == 0)
                            t->seats++;

            printf("✅ Train updated successfully.\n");
            return;
        }
    }

    if (!found)
        printf("Train not found.\n");
}


void deleteTrain(Train *train)
{
    viewAllTrains(train); 
    char trainNumber[20];
    printf("Enter Train Number to delete: ");
    scanf(" %[^\n]", trainNumber);
    int found = 0;
    for (int i = 0; i < train->trainCount; i++)
    {
        if (strcmp(train->trains[i].trainNumber, trainNumber) == 0)
        {
            found = 1;

            // Delete from train list
            for (int j = i; j < train->trainCount - 1; j++)
                train->trains[j] = train->trains[j + 1];

            train->trainCount--;

            printf("✅ Train deleted successfully.\n");

            // Now remove related bookings
            FILE *fp = fopen("bookings.csv", "r");
            FILE *temp = fopen("temp.csv", "w");

            if (fp && temp)
            {
                char line[200], user[20], trainNo[20], trainName[50];
                int comp, seat;
                while (fgets(line, sizeof(line), fp))
                {
                    if (sscanf(line, "%[^,],%[^,],%[^,],%d,%d", user, trainNo, trainName, &comp, &seat) == 5)
                    {
                        if (strcmp(trainNo, trainNumber) != 0)
                            fprintf(temp, "%s,%s,%s,%d,%d\n", user, trainNo, trainName, comp, seat);
                    }
                }
                fclose(fp);
                fclose(temp);
                remove("bookings.csv");
                rename("temp.csv", "bookings.csv");
            }

            saveTrainToFile(train);
            return;
        }
    }

    if (!found)
        printf("❌ Train not found.\n");
}


