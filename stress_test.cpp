
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include "skiplist.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define NUM_THREADS 2000
#define TEST_COUNT 2000

using namespace std;

SkipList<int, std::string> skipList(18);

 char m_ip[16]={'1','9','2','.','1','6','8','.','1','9','9','.','2','1','6'};
 int port=4000;

void *insertElement(void* threadid) {
    long tid; 
    tid = (long)threadid;
 //   std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
//	for (int i=tid*tmp, count=0; count<tmp; i++) {
//        count++;
//		//skipList.insert_element(rand() % TEST_COUNT, "a"); 
//	}
    pthread_exit(NULL);
}

void *getElement(void* threadid) {
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.search_element(rand() % TEST_COUNT); 
	}
    pthread_exit(NULL);
}
void* cntthread(void* threadid){

	long tid;
	tid = (long)threadid;

	int sockfd;
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { perror("socket"); return nullptr; }


        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_addr.s_addr=inet_addr(m_ip);
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port); // specify communication port of the server 
        if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)  // Initiate a connection request to the server.
        {
            cout<<"socket is full"<<endl;
	    cin.get();
	    perror("connect"); close(sockfd); return nullptr;
        }
	cout<<"threadid = "<<tid<<"  socket = "<<(int)sockfd<<endl;

	sleep(60);

	close(sockfd);
	pthread_exit(NULL);
}


int main() {
    srand (time(NULL));  
    {

        pthread_t threads[NUM_THREADS];
        int rc;
        int i;
	/******************************************************************************/
	//char m_ip[16]={'1','9','2','.','1','6','8','.','1','9','9','.','2','1','6'};
        //int port=4000;
	
	//int sockfd;
        //if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { perror("socket"); return -1; }


	//struct sockaddr_in servaddr;
        //memset(&servaddr, 0, sizeof(servaddr));
        //servaddr.sin_addr.s_addr=inet_addr(m_ip);
        //servaddr.sin_family = AF_INET;
        //servaddr.sin_port = htons(port); // specify communication port of the server 
        //if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)  // Initiate a connection request to the server.
        //{
       //     perror("connect"); close(sockfd); return -1;
       // }
	
	/*****************************************************************************/

        auto start = std::chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            std::cout << "main() : creating thread, " << i << std::endl;

            rc = pthread_create(&threads[i], NULL, cntthread , (void *)i); // pthread_create

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {  // pthread_join
                perror("pthread_create() error"); 
                exit(3);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    }

	pthread_exit(NULL);
    return 0;

}
