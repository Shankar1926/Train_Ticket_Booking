#ifndef USER_H
#define USER_H

#include "train.h"

typedef struct {
    char userId[10];
    char username[20];
    char password[20];
} User;

void userRegister();
int userLogin(Train *train);

#endif
