#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <pwd.h>

#define SHARED_MEM_NAME "/rushgo_memory"
#define MAX_ORDERS 100

typedef struct {
    char customer[100];
    char address[100];
    char service[10];
    char status[20];
    char handled_by[20];
} Order;

typedef struct {
    pthread_mutex_t lock;
    int total_orders;
    Order orders[MAX_ORDERS];
} SharedOrders;

SharedOrders *shared_data;

char* get_username() {
    struct passwd *pw = getpwuid(getuid());
    return pw ? pw->pw_name : "unknown";
}

void connect_to_shared_mem() {
    int mem_fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    if(mem_fd == -1) {
        perror("Failed to access delivery system");
        exit(1);
    }
    
    shared_data = mmap(NULL, sizeof(SharedOrders), PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    close(mem_fd);
}

void deliver_package(char *customer_name) {
    char *user = get_username();
    time_t now = time(NULL);
    int found = 0;

    pthread_mutex_lock(&shared_data->lock);

    for(int i = 0; i < shared_data->total_orders; i++) {
        Order *order = &shared_data->orders[i];

        if(strcmp(order->customer, customer_name) == 0 &&
           strcmp(order->service, "Reguler") == 0) {

            found = 1;

            if(strcmp(order->status, "Delivered") == 0) {
                printf("Dont worry, %s. Your package is already delivered!\n", customer_name);
            } else {
                strcpy(order->status, "Delivered");
                strcpy(order->handled_by, user);

                struct tm *tm = localtime(&now);
                char timestamp[20];
                strftime(timestamp, 20, "%d/%m/%Y %H:%M:%S", tm);

                FILE *log = fopen("delivery.log", "a");
                if(log) {
                    fprintf(log, "[%s] [AGENT %s] Reguler package delivered to %s in %s\n",
                            timestamp, user, order->customer, order->address);
                    fclose(log);
                }

                printf("%s your package has been successfully delivered!\n", customer_name);
            }
            break;
        }
    }

    pthread_mutex_unlock(&shared_data->lock);

    if (!found) {
        printf("No matching Reguler order found for %s.\n", customer_name);
    }
}

void check_status(char *customer_name) {
    pthread_mutex_lock(&shared_data->lock);
    
    for(int i = 0; i < shared_data->total_orders; i++) {
        Order *order = &shared_data->orders[i];
        
        if(strcmp(order->customer, customer_name) == 0) {
            if(strcmp(order->status, "Delivered") == 0) {
                printf("Status for %s: Delivered by Agent %s\n", customer_name, order->handled_by);
            } else {
                printf("Status for %s: Pending\n", customer_name);
            }
            break;
        }
    }
    
    pthread_mutex_unlock(&shared_data->lock);
}

void list_orders() {
    pthread_mutex_lock(&shared_data->lock);

    printf("\n            RushGo Package Statuses\n");
    printf("     ========================================\n");
    printf("     |   Customer    |  Service |  Status   |\n");
    printf("     ----------------------------------------\n");

    for(int i = 0; i < shared_data->total_orders; i++) {
        Order *order = &shared_data->orders[i];
        printf("     | %-13s | %-8s | %-9s |\n", 
               order->customer, 
               order->service, 
               order->status);
    }

    printf("     ----------------------------------------\n");
    printf("  For more informations, please hit our Contact Person\n\n");

    pthread_mutex_unlock(&shared_data->lock);
}


int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Usage:\n");
        printf("  %s -deliver [customer_name]\n", argv[0]);
        printf("  %s -status [customer_name]\n", argv[0]);
        printf("  %s -list\n", argv[0]);
        return 1;
    }
    
    connect_to_shared_mem();
    
    if(strcmp(argv[1], "-deliver") == 0 && argc == 3) {
        deliver_package(argv[2]);
    }
    else if(strcmp(argv[1], "-status") == 0 && argc == 3) {
        check_status(argv[2]);
    }
    else if(strcmp(argv[1], "-list") == 0) {
        list_orders();
    }
    else {
        printf("Invalid command!\n");
    }
    
    munmap(shared_data, sizeof(SharedOrders));
    return 0;
}