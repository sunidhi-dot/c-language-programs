#include <stdio.h>
#include <string.h>

#define MAX_SEATS 40
#define TOTAL_BUSES 3

struct Bus {
    int busNo;
    char source[30];
    char destination[30];
    char departure[15];
    float fare;
    int seats[MAX_SEATS];
};

struct Bus buses[TOTAL_BUSES] = {
    {101, "Dehradun", "Delhi", "08:00 PM", 500.0, {0}},
    {102, "Delhi", "Jaipur", "10:00 PM", 600.0, {0}},
    {103, "Dehradun", "Chandigarh", "09:00 AM", 400.0, {0}}
};

/* Show all buses */
void viewBuses() {
    printf("\n========== BUS DETAILS ==========\n");

    printf("\nBus No.\tFrom\t\tTo\t\tTime\t\tFare\n");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < TOTAL_BUSES; i++) {
        printf("%d\t%-12s\t%-12s\t%s\tRs. %.2f\n",
               buses[i].busNo,
               buses[i].source,
               buses[i].destination,
               buses[i].departure,
               buses[i].fare);
    }
}

/* Show available seats */
void viewSeats() {
    int busNo, found = 0;

    printf("\nEnter Bus Number: ");
    scanf("%d", &busNo);

    for (int i = 0; i < TOTAL_BUSES; i++) {

        if (buses[i].busNo == busNo) {
            found = 1;

            printf("\n========== SEAT STATUS ==========\n");

            for (int j = 0; j < MAX_SEATS; j++) {

                if (buses[i].seats[j] == 0)
                    printf("[%02d] Available  ", j + 1);
                else
                    printf("[%02d] Booked     ", j + 1);

                if ((j + 1) % 4 == 0)
                    printf("\n");
            }

            break;
        }
    }

    if (!found)
        printf("\nBus not found!\n");
}

/* Main menu */
int main() {
    int choice;

    do {
        printf("\n\n========================================\n");
        printf("   SMART BUS RESERVATION SYSTEM\n");
        printf("========================================\n");

        printf("1. View Bus Details\n");
        printf("2. Check Seat Availability\n");
        printf("3. Book Ticket\n");
        printf("4. Cancel Ticket\n");
        printf("5. Search Ticket\n");
        printf("6. View All Bookings\n");
        printf("7. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                viewBuses();
                break;

            case 2:
                viewSeats();
                break;

            case 3:
                printf("\nTicket Booking feature will be added next.\n");
                break;

            case 4:
                printf("\nTicket Cancellation feature will be added next.\n");
                break;

            case 5:
                printf("\nTicket Search feature will be added next.\n");
                break;

            case 6:
                printf("\nBooking Records feature will be added next.\n");
                break;

            case 7:
                printf("\nThank you for using Smart Bus Reservation System!\n");
                break;

            default:
                printf("\nInvalid choice! Please try again.\n");
        }

    } while (choice != 7);

    return 0;
}