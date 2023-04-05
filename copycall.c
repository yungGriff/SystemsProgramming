#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(){
    // Define file paths for input and output files
    const char * filePath = "genesis_2txt";
    const char * copyPath = "genesis_2copy.txt";

    // Open input and output files in binary mode
    FILE * inputStream = fopen(filePath, "rb");
    if (inputStream == NULL) {
        perror("fopen");
        exit(errno);
    }
    FILE * outputStream = fopen(copyPath, "wb");
    if (outputStream == NULL) {
        perror("fopen");
        exit(errno);
    }

    // Get size of input file and calculate midpoint
    int f_size = 0;
    fseek(inputStream, 0, SEEK_END);
    f_size = ftell(inputStream);
    int end = 0;
    end = f_size / 2;

    // Create a child process with fork()
    int p_id = fork();

    if (p_id < 0) {
        perror("fork");
        exit(errno);
    }
    // If p_id == 0, we're in the child process
    else if (p_id == 0){
        // Move file pointer to beginning of input file
        fseek(inputStream, 0, SEEK_SET);

        // Read the first half of the file into a buffer
        char buffer[end];
        fread(buffer, end, 1, inputStream);

        // Write the first half of the file to the output file
        fwrite(buffer, end, 1, outputStream);

        // Flush output buffer to ensure data is written to disk
        fflush(outputStream);

        // Close input and output files in the child process
        fclose(inputStream);
        fclose(outputStream);
    }
    // Otherwise, we're in the parent process
    else {
        // Move file pointer to midpoint + 1 (to avoid repeating bytes)
        fseek(inputStream, end + 1, SEEK_SET);

        // Read the second half of the file into a buffer
        char buffer[f_size - end];
        fread(buffer, (f_size-end), 1, inputStream);

        // Move output file pointer to midpoint + 1
        fseek(outputStream, end + 1, SEEK_SET);

        // Sleep for 1 second to allow child process to complete writing
        sleep(1);

        // Write the second half of the file to the output file
        fwrite(buffer, (f_size-end), 1, outputStream);

        // Flush output buffer to ensure data is written to disk
        fflush(outputStream);

        // Close input and output files in the parent process
        fclose(inputStream);
        fclose(outputStream);
    }

    return 0;
}