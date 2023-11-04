#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

const char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789&'@)(:,=!.-+\"?/";

char* morse[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", // A - J
                    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", // K - U
                    "...-", ".--", "-..-", "-.--", "--..", "-----", ".----", "..---", "...--", "....-", // V - Z & 0 - 4
                    ".....", "-....", "--...", "---..", "----.", // 5 - 9
                    ".-...", ".----.", ".--.-.", "-.--.-", "-.--.", "---...", "--..--", "-...-", // & - =, no <HH>
                    "-.-.--", ".-.-.-", "-....-", ".-.-.", ".-..-.", "..--..", "-..-.", ".-.-"}; // ! - - & + - /, no x & %

char decode(char* code) {
    int i = 0;
    while (strcmp(code, morse[i]) != 0) { // loop through every morse code
        i++;
    }
    return chars[i];
}

char* encode(char c) {
    int i = 0;
    char temp[2] = {'\0', '\0'};
    char temp2[2] = {'\0', '\0'};
    temp[0] = toupper(c);
    temp2[0] = chars[i];
    while (strcmp(temp, temp2) != 0) { // loop through every letter in a dumb way, but works
        temp2[0] = chars[++i]; // first increment and then set new value
    }
    return morse[i];
}
