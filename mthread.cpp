#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <memory.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <chrono>
#include <time.h>
#include "skiplist.h"

#define m_display "\n-----------------------------\n----- KV storage engine -----\n-----------------------------\n--- 1. search  element    ---\n--- 2. insert  element    ---\n--- 3. delete  element    ---\n--- 4. show DB size       ---\n--- 0.  quit  out         ---\n"
using namespace std;


const int Port = 4000;
pthread_mutex_t g_mutext;

template<class K,class V>
struct pthread_data {
    struct sockaddr_in client_addr;
    int sock_fd;
    SkipList<K,V>* p_skiplist;
};

template<class K,class V>
void serverportal(int& clientfd,SkipList<K,V>* skiplist);
void* serveForClient(void* arg);
void* timer(void* arg);
bool intAddtoChar(char* buf,int size);

SkipList<int,string> skiplist(18);
int client_cnt=0;

int main(void) {
 
    int serv_sockfd;
    int client_sockfd;
    	
    pthread_t pt;

    //char buf[BUF_SIZE];
    //memset(buf, 0, sizeof(buf));

    sockaddr_in serv_addr;
    sockaddr_in client_addr;
    socklen_t sin_size = sizeof(client_addr);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(Port);

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
    skiplist.load_file();
    cout << "listening..." << endl;
    
/******************************************************************************/	
		
	pthread_create(&pt, NULL, timer , NULL);
	 

/******************************************************************************/	

    while (1) {
	pthread_data<int,std::string> pdata;
        if((client_sockfd = accept(serv_sockfd, (sockaddr*)&client_addr, &sin_size)) < 0){
	    if (errno == EINTR || errno == ECONNABORTED)
                continue;
            else{
                cout << "accept error" << endl;
                return -4;
            }
	}
        pdata.client_addr = client_addr;
        pdata.sock_fd = client_sockfd;
	pdata.p_skiplist=&skiplist;
        pthread_create(&pt, NULL, serveForClient, (void*)&pdata);
    }
    skiplist.dump_file();
    close(serv_sockfd);
    close(client_sockfd);
    return 0;
}

void* serveForClient(void* arg) {
    ++client_cnt;
    cout<<"theard id = "<<pthread_self()<<" conneting\t\tclient num:"<<client_cnt<<endl;
    struct pthread_data<int,std::string>* pdata = (struct pthread_data<int,std::string>*)arg;
    int client_sockfd = pdata->sock_fd;
    serverportal<int ,std::string>(client_sockfd,pdata->p_skiplist);
    --client_cnt;    
    cout<<"theard id = "<<pthread_self()<<" quit out\t\tclient num:"<<client_cnt<<endl;
    pdata->p_skiplist->dump_file(); 
    pthread_exit(0);
}

void* timer(void* arg){
	auto start = std::chrono::high_resolution_clock::now();
	while(1){
		sleep(1);
		auto finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = finish - start;// doulbe 间隔时间s
		if(elapsed.count()>20 && client_cnt<=5){
			//cout<<"timer run"<<endl;
			start = finish;
			pid_t  pid=fork();
			if( pid == 0  ){
				// 子进程
				cout<<"子进程： "<<getppid() <<"   spid begin to dumpfile"<<endl;
				skiplist.dump_file();
				exit(0);
			}else if( pid > 0 ){
				//父进程
				
			}else if(pid < 0 ){
				cout<<"fork to load failed"<<endl;
			}
		}
	}
	pthread_exit(0);
}

template<class K,class V>
void serverportal(int& clientfd,SkipList<K,V>* skiplist) {
    //srand((unsigned)time(NULL));
    
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
	memset(buf,0,sizeof(buf));
        if (iret = recv(clientfd, buf, sizeof(buf), 0) <= 0) {
            cout << "recv error:" << iret << endl;
            break;
        }
        cout <<"theard id="<<pthread_self()<< "   server recv:" << buf << endl; // 这里加一个client的pid
        int choose = atoi(buf);//将client的选择读出来
	int  _key=-1;
	int size=-1;
	string* _p_val=new string();//这里直接初始化 Val的类型，没有走模板
        switch (choose) {
		memset(buf, 0, sizeof(buf));
        	case 0:
        	    strcpy(buf, "are you sure to quit out? \n Yes: y   No: n");
        	    send(clientfd, buf, strlen(buf), 0);
		    memset(buf, 0, sizeof(buf));
        	    if (iret = recv(clientfd, buf, sizeof(buf), 0) <= 0) {
        	        cout << "recv error:" << iret << endl;
        	        flag = false;
        	    }
		    if(buf[0]=='y'||buf[0]=='Y'){
			memset(buf, 0, sizeof(buf));
                        strcpy(buf, "quit_out");
			send(clientfd, buf, strlen(buf), 0);
			flag = false;
		    }
        	    break;
		case 1:
		    strcpy(buf, "-- search element --\n  please input key:");
                    send(clientfd, buf, strlen(buf), 0);
                    memset(buf, 0, sizeof(buf));	
		    recv(clientfd, buf, sizeof(buf), 0);
		    _key=atoi(buf);
		    memset(buf, 0, sizeof(buf));
		    if( skiplist->search_element( _key, _p_val ) == true ){
			for(int i=0;i<(*_p_val).size();++i)
				buf[i]=(*_p_val)[i];
   		    }else{
			strcpy(buf,"not found the elemnt");
		    }
		    strcat(buf,"\n\npress any button to continue...");
                    send(clientfd, buf, strlen(buf), 0);
		    recv(clientfd, buf, sizeof(buf), 0);
		    break;
		case 2:
		    strcpy(buf, "-- insert element --\n  please input key:");
                    send(clientfd, buf, strlen(buf), 0); 
                    memset(buf, 0, sizeof(buf));            
                    recv(clientfd, buf, sizeof(buf), 0); 
                    _key=atoi(buf);
                    memset(buf, 0, sizeof(buf));
		    strcpy(buf,"\n  please input val:");
		    send(clientfd, buf, strlen(buf), 0);
                    memset(buf, 0, sizeof(buf));
		    recv(clientfd, buf, sizeof(buf), 0);
		    (*_p_val)=(string)buf;//  这里有可能报错
	   	    memset(buf, 0, sizeof(buf));

                    if( skiplist->insert_element( _key, *_p_val ) == 1 ){
                        strcpy(buf,"the key is exist");
                    }else{
                        strcpy(buf,"insert successful");
                    }   
                    strcat(buf,"\n\npress any button to continue...");
                    send(clientfd, buf, strlen(buf), 0); 
                    recv(clientfd, buf, sizeof(buf), 0); 
		    break;
		case 4:{
		    strcpy(buf, "-- show DATABASE size --\n  size = ");
	     	    size=skiplist->size();
		    if(intAddtoChar(buf, size))
	            	strcat(buf,"\n\npress any button to continue...");	
                    send(clientfd, buf, strlen(buf), 0);
                    memset(buf, 0, sizeof(buf));
                    recv(clientfd, buf, sizeof(buf), 0);
		    break;}
        	default: break;
        }
    }
    //cout<<"theard id="<<pthread_self()<<"quit out"<<endl;
}



bool intAddtoChar(char* buf,int size){

	string str=to_string(size);
	int pos = 0;
    	while (buf[pos] != '\0') {
        	++pos;
    	}
        buf[pos++] = ' ';
    	int i = 0;
    	while (pos < 1024  && i<str.size()) {
        	buf[pos++] = str[i++];
    	}
	if(pos==1024){
		//cout<<"pos == sizeof(buf)"<<pos<<endl;
		return false;
	}
	else {
		return true;
	}
}















