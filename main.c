#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "banking.h"
#include "ui.h"

/* forward prototypes for functions implemented in modules */
void atmSession(); // from atm.c
int adminLogin(const char *pass); // auth.c
int approveLoan(int accNo); // loan.c
int applyLoanRequest(int accNo, float amount, int years); // loan.c
void loadAllAccounts(); // account.c (optional)

/* helper to create account interactively */
void interactiveCreateAccount() {
    clearScreen();
    printHeader("CREATE ACCOUNT");
    Account a;
    getchar(); // consume newline

    printf("Full name: ");
    fgets(a.name, NAME_LEN, stdin);
    // remove newline
    size_t ln = strlen(a.name);
    if (ln && a.name[ln-1] == '\n') a.name[ln-1] = '\0';

    printf("Set PIN/password (no spaces): ");
    scanf("%s", a.password);

    printf("Initial deposit: ");
    scanf("%f", &a.balance);

    // New: Account type
    int typeChoice;
    printf("Account type (1. Savings, 2. Current): ");
    scanf("%d", &typeChoice);
    a.type = (typeChoice == 2) ? CURRENT : SAVINGS;

    a.loanStatus = LOAN_NONE;
    a.loanAmount = 0.0f;
    a.loanYears = 0;
    a.accNo = 0; // will be set by addAccount

    if (addAccount(&a)) {
        char buf[80];
        snprintf(buf, sizeof(buf), "Account created. Account No: %d", getNextAccountNumber()-1);
        printSuccess(buf);
    } else {
        printError("Failed to create account.");
    }

    pauseForEnter();
    getchar(); // consume leftover newline
}


/* interactive deposit */
void interactiveDeposit() {
    clearScreen(); printHeader("DEPOSIT");
    int acc; float amt;
    printf("Account No: "); scanf("%d", &acc);
    printf("Amount to deposit: "); scanf("%f", &amt);
    if (depositToAccount(acc, amt)) printSuccess("Deposit successful.");
    else printError("Deposit failed.");
    pauseForEnter();
}

/* interactive withdraw */
void interactiveWithdraw() {
    clearScreen(); printHeader("WITHDRAW");
    int acc; float amt;
    printf("Account No: "); scanf("%d", &acc);
    printf("Amount to withdraw: "); scanf("%f", &amt);
    int res = withdrawFromAccount(acc, amt);
    if (res == 1) printSuccess("Withdrawal successful.");
    else if (res == -1) printError("Insufficient balance.");
    else printError("Withdrawal failed.");
    pauseForEnter();
}

/* view account details */
void interactiveViewAccount() {
    clearScreen(); printHeader("VIEW ACCOUNT");
    int acc; printf("Enter Account No: "); scanf("%d", &acc);
    Account a;
    if (findAccount(acc, &a)) {
        printf("\nAccNo: %d\nName: %s\nBalance: ₹%.2f\n", a.accNo, a.name, a.balance);
        if (a.loanStatus == LOAN_NONE) printf("Loan: None\n");
        else if (a.loanStatus == LOAN_PENDING) printf("Loan: Pending %.2f for %d years\n", a.loanAmount, a.loanYears);
        else if (a.loanStatus == LOAN_APPROVED) printf("Loan: Approved %.2f\n", a.loanAmount);
    } else {
        printError("Account not found.");
    }
    pauseForEnter();
}

/* transfer */
void interactiveTransfer() {
    clearScreen(); printHeader("TRANSFER FUNDS");
    int from,to; float amt;
    printf("From Account No: "); scanf("%d", &from);
    printf("To Account No: "); scanf("%d", &to);
    printf("Amount: "); scanf("%f", &amt);
    int res = transferAmount(from, to, amt);
    if (res == 1) printSuccess("Transfer successful.");
    else if (res == -1) printError("Insufficient funds.");
    else printError("Transfer failed.");
    pauseForEnter();
}

/* list all accounts (admin) */
void listAllAccounts() {
    clearScreen(); printHeader("ALL ACCOUNTS (ADMIN)");
    FILE *fp = fopen("accounts.dat", "rb");
    if (!fp) {
        printError("No accounts found.");
        pauseForEnter();
        return;
    }
    Account a;
    printf("%-8s %-20s %-10s %-10s\n", "AccNo", "Name", "Balance", "Loan");
    printf("-------------------------------------------------\n");
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        char loanbuf[32] = "None";
        if (a.loanStatus == LOAN_PENDING) snprintf(loanbuf, sizeof(loanbuf), "Pending %.2f", a.loanAmount);
        else if (a.loanStatus == LOAN_APPROVED) snprintf(loanbuf, sizeof(loanbuf), "Approved %.2f", a.loanAmount);
        printf("%-8d %-20s ₹%-9.2f %-10s\n", a.accNo, a.name, a.balance, loanbuf);
    }
    fclose(fp);
    pauseForEnter();
}

/* admin loan approval */
void adminApproveLoan() {
    clearScreen(); printHeader("APPROVE LOANS");
    int acc; printf("Enter Account No to approve loan: "); scanf("%d", &acc);
    if (approveLoan(acc)) printSuccess("Loan approved.");
    else printError("Loan approval failed (maybe not pending).");
    pauseForEnter();
}

/* apply loan as customer */
void interactiveApplyLoan() {
    clearScreen(); printHeader("APPLY FOR LOAN");
    int acc; float amt; int years;
    printf("Enter Account No: "); scanf("%d", &acc);
    printf("Loan Amount: "); scanf("%f", &amt);
    printf("Years: "); scanf("%d", &years);
    if (applyLoanRequest(acc, amt, years)) printSuccess("Loan application submitted (pending approval).");
    else printError("Loan application failed.");
    pauseForEnter();
}

/* admin menu */
void adminMenu() {
    char pass[PASS_LEN];
    clearScreen(); printHeader("ADMIN LOGIN");
    printf("Enter admin password: ");
    scanf("%s", pass);
    if (!adminLogin(pass)) {
        printError("Invalid admin password.");
        pauseForEnter();
        return;
    }
    int ch;
    do {
        clearScreen(); printHeader("ADMIN MENU");
        printf("1. List all accounts\n2. Approve loans\n3. Delete account\n0. Logout\nChoice: ");
        scanf("%d", &ch);
        if (ch == 1) listAllAccounts();
        else if (ch == 2) adminApproveLoan();
        else if (ch == 3) {
            int acc; printf("Enter AccNo to delete: "); scanf("%d", &acc);
            if (deleteAccount(acc)) printSuccess("Account deleted.");
            else printError("Delete failed.");
            pauseForEnter();
        }
    } while (ch != 0);
}

/* main menu */
int main() {
    int choice;
    do {
        clearScreen();
        printf("=============================================\n");
        printf("   BANKING MANAGEMENT SYSTEM\n");
        printf("=============================================\n\n");
        printf("1. Create Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Transfer\n");
        printf("5. View Account\n");
        printf("6. Mini Statement\n");
        printf("7. Apply for Loan\n");
        printf("8. ATM Simulation\n");
        printf("9. Admin Menu\n");  // <-- Add this line
        printf("0. Exit\n\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: interactiveCreateAccount(); break;
            case 2: interactiveDeposit(); break;
            case 3: interactiveWithdraw(); break;
            case 4: interactiveTransfer(); break;
            case 5: interactiveViewAccount(); break;
            case 6: interactiveMiniStatement(); break;
            case 7: interactiveApplyLoan(); break;
            case 8: atmSession(); break;
            case 9: adminLoginUI(); break;   // <-- call admin login
            case 0: printf("Exiting...\n"); break;
            default: printf("Invalid choice.\n"); pauseForEnter();
        }

    } while(choice != 0);

    return 0;
}


