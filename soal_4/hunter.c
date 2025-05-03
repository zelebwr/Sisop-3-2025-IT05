#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

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
int current_key;

void lock() { semop(sem_id, (struct sembuf[]){{0,-1,0}}, 1); }
void unlock() { semop(sem_id, (struct sembuf[]){{0,1,0}}, 1); }

Hunter* findHunter(int key) {
    for(int i=0; i<game->total_hunters; i++) {
        if(game->hunters[i].key == key) {
            return &game->hunters[i];
        }
    }
    return NULL;
}

void login() {
    printf("Enter key: ");
    scanf("%d", &current_key);
    
    key_t sys_key = ftok("system.c", 'S');
    shmid = shmget(sys_key, sizeof(GameData), 0666);
    game = shmat(shmid, NULL, 0);
    
    sem_id = semget(sys_key, 1, 0666);
    
    Hunter *h = findHunter(current_key);
    if(!h || h->banned) {
        printf("Invalid/banned!\n");
        exit(1);
    }
    printf("Welcome %s!\n", h->name);
}

void listDungeons() {
    lock();
    Hunter *h = findHunter(current_key);
    if(h) {
        printf("\n|||||YOUR STATS|||||\n");
        printf("Level: %d\n", h->stats.level);
        printf("EXP: %d/500\n", h->stats.exp);
        printf("ATK: %d\n", h->stats.atk);
        printf("HP: %d\n", h->stats.hp);
        printf("DEF: %d\n", h->stats.def);

        printf("\n|||||AVAILABLE DUNGEONS|||||\n");
        for (int i = 0; i < game->total_dungeons; i++) {
            Dungeon d = game->dungeons[i];
            printf("[%d] %s (Lv.%d+)\n", i, d.name, d.min_level);
            printf("   Reward: ATK+%d HP+%d DEF+%d EXP+%d\n",
                   d.rewards.atk, d.rewards.hp, d.rewards.def, d.rewards.exp);
        }
    }
    unlock();
}

void battleDungeon() {
    lock();
    Hunter *me = findHunter(current_key);
    if(!me) {
        unlock();
        return;
    }

    printf("Enter dungeon number: ");
    int num;
    scanf("%d", &num);
    
    if(num < 0 || num >= game->total_dungeons) {
        printf("Invalid!\n");
        unlock();
        return;
    }
    
    Dungeon d = game->dungeons[num];
    if(me->stats.level < d.min_level) {
        printf("Level too low!\n");
        unlock();
        return;
    }
    
    me->stats.atk += d.rewards.atk;
    me->stats.hp += d.rewards.hp;
    me->stats.def += d.rewards.def;
    me->stats.exp += d.rewards.exp;
    
    if(me->stats.exp >= 500) {
        me->stats.level++;
        me->stats.exp = 0;
        printf("Level up! Now Lv.%d\n", me->stats.level);
    }
    
    printf("Dungeon conquered!\n");
    unlock();
}

void updateDungeonNotifications() {
    while(1) {
        lock();
        system("clear");
        printf("|||||UPDATE DUNGEONS (Refresh every 3s)|||||\n");
        printf("Press Ctrl+C to stop\n\n");
        
        Hunter *me = findHunter(current_key);
        if(me) {
            for(int i=0; i<game->total_dungeons; i++) {
                Dungeon d = game->dungeons[i];
                if(me->stats.level >= d.min_level) {
                    printf("[%d] %s (Lv.%d+)\n", i, d.name, d.min_level);
                    printf("   Reward: ATK+%d HP+%d DEF+%d EXP+%d\n\n", 
                        d.rewards.atk, d.rewards.hp, d.rewards.def, d.rewards.exp);
                }
            }
        }
        unlock();
        sleep(3);
    }
}

int main() {
    login();
    
    int choice;
    while(1) {
        printf("\n/////HUNTER MENU\\\\\\\\\\\n");
        printf("1. List Dungeon\n");
        printf("2. Battle Dungeon\n");
        printf("3. Update Dungeon Notifications\n");
        printf("4. Exit\nChoice >> ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1: listDungeons(); break;
            case 2: battleDungeon(); break;
            case 3: updateDungeonNotifications(); break;
            case 4: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}