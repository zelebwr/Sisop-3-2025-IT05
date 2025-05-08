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
To **download** the needed **secret text** in the provided link.

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
To **connect** the client and the server through an **RPC socket** using **daemon** process.

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
```

The way it works is: 
1. `sock = socket(AF_INET, SOCK_STREAM, 0` to initialize the socket the program is connected to.  
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

3. `inet_pton(AF_INET, IP, &serv_addr.sin_addr)` is to convert the string address into binary form.

4. `connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)` is to make a connection on a socket.


Meanwhile, for the server it uses the `setup_server()` function.

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
```

1. `server_fd = socket(AF_INET, SOCK_STREAM, 0)` is to set up the socket.  

2. Here, the code below is to initialize socket properties.
```c
struct sockaddr_in address = {
    .sin_family = AF_INET, // IPv4
    .sin_addr.s_addr = INADDR_ANY, // any address
    .sin_port = htons(PORT) // port number
};
```

> - `.sin_addr.s_addr` is to declare what address to accept. 
> - `INADDR_ANY` is to say to accept any address.

3. `bind(server_fd, (struct sockaddr *)&address, sizeof(address)` is to bind the socket with the local socket address.

4. `listen(server_fd, 5)` is to tell the program to listen to a connection through the socket's connection.

## Sub Soal c

### Overview
To decrypt a text file by using **Reverse Text and then decode From Hex** and make the current **timestamp** as the name of the file.

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

## Sub Soal c

### Overview
To decrypt a text file by using **Reverse Text and then decode From Hex** and make the current **timestamp** as the name of the file.

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

## Sub Soal c

### Overview
To decrypt a text file by using **Reverse Text and then decode From Hex** and make the current **timestamp** as the name of the file.

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

## Sub Soal d

### Overview
To make a client CLI where it can **repeatedly input a command**.  

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

## Sub Soal e

### Overview
To decrypt a text file by using **Reverse Text and then decode From Hex** and make the current **timestamp** as the name of the file.

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

## Sub Soal f

### Overview
To decrypt a text file by using **Reverse Text and then decode From Hex** and make the current **timestamp** as the name of the file.

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

## Sub Soal g

### Overview
To decrypt a text file by using **Reverse Text and then decode From Hex** and make the current **timestamp** as the name of the file.

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


# Soal 3

## Sub Soal a

### Overview
dungeon.c acts as the game server, while player.c acts as the client connected via TCP socket

### Code Block
```c
int main() {
    srand(time(NULL));
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, MAX_CLIENT);

    printf("Dungeon server listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
        if (client_fd < 0) continue;

        printf("Client connected.\n");
        pid_t pid = fork();
        if (pid == 0) {
            close(server_fd);
            handle_client(client_fd);
        } else {
            close(client_fd);
            while (waitpid(-1, NULL, WNOHANG) > 0);
        }
    }

    close(server_fd);
    return 0;
}
```

### Explanation
The main() function is responsible for:
> - Initializing the server socket and setting options.
> - Binding to port 12345.
> - Listening for incoming client connections.
> - Accepting connections using accept().
> - Creating a new process for each client using fork().
> - Calling handle_client() to start the game loop for each player.

## Sub Soal b

### Overview
Sightseeing
When player.c is run, it connects to the dungeon server (dungeon.c) and displays a main menu. Players find interesting things around the dungeon, like a weapon shop and a mysterious door.

### Output
![b.png](assets/soal_3/mainmenu.txt)

### Code Block
```c
typedef struct {
    int gold;
    int base_damage;
    char weapon_name[50];
    int kills;
    int inventory_count;
    int inventory_index[MAX_INVENTORY];
} Player;
```
### Explanation
Keeps track of the player's current state including money, equipped weapon, performance stats, and weapon inventory. It is used and updated throughout the game to reflect the player's progress.

```c
void send_main_menu(int client_fd) {
    char menu[] =
        "\n=== The Lost Dungeon ===\n"
        "1. Show Player Stats\n"
        "2. Shop (Buy Weapons)\n"
        "3. View Inventory & Equip Weapons\n"
        "4. Battle Mode\n"
        "5. Exit\n";
    send(client_fd, menu, strlen(menu), 0);
}
```
### Explanation
This function sends the main menu options to the client. The menu includes five available actions

```c
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    int running = 1;

    Player player = {
        .gold = 500,
        .base_damage = 10,
        .weapon_name = "Fists",
        .kills = 0,
        .inventory_count = 1,
        .inventory_index = {0}
    };
```

### Explanation
This function manages the entire game logic for one connected client.
> - At the start, it initializes a Player structure with default values

## Sub Soal c

### Overview
Status Check
Choosing Show Player Stats displays the player's current gold, equipped weapon, base damage, and total kills. Useful before buying weapons.

### Output
![c.png](assets/soal_3/stats.txt)

### Code Block
```c
case 1: {
                Weapon *equipped = get_equipped_weapon(&player);
                if (equipped && equipped->has_passive) {
                    snprintf(buffer, BUFFER_SIZE,
                        "Gold: %d | Weapon: %s | Damage: %d | Kills: %d\nPassive: %s\n",
                        player.gold, player.weapon_name, player.base_damage, player.kills,
                        equipped->passive);
                } else {
                    snprintf(buffer, BUFFER_SIZE,
                        "Gold: %d | Weapon: %s | Damage: %d | Kills: %d\n",
                        player.gold, player.weapon_name, player.base_damage, player.kills);
                }
                send(client_fd, buffer, strlen(buffer), 0);
                break;
            }
```

### Explanation
> - Displays current gold, weapon name, base damage, and kill count.
> - Also shows passive ability if the equipped weapon has one.
> - Helps the player evaluate their readiness for battle or shopping.

## Sub Soal d

### Overview
Weapon Shop
Upon entering the shop, players can view a list of at least 5 weapons, each with price, damage, and optional passive effects. Two or more weapons must have unique passives.


### Output
![d.png](assets/soal_3/shop.txt)

### Code Block in shop.c
```c
typedef struct {
    char name[50];
    int damage;
    int price;
    char passive[100];
    int has_passive;
} Weapon;
```
### Explanation
This struct represents a weapon with its attributes: name, damage, price, and an optional passive ability.

```c
#define MAX_WEAPONS 6

Weapon weapon_list[MAX_WEAPONS] = {
    {"Fists", 10, 0, "", 0},
    {"Pentung Tung", 15, 30, "", 0},
    {"Manyala Kaka", 12, 50, "Burn (Extra Damage)", 1},
    {"Tupperware Ibu", 20, 80, "", 0},
    {"Pulpen Sieun", 18, 70, "Critical Boost", 1},
    {"Bingchilling", 5, 10, "", 0}
};
```
### Explanation
> - A predefined array of 6 weapons available in the shop.
> - Index 0 ("Fists") is the default weapon and cannot be bought.
> - At least 2 weapons have unique passive effects (Burn and Critical Boost).


```c
void list_weapons(char *out) {
    strcpy(out, "\n=== Weapon Shop ===\n");
    for (int i = 1; i < MAX_WEAPONS; ++i) { // Lewati indeks 0 (Fists)
        char line[256];
        snprintf(line, sizeof(line), "%d. %s | DMG: %d | Gold: %d%s%s\n",
                 i,
                 weapon_list[i].name,
                 weapon_list[i].damage,
                 weapon_list[i].price,
                 weapon_list[i].has_passive ? " | Passive: " : "",
                 weapon_list[i].has_passive ? weapon_list[i].passive : "");
        strcat(out, line);
    }
}
```
### Explanation
> - This function builds a string listing all buyable weapons (excluding Fists) and stores it in out.
> - Each weapon's name, damage, price, and passive (if any) is displayed.

```c
int buy_weapon(int index, int *gold, char *equipped_weapon, int *base_damage) {
    if (index <= 0 || index >= MAX_WEAPONS)
        return -2; // indeks tidak valid atau mencoba beli "Fists"

    Weapon w = weapon_list[index];
    if (*gold < w.price)
        return -1; 

    *gold -= w.price;
    strcpy(equipped_weapon, w.name);
    *base_damage = w.damage;

    return 0; 
}

```
### Explanation
This function processes the weapon purchase:
> - Returns -2 if the index is invalid or refers to "Fists".
> - Returns -1 if the player has insufficient gold.
> - On success (0), deducts gold, equips the weapon, and updates the player's damage.

## Code block's in dungeon.c
```c
Case 2: {
                char shop_menu[2048];
                list_weapons(shop_menu);
                send(client_fd, shop_menu, strlen(shop_menu), 0);

                send(client_fd, "\nEnter weapon number to buy: ", 31, 0);
                memset(buffer, 0, BUFFER_SIZE);
                recv(client_fd, buffer, BUFFER_SIZE, 0);
                int chosen_index = atoi(buffer);

                int res = buy_weapon(chosen_index, &player.gold, player.weapon_name, &player.base_damage);
                if (res == 0) {
                    if (player.inventory_count < MAX_INVENTORY)
                        player.inventory_index[player.inventory_count++] = chosen_index;
                    snprintf(buffer, BUFFER_SIZE,
                        "Purchase successful! Equipped: %s | Damage: %d | Gold: %d\n",
                        player.weapon_name, player.base_damage, player.gold);
                } else if (res == -1) {
                    snprintf(buffer, BUFFER_SIZE, "Not enough gold!\n");
                } else {
                    snprintf(buffer, BUFFER_SIZE, "Invalid weapon!\n");
                }
                send(client_fd, buffer, strlen(buffer), 0);
                break;
                }
```

### Explanation
> - Shows all available weapons to purchase.
> - Prompts player to enter weapon number.
> - Buys and equips the weapon if valid and enough gold.
> - Adds weapon to inventory if there's space.
> - Sends error message for invalid input or insufficient gold.


## Sub Soal e

### Overview
Handy Inventory
Players can view and equip weapons from their inventory. If a weapon has a passive, it is shown. Equipping a new weapon updates the player's damage and passive status.

### Output
![e.png](assets/soal_3/inven.txt)

### Code Block
```c
case 3: {
                char inv[2048] = "=== Inventory ===\n";
                for (int i = 0; i < player.inventory_count; i++) {
                    int idx = player.inventory_index[i];
                    Weapon w = weapon_list[idx];
                    char line[256];
                    snprintf(line, sizeof(line), "%d. %s | DMG: %d%s%s\n",
                        i + 1, w.name, w.damage,
                        w.has_passive ? " | Passive: " : "",
                        w.has_passive ? w.passive : "");
                    strcat(inv, line);
                }
                send(client_fd, inv, strlen(inv), 0);

                send(client_fd, "\nEnter number to equip weapon: ", 34, 0);
                memset(buffer, 0, BUFFER_SIZE);
                recv(client_fd, buffer, BUFFER_SIZE, 0);
                int chosen = atoi(buffer) - 1;
                if (chosen >= 0 && chosen < player.inventory_count) {
                    int idx = player.inventory_index[chosen];
                    Weapon w = weapon_list[idx];
                    strcpy(player.weapon_name, w.name);
                    player.base_damage = w.damage;
                    snprintf(buffer, BUFFER_SIZE,
                        "Equipped: %s | Damage: %d%s%s\n",
                        w.name, w.damage,
                        w.has_passive ? " | Passive: " : "",
                        w.has_passive ? w.passive : "");
                } else {
                    snprintf(buffer, BUFFER_SIZE, "Invalid choice!\n");
                }
                send(client_fd, buffer, strlen(buffer), 0);
                break;
            }
```

### Explanation
> - Shows all weapons the player owns.
> - Displays passive effects if available.
> - Prompts player to choose a weapon to equip.
> - Updates player's weapon_name and base_damage.
> - Sends error message if input is invalid.

## Sub Soal f

### Overview
Enemy Encounter
Entering Battle Mode spawns a random enemy (50–200 HP). Players can attack or exit. Enemy HP and health bar are updated each turn. Defeating an enemy grants gold and spawns a new one.


### Output
![f.png](assets/soal_3/battle.txt)

### Code Block

```c
typedef struct {
    int max_hp;
    int hp;
} Enemy;
```
```c
Enemy generate_enemy() {
    Enemy e;
    e.max_hp = (rand() % 151) + 50;
    e.hp = e.max_hp;
    return e;
}
```
### Explanation
This function creates and returns a new enemy with random health points.


```c
void render_health_bar(char *dest, int hp, int max_hp) {
    int bar_width = 20;
    int filled = (hp * bar_width) / max_hp;
    strcpy(dest, "[");
    for (int i = 0; i < bar_width; i++) {
        strcat(dest, (i < filled) ? "█" : " ");
    }
    strcat(dest, "]");
}
```
### Explanation
This function generates a visual health bar string (e.g., [█████ ]) based on the enemy’s current hp and max_hp.

```c
case 4: {
                Enemy enemy = generate_enemy();
                Weapon *equipped = get_equipped_weapon(&player);
                while (enemy.hp > 0) {
                    char bar[64];
                    render_health_bar(bar, enemy.hp, enemy.max_hp);
                    snprintf(buffer, BUFFER_SIZE,
                        "Enemy HP: %s %d/%d\nCommand: attack / exit\n",
                        bar, enemy.hp, enemy.max_hp);
                    send(client_fd, buffer, strlen(buffer), 0);

                    memset(buffer, 0, BUFFER_SIZE);
                    recv(client_fd, buffer, BUFFER_SIZE, 0);
                    buffer[strcspn(buffer, "\n")] = 0;

                    if (strcmp(buffer, "exit") == 0) {
                        snprintf(buffer, BUFFER_SIZE, "You fled the battle!\n");
                        send(client_fd, buffer, strlen(buffer), 0);
                        break;
                    } else if (strcmp(buffer, "attack") != 0) {
                        snprintf(buffer, BUFFER_SIZE, "Unknown command! Type 'attack' or 'exit'.\n");
                        send(client_fd, buffer, strlen(buffer), 0);
                        continue;
                    }
```

### Explanation
> - This block implements the real-time combat logic, where:
> - The player is repeatedly prompted to either attack or flee.
> - The enemy’s health is updated every turn.
> - The player’s command is validated and handled accordingly.

## Sub Soal g
Other Battle Logic

### Overview
- Health & Rewards
- Damage Equation
![g.png](assets/soal_3/g.txt)
- Passive
![passive.png](assets/soal_3/passive.txt)

## Sub Soal h

### Overview
Error Handling
Invalid inputs (like unknown menu options) are properly handled and trigger warning messages.


### Output
![g.png](assets/soal_3/errorh.txt)

### Code Block
```c
default:
                snprintf(buffer, BUFFER_SIZE, "Invalid choice!\n");
                send(client_fd, buffer, strlen(buffer), 0);
                break;
```

### Explanation
it ensures that the player is notified when entering an unrecognized menu option.


### Player.c

### Code Block
```c
int sockfd;
struct sockaddr_in server_addr;
char buffer[BUFFER_SIZE];

sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
}

server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
inet_pton(AF_INET, IP, &server_addr.sin_addr);

if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Connection failed");
    close(sockfd);
    exit(EXIT_FAILURE);
}
```

### Explanation
> - This creates a TCP socket ```(SOCK_STREAM)```. If socket creation fails, the program prints an error and exits.
> - Sets the address and port of the server to connect to (127.0.0.1:PORT) using IPv4 (AF_INET).
> - Attempts to connect to the server. If the connection fails, it closes the socket and exits.

### Code Block
```c
while (1) {
    memset(buffer, 0, BUFFER_SIZE);
    int bytes = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes <= 0) break;
    buffer[bytes] = '\0';
    printf("%s", buffer);
```

### Explanation
Receives and displays the main menu sent by the server. If the server disconnects or there's an error, it breaks the loop.

### Code Block
```c
    printf("> ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    send(sockfd, buffer, strlen(buffer), 0);

    if (strcmp(buffer, "5") == 0) break;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        printf("%s", buffer);

        if (strstr(buffer, "Enter") || strstr(buffer, "Command:")) {
            printf("> ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            send(sockfd, buffer, strlen(buffer), 0);
        } else {
            break;
        }
    }
}

close(sockfd);
return 0;

```

### Explanation
> - Reads the player's choice (menu option), removes the newline, and sends it to the server. If the input is "5", it exits the game.
> - Receives additional responses from the server (e.g., shop info, battle updates, etc.).
> - If the server’s message includes "Enter" or "Command:", the client assumes the server is waiting for additional input (e.g., choosing a weapon, typing "attack"). If not, it breaks and goes back to the main menu loop.
> - Once the player exits the game (option 5), the client socket is closed and the program ends cleanly.




