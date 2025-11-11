#include "banking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ACC_FILE "accounts.dat"

static int file_exists(const char *fname) {
    FILE *f = fopen(fname, "rb");
    if (!f) return 0;
    fclose(f);
    return 1;
}

int getNextAccountNumber() {
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp) return 1001; // start
    Account a;
    int last = 1000;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo > last) last = a.accNo;
    }
    fclose(fp);
    return last + 1;
}

int addAccount(Account *acc) {
    FILE *fp = fopen(ACC_FILE, "ab");
    if (!fp) return 0;
    if (acc->accNo == 0) acc->accNo = getNextAccountNumber();
    fwrite(acc, sizeof(Account), 1, fp);
    fclose(fp);
    return 1;
}

int findAccount(int accNo, Account *out) {
    FILE *fp = fopen(ACC_FILE, "rb");
    if (!fp) return 0;
    Account a;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo == accNo) {
            if (out) *out = a;
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int updateAccount(Account *acc) {
    FILE *fp = fopen(ACC_FILE, "rb");
    FILE *tmp = fopen("temp_acc.dat", "wb");
    if (!fp || !tmp) {
        if (fp) fclose(fp);
        if (tmp) fclose(tmp);
        return 0;
    }
    Account a;
    int updated = 0;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo == acc->accNo) {
            fwrite(acc, sizeof(Account), 1, tmp);
            updated = 1;
        } else {
            fwrite(&a, sizeof(Account), 1, tmp);
        }
    }
    fclose(fp); fclose(tmp);
    if (updated) {
        remove(ACC_FILE);
        rename("temp_acc.dat", ACC_FILE);
    } else {
        remove("temp_acc.dat");
    }
    return updated;
}

int deleteAccount(int accNo) {
    FILE *fp = fopen(ACC_FILE, "rb");
    FILE *tmp = fopen("temp_acc.dat", "wb");
    if (!fp || !tmp) {
        if (fp) fclose(fp);
        if (tmp) fclose(tmp);
        return 0;
    }
    Account a;
    int found = 0;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo == accNo) {
            found = 1;
            continue;
        }
        fwrite(&a, sizeof(Account), 1, tmp);
    }
    fclose(fp); fclose(tmp);
    if (found) {
        remove(ACC_FILE);
        rename("temp_acc.dat", ACC_FILE);
    } else {
        remove("temp_acc.dat");
    }
    return found;
}

void loadAllAccounts() {
    /* no-op; file operations are on-demand */
    (void)file_exists;
}

/* high-level operations (use recordTransaction from transaction.c) */
int depositToAccount(int accNo, float amt) {
    Account a;
    if (!findAccount(accNo, &a)) return 0;
    if (amt <= 0) return 0;
    a.balance += amt;
    if (!updateAccount(&a)) return 0;

    Transaction t;
    t.accNo = accNo;
    strcpy(t.type, "Deposit");
    t.amount = amt;
    snprintf(t.desc, MAX_DESC, "Deposit to %d", accNo);
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    snprintf(t.datetime, sizeof(t.datetime), "%04d-%02d-%02d %02d:%02d:%02d",
             tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    recordTransaction(&t);
    return 1;
}

int withdrawFromAccount(int accNo, float amt) {
    Account a;
    if (!findAccount(accNo, &a)) return 0;
    if (amt <= 0) return 0;
    if (a.balance < amt) return -1; /* insufficient */
    a.balance -= amt;
    if (!updateAccount(&a)) return 0;

    Transaction t;
    t.accNo = accNo;
    strcpy(t.type, "Withdraw");
    t.amount = amt;
    snprintf(t.desc, MAX_DESC, "Withdraw from %d", accNo);
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    snprintf(t.datetime, sizeof(t.datetime), "%04d-%02d-%02d %02d:%02d:%02d",
             tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    recordTransaction(&t);
    return 1;
}

int transferAmount(int fromAcc, int toAcc, float amt) {
    if (amt <= 0) return 0;
    Account a, b;
    if (!findAccount(fromAcc, &a)) return 0;
    if (!findAccount(toAcc, &b)) return 0;
    if (a.balance < amt) return -1;
    a.balance -= amt;
    b.balance += amt;
    if (!updateAccount(&a)) return 0;
    if (!updateAccount(&b)) return 0;

    Transaction t1;
    t1.accNo = fromAcc;
    strcpy(t1.type, "Transfer");
    t1.amount = amt;
    snprintf(t1.desc, MAX_DESC, "Transfer to %d", toAcc);
    time_t now = time(NULL);
    struct tm *tm1 = localtime(&now);
    snprintf(t1.datetime, sizeof(t1.datetime), "%04d-%02d-%02d %02d:%02d:%02d",
             tm1->tm_year+1900, tm1->tm_mon+1, tm1->tm_mday, tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
    recordTransaction(&t1);

    Transaction t2;
    t2.accNo = toAcc;
    strcpy(t2.type, "Transfer");
    t2.amount = amt;
    snprintf(t2.desc, MAX_DESC, "Transfer from %d", fromAcc);
    snprintf(t2.datetime, sizeof(t2.datetime), "%04d-%02d-%02d %02d:%02d:%02d",
             tm1->tm_year+1900, tm1->tm_mon+1, tm1->tm_mday, tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
    recordTransaction(&t2);

    return 1;
}
