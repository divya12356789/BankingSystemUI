#include "banking.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* This file acts as the main control layer for admin & customer operations */

/* ---------- ADMIN FUNCTIONS ---------- */
void createNewAccount() {
    printHeader("CREATE NEW ACCOUNT");

    Account acc;
    acc.accNo = getNextAccountNumber();

    printf("Enter Customer Name: ");
    getchar(); // clear newline
    fgets(acc.name, NAME_LEN, stdin);
    acc.name[strcspn(acc.name, "\n")] = 0;

    printf("Set Password: ");
    scanf("%s", acc.password);
    acc.balance = 0.0f;
    acc.loanStatus = LOAN_NONE;
    acc.loanAmount = 0.0f;
    acc.loanYears = 0;

    if (addAccount(&acc)) {
        printSuccess("Account created successfully!");
        printf("New Account Number: %d\n", acc.accNo);
    } else {
        printError("Failed to create account!");
    }
    pauseForEnter();
}
void interactiveMiniStatement() {
    int accNo;
    printf("Enter Account Number: ");
    scanf("%d", &accNo);

    Transaction arr[10];
    int found;
    getLastTransactions(accNo, 5, arr, &found);

    if (found == 0) {
        printf("No transactions found.\n");
    } else {
        printf("\nMini Statement (last %d):\n", found);
        for (int i = 0; i < found; i++) {
            printf("%s | %s | ₹%.2f | %s\n",
                   arr[i].datetime, arr[i].type, arr[i].amount, arr[i].desc);
        }
    }
}

void viewAllAccounts() {
    printHeader("ALL CUSTOMER ACCOUNTS");

    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) {
        printError("No account records found!");
        pauseForEnter();
        return;
    }

    Account a;
    printf("%-10s %-20s %-10s %-10s\n", "AccNo", "Name", "Balance", "LoanStatus");
    printf("-----------------------------------------------------------\n");
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        const char *status = (a.loanStatus == LOAN_NONE) ? "None" :
                             (a.loanStatus == LOAN_PENDING) ? "Pending" : "Approved";
        printf("%-10d %-20s %-10.2f %-10s\n", a.accNo, a.name, a.balance, status);
    }
    fclose(fp);
    pauseForEnter();
}

void deleteAccountUI() {
    printHeader("DELETE ACCOUNT");
    int accNo;
    printf("Enter Account Number to delete: ");
    scanf("%d", &accNo);

    if (deleteAccount(accNo))
        printSuccess("Account deleted successfully!");
    else
        printError("Account not found!");
    pauseForEnter();
}

void approveLoanUI() {
    printHeader("APPROVE LOAN REQUEST");
    int accNo;
    printf("Enter Account Number: ");
    scanf("%d", &accNo);

    if (approveLoan(accNo))
        printSuccess("Loan approved and credited successfully!");
    else
        printError("Loan not found or already approved.");
    pauseForEnter();
}

/* ---------- CUSTOMER FUNCTIONS ---------- */
void customerMenu(int accNo) {
    int choice;
    do {
        clearScreen();
        printHeader("CUSTOMER MENU");
        printf("1. Check Balance\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Transfer Money\n");
        printf("5. Apply for Loan\n");
        printf("6. View Last 5 Transactions\n");
        printf("0. Logout\n");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            Account a;
            if (findAccount(accNo, &a))
                printf("\nYour Current Balance: ₹%.2f\n", a.balance);
            else
                printError("Account not found!");
            pauseForEnter();
        }

        else if (choice == 2) {
            float amt;
            printf("Enter amount to deposit: ");
            scanf("%f", &amt);
            if (depositToAccount(accNo, amt))
                printSuccess("Deposit successful!");
            else
                printError("Deposit failed!");
            pauseForEnter();
        }

        else if (choice == 3) {
            float amt;
            printf("Enter amount to withdraw: ");
            scanf("%f", &amt);
            int res = withdrawFromAccount(accNo, amt);
            if (res == 1)
                printSuccess("Withdrawal successful!");
            else if (res == -1)
                printError("Insufficient balance!");
            else
                printError("Withdrawal failed!");
            pauseForEnter();
        }

        else if (choice == 4) {
            int toAcc;
            float amt;
            printf("Enter Receiver Account No: ");
            scanf("%d", &toAcc);
            printf("Enter Amount: ");
            scanf("%f", &amt);
            int res = transferAmount(accNo, toAcc, amt);
            if (res == 1)
                printSuccess("Transfer successful!");
            else if (res == -1)
                printError("Insufficient balance!");
            else
                printError("Transfer failed!");
            pauseForEnter();
        }

        else if (choice == 5) {
            float amt;
            int years;
            printf("Enter Loan Amount: ");
            scanf("%f", &amt);
            printf("Enter Duration (in years): ");
            scanf("%d", &years);
            if (applyLoanRequest(accNo, amt, years))
                printSuccess("Loan request submitted!");
            else
                printError("Unable to submit loan request!");
            pauseForEnter();
        }

        else if (choice == 6) {
            Transaction t[10];
            int found;
            if (getLastTransactions(accNo, 5, t, &found) && found > 0) {
                printf("\nLast %d Transactions:\n", found);
                printf("Date/Time\t\tType\tAmount\tDescription\n");
                for (int i = 0; i < found; ++i) {
                    printf("%s\t%s\t₹%.2f\t%s\n", t[i].datetime, t[i].type, t[i].amount, t[i].desc);
                }
            } else {
                printError("No transactions found.");
            }
            pauseForEnter();
        }

    } while (choice != 0);
}

/* ---------- LOGIN MENUS ---------- */


void adminLoginUI() {
    printHeader("ADMIN LOGIN");
    char pass[PASS_LEN];
    printf("Enter Admin Password: ");
    scanf("%s", pass);
    if (adminLogin(pass))
        adminMenu();
    else {
        printError("Invalid Password!");
        pauseForEnter();
    }
}

void customerLoginUI() {
    printHeader("CUSTOMER LOGIN");
    int accNo;
    char pass[PASS_LEN];
    printf("Enter Account Number: ");
    scanf("%d", &accNo);
    printf("Enter Password: ");
    scanf("%s", pass);
    if (customerLogin(accNo, pass))
        customerMenu(accNo);
    else {
        printError("Invalid Credentials!");
        pauseForEnter();
    }
}

