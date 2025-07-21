#ifndef ADMIN_H
#define ADMIN_H

#include "train.h"

void addTrain(Train *train);
void viewAllTrains(Train *train);
int adminLogin(Train *train);
void editTrain( Train *train);
void deleteTrain( Train *train);
void viewBookedTickets();

#endif
