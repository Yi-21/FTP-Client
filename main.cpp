#include "ftp.h"

//logger kLogRun;
//async_writer_thread awt;

int main(int argc, char **argv)
{
    int                     	socket_fd = -1;
    int		        	    	data_fd = -1;
    int                         ack_code;
    FILE*			            local_fs = NULL;
    unsigned long         	    ip;
    struct sockaddr_in      	server_addr;
    struct sockaddr_in      	data_addr;
    char		            	buf[COMM_MAX_BUFLEN];
    char		     	        cmd[CMD_LEN];
    int			            	data_port = 0;

    unsigned long int           total_size;
    char                        username[50];
    char                        password[50];
    char                        loclfile[50];
    char                        sftppath[50];
    char                        type[] = "A";

    vector<pair<string, string>> file_list;

    if (argc <= 1) {
        fprintf(stderr, "config file not specified\n");
        return 1;
    }
    char* conf_file = argv[1];
    XMLDocument doc;
    if (doc.LoadFile(conf_file) != 0) {
        fprintf(stderr, "load config file error\n");
        return 1;
    }

    //读取服务器和用户配置信息
    XMLElement* root = doc.FirstChildElement("ROOT");
    XMLElement* serverConf = root->FirstChildElement("Server");
    ip = inet_addr(serverConf->FindAttribute("host")->Value());
    memcpy(username, serverConf->FindAttribute("user")->Value(), 50);
    memcpy(password, serverConf->FindAttribute("passwd")->Value(), 50);

    //读取上传文件列表
    XMLElement* file_conf = root->FirstChildElement("Files")->FirstChildElement("File");
    while (file_conf) {
        memcpy(loclfile, file_conf->FindAttribute("src")->Value(), 50);
        memcpy(sftppath, file_conf->FindAttribute("dst")->Value(), 50);
        file_list.push_back(make_pair(loclfile, sftppath));
        file_conf = file_conf->NextSiblingElement();
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("Fail to create a socket to connect with server :%s\n", strerror(errno));
        return -1;
    }
    
    //连接服务器
    server_addr.sin_port = htons(PORT);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = ip;
    memset(buf, 0, COMM_MAX_BUFLEN);

    if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Fail to connect with the server %ld :%s\n", ip, strerror(errno));
        return -1;
    }
    else
    {
    	recv(socket_fd, buf, COMM_MAX_BUFLEN, 0);
    	printf("%s\n", buf);
    	memset(buf, 0, COMM_MAX_BUFLEN);
    }

    //登陆服务器输入用户名
    sprintf(cmd, "USER %s\r\n", username);
    if(FtpSendCmd(socket_fd, cmd, strlen(cmd), 20) != E_OK || FtpReceiveAck(socket_fd, &ack_code, 20) != E_OK ||
        (ack_code != 331 && ack_code != 230)){
        return -1;
    }

    //登陆服务器输入密码
    if(ack_code == 331){
        sprintf(cmd, "PASS %s\r\n", password);
        if(FtpSendCmd(socket_fd, cmd, strlen(cmd), 20) != E_OK || FtpReceiveAck(socket_fd, &ack_code, 20) != E_OK ||
            (ack_code != 230)){
            return -1;
        }
    }

    //切换传输模式
    sprintf(cmd, "TYPE %s\r\n", type);
    if(FtpSendCmd(socket_fd, cmd, strlen(cmd), 20) != E_OK || FtpReceiveAck(socket_fd, &ack_code, 20) != E_OK ||
        (ack_code != 200)){
        return -1;
    }

    //遍历文件列表，逐个上传
    while(!file_list.empty()){
        pair<string, string> file_pair = file_list.back();
        file_list.pop_back();
        memcpy(loclfile, file_pair.first.c_str(), 50);
        memcpy(sftppath, file_pair.second.c_str(), 50);

        int ret = IsDir(loclfile);
        if(ret == E_FAIL)
            continue;

        if(ret){
            DIR *dir = NULL;  
            struct dirent *entry;
            if(!(dir = opendir(loclfile)))  
            {  
                printf("opendir failed!\n");  
                continue;  
            }

            sprintf(sftppath, "%s%s/", sftppath, basename(loclfile));

            //创建远端文件夹，如果文件夹已存在会返回创建失败
            sprintf(cmd, "MKD %s\r\n", sftppath);
            if(FtpSendCmd(socket_fd, cmd, strlen(cmd), 20) != E_OK || FtpReceiveAck(socket_fd, &ack_code, 20) != E_OK){
                return -1;
            }

            //遍历文件夹下内容
            while (entry = readdir(dir)) {
                if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                    char entry_name[50];
                    sprintf(entry_name, "%s/%s", loclfile, entry->d_name);
                    file_list.push_back(make_pair(entry_name, sftppath));
                }
            }
     		closedir(dir);
            continue;
        }
        else{
            local_fs = fopen(loclfile, "r");
            if (!local_fs) 
            {
                printf("Fail to open the local file :%s\n", strerror(errno));
                close(data_fd);
                continue;
            }
        }

        //拼接远端路径和文件名
        strcat(sftppath, basename(loclfile));

        //被动模式获取服务器开放的数据端口号
        if((FtpPasvMode(socket_fd,  &ack_code, &data_port, 20) != E_OK) ||
            (ack_code != 227)){
            printf("PASV error!\n");
            return -1;
        }

        //连接数据端口
        data_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(data_fd < 0)
        {
            printf("Fail to create a data socket :%s\n", strerror(errno));
            break;
        }
        data_addr.sin_family = AF_INET;
        data_addr.sin_port = htons(data_port);
        data_addr.sin_addr.s_addr = ip;

        if(connect(data_fd, (struct sockaddr*)&data_addr, sizeof(data_addr)) < 0)
        {
            printf("Fail to connect with the data port %ld :%s", ip, strerror(errno));
            break;
        }

        //发送上传文件名
        sprintf(cmd, "STOR %s\r\n", sftppath);
        if(FtpSendCmd(socket_fd, cmd, strlen(cmd), 20) != E_OK || FtpReceiveAck(socket_fd, &ack_code, 20) != E_OK ||
            (ack_code != 150)){
            fclose(local_fs);
            close(data_fd);
            continue;
        }

        total_size = get_file_size_by_stat(loclfile);
        printf("upload: %s to %s ...\n", loclfile, sftppath);
        if(total_size > 0)
            Upload(data_fd, local_fs, total_size);

        fclose(local_fs);
        close(data_fd);
        memset(buf, 0, COMM_MAX_BUFLEN);
        if(FtpReceiveAck(socket_fd, &ack_code, 20) != E_OK || ack_code != 226){
            break;
        }
    }

    //退出登陆
    strcpy(cmd, "QUIT\r\n");
    if(FtpSendCmd(socket_fd, cmd, strlen(cmd), 20) != E_OK || FtpReceiveAck(socket_fd, &ack_code, 20) != E_OK ||
        (ack_code != 221)){
        return -1;
    }

    close(socket_fd);
    return 0;
}

