#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

#define MAX_HUNTERS 10
#define MAX_DUNGEONS 10

typedef struct {
    int level;
    int atk;
    int hp;
    int def;
    int exp;
} Stats;

typedef struct {
    char name[20];
    int key;
    int banned;
    Stats stats;
} Hunter;

typedef struct {
    char name[20];
    int min_level;
    Stats rewards;
} Dungeon;

typedef struct {
    Hunter hunters[MAX_HUNTERS];
    Dungeon dungeons[MAX_DUNGEONS];
    int total_hunters;
    int total_dungeons;
} GameData;

int shmid;
GameData *game;
int sem_id;

void lock() { semop(sem_id, (struct sembuf[]){{0, -1, 0}}, 1); }
void unlock() { semop(sem_id, (struct sembuf[]){{0, 1, 0}}, 1); }

void init() {
    key_t key = ftok("system.c", 'S');
    shmid = shmget(key, sizeof(GameData), IPC_CREAT | 0666);
    game = shmat(shmid, NULL, 0);

    sem_id = semget(key, 1, IPC_CREAT | 0666);
    semctl(sem_id, 0, SETVAL, 1);

    game->total_hunters = 0;
    game->total_dungeons = 0;
}

void registerHunter() {
    lock();
    if (game->total_hunters >= MAX_HUNTERS) {
        printf("Hunter list full!\n");
        unlock();
        return;
    }

    Hunter h;
    printf("Enter hunter name: ");
    scanf("%s", h.name);

    h.key = rand() % 9000 + 1000;
    h.banned = 0;
    h.stats = (Stats){1, 0, 10, 100, 5};

    game->hunters[game->total_hunters] = h;
    game->total_hunters++;
    printf("Hunter registered! Key: %d\n", h.key);
    unlock();
}

void createDungeon() {
    lock();
    if (game->total_dungeons >= MAX_DUNGEONS) {
        printf("Dungeon list full!\n");
        unlock();
        return;
    }

    Dungeon d;
    sprintf(d.name, "Dungeon%d", game->total_dungeons + 1);
    d.min_level = rand() % 5 + 1;
    d.rewards.atk = rand() % 51 + 100;
    d.rewards.hp = rand() % 51 + 50;
    d.rewards.def = rand() % 26 + 25;
    d.rewards.exp = rand() % 151 + 150;

    game->dungeons[game->total_dungeons] = d;
    game->total_dungeons++;

    printf("New dungeon created: %s (Min Lv.%d)\n", d.name, d.min_level);
    unlock();
}

void listHunters() {
    lock();
    printf("\n=== HUNTERS LIST ===\n");
    for (int i = 0; i < game->total_hunters; i++) {
        Hunter h = game->hunters[i];
        printf("%s (Key:%d) Lv.%d %s\n",
               h.name, h.key, h.stats.level,
               h.banned ? "[BANNED]" : "");
    }
    unlock();
}

void banHunter() {
    lock();
    printf("Enter hunter key to ban: ");
    int key;
    scanf("%d", &key);

    for (int i = 0; i < game->total_hunters; i++) {
        if (game->hunters[i].key == key) {
            game->hunters[i].banned = 1;
            printf("Hunter banned!\n");
            unlock();
            return;
        }
    }

    printf("Hunter not found!\n");
    unlock();
}

int main() {
    srand(time(NULL));
    init();

    int choice;
    while (1) {
        printf("\n|||||SYSTEM MENU|||||\n");
        printf("1. Register Hunter\n");
        printf("2. Create Dungeon\n");
        printf("3. List Hunters\n");
        printf("4. Ban Hunter\n");
        printf("5. Exit\n>> ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: registerHunter(); break;
            case 2: createDungeon(); break;
            case 3: listHunters(); break;
            case 4: banHunter(); break;
            case 5:
                shmdt(game);
                shmctl(shmid, IPC_RMID, NULL);
                semctl(sem_id, 0, IPC_RMID);
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }
}