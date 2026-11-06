#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Structure for expense categories
struct ExpenseCategory {
    char name[20];
    float amount;
    int priority;
};

// Structure for user information
struct User {
    char username[50];
    char password[50];
    char fullname[100];
    int age;
};

// Static categories
struct ExpenseCategory categories[7] = {
    {"Rent", 0.0f, 0},
    {"Groceries", 0.0f, 0},
    {"Insurance", 0.0f, 0},
    {"Education", 0.0f, 0},
    {"Entertainment", 0.0f, 0},
    {"Others", 0.0f, 0},
    {"Income", 0.0f, 0}
};

const int count = 7;
bool usedPriorities[7] = { false };

bool fileExists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

void getCategoryDetails() {
    printf("\nEnter your monthly income and expenses for each category:\n");

    for (int i = 0; i < count; i++) {
        printf("%s: ", categories[i].name);
        scanf("%f", &categories[i].amount);

        if (i < count - 1) {
            int p;
            do {
                printf("Priority (1-%d, unique): ", count - 1);
                scanf("%d", &p);
            } while (p < 1 || p > count - 1 || usedPriorities[p - 1]);

            usedPriorities[p - 1] = true;
            categories[i].priority = p;
        } else {
            categories[i].priority = count;
        }
    }
}

void sortCategories() {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count - 1; j++) {
            if (categories[j].priority < categories[i].priority) {
                struct ExpenseCategory temp = categories[i];
                categories[i] = categories[j];
                categories[j] = temp;
            }
        }
    }
}

void saveToFile(const char *fileName, float totalExpenses) {
    FILE *file = fopen(fileName, "a");
    float income = categories[count - 1].amount;

    fprintf(file, "\n--- Monthly Budget ---\n");
    fprintf(file, "Total Budget: %.2f\n", income);
    for (int i = 0; i < count - 1; i++) {
        fprintf(file, "%s: %.2f (Priority: %d)\n", categories[i].name, categories[i].amount, categories[i].priority);
    }
    fprintf(file, "Total Expenses: %.2f\n", totalExpenses);
    fclose(file);
}

void analyzeBudget(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (!file) return;

    char line[256];
    float income = 0, expenses = 0;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "Total Budget: %f", &income) == 1) continue;
        if (sscanf(line, "Total Expenses: %f", &expenses) == 1) continue;
    }
    fclose(file);

    printf("\n\U0001F4CA Budget Summary:\nBudget: %.2f\nExpenses: %.2f\n", income, expenses);
    if (expenses > income)
        printf("\u26A0\uFE0F Overspending by %.2f\n", expenses - income);
    else
        printf("\u2705 You're within budget!\n");

    FILE *temp = fopen(".current_user", "w");
    fprintf(temp, "%s", fileName);
    fclose(temp);
    system("python3 orion_advice.py");
}

void loadCategoryDetails(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (!file) {
        printf("\u274C Could not open file for reading expenses.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        for (int i = 0; i < count - 1; i++) {
            sscanf(line, "%[^:]: %f (Priority: %d)", categories[i].name, &categories[i].amount, &categories[i].priority);
        }
        sscanf(line, "Total Budget: %f", &categories[count - 1].amount);
    }

    fclose(file);
}

void dataManipulation(const char *fileName) {
    getCategoryDetails();
    sortCategories();
    float total = 0;
    for (int i = 0; i < count - 1; i++) total += categories[i].amount;
    saveToFile(fileName, total);

    char choice;
    printf("\nRun Orion AI advisor? (Y/N): ");
    scanf(" %c", &choice);
    if (tolower(choice) == 'y') {
        analyzeBudget(fileName);
    }
}

void YesAccount(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("\u274C Error: Could not open user file.\n");
        return;
    }

    struct User user;
    char inputPassword[50];

    if (fscanf(file, "%49s %49s \"%99[^\"]\" %d", user.username, user.password, user.fullname, &user.age) != 4) {
        printf("\u274C Error reading user data. Ensure the file is correctly formatted.\n");
        fclose(file);
        return;
    }

    fclose(file);

    printf("\U0001F511 Enter your password: ");
    fgets(inputPassword, sizeof(inputPassword), stdin);
    inputPassword[strcspn(inputPassword, "\n")] = '\0';

    if (strcmp(user.password, inputPassword) != 0) {
        printf("\u274C Invalid password.\n");
        return;
    }

    printf("\u2705 Welcome to Orion, %s!\n", user.fullname);
    loadCategoryDetails(fileName);
    analyzeBudget(fileName);
}

void NewAccount(const char *fileName) {
    FILE *file = fopen(fileName, "w");
    if (!file) {
        printf("\u274C Could not create file.\n");
        return;
    }

    struct User user;
    printf("\nCreate a new Orion account:\n");

    printf("Username: ");
    scanf("%s", user.username);
    getchar();

    printf("Password: ");
    scanf("%s", user.password);
    getchar();

    printf("Full name: ");
    fgets(user.fullname, sizeof(user.fullname), stdin);
    user.fullname[strcspn(user.fullname, "\n")] = '\0';

    printf("Age: ");
    scanf("%d", &user.age);
    getchar();

    fprintf(file, "%s %s \"%s\" %d\n", user.username, user.password, user.fullname, user.age);
    fclose(file);
    printf("\u2705 Account created successfully.\n");
    dataManipulation(fileName);
}

int main() {
    char filename[100];
    char choice;

    printf("Welcome to Orion Budget Manager!\n");
    printf("Do you have an existing account? (Y/N): ");
    scanf(" %c", &choice);
    getchar();

    printf("Enter your Orion username: ");
    scanf("%s", filename);
    getchar();

    strcat(filename, ".txt");

    if (tolower(choice) == 'y') {
        if (!fileExists(filename)) {
            printf("\u274C Account does not exist.\n");
            return 1;
        }
        YesAccount(filename);
    } else if (tolower(choice) == 'n') {
        if (fileExists(filename)) {
            printf("\u26A0\uFE0F That username is taken. Try another.\n");
            return 1;
        }
        NewAccount(filename);
    } else {
        printf("\u274C Invalid option.\n");
    }

    return 0;
}