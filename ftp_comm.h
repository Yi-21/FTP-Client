#include <stdio.h>
#include <sys/stat.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define IP_LEN              32

void print_progress(double progress);
unsigned long int get_file_size_by_stat(const char* file);
void dns(char *domain_name, char **ip);
int IsDir(char* path);