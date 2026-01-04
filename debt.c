#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>


#define FILENAME "debts.txt"
#define TEMP_FILENAME "temp.txt"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"


struct Debt {
    char id[10];
    char name[50];
    float total;
    float remaining;
};


void menu();
void addDebt();
void showAllDebts();
void showUserDebt();
void simulateRepayment();
void makePayment();
void removeDebt();


int main() {
    srand(time(NULL));

    int choice = -1;
    char term;

    while (choice != 0) {
        menu();
        
        while (true) {
            if (scanf("%d%c", &choice, &term) != 2 || term != '\n') {
                printf(RED "Моля, въведете само число!\n" RESET);
                while (getchar() != '\n');

                printf("Избери опция: ");
                continue;
            }

            if (choice < 0 || choice > 6) {
                printf(RED "Числото трябва да е между 0 и 6!\n" RESET);

                printf("Избери опция: ");
                continue;
            }

            break;      
        }

        switch (choice) {
            case 1:
                addDebt();
                break;
            case 2:
                showAllDebts();
                break;
            case 3:
                showUserDebt();
                break;
            case 4:
                simulateRepayment();
                break;
            case 5:
                makePayment();
                break;
            case 6:
                removeDebt();
                break;
            case 0:
                printf(RED "Излизане от програмата...\n\n" RESET);
                break;
        }
    }

    return 0;
}


bool isValidName(char *name) {
    if (strlen(name) < 2) {
        return false;
    }

    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i])) {
            return false;
        }
    }

    return true;
}

bool isValidAmount(char *amount) {
    int dotCount = 0;

    if (strlen(amount) == 0) {
        return false;
    }

    for (int i = 0; amount[i] != '\0'; i++) {
        if (amount[i] == '.') {
            dotCount++;
            if (dotCount > 1) {
                return false;
            }
        }
        else if (!isdigit(amount[i])) {
            return false;
        }
    }

    return true;
}

bool isValidID(char *id) {
    if (id == NULL || id[0] == '\0') {
        return false;
    }

    if (strlen(id) != 4) {
        return false; 
    }

    for (int i = 0; id[i] != '\0'; i++) {
        if (!isdigit(id[i])) {
            return false;
        }
    }

    return true;
}

bool doesIDExist(char *id) {
    struct Debt d;
    bool found = false;

    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        return false;
    }

    while (fscanf(file, "%s %s %f %f", d.id, d.name, &d.total, &d.remaining) == 4) {
        if (strcmp(d.id, id) == 0) {
            found = true;
            break;
        }
    }

    fclose(file);
    return found;
}

void getValidUserID(char *id) {
    while (true) {
        printf("Напишете ID-то на вашия заем: ");
        scanf("%s", id);

        if (!isValidID(id)) {
            printf(RED "Моля въведете валидно ID!\n" RESET);
            continue;
        }

        if (doesIDExist(id)) {
            break;
        } 
        else {
            printf(RED "Няма заем с ID %s! Опитайте отново.\n" RESET, id);
        }
    }
}


void menu() {
    printf("==== ПЛАНЕР ЗА ИЗПЛАЩАНЕ НА ДЪЛГОВЕ ====\n");
    printf("1. Добави нов заем\n");
    printf("2. Покажи всички заеми\n");
    printf("3. Покажи заем на потребител\n");
    printf("4. Симулация на погасяване на заем\n");
    printf("5. Направи плащане\n");
    printf("6. Изтрий заем\n");
    printf("0. Изход\n");
    printf("========================================\n");
    printf("Избери опция: ");
}

void addDebt() {
    struct Debt d;
    char generateID[10];
    char strAmount[50];

    do { 
        int randomNum = rand() % 10000; 
        sprintf(generateID, "%04d", randomNum); 
    } while (doesIDExist(generateID));

    strcpy(d.id, generateID); 

    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        printf(RED "Грешка при отваряне на файла!\n\n" RESET);
        return;
    }

    while (true) {
        printf("Име: ");
        scanf("%s", d.name);

        if (isValidName(d.name)) {
            break;
        } 
        else {
            printf(RED "Невалидно име! Моля използвайте само букви.\n" RESET);
        }
    }

    while (true) {
        printf("Въведи обща сума: ");
        scanf("%s", strAmount);

        if (isValidAmount(strAmount)) {
            d.total = atof(strAmount);
            if (d.total > 0) {
                break;
            }
            printf(RED "Сумата трябва да е по-голяма от 0!\n" RESET);
        } 
        else {
            printf(RED "Моля въведете валидно число.\n" RESET);
        }
    }

    d.remaining = d.total;

    fprintf(file, "%s %s %.2f %.2f\n", d.id, d.name, d.total, d.remaining);
    fclose(file);

    printf(GREEN "\nВашето id е %s\n" RESET, d.id);
    printf(GREEN "Заемът е добавен успешно!\n\n" RESET);
}

void showAllDebts() {
    struct Debt d;
    FILE *file = fopen(FILENAME, "r");

    if (file == NULL) {
        printf(RED "Грешка при отваряне на файла!\n\n" RESET);
        return;
    }

    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        printf(RED "Списъкът със заеми е празен :(\n\n" RESET);
        fclose(file);
        return;
    }

    rewind(file);
    printf("\n%-6s %-15s %-20s %-20s\n", "ID", "Име", "Обща сума", "Оставаща сума");
    printf("----------------------------------------------\n");

    while (fscanf(file, "%s %s %f %f", d.id, d.name, &d.total, &d.remaining) != EOF) {
        printf("%-6s %-12s %-12.2f %-12.2f\n", d.id, d.name, d.total, d.remaining);
    }

    fclose(file);
    printf(GREEN "Списъкът със заеми беше показан успешно.\n\n" RESET);
}

void showUserDebt() {
    struct Debt d;
    char userID[10];

    getValidUserID(userID);

    FILE *file = fopen(FILENAME, "r");

    if (file == NULL) {
        printf(RED "Грешка при отваряне на файла!\n\n" RESET);
        return;
    }

    printf("\n%-6s %-15s %-20s %-20s\n", "ID", "Име", "Обща сума", "Оставаща сума");
    printf("----------------------------------------------\n");

    while (fscanf(file, "%s %s %f %f", d.id, d.name, &d.total, &d.remaining) == 4) {
        if (strcmp(d.id, userID) == 0) {
            printf("%-6s %-12s %-12.2f %-12.2f\n", d.id, d.name, d.total, d.remaining);
            printf(GREEN "Заемът беше намерен успешно!\n\n" RESET);
            break;
        }
    }

    fclose(file);
}

void simulateRepayment() {
    struct Debt d;
    char userID[10];
    char strPayment[50];
    float payment;

    getValidUserID(userID);

    while (true) {
        printf("Напишете каква сума искате да внасяте всеки месец: ");
        scanf("%s", strPayment);

        if (isValidAmount(strPayment)) {
            payment = atof(strPayment);

            if (payment > 0) {
                break;
            } 
            else {
                printf(RED "Вноската трябва да е по-голяма от 0!\n" RESET);
            }
        } 
        else {
            printf(RED "Невалидна сума!\n" RESET);
        }
    }


    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf(RED "Грешка при отваряне на файла!\n\n" RESET);
        return;
    }

    while (fscanf(file, "%s %s %f %f", d.id, d.name, &d.total, &d.remaining) == 4) {
        if (strcmp(d.id, userID) == 0) {
            break;
        }
    }
    fclose(file);

    if (d.remaining <= 0) {
        printf(GREEN "Този дълг вече е изплатен!\n\n" RESET);
        return;
    }

    int months = 0;
    float currentDebt = d.remaining;

    while (currentDebt > 0) {
        currentDebt -= payment;
        months++;
    }

    printf("\nИме: %s\n", d.name);
    printf("Оставаща сума: %.2f €\n", d.remaining);
    printf("При вноска от %.2f € на месец:\n", payment);
    printf(GREEN "-> Ще изплатите дълга за %d месеца.\n\n" RESET, months);
}

void makePayment() {
    struct Debt d;
    char userID[10];
    char strPayment[50];
    float payment;

    getValidUserID(userID);

    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Грешка при отваряне на файла.\n");
        return;
    }

    FILE *tempFile = fopen(TEMP_FILENAME, "w");
    if (tempFile == NULL) {
        printf("Грешка при създаване на временен файл.\n");
        fclose(file);
        return;
    }

    while (fscanf(file, "%s %s %f %f", d.id, d.name, &d.total, &d.remaining) == 4) {
        if (strcmp(d.id, userID) == 0) {
            printf("\nЗдравейте, %s!\n", d.name);
            printf("Текущ остатък: %.2f €\n", d.remaining);

            while (true) {
                printf("\nКолко искате да изплатите? ");
                scanf("%s", strPayment);

                if (isValidAmount(strPayment)) {
                    payment = atof(strPayment);

                    if (payment > 0) {
                        break;
                    } 
                    else {
                        printf(RED "Сумата трябва да е по-голяма от 0!\n" RESET);
                    }
                } 
                else {
                    printf(RED "Невалидна сума!\n" RESET);
                }
            }

            d.remaining -= payment;

            if (d.remaining <= 0) {
                float change = -d.remaining;
                if (change > 0) {
                    printf(GREEN "Дългът е изплатен! Имате ресто от %.2f €.\n\n" RESET, change);
                }
                else {
                    printf(GREEN "Дългът е изплатен точно!\n\n" RESET);
                }
                d.remaining = 0;
            } 
            else {
                printf(GREEN "Плащането е записано! Нова сума: %.2f €.\n\n" RESET, d.remaining);
            }
        }

        fprintf(tempFile, "%s %s %.2f %.2f\n", d.id, d.name, d.total, d.remaining);
    }


    fclose(file);
    fclose(tempFile);

    remove(FILENAME);
    rename(TEMP_FILENAME, FILENAME);
}

void removeDebt() {
    struct Debt d;
    char userID[10];

    getValidUserID(userID);

    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Грешка при отваряне на файла.\n");
        return;
    }

    FILE *tempFile = fopen(TEMP_FILENAME, "w");
    if (tempFile == NULL) {
        printf("Грешка при създаване на временен файл.\n");
        fclose(file);
        return;
    }

    while (fscanf(file, "%s %s %f %f", d.id, d.name, &d.total, &d.remaining) == 4) {
        
        if (strcmp(d.id, userID) == 0) {
            printf("\nОткрит е заем на име: %s (%.2f €)\n", d.name, d.total);
            continue;
        }

        fprintf(tempFile, "%s %s %.2f %.2f\n", d.id, d.name, d.total, d.remaining);
    }

    fclose(file);
    fclose(tempFile);

    remove(FILENAME);
    rename(TEMP_FILENAME, FILENAME);

    printf(GREEN "Заемът беше изтрит успешно!\n\n" RESET);
}