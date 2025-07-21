#ifndef BOOKING_H
#define BOOKING_H

#include "train.h"

void bookTicket(const char *username, Train *train);
void cancelTicket(const char *username, Train *train);
void viewMyBookings(const char *username);
void displaySeats(const TrainDetails *t);

#endif
