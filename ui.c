#include "ui.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#define CLS_CMD "cls"
#else
#define CLS_CMD "clear"
#endif

/* flush leftover input up to newline */
static void flush_input_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void clearScreen(void) {
    system(CLS_CMD);
}

void pauseForEnter(void) {
    printf("\nPress Enter to continue...");
    flush_input_line(); /* remove any leftover input */
    getchar();
}

void printHeader(const char *title) {
    clearScreen();
    printf("=============================================\n");
    printf("   %s\n", title);
    printf("=============================================\n\n");
}

void printSuccess(const char *msg) {
    printf("\n[OK] %s\n", msg);
}

void printError(const char *msg) {
    printf("\n[ERROR] %s\n", msg);
}
