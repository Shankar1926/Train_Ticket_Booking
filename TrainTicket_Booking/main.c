#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user.h"
#include "admin.h"
#include "train.h"
#include "file.h"

int main()
{
    Train train;
    train.trainCount = 0;

    loadTrainFromFile(&train);

    int choice;

    do
    {
        printf("\n------------------------------\n");
        printf("      Train Ticket Booking\n");
        printf("------------------------------\n");

        printf("1. User Login\n");
        printf("2. User Register\n");
        printf("3. Admin Login\n");
        printf("4. Exit\n");

        printf("------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                userLogin(&train);
                break;

            case 2:
                userRegister();
                break;

            case 3:
                adminLogin(&train);
                break;

            case 4:
                printf("Thank You\n");
                saveTrainToFile(&train);
                break;

            default:
                printf("Invalid choice! Try again.\n");
        }

    } while (choice != 4);

    return 0;
}
