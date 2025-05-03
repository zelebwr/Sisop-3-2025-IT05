#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

// declarations
#define PORT 8080
#define SERVER_DIR "/home/zele/college/programming/sms2/sisop/module3/Sisop-03-2025-IT05/soal_1/server"
#define SERVER_DATABASE "/home/zele/college/programming/sms2/sisop/module3/Sisop-03-2025-IT05/soal_1/server/database"
#define SERVER_LOG "/home/zele/college/programming/sms2/sisop/module3/Sisop-03-2025-IT05/soal_1/server/server.log"
#define CLIENT_DIR "/home/zele/college/programming/sms2/sisop/module3/Sisop-03-2025-IT05/soal_1/client"
#define CLIENT_SECRETS "/home/zele/college/programming/sms2/sisop/module3/Sisop-03-2025-IT05/soal_1/client/secrets"
#define LOG_FILE "/home/zele/college/programming/sms2/sisop/module3/Sisop-03-2025-IT05/soal_1/server/server.log"

FILE *log_file;

void daemonize_process() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // parent process exit
    if (setsid() < 0) exit(EXIT_FAILURE); // create new session
    if (chdir("/") < 0) exit(EXIT_FAILURE); // change working directory
    umask(0); // set file mode creation mask
    // close file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int setup_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // create socket
    if (server_fd < 0) { // check if socket creation failed
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address = {
        .sin_family = AF_INET, // IPv4
        .sin_addr.s_addr = INADDR_ANY, // any address
        .sin_port = htons(PORT) // port number
    };

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { // bind socket to address
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) { // listen for incoming connections
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

void logging(const char *source, const char *action, const char *info) {
    time_t now = time(NULL); // get current time
    struct tm *tm_info = localtime(&now);
    char timestamp[20]; 
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info); // format timestamp
    fprintf(log_file, "[%s][%s]: [%s] [%s]\n", source, timestamp, action, info); // log message
    fflush(log_file);
}

void hex_to_bin(const char *hex, unsigned char *bin, size_t len) {
    for (size_t i = 0; i < len; i++) {
        sscanf(hex + (i * 2), "%2hhx", &bin[i]); // convert hex to binary
    }
}

void decrypt(const char *filename, char *content, int client_fd) {
    // reverse the content
    int len = strlen(content); // get length of content
    for (int i = 0; i < len / 2; i++) {
        char temp = content[i]; // swap characters
        content[i] = content[len - i - 1]; // reverse content
        content[len - i - 1] = temp; // reverse content
    }

    size_t bin_len = len / 2; // length of binary data
    unsigned char *bin = malloc(bin_len); // allocate memory for binary data
    if (!bin) {
        perror("Failed to allocate memory");
        free(content);
        exit(EXIT_FAILURE);
    }

    // decrypt content
    hex_to_bin(content, bin, bin_len); // convert hex to binary

    // saving to database
    time_t now = time(NULL); // get current time
    char output[256]; // output filename
    sprintf(output, "%s/%ld.jpeg", SERVER_DATABASE, now); // create output filename
    FILE *output_file = fopen(output, "wb"); // open output file
    if (!output_file) {
        perror("Failed to open output file");
        free(content);
        free(bin);
        exit(EXIT_FAILURE);
    }
    
    char success_msg[256];
    snprintf(success_msg, sizeof(success_msg), "Successfully decrypted! File: %ld.jpeg", now);
    success_msg[strlen(success_msg)] = '\0'; // null-terminate string
    send(client_fd, success_msg, strlen(success_msg), 0);

    char filename_msg[256];
    snprintf(filename_msg, sizeof(filename_msg), "%ld.jpeg", now);
    logging("Server", "SAVE", filename_msg); // log message
    
    // writing binary data to output file
    fwrite(bin, 1, len, output_file); // write binary data
    fclose(output_file); // close output file

    // free memory
    /* free(content); // free content
    free(bin); // free binary data */
    return; 
}

void client_handler(int client_fd) {
    char buffer[7000]; // buffer for incoming data
    if (read(client_fd, buffer, sizeof(buffer)) < 0) {// read data from client
        perror("Failed to read from client");
        close(client_fd);
        return;
    }

    if (strncmp(buffer, "DECRYPT ", 8) == 0) {
        char *newline = strchr(buffer, '\n'); // find newline character
        if (!newline) {
            send(client_fd, "ERROR Invalid format", 20, 0); // send error response
            close(client_fd);
            return;
        }

        *newline = '\0'; // null-terminate string

        char *filename = buffer + 8; // get filename from buffer after eight position
        char *content = newline + 1; // get content from buffer after newline

        logging("Client", "DECRYPT", filename); // log message
        decrypt(filename, content, client_fd); // decrypt file
    }
    else if (strncmp(buffer, "DOWNLOAD ", 9) == 0) {
        char *filename = buffer + 9; // get filename from buffer
        char filepath[256]; // file path
        snprintf(filepath, sizeof(filepath), "%s/%s", SERVER_DATABASE, filename); // create file path

        FILE *file = fopen(filepath, "rb"); // open file for reading
        if (!file) { // check if file opened successfully
            send(client_fd, "ERROR File not found", 20, 0); // send error response
            close(client_fd);
            return;
        }

        logging("Client", "DOWNLOAD", filename); // log message

        char file_data[7000]; // buffer for file data   
        size_t bytes_read; 
        
        while ((bytes_read = fread(file_data, 1, sizeof(file_data), file)) > 0) { // read file data
            send(client_fd, file_data, bytes_read, 0); // send file data to client
        }

        logging("Server", "UPLOAD", filename); // log message

        fclose(file); // close file
        close(client_fd); // close client socket
        return;
    }
    else if (strncmp(buffer, "EXIT", 4) ==0) {
        logging("Client", "EXIT", "Client requested to exit"); // log message
        close(client_fd); // close client socket
        return;
    }


    close(client_fd); // close client socket
    return;
}

int main() {
    // directory creation
    struct stat st;
    if (stat(SERVER_DIR, &st) != 0) { // check if server folder exists
        if (mkdir(SERVER_DIR, 0777) != 0) { // create server folder
            perror("Failed to create server directory");
            exit(EXIT_FAILURE);
        }
    }
    if (stat(SERVER_DATABASE, &st) != 0) { // check if server database exists
        if (mkdir(SERVER_DATABASE, 0777) != 0) { // create server database
            perror("Failed to create server database directory");
            exit(EXIT_FAILURE);
        }
    }
    if (stat(CLIENT_DIR, &st) != 0) { // check if client folder exists
        if (mkdir(CLIENT_DIR, 0777) != 0) { // create server database
            perror("Failed to create client directory");
            exit(EXIT_FAILURE);
        }
    }
    if (stat(CLIENT_SECRETS, &st) != 0) { // check if client secrets exists
        if (mkdir(CLIENT_SECRETS, 0777) != 0) { // create server database
            perror("Failed to create client secrets directory");
            exit(EXIT_FAILURE);
        }
    }
    
    daemonize_process(); // daemonize process
    
    log_file = fopen(SERVER_LOG, "a"); // open log file

    // MAKING THE SERVER
    int server_fd;
    if ((server_fd = setup_server()) < 0) { // setup server
        perror("Server setup failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len); // accept incoming connection

        if (client_sock < 0) { // check if accept failed
            perror("Accept failed");
            continue; // continue to next iteration
        }

        if (fork() == 0) { // child process
            close(server_fd); // close server socket in child process
            client_handler(client_sock); // handle client request
            exit(0); // exit child process
        }
        close(client_sock); // close client socket in parent process
    }

    fclose(log_file); // close log file
    return 0; // exit program
}