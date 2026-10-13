#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define FILE_NAME   "appointments.dat"
#define MAX_APPT    200
#define NUM_DOCTORS 2
#define NUM_SLOTS   8

typedef struct {
    int  id;
    char name[30];
    char specialty[30];
} Doctor;

typedef struct {
    int  id;
    int  doctorId;
    char patient[40];
    char phone[16];
    char date[11];      /* DD-MM-YYYY */
    int  slot;          /* index of slots[] */
    int  active;        /* 1 = booked, 0 = cancelled */
} Appointment;

/* The two doctors are fixed here. Change names/specialties as you like. */
Doctor doctors[NUM_DOCTORS] = {
    {1, "Dr. Rohit Sharma", "Cardiologist"},
    {2, "Dr. Anita Verma",  "General Dentist"}
};

const char *slots[NUM_SLOTS] = {
    "09:00", "10:00", "11:00", "12:00", "14:00", "15:00", "16:00", "17:00"
};

Appointment list[MAX_APPT];     /* all appointments live in this array */
int count = 0;                  /* how many are stored */

/* ---------------------- input helpers ---------------------- */

void read_line(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    if (!fgets(buf, size, stdin)) {         /* input closed (Ctrl+D / Ctrl+Z) */
        printf("\nGoodbye!\n");
        exit(0);
    }
    size_t n = strlen(buf);
    if (n > 0 && buf[n - 1] == '\n') {
        buf[n - 1] = '\0';
    } else {                                /* too long: throw away the rest */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
}

int read_int(const char *prompt, int min, int max) {
    char buf[32];
    for (;;) {
        read_line(prompt, buf, sizeof buf);
        char *end;
        long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0' && v >= min && v <= max) return (int)v;
        printf("Please enter a number between %d and %d.\n", min, max);
    }
}

void read_phone(char phone[16]) {
    for (;;) {
        read_line("Phone number (10 digits): ", phone, 16);
        int ok = (strlen(phone) == 10);
        for (int i = 0; ok && i < 10; i++)
            if (!isdigit((unsigned char)phone[i])) ok = 0;
        if (ok) return;
        printf("Invalid phone number.\n");
    }
}

/* ---------------------- date helpers ---------------------- */

int days_in_month(int m, int y) {
    int d[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return d[m - 1];
}

/* Returns YYYYMMDD if the date (DD-MM-YYYY) is valid, otherwise 0 */
int date_key(const char *s) {
    if (strlen(s) != 10 || s[2] != '-' || s[5] != '-') return 0;
    for (int i = 0; i < 10; i++)
        if (i != 2 && i != 5 && !isdigit((unsigned char)s[i])) return 0;
    int d = atoi(s), m = atoi(s + 3), y = atoi(s + 6);
    if (y < 2000 || m < 1 || m > 12 || d < 1 || d > days_in_month(m, y)) return 0;
    return y * 10000 + m * 100 + d;
}

int today_key(void) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    return (tm->tm_year + 1900) * 10000 + (tm->tm_mon + 1) * 100 + tm->tm_mday;
}

/* ---------------------- file handling ---------------------- */

void load_data(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return;                        /* first run: no file yet */
    count = (int)fread(list, sizeof(Appointment), MAX_APPT, fp);
    fclose(fp);
}

void save_data(void) {
    FILE *fp = fopen(FILE_NAME, "wb");
    if (!fp) {
        printf("Error: could not save data!\n");
        return;
    }
    fwrite(list, sizeof(Appointment), (size_t)count, fp);
    fclose(fp);
}

/* ---------------------- display helpers ---------------------- */

void show_doctors(void) {
    printf("\nID  Name                 Specialty\n");
    printf("--------------------------------------------\n");
    for (int i = 0; i < NUM_DOCTORS; i++)
        printf("%-3d %-20s %s\n", doctors[i].id, doctors[i].name, doctors[i].specialty);
}

void print_header(void) {
    printf("\n%-4s %-17s %-15s %-11s %-6s %s\n",
           "ID", "Doctor", "Patient", "Date", "Time", "Status");
    printf("-----------------------------------------------------------------\n");
}

void print_row(const Appointment *a) {
    printf("%-4d %-17.17s %-15.15s %-11s %-6s %s\n",
           a->id, doctors[a->doctorId - 1].name, a->patient,
           a->date, slots[a->slot], a->active ? "Booked" : "Cancelled");
}

/* ---------------------- main features ---------------------- */

/* A slot is taken if the same doctor already has an active booking at that date + time */
int slot_taken(int doctorId, const char *date, int slot) {
    for (int i = 0; i < count; i++)
        if (list[i].active && list[i].doctorId == doctorId &&
            list[i].slot == slot && strcmp(list[i].date, date) == 0)
            return 1;
    return 0;
}

void book_appointment(void) {
    if (count >= MAX_APPT) {
        printf("Sorry, appointment list is full.\n");
        return;
    }
    show_doctors();
    int did = read_int("Choose Doctor ID (0 = back): ", 0, NUM_DOCTORS);
    if (did == 0) return;

    Appointment a;
    memset(&a, 0, sizeof a);
    a.doctorId = did;

    do {
        read_line("Patient name: ", a.patient, sizeof a.patient);
    } while (a.patient[0] == '\0');
    read_phone(a.phone);

    for (;;) {
        read_line("Date (DD-MM-YYYY): ", a.date, sizeof a.date);
        int k = date_key(a.date);
        if (k == 0)                printf("Invalid date. Example: 25-12-2026\n");
        else if (k < today_key())  printf("That date is in the past.\n");
        else break;
    }

    printf("\nTime slots for %s on %s:\n", doctors[did - 1].name, a.date);
    for (int s = 0; s < NUM_SLOTS; s++)
        printf(" %d. %s %s\n", s + 1, slots[s],
               slot_taken(did, a.date, s) ? "(booked)" : "");

    int choice = read_int("Choose slot number (0 = back): ", 0, NUM_SLOTS);
    if (choice == 0) return;
    if (slot_taken(did, a.date, choice - 1)) {
        printf("Sorry, that slot is already booked. Try again.\n");
        return;
    }

    a.slot = choice - 1;
    a.active = 1;
    a.id = (count > 0) ? list[count - 1].id + 1 : 1;   /* ids only ever increase */
    list[count++] = a;
    save_data();
    printf("\nAppointment booked! ID: %d | %s | %s at %s\n",
           a.id, doctors[did - 1].name, a.date, slots[a.slot]);
}

/* Prints all appointments of a phone number; returns how many were found */
int show_by_phone(const char *phone) {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].phone, phone) != 0) continue;
        if (found == 0) print_header();
        print_row(&list[i]);
        found++;
    }
    if (found == 0) printf("\nNo appointments found for this phone number.\n");
    return found;
}

void view_my_appointments(void) {
    char phone[16];
    read_phone(phone);
    show_by_phone(phone);
}

void cancel_appointment(void) {
    char phone[16];
    read_phone(phone);
    if (show_by_phone(phone) == 0) return;

    int id = read_int("Enter appointment ID to cancel (0 = back): ", 0, 100000);
    if (id == 0) return;

    for (int i = 0; i < count; i++) {
        if (list[i].id == id && strcmp(list[i].phone, phone) == 0) {
            if (!list[i].active) {
                printf("That appointment is already cancelled.\n");
                return;
            }
            list[i].active = 0;
            save_data();
            printf("Appointment %d cancelled.\n", id);
            return;
        }
    }
    printf("No appointment with that ID found for this phone number.\n");
}

void doctor_schedule(void) {
    show_doctors();
    int did = read_int("Choose Doctor ID (0 = back): ", 0, NUM_DOCTORS);
    if (did == 0) return;

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (list[i].doctorId != did || !list[i].active) continue;
        if (found == 0) print_header();
        print_row(&list[i]);
        found++;
    }
    if (found == 0) printf("\nNo appointments for this doctor yet.\n");
}

/* ---------------------- main ---------------------- */

int main(void) {
    load_data();
    for (;;) {
        printf("\n===== DOCTOR APPOINTMENT SYSTEM =====\n"
               " 1. View doctors\n"
               " 2. Book appointment\n"
               " 3. View my appointments\n"
               " 4. Cancel appointment\n"
               " 5. Doctor's schedule\n"
               " 0. Exit\n");
        switch (read_int("Choose: ", 0, 5)) {
            case 1: show_doctors();         break;
            case 2: book_appointment();     break;
            case 3: view_my_appointments(); break;
            case 4: cancel_appointment();   break;
            case 5: doctor_schedule();      break;
            case 0: printf("Goodbye!\n");   return 0;
        }
    }
}