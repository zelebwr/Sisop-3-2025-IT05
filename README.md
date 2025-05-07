# Sisop-3-2025-IT05
Below are the steps on how to use the template: 
1. Copy paste the template as many as how many questions there are
2. Change the contents of the template according to your needs
3. Don't forget to delete the template and this message before submitting the official report

Below are the template:


# Soal {n, n = the n-th question}

## Sub Soal {x, x = which sub question}

### Overview
{Fill this with a small overview on what the sub question wants you to do}

### Input/&Output
![ThisIsInput/OutputImageOfAnExample.png](assets/temp.txt)

### Code Block
```c
int main() {
    printf(%s, "fill this with your code block that function for mainly the asked purpose of the sub question");
    return 0;
}
```

### Explanation
{Fill this with your explanation about the code}

### Revision
{Here if you have any revisions that you were told to do}


# Copy paste the above template below 

# Soal 1

## Sub Soal a

### Overview
To download the needed secret text in the provided link.

### Input/&Output
![ThisIsInput/OutputImageOfAnExample.png](assets/temp.txt)

### Code Block
```bash
wget -O secret.zip "https://drive.usercontent.google.com/u/0/uc?id=15mnXpYUimVP1F5Df7qd_Ahbjor3o1cVw&export=download"

unzip secret.zip
```

### Explanation
`wget` is the command to be able to download a file through external links. By using the code above, the file in the link can be downloaded.

## Sub Soal b

### Overview
To connect the client and the server through an RPC socket using daemon process.

### Input/&Output
![ThisIsInput/OutputImageOfAnExample.png](assets/temp.txt)

### Code Block

#### Image Client
```c
int connect_to_server() {
    int sock = 0; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  // create socket
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET, // IPv4
        .sin_port = htons(PORT) // port number
    };

    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) { // convert IP address
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { // connect to server
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    return sock;
}

int main() {
    int sock = connect_to_server(); // connect to server
    if (sock < 0) { // check if connection failed
        perror("Failed to connect to server");
        continue;
    }
    return 0;
}
```

#### Server
```c 
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

int main () {
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
```

### Explanation
For the client itself, it uses the `connect_to_server()` function to connect itself to the server.

The way it works is: 
1. `sock = socket(AF_INET, SOCK_STREAM, 0` to initialize the socket the program is connected to
> - `socket()` function is to create the socket, and it returns an integer of the socket it's connected to.  
> - `AF_INET` is to address the family internet as IPv4.  
> - `SOCK_STREAM` is to specify the type of socket. Here, it specifies it as a TCP (Transmission Control Protocol) protocol, as in a connection-oriented, reliable, two-way communication using a stream of bytes protocol.  
> - `0` is the protocol value for that the socket uses. Here, it is specifying it as Internet Protocol (IP), which is 0.  

2. Here, the code below is for initializing the socket properties.
```c
struct sockaddr_in serv_addr = {
        .sin_family = AF_INET, // IPv4
        .sin_port = htons(PORT) // port number
    };
```
> - Using `sockaddr_in` to change to modify the properties of the socket. 
> - `.sin_family` is to declare the family internet it's using.  
> - `.sin_port` using the `htons()` to declare the port number it's using.


