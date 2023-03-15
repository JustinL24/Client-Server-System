#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cmath>
#include <cstring>
#include <string>
#include <cstdlib>

//Calculation funtion here
std::string BinaryConvert(double px, double fx){
    //first calculate fbar
    double Fx;
    int length;
    Fx = px/2 + (fx - px); // calculation for Fbar
    length = ceil(log2(1/px) + 1); // calculattion for the length
    std::string B = "";
    while(length--){
        Fx = Fx*2;
        int fract_bit = Fx;
        if(fract_bit == 1){ // convert Fbar to Binary
            Fx = Fx - fract_bit;
            B = B + '1';
        }
        else
            B = B + '0';
            B = B + "";
    }
    return B;
    
    //return Fx;
}

void fireman(int)// code blow is from professor Rincon 
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    signal(SIGCHLD, fireman); 
    if (argc < 2)
    {
        std::cerr << "ERROR, no port provided\n";
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding";
        exit(1);
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0)
        {
            if (newsockfd < 0)
            {
                std::cerr << "ERROR on accept";
                exit(1);
            }
            double ServerP;
            n = read(newsockfd, &ServerP, sizeof(double)); // read probabilites from client
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }
            double ServerFx;
            n = read(newsockfd, &ServerFx, sizeof(double)); // read cumlative probabilities from client
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }
            std::string binary = BinaryConvert(ServerP,ServerFx);
            int l = binary.length();
            char message[l+1];
            strcpy(message, binary.c_str()); // convert binary to char array
            
		    int smessage = strlen(message);    // size of the char array
            n = write(newsockfd, &smessage, sizeof(int)); // first send the size of char array
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            n = write(newsockfd, message, smessage); // send char array itself 
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            
            close(newsockfd);
            //delete[] message;
            _exit(0);
        }
    }
    close(sockfd);
    return 0;
}
