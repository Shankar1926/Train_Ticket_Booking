#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "user.h"
#include "admin.h"
#include "booking.h"
#include "file.h"

#define MAX_USERS 100

User users[MAX_USERS];
int userCount = 0;

void userRegister()
{
    char userId[10], username[20], password[20];

    printf("\n-------------------------\n");
    printf("     User Registration\n");
    printf("-------------------------\n");

    printf("Enter Unique User Id : ");
    scanf(" %[^\n]", userId);

    FILE *fp = fopen("users.csv", "r");
    char line[100], checkId[10], checkUser[20], checkPass[20];

    while (fp && fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%[^,],%[^,],%[^,\n]", checkId, checkUser, checkPass) == 3)
        {
            if (strcmp(userId, checkId) == 0)
            {
                printf("User already exists! Try different User ID.\n");
                fclose(fp);
                return;
            }
        }
    }
    if (fp) fclose(fp);

    printf("Enter Username: ");
    scanf(" %[^\n]", username);

    printf("Enter Password (min 4 characters): ");
    scanf(" %[^\n]", password);

    if (strlen(password) < 4)
    {
        printf("Password too short. Registration failed.\n");
        return;
    }

    fp = fopen("users.csv", "a");
    if (fp == NULL)
    {
        printf("Error saving user.\n");
        return;
    }

    fprintf(fp, "%s,%s,%s\n", userId, username, password);
    fclose(fp);

    printf("✅ User registered successfully with ID: %s\n", userId);
}

int userLogin(Train *train)
{
    char userId[10], username[20], password[20];

    printf("------------------------------\n");
    printf("          User Login\n");
    printf("------------------------------\n");

    printf("Enter User ID    : ");
    scanf(" %[^\n]", userId);

    printf("Enter Password   : ");
    scanf(" %[^\n]", password);

    FILE *fp = fopen("users.csv", "r");
    if (fp == NULL)
    {
        printf("User database not found.\n");
        return 0;
    }

    char line[100], checkId[10], checkName[20], checkPass[20];

    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%[^,],%[^,],%[^\n]", checkId, checkName, checkPass) == 3)
        {
            if (strcmp(userId, checkId) == 0 && strcmp(password, checkPass) == 0)
            {
                fclose(fp);
                strcpy(username, checkName);
                printf("------------------------------\n");
                printf("Welcome, %s! Login successful.\n", username);
                printf("------------------------------\n");

                int choice;
                do
                {
                    printf("\n------------------------------\n");
                    printf("          User Menu\n");
                    printf("------------------------------\n");
                    printf("1. View Trains\n");
                    printf("2. Book Ticket\n");
                    printf("3. Cancel Ticket\n");
                    printf("4. View My Bookings\n");
                    printf("5. Logout\n");

                    printf("Enter your choice : ");
                    scanf("%d", &choice);

                    switch (choice)
                    {
                        case 1: viewAllTrains(train);           break;
                        case 2: bookTicket(userId, train);      break;
                        case 3: cancelTicket(userId, train);    break;
                        case 4: viewMyBookings(userId);         break;
                        case 5: printf("\n---------------------------------\n"); 
                                printf("        Logging out...\n");
                                printf("---------------------------------\n");
                                saveTrainToFile(train);
                                break;
                        default: printf("Invalid choice!\n");
                    }

                } while (choice != 5);

                return 1;
            }
        }
    }

    fclose(fp);
    printf("❌ Invalid User ID or Password!\n");
    return 0;
}