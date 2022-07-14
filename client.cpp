/*
 * 程序名：client.cpp，此程序用于演示socket的客户端
 * 作者：C语言技术网(www.freecplus.net) 日期：20190525
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;
//int main(int argc, char* argv[])
int main()
{

	char m_ip[16];
	int port;

	cout<<"please input your ip address:";
	cin>>m_ip;
	cout<<endl;
	cout<<"please input your port:";
	cin>>port;
	

        // if (argc != 3)
        // {
        //     printf("Using:./client ip port\nExample:./client 127.0.0.1 5005\n\n"); return -1;
        // }

        // step1: bulid client socket
        int sockfd;
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { perror("socket"); return -1; }

        // step2: initiate a connection request to the server
        // servaddr.sin_addr.s_addr=inet_addr(argv[1]);
        /*
        struct hostent* h;
        if ((h = gethostbyname(argv[1])) == 0)   // Specify the ip address of the server
        {
            printf("gethostbyname failed.\n"); close(sockfd); return -1;
        }
        */
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        //add
        servaddr.sin_addr.s_addr=inet_addr(m_ip);
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port); // specify communication port of the server 
        // memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);
        if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)  // Initiate a connection request to the server.
        {
            perror("connect"); close(sockfd); return -1;
        }


        // step3: communicate with server, send a message and wait request,then send another
	int iret,pos;
	char buffer[1024];
	string choose;
        char makesure='y';
        std::string key,val;

	while(1){
	    
	    memset(buffer, 0, sizeof(buffer));	
            if ((iret = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) //recieve the message from server
            {
                printf(" quit process\n", iret); break;
            }

	    if( strcmp(buffer,"quit_out") == 0 )
		break;
	   	
	    printf("%s\n",buffer);
	    //cin.get();
	    //cout<<"please input your choose:";
	    cin>>choose;
	    memset(buffer, 0, sizeof(buffer));
	    for(pos=0;pos<choose.size();++pos){
		buffer[pos]=choose[pos];
	    }
	    if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) // 发送用户选项给server
            {
                      perror("send error"); break;
            }	
	    system("clear");
	/*********************************************************************************/
		//int m_ch=choose[0]-'0';
		//cout<<"m_ch is"<<m_ch<<endl;
		// switch(m_ch){
                //    case 0:
                //       	memset(buffer, 0, sizeof(buffer));
		//	recv(sockfd, buffer, sizeof(buffer), 0);// 收到：是否确定要退出 
                //        printf("\n%s\n",buffer);
                //        cin>>makesure;
                //        if(makesure=='n'){
		//		memset(buffer,0,sizeof(buffer));
		//		strcpy(buffer,"ok");
		//		send(sockfd, buffer, strlen(buffer), 0);
                //                //system("clear");
                //                //continue;
                //        }else{
		//		flag=false;
		//	}
                //        break;
                //    case 1:
                //       	cout<<"---- search ----";
                //        cin>>key;
		//	for(int i=0;i<key.size();++i){
		//		buffer[pos++]=key[i];
		//	}
		//	if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) // send the request message to  the server
            	//	{
                //		perror("send error"); break;
           	//	}
		//	memset(buffer, 0, sizeof(buffer));
		//	//iret = recv(sockfd, buffer, sizeof(buffer), 0)
		//	if ((iret = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) // send the request message to  the server
                //        {
                //                perror("recv error"); break;
                //        }		
		//	printf("%s\n",buffer);		
		//	cin.get();
                //        // skipList.search_element(stoi(key));
                //        break;
                //    case 2:
                //       // cout<<"-- 插入数据 --"<<endl;
                //       // cout<<"请输入键(int)： ";
                //       // cin>>key;
                //       // cout<<"请输入值(string)： ";
                //       // cin>>val;
                //       // skipList.insert_element(stoi(key),val);
                //       // cout<<"插入成功"<<endl;
                //        break;
                //    default: break;
		//}
	/*********************************************************************************/ 	    

    
            //if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) // send the request message to  the server
            //{
            //    perror("send"); break;
            //}
	
	
	}


       // for( int ii = 0; ii < 10; ii++)
       // {
       //     int iret;
       //     memset(buffer, 0, sizeof(buffer));
       //     sprintf(buffer,  " this is the %d message, id is %03d "  , ii + 1, ii + 1);
       //     if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) // send the request message to  the server
       //     {
       //         perror("send"); break;
       //     }
       //     printf("send : %s\n", buffer);

       //     memset(buffer, 0, sizeof(buffer));
       //     if ((iret = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0) //recieve the message from server
       //     {
       //         printf("iret=%d\n", iret); break;
       //     }
       //     printf("receive : %s\n", buffer);
       // }

        // step4: close socket, release the source
        close(sockfd);
}
