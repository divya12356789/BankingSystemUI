#ifndef BANKING_H
#define BANKING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>  // For pow in calculateEMI

#define NAME_LEN 50
#define PASS_LEN 20
#define MAX_DESC 100
#define DATETIME_LEN 32

/* Loan status enum */
typedef enum {
    LOAN_NONE = 0,
    LOAN_PENDING = 1,
    LOAN_APPROVED = 2
} LoanStatus;

/* Account type enum */
typedef enum {
    SAVINGS = 0,
    CURRENT = 1
} AccountType;

/* Account structure */
typedef struct {
    int accNo;
    char name[NAME_LEN];
    char password[PASS_LEN];
    float balance;
    LoanStatus loanStatus;
    float loanAmount;
    int loanYears;
    AccountType type;  // Savings or Current
} Account;

/* Transaction structure */
typedef struct {
    int accNo;
    char type[20];  /* "Deposit", "Withdraw", "Transfer", "Loan" */
    float amount;
    char desc[MAX_DESC];
    char datetime[DATETIME_LEN];
} Transaction;

/* ----------------- Account operations ----------------- */
int addAccount(Account *acc);
int findAccount(int accNo, Account *out);
int updateAccount(Account *acc);
int deleteAccount(int accNo);
int getNextAccountNumber();
void loadAllAccounts();
void adminMenu(void);

/* ----------------- Loan / EMI operations ----------------- */
float calculateEMI(float principal, float annualRate, int years);
int applyLoanRequest(int accNo, float amount, int years);
int approveLoan(int accNo);

/* ----------------- High-level account ops ----------------- */
int depositToAccount(int accNo, float amt);
int withdrawFromAccount(int accNo, float amt);
int transferAmount(int fromAcc, int toAcc, float amt);

/* ----------------- Transaction operations ----------------- */
int recordTransaction(Transaction *t);
int getLastTransactions(int accNo, int n, Transaction *out, int *found);

/* ----------------- Authentication ----------------- */
int adminLogin(const char *pass);
int customerLogin(int accNo, const char *pass);

/* ----------------- UI functions ----------------- */
void interactiveMiniStatement();
void adminLoginUI();

#endif
