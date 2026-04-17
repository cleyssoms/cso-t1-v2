#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void print_uptime() {
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp) {
        double uptime_seconds;
        fscanf(fp, "%lf", &uptime_seconds);
        fclose(fp);

        int d = (int)uptime_seconds / 86400;
        int h = ((int)uptime_seconds % 86400) / 3600;
        int m = ((int)uptime_seconds % 3600) / 60;
        int s = (int)uptime_seconds % 60;
        printf("<p><b>Uptime:</b> %dd %dh %dm %ds</p>", d, h, m, s);
    }
}

void print_meminfo() {
    FILE *fp = fopen("/proc/meminfo", "r");
    char line[256];
    long total = 0, free = 0;
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "MemTotal:", 9) == 0) sscanf(line, "MemTotal: %ld", &total);
            if (strncmp(line, "MemFree:", 8) == 0) sscanf(line, "MemFree: %ld", &free);
        }
        fclose(fp);
        printf("<p><b>RAM:</b> Total: %ld MB | Usada: %ld MB</p>", total/1024, (total-free)/1024);
    }
}

//vesao do sistema
void print_version() {
    FILE *fp = fopen("/proc/version", "r");
    char line[256];

    if (fp && fgets(line, sizeof(line), fp)) {
        printf("<p><b>Kernel:</b> %s</p>", line);
        fclose(fp);
    }
}

//tempo ocioso
void print_idle() {
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp) {
        double up, idle;
        fscanf(fp, "%lf %lf", &up, &idle);
        fclose(fp);

        int d = (int)idle / 86400;
        int h = ((int)idle % 86400) / 3600;
        int m = ((int)idle % 3600) / 60;
        int s = (int)idle % 60;

        printf("<p><b>Idle:</b> %dd %dh %dm %ds</p>", d,h,m,s);
    }
}

//data e hora

void print_datetime() {
    FILE *fp = fopen("/proc/driver/rtc", "r");
    char line[256];
    char date[64] = "";
    char time[64] = "";

    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "rtc_date", 8) == 0) {
                sscanf(line, "rtc_date : %s", date);
            }
            if (strncmp(line, "rtc_time", 8) == 0) {
                sscanf(line, "rtc_time : %s", time);
            }
        }
        fclose(fp);

        printf("<p><b>Data/Hora:</b> %s %s</p>", date, time);
    }
}

//cpu modelo e nucleos

void print_cpu() {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    char line[256];
    int cores = 0;

    if (fp) {
        printf("<p><b>CPU:</b><br>");
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "model name", 10) == 0) {
                printf("%s<br>", line);
                break;
            }
        }

        rewind(fp);
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "processor", 9) == 0) cores++;
        }

        printf("Núcleos: %d</p>", cores);
        fclose(fp);
    }
}

//carga do sistema
void print_load() {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp) {
        double l1, l5, l15;
        fscanf(fp, "%lf %lf %lf", &l1, &l5, &l15);
        fclose(fp);

        printf("<p><b>Carga:</b> %.2f %.2f %.2f</p>", l1, l5, l15);
    }
}

//uso da cpu

void print_cpu_usage() {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return;

    long user, nice, system, idle;
    fscanf(fp, "cpu %ld %ld %ld %ld", &user, &nice, &system, &idle);
    fclose(fp);

    long total = user + nice + system + idle;
    long used = total - idle;

    printf("<p><b>CPU Uso:</b> %.2f%%</p>", (double)used/total * 100);
}
//i/o

void print_io() {
    FILE *fp = fopen("/proc/diskstats", "r");
    char line[256];
    printf("<p><b>I/O (leituras/escritas):</b><br>");
    while (fp && fgets(line, sizeof(line), fp)) {
        char dev[32];
        unsigned long rd_ios, rd_sec, wr_ios, wr_sec;
        sscanf(line, "%*d %*d %s %lu %*d %lu %*d %lu %*d %lu",
               dev, &rd_ios, &rd_sec, &wr_ios, &wr_sec);
        if (strcmp(dev, "sda") == 0) {  // ou o disco principal do QEMU
            printf("Leituras: %lu operações, %lu setores<br>", rd_ios, rd_sec);
            printf("Escritas: %lu operações, %lu setores", wr_ios, wr_sec);
            break;
        }
    }
    if (fp) fclose(fp);
    printf("</p>");
}
//sistema de arquivos
void print_filesystems() {
    FILE *fp = fopen("/proc/filesystems", "r");
    char line[256];

    printf("<p><b>Filesystems:</b><br>");
    while (fp && fgets(line, sizeof(line), fp)) {
        printf("%s<br>", line);
    }
    printf("</p>");
    if (fp) fclose(fp);
}
//dispositivos
void print_devices() {
    FILE *fp = fopen("/proc/devices", "r");
    char line[256];

    printf("<p><b>Devices:</b><br>");
    while (fp && fgets(line, sizeof(line), fp)) {
        printf("%s<br>", line);
    }
    printf("</p>");
    if (fp) fclose(fp);
}
//rede
void print_net() {
    FILE *fp = fopen("/proc/net/dev", "r");
    char line[256];

    printf("<p><b>Rede:</b><br>");
    while (fp && fgets(line, sizeof(line), fp)) {
        printf("%s<br>", line);
    }
    printf("</p>");
    if (fp) fclose(fp);
}
//procesos
void print_processes() {
    DIR *dir = opendir("/proc");
    struct dirent *entry;

    printf("<p><b>Processos:</b><br>");

    while ((entry = readdir(dir)) != NULL) {
        int pid = atoi(entry->d_name);
        if (pid > 0) {
            char path[256];
            sprintf(path, "/proc/%d/comm", pid);

            FILE *fp = fopen(path, "r");
            if (fp) {
                char name[256];
                if (fgets(name, sizeof(name), fp)) {
                    printf("PID %d: %s<br>", pid, name);
                }
                fclose(fp);
            }
        }
    }
    printf("</p>");
    closedir(dir);
}

int main(void) {
    
    printf("Content-Type: text/html\r\n\r\n");
    
    printf("<html><head><meta charset='UTF-8'><title>Monitor do Sistema</title>");
    // Auto-refresh a cada 5 segundos para dinamismo
    printf("<meta http-equiv='refresh' content='5'></head><body>");
    printf("<h1>Status do Sistema Target</h1>");
    
    print_version();
	print_uptime();
	print_idle();
	print_datetime();
	print_cpu();
	print_load();
	print_cpu_usage();
	print_meminfo();
	print_io();
	print_filesystems();
	print_devices();
	print_net();
	print_processes();
    
    printf("</body></html>");
    return 0;
}
