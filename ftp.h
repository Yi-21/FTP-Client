#ifndef _FTP_H
#define _FTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <vector>
#include <utility>



#include "tinyxml2.h"
#include "wbl/logger.h"

#include "ftp_comm.h"

using namespace tinyxml2;
using namespace wbl::log;
using namespace std;

#define E_OK 0
#define E_FAIL -1
#define COMM_MAX_BUFLEN 2048

#define PORT                21
#define CMD_LEN             64

int GetDataPort(char *buf);
int FtpPasvMode(int socket, int *ack_code, int* data_port, int wait_time);
int FtpSendCmd(int socket, char *cmd, int cmd_len, int wait_time);
int FtpReceiveAck(int socket, int* ack_code, int wait_time);
void Upload(int data_socket_fd, FILE* local_fs, unsigned long int total_size);
int initWblLog();

int ReceiveResponse(int socket, char* ftp_recv_buf, int buf_size, int wait_time);
int DoReceive(int socket, char* ftp_recv_buf, int buf_size, int wait_time);
int DoSend(int socket, char* ftp_send_buf, int buf_size);

//class CFtp{
//public:
//    CFtp();
//    ~CFtp();
//    int InitConfig(const char* strConfName);
//private:
//    int control_socket_fd;
//    int	data_socket_fd;
//    int ack_code;
//    int	data_port;
//    FILE* local_fs;
//    unsigned long int total_size; 
//    struct sockaddr_in server_addr;
//    struct sockaddr_in data_addr;
//    char buf[COMM_MAX_BUFLEN];
//    char cmd[CMD_LEN];
//    char local_path[50];
//    char local_filename[50];
//    char remote_path[50];
//    char remote_filename[50];
//public:
//    char ip[IP_LEN];
//    char username[50];
//    char password[50];
//    char loclfile[50];
//    char sftppath[50];
//};
// 110    Restart marker reply.                                 ����������Ǵ𸴡�
// 120    Service ready in nnn minutes.                         �����Ѿ�������nnn���Ӻ�ʼ��   
// 125    Data connection already open; transfer starting.      ���������Ѵ򿪣����ڿ�ʼ���䡣 
// 150    File status okay; about to open data connection.      �ļ�״̬������׼�����������ӡ�
// 200    Command okay.                                         ����ȷ����
// 202    Command not implemented, superfluous at this site.    δִ�����վ���ϵ��������
// 211    System status, or system help reply.                  ϵͳ״̬����ϵͳ�����𸴡�
// 212    Directory status.                                     Ŀ¼״̬��
// 213    File status.                                          �ļ�״̬��
// 214    Help message.                                         ������Ϣ��
// 215    NAME system type.                                     NAMEϵͳ���ͣ����У�NAME��Assigned Numbers�ĵ������е���ʽϵͳ���ơ�
// 220    Service ready for new user.                           �������������ִ�����û�������
// 221    Service closing control connection.                   ����رտ������ӡ�����ʵ�����ע����
// 225    Data connection open; no transfer in progress.        �������Ӵ򿪣�û�н����еĴ��䡣
// 226    Closing data connection.                              �ر��������ӡ�������ļ������ѳɹ������磬�����ļ�������ļ�����
// 227    Entering Passive Mode <h1,h2,h3,h4,p1,p2>.            ���뱻��ģʽ(h1,h2,h3,h4,p1,p2)��
// 228    Entering Long Passive Mode.
// 229    Extended Passive Mode Entered.
// 230    User logged in, proceed.                              �û��ѵ�¼���������С�
// 250    Requested file action okay, completed.                ������ļ�������ȷ������ɡ�
// 257    "PATHNAME" created.                                   �Ѵ�����PATHNAME����
// 331    User name okay, need password.                        �û�����ȷ����Ҫ���롣
// 332    Need account for login.                               ��Ҫ��¼�ʻ���
// 350    Requested file action pending further information.    ������ļ��������ڵȴ���һ������Ϣ��
// 421    Service not available, closing control connection.    ���񲻿��ã����ڹرտ������ӡ��������ȷ��������رգ������κ��������һӦ��
// 425    Can't open data connection.                           �޷����������ӡ�
// 426    Connection closed; transfer aborted.                  ���ӱ��رգ����ݴ����жϡ�
// 450    Requested file action not taken.                      δִ��������ļ��������ļ������á�
// 451    Requested action aborted. Local error in processing.  ����Ĳ����쳣��ֹ�����ڴ����ش���
// 452    Requested action not taken.                           δִ������Ĳ�����ϵͳ�洢�ռ䲻����
// 500    Syntax error, command unrecognized.                   �﷨���������޷�ʶ������ܰ�������������̫��֮��Ĵ���
// 501    Syntax error in parameters or arguments.              �ڲ��������﷨����
// 502    Command not implemented.                              δִ�����
// 503    Bad sequence of commands.                             ������������С�
// 504    Command not implemented for that parameter.           δִ�иò��������
// 521    Supported address families are <af1, .., afn>
// 522    Protocol not supported.
// 530    Not logged in.                                        δ��¼��
// 532    Need account for storing files.                       �洢�ļ���Ҫ�ʻ���
// 550    Requested action not taken.                           δִ������Ĳ������ļ������á�
// 551    Requested action aborted. Page type unknown.          ����Ĳ����쳣��ֹ��δ֪��ҳ�����͡�
// 552    Requested file action aborted.                        ������ļ������쳣��ֹ�������洢���䣨���ڵ�ǰĿ¼�����ݼ�����
// 553    Requested action not taken.                           δִ������Ĳ�������������ļ�����
// 554    Requested action not taken: invalid REST parameter.
// 555    Requested action not taken: type or struct mismatch.

#endif
