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
// 110    Restart marker reply.                                 重新启动标记答复。
// 120    Service ready in nnn minutes.                         服务已就绪，在nnn分钟后开始。   
// 125    Data connection already open; transfer starting.      数据连接已打开，正在开始传输。 
// 150    File status okay; about to open data connection.      文件状态正常，准备打开数据连接。
// 200    Command okay.                                         命令确定。
// 202    Command not implemented, superfluous at this site.    未执行命令，站点上的命令过多
// 211    System status, or system help reply.                  系统状态，或系统帮助答复。
// 212    Directory status.                                     目录状态。
// 213    File status.                                          文件状态。
// 214    Help message.                                         帮助消息。
// 215    NAME system type.                                     NAME系统类型，其中，NAME是Assigned Numbers文档中所列的正式系统名称。
// 220    Service ready for new user.                           服务就绪，可以执行新用户的请求。
// 221    Service closing control connection.                   服务关闭控制连接。如果适当，请注销。
// 225    Data connection open; no transfer in progress.        数据连接打开，没有进行中的传输。
// 226    Closing data connection.                              关闭数据连接。请求的文件操作已成功（例如，传输文件或放弃文件）。
// 227    Entering Passive Mode <h1,h2,h3,h4,p1,p2>.            进入被动模式(h1,h2,h3,h4,p1,p2)。
// 228    Entering Long Passive Mode.
// 229    Extended Passive Mode Entered.
// 230    User logged in, proceed.                              用户已登录，继续进行。
// 250    Requested file action okay, completed.                请求的文件操作正确，已完成。
// 257    "PATHNAME" created.                                   已创建“PATHNAME”。
// 331    User name okay, need password.                        用户名正确，需要密码。
// 332    Need account for login.                               需要登录帐户。
// 350    Requested file action pending further information.    请求的文件操作正在等待进一步的信息。
// 421    Service not available, closing control connection.    服务不可用，正在关闭控制连接。如果服务确定它必须关闭，将向任何命令发送这一应答。
// 425    Can't open data connection.                           无法打开数据连接。
// 426    Connection closed; transfer aborted.                  连接被关闭，数据传输中断。
// 450    Requested file action not taken.                      未执行请求的文件操作。文件不可用。
// 451    Requested action aborted. Local error in processing.  请求的操作异常终止：正在处理本地错误。
// 452    Requested action not taken.                           未执行请求的操作。系统存储空间不够。
// 500    Syntax error, command unrecognized.                   语法错误，命令无法识别。这可能包括诸如命令行太长之类的错误。
// 501    Syntax error in parameters or arguments.              在参数中有语法错误。
// 502    Command not implemented.                              未执行命令。
// 503    Bad sequence of commands.                             错误的命令序列。
// 504    Command not implemented for that parameter.           未执行该参数的命令。
// 521    Supported address families are <af1, .., afn>
// 522    Protocol not supported.
// 530    Not logged in.                                        未登录。
// 532    Need account for storing files.                       存储文件需要帐户。
// 550    Requested action not taken.                           未执行请求的操作。文件不可用。
// 551    Requested action aborted. Page type unknown.          请求的操作异常终止：未知的页面类型。
// 552    Requested file action aborted.                        请求的文件操作异常终止：超出存储分配（对于当前目录或数据集）。
// 553    Requested action not taken.                           未执行请求的操作。不允许的文件名。
// 554    Requested action not taken: invalid REST parameter.
// 555    Requested action not taken: type or struct mismatch.

#endif
