#include "ftp_comm.h"

void print_progress(double progress) {
    int barWidth = 70;
    fprintf(stdout, "[");
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            fprintf(stdout, "=");
        else if (i == pos)
            fprintf(stdout, ">");
        else
            fprintf(stdout, " ");
    }
    fprintf(stdout, "]%d%\r", int(progress * 100.0));
    fflush(stdout);
}

unsigned long int get_file_size_by_stat(const char* file){
    int ret;
    struct stat64 file_info;
    ret = stat64(file, &file_info);
    if(ret >= 0)
        return file_info.st_size;
    else
        return ret;
}

void dns(char *domain_name, char **ip){
    struct hostent *host;
    host = gethostbyname(domain_name);

    inet_ntop(host->h_addrtype, host->h_addr, *ip, IP_LEN);
}

int IsDir(char* path){
    struct stat stat_info;
    if(lstat(path, &stat_info) < 0){
        printf("%s: %s\n", path, strerror(errno));
        return -1;
    }

    return S_ISDIR(stat_info.st_mode);
}