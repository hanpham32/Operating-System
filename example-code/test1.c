#include <stdio.h>
#include <stdlib.h>

// Function prototypes
FILE *open_file(const char *filename);
int move_in_file(FILE *file, long location);
char *allocate_memory(size_t size);
void print_bytes(const char *buffer, size_t size);

int main() {
    FILE *file;
    long location;
    size_t bytes_to_read;
    char *buffer;

    // Open the file in read mode
    file = open_file("example.txt");
    if (file == NULL) {
        return 1;
    }

    // Specify the location to move to and the number of bytes to read
    location = 10; // for example, move to the 10th byte in the file
    bytes_to_read = 5; // for example, read 5 bytes from that location

    // Move to the specified location
    if (fseek(file, location, SEEK_SET) != 0) {
        perror("Error moving to specified location in file");
        fclose(file);
        return 1;
    }

    // Allocate memory to store the read bytes
    buffer = malloc(bytes_to_read);
    if (buffer == NULL) {
        perror("Error allocating memory for buffer");
        fclose(file);
        return 1;
    }

    // Read bytes from the file
    if (fread(buffer, 1, bytes_to_read, file) != bytes_to_read) {
        perror("Error reading from file");
        fclose(file);
        free(buffer);
        return 1;
    }

    // For demonstration, print the read bytes
    printf("Read bytes: ");
    for (size_t i = 0; i < bytes_to_read; i++) {
        printf("%c", buffer[i]);
    }
    printf("\n");

    // Clean up
    fclose(file);
    free(buffer);

    return 0;
}

FILE *open_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
    }
    return file;
}
