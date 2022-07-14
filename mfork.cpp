#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include "skiplist.h"
#include<string>

#define m_display "\n-----------------------------\n----- KV storage engine -----\n--- 1. search  element    ---\n--- 2. insert  element    ---\n--- 3. delete  element    ---\n"

const int BUF_SIZE = 1024;

using namespace std;

SkipList<int, std::string> skipList(20);
int cnt=0;

void serverportal(int& clientfd);
void sig_chld(int sig)
{
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        cout << "child " << pid << " termination" << endl;
    return;
}

int main()
{
    int serv_sockfd;
    int client_sockfd;

    char buf[BUF_SIZE];
    memset(buf, 0, sizeof(buf));

    sockaddr_in serv_addr;
    sockaddr_in client_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8000);

    if ((serv_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "socket error" << endl;
        return -1;
    }

    if ((bind(serv_sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        cout << "bind error" << endl;
        return -2;
    }

    if ((listen(serv_sockfd, 5)) < 0)
    {
        cout << "listen error" << endl;
        return -3;
    }
    cout << "listening..." << endl;

    signal(SIGCHLD, sig_chld);

    socklen_t sin_size;
    int read_len = 0;
    int child_pid;
    while (1)
    {
        sin_size = sizeof(client_addr);
        memset(&client_addr, 0, sizeof(client_addr));
        if ((client_sockfd = accept(serv_sockfd, (sockaddr*)&client_addr, &sin_size)) < 0)
        {
            if (errno == EINTR || errno == ECONNABORTED)
                continue;
            else
            {
                cout << "accept error" << endl;
                return -4;
            }
        }

        if ((child_pid = fork()) == 0)
        {
            cout << "client " << inet_ntoa(client_addr.sin_addr) << " 进程号：" << getpid() << endl;
            //write(client_sockfd, "Welcome to my server", 21);
            close(serv_sockfd);
            //进入KV存储界面
	    serverportal(client_sockfd);
	    close(client_sockfd);
            exit(-6);
        }
        else if (child_pid > 0)
            close(client_sockfd);
    }
    return 0;
}



void serverportal(int& clientfd) {
	char buf[1024];
	bool flag = true;
	memset(buf, 0, sizeof(buf));
	while (flag) {
        	int iret;//返回值
        	strcpy(buf, m_display);
        	if (iret = send(clientfd, buf, strlen(buf), 0) <= 0) {
            		cout << "send error:" << iret << endl;
            		break;
        	}
        	//cout << "server send" << buf << endl;
        	//接收客户端的请求值
        	memset(buf, 0, sizeof(buf));
        	if (iret = recv(clientfd, buf, sizeof(buf), 0) <= 0) {
        		cout << "recv error:" << iret << endl;
       			break;
                }
        	cout << "server recv:" << buf << endl; // 这里加一个client的pid
        	int choose = atoi(buf);
        	cout << choose << endl;
        	//memset(buf,0,sizeof(buf));
        //***************************************************************************/
        skipList.insert_element(cnt++,"hello");
        switch (choose) {
                case 0:
        		memset(buf, 0, sizeof(buf));
        		strcpy(buf, "are you sure to quit out? \n Yes: y   No: n");
        		send(clientfd, buf, strlen(buf), 0);
        		memset(buf, 0, sizeof(buf));
        		if (iret = recv(clientfd, buf, sizeof(buf), 0) <= 0) {
        			cout << "recv error:" << iret << endl;
        			flag = false;
                     	}
         		break;
		
                default: break;
        }
	skipList.display_list();    
	//***************************************************************************/
      }
}


