#include <stdio.h>
#include <string.h>

typedef struct {
    char name[50];
    int damage;
    int price;
    char passive[100];
    int has_passive;
} Weapon;

#define MAX_WEAPONS 6

Weapon weapon_list[MAX_WEAPONS] = {
    {"Fists", 10, 0, "", 0},
    {"Pentung Tung", 15, 30, "", 0},
    {"Manyala Kaka", 12, 50, "Burn (Extra Damage)", 1},
    {"Tupperware Ibu", 20, 80, "", 0},
    {"Pulpen Sieun", 18, 70, "Critical Boost", 1},
    {"Bingchilling", 5, 10, "", 0}
};

void list_weapons(char *out) {
    strcpy(out, "\n=== Weapon Shop ===\n");
    for (int i = 1; i < MAX_WEAPONS; ++i) {
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

int buy_weapon(int index, int *gold, char *equipped_weapon, int *base_damage) {
    if (index <= 0 || index >= MAX_WEAPONS)
        return -2;

    Weapon w = weapon_list[index];
    if (*gold < w.price)
        return -1;

    *gold -= w.price;
    strcpy(equipped_weapon, w.name);
    *base_damage = w.damage;

    return 0;
}
