#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateRandomChars(char *str, int length) {
    for (int i = 0; i < length; i++) {
        // Generate a random character from 'a' to 'z'
        str[i] = 'a' + (rand() % 26);
    }
    str[length] = '\0'; // Null-terminate the string
}

int main() {
    FILE *file = fopen("data.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    srand(time(NULL)); // Seed the random number generator

    int numLines = 100; // Number of lines in the file
    for (int line = 1; line <= numLines; line++) {
        char randomChars[line + 1]; // +1 for the null terminator
        generateRandomChars(randomChars, line);
        if(line == numLines)
            fprintf(file, "%s", randomChars);
        else
            fprintf(file, "%s\n", randomChars);
    }

    fclose(file);
    return 0;
}
