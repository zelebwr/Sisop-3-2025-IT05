#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define SHARED_MEM_NAME "/rushgo_memory"
#define MAX_ORDERS 100

typedef struct {
    char nama[100];
    char alamat[100];
    char service[10];
    char status[20];
    char handled_by[20];
} Order;

typedef struct {
    pthread_mutex_t lock;
    int total_orders;
    Order orders[MAX_ORDERS];
} SharedOrders;

SharedOrders *order_data;

void write_log(char *agent, char *nama, char *alamat, char *service_type)
{
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, 20, "%d/%m/%Y %H:%M:%S", timeinfo);
    
    FILE *logfile = fopen("delivery.log", "a");
    if(logfile) {
        fprintf(logfile, "[%s] [%s] %s package delivered to %s at %s\n",
                timestamp, agent, service_type, nama, alamat);
        fclose(logfile);
    }
}

void *AGENTNAME(void *agent_name)
{
    char *agent = (char *)agent_name;
    
    while(1) {
        pthread_mutex_lock(&order_data->lock);
        
        int order_found = 0;
        for(int i = 0; i < order_data->total_orders; i++) {
            Order *current = &order_data->orders[i];
            
            if(strcmp(current->service, "Express") == 0 && 
               strcmp(current->status, "Pending") == 0) {
                
                strcpy(current->status, "Delivered");
                strncpy(current->handled_by, agent, 19);
                order_found = 1;
                
                write_log(agent, current->nama, current->alamat, "Express");
                break;
            }
        }
        
        pthread_mutex_unlock(&order_data->lock);
        
        if(order_found) sleep(1);
        else sleep(2);
    }
    return NULL;
}

void Del_Whitespace(char *str)
{
    if(!str) return;
    
    char *start = str;
    while(*start && *start == ' ') start++;
    
    char *end = start + strlen(start) - 1;
    while(end > start && (*end == ' ' || *end == '\n' || *end == '\r')) end--;
    
    *(end + 1) = '\0';
    if(start != str) memmove(str, start, end - start + 2);
}

void load_orders()
{
    if(access("delivery_order.csv", F_OK) != 0) {
        printf("Database isn't found! Please Re-check!!!!\n");
        printf("Place the CSV file in this folder and try again.\n");
        exit(1);
    }

    FILE *file = fopen("delivery_order.csv", "r");
    if(!file) {
        perror("Failed to open the data.");
        exit(1);
    }

    char line[256];
    fgets(line, sizeof(line), file);

    int count = 0;
    while(fgets(line, sizeof(line), file) && count < MAX_ORDERS) {
        char *nama = strtok(line, ",");
        char *alamat = strtok(NULL, ",");
        char *jenis = strtok(NULL, "\n");

        Del_Whitespace(nama);
        Del_Whitespace(alamat);
        Del_Whitespace(jenis);

        Order new_order;
        memset(&new_order, 0, sizeof(Order));
        
        if(nama) strncpy(new_order.nama, nama, 99);
        if(alamat) strncpy(new_order.alamat, alamat, 99);
        if(jenis) strncpy(new_order.service, jenis, 9);
        strcpy(new_order.status, "Pending");
        new_order.handled_by[0] = '\0';
        
        order_data->orders[count] = new_order;
        count++;
    }
    order_data->total_orders = count;
    fclose(file);
}

int main()
{
    int mem_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(mem_fd, sizeof(SharedOrders));
    order_data = mmap(NULL, sizeof(SharedOrders), PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    
    pthread_mutexattr_t atribut_kunci;
    pthread_mutexattr_init(&atribut_kunci);
    pthread_mutexattr_setpshared(&atribut_kunci, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&order_data->lock, &atribut_kunci);
    
    load_orders();
    
    pthread_t agents[3];
    char *agent_ids[] = {"AGENT A", "AGENT B", "AGENT C"};
    
    for(int i = 0; i < 3; i++) {
        pthread_create(&agents[i], NULL, AGENTNAME, agent_ids[i]);
    }
    
    for(int i = 0; i < 3; i++) {
        pthread_join(agents[i], NULL);
    }
    
    munmap(order_data, sizeof(SharedOrders));
    close(mem_fd);
    shm_unlink(SHARED_MEM_NAME);
    
    return 0;
}