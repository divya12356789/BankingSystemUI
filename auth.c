#include "banking.h"
#include <string.h>

#define ADMIN_PASS "admin123"

int adminLogin(const char *pass) {
    return (strcmp(pass, ADMIN_PASS) == 0) ? 1 : 0;
}

int customerLogin(int accNo, const char *pass) {
    Account a;
    if (!findAccount(accNo, &a)) return 0;
    return (strcmp(a.password, pass) == 0) ? 1 : 0;
}
