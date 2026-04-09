
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

unsigned int enterChoice(void);
void displayAll(FILE *fPtr);
void searchAccount(FILE *fPtr);
void deposit(FILE *fPtr);
void withdraw(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void transfer(FILE *fPtr);
void viewTransactions();
void logTransaction(unsigned int acc, char type[], double amount, double balance);
void hiddenPassword(char password[]);
int login();

void hiddenPassword(char password[])
{
    int i = 0;
    char ch;

    while ((ch = getch()) != 13)
    {
        if (ch == 8 && i > 0)
        {
            i--;
            printf("\b \b");
        }
        else if (ch != 8)
        {
            password[i++] = ch;
            printf("*");
        }
    }

    password[i] = '\0';
}

int login()
{
    char username[20], password[20];

    printf("==== BANK LOGIN ====\n");

    printf("Username: ");
    scanf("%s", username);

    printf("Password: ");
    hiddenPassword(password);

    if (strcmp(username, "admin") == 0 && strcmp(password, "1234") == 0)
    {
        printf("\nLogin successful!\n\n");
        return 1;
    }
    else
    {
        printf("\nInvalid login!\n");
        return 0;
    }
}

int main()
{
    FILE *cfPtr;
    unsigned int choice;

    if (!login())
    {
        return 0;
    }

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("File could not be opened.\n");
        exit(1);
    }

    while ((choice = enterChoice()) != 9)
    {
        switch (choice)
        {
        case 1:
            displayAll(cfPtr);
            break;
        case 2:
            searchAccount(cfPtr);
            break;
        case 3:
            deposit(cfPtr);
            break;
        case 4:
            withdraw(cfPtr);
            break;
        case 5:
            newRecord(cfPtr);
            break;
        case 6:
            deleteRecord(cfPtr);
            break;
        case 7:
            transfer(cfPtr);
            break;
        case 8:
            viewTransactions();
            break;
        default:
            printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
    return 0;
}

unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n1 - Display all accounts\n");
    printf("2 - Search account\n");
    printf("3 - Deposit\n");
    printf("4 - Withdraw\n");
    printf("5 - Add account\n");
    printf("6 - Delete account\n");
    printf("7 - Transfer money\n");
    printf("8 - View transactions\n");
    printf("9 - Exit\n");
    printf("Enter choice: ");

    scanf("%u", &choice);
    return choice;
}

void displayAll(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6d%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

void searchAccount(FILE *fPtr)
{
    unsigned int account;
    struct clientData client;

    printf("Enter account number to search: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\nAccount Found:\n");
        printf("Account Number : %d\n", client.acctNum);
        printf("Last Name     : %s\n", client.lastName);
        printf("First Name    : %s\n", client.firstName);
        printf("Balance       : %.2f\n", client.balance);
    }
}

void deposit(FILE *fPtr)
{
    unsigned int account;
    double amount;
    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("Enter amount to deposit: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }

    client.balance += amount;

    fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    logTransaction(account, "Deposit", amount, client.balance);

    printf("Deposit successful. New balance: %.2f\n", client.balance);
}

void withdraw(FILE *fPtr)
{
    unsigned int account;
    double amount;
    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("Enter amount to withdraw: ");
    scanf("%lf", &amount);

    if (amount <= 0 || amount > client.balance)
    {
        printf("Invalid amount or insufficient balance.\n");
        return;
    }

    client.balance -= amount;

    fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    logTransaction(account, "Withdraw", amount, client.balance);

    printf("Withdraw successful. New balance: %.2f\n", client.balance);
}

void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter last name first name balance: ");
    scanf("%14s %9s %lf", client.lastName, client.firstName, &client.balance);

    client.acctNum = accountNum;

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created successfully.\n");
}

void deleteRecord(FILE *fPtr)
{
    struct clientData blankClient = {0, "", "", 0.0};
    struct clientData client;
    unsigned int accountNum;

    printf("Enter account number to delete: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted successfully.\n");
}

void transfer(FILE *fPtr)
{
    struct clientData sender, receiver;
    unsigned int senderAcc, receiverAcc;
    double amount;

    printf("Enter sender account number: ");
    scanf("%u", &senderAcc);

    printf("Enter receiver account number: ");
    scanf("%u", &receiverAcc);

    if (senderAcc == receiverAcc)
    {
        printf("Cannot transfer to same account.\n");
        return;
    }

    fseek(fPtr, (senderAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&sender, sizeof(struct clientData), 1, fPtr);

    fseek(fPtr, (receiverAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&receiver, sizeof(struct clientData), 1, fPtr);

    if (sender.acctNum == 0 || receiver.acctNum == 0)
    {
        printf("One or both accounts do not exist.\n");
        return;
    }

    printf("Enter amount to transfer: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }

    if (amount > sender.balance)
    {
        printf("Insufficient balance.\n");
        return;
    }

    sender.balance -= amount;
    receiver.balance += amount;

    fseek(fPtr, (senderAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&sender, sizeof(struct clientData), 1, fPtr);

    fseek(fPtr, (receiverAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&receiver, sizeof(struct clientData), 1, fPtr);

    logTransaction(senderAcc, "Transfer Sent", amount, sender.balance);
    logTransaction(receiverAcc, "Transfer Received", amount, receiver.balance);

    printf("Transfer from %s to %s successful.\n", sender.firstName, receiver.firstName);
    printf("Sender New Balance: %.2f\n", sender.balance);
    printf("Receiver New Balance: %.2f\n", receiver.balance);
}

void logTransaction(unsigned int acc, char type[], double amount, double balance)
{
    FILE *fp = fopen("transactions.txt", "a");

    if (fp != NULL)
    {
        fprintf(fp, "Acc:%u Type:%s Amount:%.2f Balance:%.2f\n",
                acc, type, amount, balance);
        fclose(fp);
    }
}

void viewTransactions()
{
    FILE *fp = fopen("transactions.txt", "r");
    char line[200];

    if (fp == NULL)
    {
        printf("No transaction history found.\n");
        return;
    }

    printf("\n===== TRANSACTION HISTORY =====\n");

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        printf("%s", line);
    }

    fclose(fp);
}

