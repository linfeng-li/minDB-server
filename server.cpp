#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include<string>

#define m_display "\n-----------------------------\n----- KV storage engine -----\n--- 1. search  element    ---\n--- 2. insert  element    ---\n--- 3. delete  element    ---\n"

using namespace std;
typedef int port_type;

void rec_send(int& clientfd);
void display();

int main(){
	port_type m_port;
	cout<<"----server-----"<<endl;
	cout<<"请输入端口号：";
	cin>>m_port;


	//step1:socket
	cout<<"1.listen()"<<endl;
	int listenfd;
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd==-1){
		cout<<"socket build failed"<<endl;
	}

	//step2:bind
	//结构体 struct sockaddr_in
	struct sockaddr_in m_servaddr;    // 服务端地址信息的数据结构。
	memset(&m_servaddr,0,sizeof(m_servaddr));	
	m_servaddr.sin_family=AF_INET;
	m_servaddr.sin_addr.s_addr=htonl(INADDR_ANY);// 任意ip地址
	// htons  将一个无符号短整型数值转换为网络字节序; htonl 将主机的无符号长整形数转换成网络字节顺序
	m_servaddr.sin_port=htons(m_port);//端口号 
	
	cout<<"2. bind()"<<endl;
	if(bind(listenfd, (const sockaddr*)&m_servaddr, sizeof(m_servaddr))!=0){
		cout<<"bind error  port is used"<<endl;
		close(listenfd);
		return -1;
	}


	//step3:listen
	cout<<"3. listening"<<endl;
	if(listen(listenfd,5)!=0){
		close(listenfd);
		cout<<"listen is error"<<endl;
		return 0;
	}


	//step4:accept
	//
	int  clientfd; // client's socket 
	int  socklen = sizeof(struct sockaddr_in); //size of  struct sockaddr_in
	struct sockaddr_in clientaddr;  // the information of client ip address 
	clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, (socklen_t*)&socklen);	
	cout<<"connected -- clientfd="<<clientfd<<endl;
	// struct sockaddr_in* clientaddr=new sockaddr_in;
	
	//step5:recv/send
	rec_send(clientfd);
	
	//step6:关闭资源
	close(clientfd);
	close(listenfd);

	return 0;
}


void rec_send(int& clientfd){

	char buf[1024];
	bool flag=true;
	memset(buf,0,sizeof(buf));
	while (flag) {
		
		int iret;//返回值

		strcpy(buf, m_display);	
	        if (iret = send(clientfd, buf, strlen(buf), 0) <= 0) {
	        
	        	cout << "send error:" << iret << endl;
	                break;
	        }
	        //cout << "server send" << buf << endl;
		
		
		//接收客户端的请求值
		memset(buf,0,sizeof(buf));
		if (iret = recv(clientfd, buf, sizeof(buf), 0) <= 0) {
	       		cout << "recv error:" << iret << endl;
	        	break;
		}
		cout << "server recv:" << buf << endl;
		
		int choose=atoi(buf);
		cout<<choose<<endl;
		//memset(buf,0,sizeof(buf));
		//***************************************************************************/
		switch(choose){
                    case 0:
                        memset(buf, 0, sizeof(buf));
			strcpy(buf,"are you sure to quit out? \n Yes: y   No: n");
                        send(clientfd, buf, strlen(buf), 0);
                        memset(buf, 0, sizeof(buf));
			if (iret = recv(clientfd, buf, sizeof(buf), 0) <= 0) {
	                    cout << "recv error:" << iret << endl; 
			    flag=false;
	                    break;
	                }
			break;
		    default: break;				
			
		}
		//***************************************************************************/
		
	}
}

//void addtobuf(char* buf,int key,string val){
//	memset(buf,0,1024);
//	int i=0;
//	if(key>=0){
//		strcut(buf,"key :");
//	}
//}



void display(){ 
                cout<<"-----------------------------"<<endl;
                cout<<"----- KV storage engine -----"<<endl;
                cout<<"-----------------------------"<<endl;
                cout<<"--- 1. search  element    ---"<<endl;
                cout<<"--- 2. insert  element    ---"<<endl;
                cout<<"--- 3. delete  element    ---"<<endl; 
                cout<<"--- 4. change  element    ---"<<endl;
                cout<<"--- 5. show the size      ---"<<endl;
                cout<<"--- 0. quit               ---"<<endl;
                cout<<"-----------------------------"<<endl;
        return;
}   

