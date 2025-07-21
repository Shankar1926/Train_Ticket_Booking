🚆 Train Ticket Booking System (C Project)
A modular and fully functional Train Ticket Booking System implemented in the C programming language. This system allows multiple users to register, book, view, and cancel train tickets with support for compartments, seat layouts, and persistent file storage.

📌 Features
👤 User Module
User Registration and Login (credentials saved in users.csv)

View all available trains

Book ticket by selecting train, compartment, and seat

Cancel booked ticket with user-friendly selection

View all bookings grouped by train and compartment

🛠️ Admin Module
Admin Login (hardcoded for demo: username = shankar, password = 123)

Add new trains with:

Train Number, Name, Source, Destination, Time

Custom compartment count (each with 5x4 seat layout)

View all trains

View all booked tickets by all users (grouped by train and compartment)

Edit Train details (name, source, destination, time, compartment count)

Delete Train and automatically remove all related bookings

💾 Persistent Storage
train_details.csv – stores train metadata

bookings.csv – stores booking details (user-wise)

users.csv – stores user login data

 Compilation & Run Instructions
🔧 Compile
gcc main.c admin.c user.c booking.c file.c -o train_booking
▶️ Run
./train_booking

📂 Future Improvements
Add seat class (e.g., Sleeper, AC, General)

Encrypt user passwords

Implement train route filtering

Add departure date and time for each train

Support for viewing ticket PDF receipts
