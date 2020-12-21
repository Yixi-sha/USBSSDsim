#include <iostream>
#include <cstdio>
#include <functional>

#include <libusb/libusb.h>

#include "base/inc/mutex.h"
#include "base/inc/thread.h"
#include "base/inc/channel.h"
#include "base/inc/eventloop.h"
#include "base/inc/eventloopThreadPool.h"

extern "C"{
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
}

void socket_callback(int fd, EDEN::Timestamp time){
	char buf[4096];
	std::cout <<"net " <<gettid() << " " << time.to_formatted_string() << std::endl;
	int n = read(fd, buf, 4096);
	if(n < 0){
		std::cout << "n <= 0" << std::endl;
		exit(1);
	}
	buf[n] = '\0';
	std::cout << buf << std::endl;
}

void std_callback(int fd, int sockfd,EDEN::EventLoopThreadPool *pool ,EDEN::Timestamp time){
	char buf[4096];
	std::cout << "terminate " <<time.to_formatted_string() << std::endl;
	int n = read(fd, buf, 4096);
	if(n < 0){
		std::cout << "n <= 0" << std::endl;
		exit(1);
	}else if(n == 0){
		pool->end_all_looping();
	}else{
		buf[n] = '\0';
		write(sockfd, buf, n);
	}
}

void set_and_enable_read(EDEN::Channel* channel, const EDEN::Channel::EventCallback& cb){
	channel->set_read_callback(cb);
	channel->enable_read();
}

int main(int argc, char* argv[]){
	EDEN::EventLoopThreadPool *ETloop = new EDEN::EventLoopThreadPool(5);
	ETloop->start();
	int sockFd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in deslAddr;
	bzero(&deslAddr, sizeof(deslAddr));
	if(sockFd < 0){
		std::cout << "sockFd < 0" << std::endl;
		return 1;
	}

	inet_pton(AF_INET, argv[1], &deslAddr);
    deslAddr.sin_port = htons(8888);
    deslAddr.sin_family = AF_INET;
	
	std::cout << gettid() << std::endl;
	if(connect(sockFd, (const struct sockaddr*)(&deslAddr), sizeof(deslAddr))< 0){
		std::cout << "connect(sockFd, (const struct sockaddr *)&deslAddr, sizeof(deslAddr))< 0" << std::endl;
		return 1;
	}
	std::cout << "connect(sockFd, (const struct sockaddr *)&deslAddr, sizeof(deslAddr)) >= 0" << std::endl;
	
	EDEN::Channel *sockChann = EDEN::Channel::construct_channel(ETloop->get_next(), sockFd);
	
	sockChann->set_read_callback(bind(socket_callback, sockFd,std::placeholders::_1));
	sockChann->enable_read();

	EDEN::Channel *inChann = EDEN::Channel::construct_channel (ETloop->get_next(), STDIN_FILENO);
	inChann->set_read_callback(bind(std_callback, STDIN_FILENO,sockFd, ETloop,std::placeholders::_1));
	inChann->enable_read();
	ETloop->loop_wait();
	delete sockChann;
	delete inChann;
	delete ETloop;

	std::cout << "end" << std::endl;

    return 0;
}