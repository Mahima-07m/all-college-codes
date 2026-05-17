#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef struct {
    int id;
    char title[80];
    char author[60];
    char genre[40];
    int isIssued;
    char issuedTo[60];
    char dueDate[11];
} Book;

Book library[100];
int bookCount = 0;

void saveLibrary() {
    FILE *fp = fopen("library.dat", "wb");
    if (fp != NULL) {
        fwrite(&bookCount, sizeof(int), 1, fp);
        fwrite(library, sizeof(Book), bookCount, fp);
        fclose(fp);
    }
}

void loadLibrary() {
    FILE *fp = fopen("library.dat", "rb");
    if (fp != NULL) {
        fread(&bookCount, sizeof(int), 1, fp);
        fread(library, sizeof(Book), bookCount, fp);
        fclose(fp);
    }
}

void read_line(char *str, int size) {
    fgets(str, size, stdin);
    str[strcspn(str, "\n")] = '\0';
}

void addBook() {
    Book b;
    b.id = bookCount + 1;
    printf("Enter title: "); read_line(b.title, 80);
    printf("Enter author: "); read_line(b.author, 60);
    printf("Enter genre: "); read_line(b.genre, 40);
    b.isIssued = 0;
    library[bookCount++] = b;
    saveLibrary();
    printf("Book added successfully!\n");
}

void listBooks() {
    printf("ID  Title\tAuthor\tGenre\tStatus\n");
    for (int i = 0; i < bookCount; i++) {
        printf("%-3d %-20s %-15s %-10s %s\n",
               library[i].id, library[i].title, library[i].author,
               library[i].genre, library[i].isIssued ? "Issued" : "Available");
    }
}

int findBookById(int id) {
    for (int i = 0; i < bookCount; i++) {
        if (library[i].id == id) return i;
    }
    return -1;
}

void addReview(int id) {
    char fname[40];
    sprintf(fname, "reviews_%d.txt", id);
    FILE *fp = fopen(fname, "a");
    if (fp == NULL) {
        printf("Error opening file %s\n", fname);
        return;
    }
    char review[200];
    printf("Enter review: ");
    read_line(review, 200);
    fprintf(fp, "%s\n", review);
    fclose(fp);
    printf("Review added successfully.\n");
}

void showReviews(int id) {
    char fname[40];
    sprintf(fname, "reviews_%d.txt", id);
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("No reviews yet for this book.\n");
        return;
    }
    char line[200];
    printf("Reviews:\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("- %s", line);
    }
    fclose(fp);
}

void searchBook() {
    int id;
    printf("Enter Book ID to search: ");
    scanf("%d", &id); getchar();
    int index = findBookById(id);
    if (index == -1) {
        printf("Book not found.\n");
        return;
    }
    Book b = library[index];
    printf("ID: %d\nTitle: %s\nAuthor: %s\nGenre: %s\nStatus: %s\n",
           b.id, b.title, b.author, b.genre, b.isIssued ? "Issued" : "Available");
    showReviews(b.id);
}

void issueBook() {
    int id;
    printf("Enter Book ID to issue: ");
    scanf("%d", &id); getchar();
    int index = findBookById(id);
    if (index == -1 || library[index].isIssued) {
        printf("Book not available.\n");
        return;
    }
    printf("Enter borrower name: ");
    read_line(library[index].issuedTo, 60);
    printf("Enter due date (YYYY-MM-DD): ");
    read_line(library[index].dueDate, 11);
    library[index].isIssued = 1;
    saveLibrary();
    printf("Book issued successfully!\n");
}

void recommendBooks(Book b) {
    printf("\nRecommendations:\n");
    int found = 0;
    for (int i = 0; i < bookCount; i++) {
        if (!library[i].isIssued && strcmp(library[i].author, b.author) == 0 && library[i].id != b.id) {
            printf("- %s (%s)\n", library[i].title, library[i].author);
            found = 1;
        }
    }
    if (!found) {
        for (int i = 0; i < bookCount; i++) {
            if (!library[i].isIssued && strcmp(library[i].genre, b.genre) == 0 && library[i].id != b.id) {
                printf("- %s (%s)\n", library[i].title, library[i].author);
            }
        }
    }
}

int calculateFine(char dueDate[], char returnDate[])
{
    struct tm due = {0}, ret = {0} ;
    scanf (dueDate, "%Y-%m-%d",&due);
    scanf (returnDate, "%Y-%m-%d",&ret);
    time_t due_time = mktime(&due);
    time_t ret_time = mktime(&ret);

    double diff= difftime(ret_time, due_time)/(60*60*24);
    if (diff>0)
    {

        return (int)diff * 10;//10rs per extra day
    }
    return 0;
}

void returnBook() {
    int id;
    printf("Enter Book ID to return: ");
    scanf("%d", &id); getchar();

    int index = findBookById(id);
    if (index == -1 || !library[index].isIssued) {
        printf("Invalid Book ID.\n");
        return;
    }

    int dy, dm, dd;
    sscanf(library[index].dueDate, "%d-%d-%d", &dy, &dm, &dd);

    struct tm due = {0}, ret = {0};
    due.tm_year = dy - 1900;
    due.tm_mon  = dm - 1;
    due.tm_mday = dd;

    int ry, rm, rd;
    printf("Enter actual return date (YYYY-MM-DD): ");
    scanf("%d-%d-%d", &ry, &rm, &rd); getchar();

    ret.tm_year = ry - 1900;
    ret.tm_mon  = rm - 1;
    ret.tm_mday = rd;

    time_t dueTime = mktime(&due);
    time_t retTime = mktime(&ret);

    double diff = difftime(retTime, dueTime) / (60 * 60 * 24);

    if (diff > 0) {
        printf("Returned %d days late. FINE = Rs %d\n", (int)diff, (int)diff * 10);
    } else {
        printf("Book returned on time. No Fine :)\n");
    }

    library[index].isIssued = 0;
    library[index].issuedTo[0] = '\0';
    library[index].dueDate[0] = '\0';

    printf("Enter review for this book: ");
    addReview(id);

    recommendBooks(library[index]);

    saveLibrary();
    printf("Book returned successfully! THANK YOU :)\n");
}

void deleteBook() {
    int id;
    printf("Enter Book ID to delete: ");
    scanf("%d", &id); getchar();
    int index = findBookById(id);
    if (index == -1) {
        printf("Book not found.\n");
        return;
    }
    for (int i = index; i < bookCount - 1; i++) {
        library[i] = library[i + 1];
    }
    bookCount--;
    saveLibrary();
    printf("Book deleted successfully!\n");
}

int main() {
    loadLibrary();
    int choice;
    do {
        printf("\n====================================\n");
        printf("  Library Management System - Menu\n");
        printf("====================================\n");
        printf("1. Add Book\n");
        printf("2. List Books\n");
        printf("3. Search Book\n");
        printf("4. Issue Book\n");
        printf("5. Return Book\n");
        printf("6. Delete Book\n");
        printf("0. Exit\n");
        printf("------------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice); getchar();

        switch (choice) {
            case 1: addBook(); break;
            case 2: listBooks(); break;
            case 3: searchBook(); break;
            case 4: issueBook(); break;
            case 5: returnBook(); break;
            case 6: deleteBook(); break;
            case 0: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 0);

    return 0;
}
