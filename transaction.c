#include "banking.h"
#include <stdio.h>
#include <stdlib.h>

#define TRANS_FILE "transactions.dat"

/* Append transaction to file */
int recordTransaction(Transaction *t) {
    FILE *fp = fopen(TRANS_FILE, "ab");
    if (!fp) return 0;
    fwrite(t, sizeof(Transaction), 1, fp);
    fclose(fp);
    return 1;
}

/* Get up to n last transactions for accNo. out must have room for n. found set to count returned. */
int getLastTransactions(int accNo, int n, Transaction *out, int *found) {
    *found = 0;
    FILE *fp = fopen(TRANS_FILE, "rb");
    if (!fp) return 0;

    // read and collect matches
    Transaction t;
    Transaction *matches = NULL;
    int cap = 0, count = 0;
    while (fread(&t, sizeof(Transaction), 1, fp) == 1) {
        if (t.accNo == accNo) {
            if (count >= cap) {
                int newcap = (cap == 0) ? 64 : cap * 2;
                Transaction *tmp = realloc(matches, sizeof(Transaction) * newcap);
                if (!tmp) { free(matches); fclose(fp); return 0; }
                matches = tmp;
                cap = newcap;
            }
            matches[count++] = t;
        }
    }
    fclose(fp);

    if (count == 0) {
        free(matches);
        *found = 0;
        return 1;
    }

    int start = count - n;
    if (start < 0) start = 0;
    int outCount = 0;
    for (int i = start; i < count; ++i) {
        out[outCount++] = matches[i];
    }
    *found = outCount;
    free(matches);
    return 1;
}
