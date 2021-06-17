#include "ftp.h"

//extern logger kLogRun;
//extern async_writer_thread awt;

// int initWblLog(){
// 	// string strLogHead = m_strLogPath + m_strLogPrefix;
//     // string strLogRunHead = strLogHead + "_run.log"; //运行日志

//     // if (!g_logRun.initialize (m_iLogLevelRun, awt.async_file (strLogRunHead.c_str(),
//     //                             cut_by_size_and_day (m_iRunLogSizeM * 1024 * 1024)), "microsecond")){
//     //     cout << "[ERROR]run log init failed" << endl;
//     //     return E_FAIL;
//     // }

//     awt.start ();
//     // cout << "run log init success" << endl;
//     // g_logRun.info("run log init success");
//     return E_OK;
// }
int GetDataPort(char *buf)
{
	int            code= 0;
    int            a,b,c,d,e,f = 0;
	if(sscanf(buf, "%d%*[^(](%d,%d,%d,%d,%d,%d", &code,&a,&b,&c,&d,&e,&f) == 7)
    {
        return e * 256 + f;
    }
    return 0;
}

int FtpSendCmd(int socket, char *cmd, int cmd_len, int wait_time){
    if(!cmd)
        return E_FAIL;

    int ret = 0;
    struct timeval timeout;
	timeout.tv_sec = wait_time;
    timeout.tv_usec = 0;

    //设置发送超时时间
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

    //发送指令
	ret = DoSend(socket, cmd, cmd_len);
    if(ret < 0){
        printf("send cmd error\n");
        return E_FAIL;
    }
    return E_OK;
}

int FtpReceiveAck(int socket, int* ack_code, int wait_time){
    char ftp_recv_buf[64];
    memset(ftp_recv_buf, 0, sizeof(ftp_recv_buf));
    int ret = 0;

    struct timeval timeout;
	timeout.tv_sec = wait_time;
    timeout.tv_usec = 0;

    //设置接受超时时间
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    ret = ReceiveResponse(socket, ftp_recv_buf, sizeof(ftp_recv_buf), 20);
    if(ret == E_FAIL)
        printf("receiving server response error!\n");

    if(1 == sscanf(ftp_recv_buf, "%d", ack_code))
    {
		printf("%s\n", ftp_recv_buf);
        return E_OK;
    }
    else
    {
        printf("Rcv data:%s\n", ftp_recv_buf);
        return E_FAIL;
    }
}

int FtpPasvMode(int socket, int *ack_code, int *data_port, int wait_time)
{
    char ftp_recv_buf[64];
    char cmd[] = "PASV\r\n";
    int cmd_len = strlen(cmd);

    int code = 0;
    int ret;
	
	memset(ftp_recv_buf, 0, sizeof(ftp_recv_buf));

    struct timeval timeout;
	timeout.tv_sec = wait_time;
    timeout.tv_usec = 0;

    //设置接发超时时间
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    if(!ack_code || !data_port){
        return E_FAIL;
    }

    //发送指令
	ret = DoSend(socket, cmd, cmd_len);
    if(ret < 0){
        printf("send cmd error\n");
        return E_FAIL;
    }

    //接收服务器应答
    ReceiveResponse(socket, ftp_recv_buf, sizeof(ftp_recv_buf), 20);
    
	if(1 == sscanf(ftp_recv_buf, "%d", &code)){
        *ack_code = code;
		printf("%s\n", ftp_recv_buf);
    }
    else{
        printf("Rcv data:%s\n", ftp_recv_buf);
        return E_FAIL;
    }

    //取出数据端口号
    *data_port = GetDataPort(ftp_recv_buf);
	if(*data_port <= 1024){
		return E_FAIL;
	}
	return E_OK;
}

void Upload(int data_socket_fd, FILE* local_fs, unsigned long int total_size){
	size_t nread;
	char buf[1024*50];
	unsigned long int sent = 0;
	int ret;

	do {
		nread = fread(buf, 1, sizeof(buf), local_fs);
		char* ptr = buf;
		do {
			//循环发送缓冲区内数据
			ret = send(data_socket_fd, ptr, nread, 0);
			if (ret < 0)
				break;
			ptr += ret;
			nread -= ret;
			sent += ret;
		} while (nread);
		print_progress(sent / (double)total_size);
		if (sent == total_size) {
			//文件全部发送
			break;
		}
	} while (ret > 0);
	printf("\n");
}

int ReceiveResponse(int socket, char* ftp_recv_buf, int buf_size, int wait_time = 20){
    int ret;
    char* recv_payload = ftp_recv_buf;
    int recv_size = 0;
    do {
		ret = DoReceive(socket, recv_payload, buf_size - recv_size, wait_time);
		if (ret < 0) {
			return E_FAIL;
		}
		recv_size += ret;
		recv_payload += ret;
    }while(*(ftp_recv_buf + recv_size - 1) != '\n');
    return E_OK;
}

int DoReceive(int socket, char* ftp_recv_buf, int buf_size, int wait_time){
    int ret;
    fd_set readset;
	FD_ZERO(&readset);
	FD_SET(socket, &readset);

    struct timeval timeout;
	timeout.tv_sec = wait_time;
    timeout.tv_usec = 0;

    //等待应答
    if (select(socket + 1, &readset, NULL, NULL, &timeout) <= 0) {
        printf("select socket error!\n");
        return E_FAIL;
    }
    ret = recv(socket, ftp_recv_buf, buf_size, 0);
    if (ret < 0) {
        return E_FAIL;
    }

    return ret;
}


int DoSend(int socket, char* ftp_send_buf, int buf_size){
    char* send_payload = ftp_send_buf;
    int sent_size = 0;
    int ret;

    while(sent_size < buf_size){
    	ret = send(socket, send_payload, buf_size - sent_size, 0);
		if (ret < 0) {
			return E_FAIL;
    	}
		sent_size += ret;
        send_payload += ret;
	}
    return E_OK;
}

// CFtp::~CFtp(){
//     if(local_fs){
//         fclose(local_fs);
//     }
//     if(control_socket_fd >= 0){
//         close(socket_fd);
//     }
//     if(data_socket_fd >= 0){
//         close(data_fd);
//     }
// }