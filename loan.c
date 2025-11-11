#include "banking.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>


int applyLoanRequest(int accNo, float amount, int years) {
    Account a;
    if (!findAccount(accNo, &a)) return 0;
    if (a.loanStatus == LOAN_APPROVED) return 0;
    a.loanStatus = LOAN_PENDING;
    a.loanAmount = amount;
    a.loanYears = years;
    if (!updateAccount(&a)) return 0;

    Transaction t;
    t.accNo = accNo;
    strcpy(t.type, "Loan");
    t.amount = amount;
    snprintf(t.desc, MAX_DESC, "Loan applied %.2f for %d years", amount, years);
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    snprintf(t.datetime, sizeof(t.datetime), "%04d-%02d-%02d %02d:%02d:%02d",
             tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    recordTransaction(&t);
    return 1;
}

int approveLoan(int accNo) {
    Account a;
    if (!findAccount(accNo, &a)) return 0;
    if (a.loanStatus != LOAN_PENDING) return 0;
    a.loanStatus = LOAN_APPROVED;
    a.balance += a.loanAmount;
    if (!updateAccount(&a)) return 0;

    Transaction t;
    t.accNo = accNo;
    strcpy(t.type, "Loan");
    t.amount = a.loanAmount;
    snprintf(t.desc, MAX_DESC, "Loan approved %.2f", a.loanAmount);
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    snprintf(t.datetime, sizeof(t.datetime), "%04d-%02d-%02d %02d:%02d:%02d",
             tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    recordTransaction(&t);
    return 1;
}
float calculateEMI(float principal, float annualRate, int years) {
    float r = annualRate / 12 / 100;  // monthly interest
    int n = years * 12;
    float emi = (principal * r * pow(1 + r, n)) / (pow(1 + r, n) - 1);
    return emi;
}
