#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <time.h>
#include "shop.c"

#define PORT 12345
#define MAX_CLIENT 10
#define BUFFER_SIZE 1024
#define MAX_INVENTORY 10

typedef struct {
    int gold;
    int base_damage;
    char weapon_name[50];
    int kills;
    int inventory_count;
    int inventory_index[MAX_INVENTORY];
} Player;

typedef struct {
    int max_hp;
    int hp;
} Enemy;

Enemy generate_enemy() {
    Enemy e;
    e.max_hp = (rand() % 151) + 50;
    e.hp = e.max_hp;
    return e;
}

void render_health_bar(char *dest, int hp, int max_hp) {
    int bar_width = 20;
    int filled = (hp * bar_width) / max_hp;
    strcpy(dest, "[");
    for (int i = 0; i < bar_width; i++) {
        strcat(dest, (i < filled) ? "█" : " ");
    }
    strcat(dest, "]");
}

Weapon* get_equipped_weapon(Player *p) {
    for (int i = 0; i < MAX_WEAPONS; i++) {
        if (strcmp(p->weapon_name, weapon_list[i].name) == 0)
            return &weapon_list[i];
    }
    return NULL;
}

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

    while (running) {
        send_main_menu(client_fd);
        send(client_fd, "Enter your choice: ", 20, 0);

        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0) {
            printf("Client disconnecting...\n");
            break;
        }

        int choice = atoi(buffer);
        memset(buffer, 0, BUFFER_SIZE);

        switch (choice) {
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
            case 2: {
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

                    int crit_chance = (equipped && equipped->has_passive && strcmp(equipped->passive, "Critical Boost") == 0) ? 2 : 5;
                    int crit = rand() % crit_chance == 0;
                    int damage = player.base_damage + (rand() % 6);
                    if (crit) damage *= 2;
                    enemy.hp -= damage;
                    if (enemy.hp < 0) enemy.hp = 0;
                    snprintf(buffer, BUFFER_SIZE, "%sYou dealt %d damage!\n",
                        crit ? "Critical Hit! " : "", damage);
                    send(client_fd, buffer, strlen(buffer), 0);

                    if (equipped && equipped->has_passive && strcmp(equipped->passive, "Burn (Extra Damage)") == 0) {
                        int burn = 3 + rand() % 3;
                        enemy.hp -= burn;
                        if (enemy.hp < 0) enemy.hp = 0;
                        snprintf(buffer, BUFFER_SIZE, "Burn passive! %d extra damage.\n", burn);
                        send(client_fd, buffer, strlen(buffer), 0);
                    }

                    if (enemy.hp == 0) {
                        int reward = (rand() % 51) + 50;
                        player.gold += reward;
                        player.kills++;
                        snprintf(buffer, BUFFER_SIZE,
                            "Enemy defeated! +%d Gold\nTotal Kills: %d | Gold: %d\n",
                            reward, player.kills, player.gold);
                        send(client_fd, buffer, strlen(buffer), 0);
                        enemy = generate_enemy();
                    }
                }
                break;
            }
            case 5: {
                snprintf(buffer, BUFFER_SIZE, "Exiting...\n");
                send(client_fd, buffer, strlen(buffer), 0);
                printf("Client disconnecting...\n");
                running = 0;
                break;
            }
            default:
                snprintf(buffer, BUFFER_SIZE, "Invalid choice!\n");
                send(client_fd, buffer, strlen(buffer), 0);
                break;
        }
    }

    close(client_fd);
    exit(0);
}

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
