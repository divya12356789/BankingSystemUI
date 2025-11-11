#include "banking.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>

/* ATM simulation: simple PIN check (password field) then allow basic ops */
void changePIN(int accNo); // forward declaration

void atmSession() {
    int accNo;
    char pass[PASS_LEN];
    printHeader("ATM SIMULATION");

    printf("Enter Account Number: ");
    scanf("%d", &accNo);
    printf("Enter PIN/Password: ");
    scanf("%s", pass);

    Account a;
    if (!findAccount(accNo, &a)) {
        printError("Account not found.");
        pauseForEnter();
        return;
    }

    if (strcmp(a.password, pass) != 0) {
        printError("Invalid PIN.");
        pauseForEnter();
        return;
    }
      if (a.loanStatus == LOAN_APPROVED) {
        printf("Loan: Approved ₹%.2f\n", a.loanAmount);
        float emi = calculateEMI(a.loanAmount, 10.0, a.loanYears); // 10% interest example
        printf("EMI per month: ₹%.2f for %d years\n", emi, a.loanYears);
    } else if (a.loanStatus == LOAN_PENDING) {
        printf("Loan: Pending ₹%.2f for %d years\n", a.loanAmount, a.loanYears);
    } else {
        printf("No Loan\n");
    }

    int ch;
    do {
        clearScreen();
        printHeader("ATM MENU");

        // Display balance
        printf("Current Balance: ₹%.2f\n", a.balance);

        // Display loan info
        if (a.loanStatus == LOAN_APPROVED) {
            printf("Loan Approved: ₹%.2f\n", a.loanAmount);
            float emi = calculateEMI(a.loanAmount, 10.0, a.loanYears); // 10% interest example
            printf("EMI per month: ₹%.2f for %d years\n", emi, a.loanYears);
        } else if (a.loanStatus == LOAN_PENDING) {
            printf("Loan Pending: ₹%.2f for %d years\n", a.loanAmount, a.loanYears);
        } else {
            printf("No Loan\n");
        }

        printf("\n1. Check Balance\n2. Withdraw\n3. Deposit\n4. Mini Statement (last 5)\n5. Change PIN\n0. Exit\nChoice: ");
        scanf("%d", &ch);

        if (ch == 1) {
            printf("\nBalance: ₹%.2f\n", a.balance);
            pauseForEnter();
        } else if (ch == 2) {
            float amt;
            printf("Enter amount to withdraw: ");
            scanf("%f", &amt);
            int res = withdrawFromAccount(accNo, amt);
            if (res == 1) printSuccess("Withdrawal successful.");
            else if (res == -1) printError("Insufficient balance.");
            else printError("Withdrawal failed.");
            pauseForEnter();
            findAccount(accNo, &a); // refresh
        } else if (ch == 3) {
            float amt;
            printf("Enter amount to deposit: ");
            scanf("%f", &amt);
            if (depositToAccount(accNo, amt)) printSuccess("Deposit successful.");
            else printError("Deposit failed.");
            pauseForEnter();
            findAccount(accNo, &a); // refresh
        } else if (ch == 4) {
            Transaction arr[10];
            int found;
            getLastTransactions(accNo, 5, arr, &found);
            if (found == 0) {
                printError("No transactions found.");
            } else {
                printf("\nMini Statement (last %d):\n", found);
                for (int i = 0; i < found; ++i) {
                    printf("%s | %s | ₹%.2f | %s\n", arr[i].datetime, arr[i].type, arr[i].amount, arr[i].desc);
                }
            }
            pauseForEnter();
        } else if (ch == 5) {
            changePIN(accNo);
        }

    } while (ch != 0);
}

void changePIN(int accNo) {
    Account a;
    if (!findAccount(accNo, &a)) {
        printError("Account not found.");
        pauseForEnter();
        return;
    }

    char oldPIN[PASS_LEN], newPIN[PASS_LEN];
    printf("Enter current PIN: ");
    scanf("%s", oldPIN);
    if (strcmp(oldPIN, a.password) != 0) {
        printError("Incorrect current PIN.");
        pauseForEnter();
        return;
    }
    printf("Enter new PIN: ");
    scanf("%s", newPIN);
    strcpy(a.password, newPIN);
    if (updateAccount(&a)) printSuccess("PIN updated successfully.");
    else printError("Failed to update PIN.");
    pauseForEnter();
}

