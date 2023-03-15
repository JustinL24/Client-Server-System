#include<iostream>
#include<cstdio>
#include<unordered_map>
#include<pthread.h>
#include<vector>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string>

struct Shannon{
    int size;
    std::string s;
    double fre;
    double px;
    double fx;
    double Fbar;
    int portno;
    char* hostn;
    pthread_t id;
    std::string Bin;
};

void *SendtoServer(void*void_ptr){
    Shannon* ladyD = (Shannon*)void_ptr;
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        std::cerr << "ERROR opening socket";
    server = gethostbyname(ladyD -> hostn);
    if (server == NULL)
    {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(ladyD -> portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting";
        exit(1);
    }
   
    double Nprob = ladyD -> px;
    n = write(sockfd, &Nprob, sizeof(double)); // first send probabilies to server 
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    double Nfx = ladyD -> fx;
     n = write(sockfd, &Nfx, sizeof(double)); // send cumlative probabilies to server
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    
    int s;
    n = read(sockfd, &s, sizeof(int)); // read the size of the char array from server
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket";
        exit(1);
    }
    
    char *buffer = new char[s + 1];
    bzero(buffer, s + 1);
    n = read(sockfd, buffer, s); // read char array from server 

    std::string strBin = std::string(buffer);// convert char array to string
    //std::cout << strBin << std::endl;
    ladyD -> Bin = strBin; //assign strBin to each Bin in the struct
    
    delete [] buffer;
    close(sockfd);
}

int main(int argc, char*argv[]){
    
   if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    int portno, n;
    char* hostn;
    hostn=argv[1];
    portno = atoi(argv[2]);
   std::string input;
   std::unordered_map<char,int>map;
   getline(std::cin,input);
   //std::vector<char>iv; // vector to store the entire input string
   //for(int i=0;i<input.length();i++){// store the input string to vector
       //iv.push_back(input[i]);
   //}

   for(int i=0;i<input.length()-1;i++){// bubble sort to sort the input charater 
       for(int j=0;j<input.length()-1-i;j++){
           if(input[j]>input[j+1]){
               char temp = input[j];
               input[j] = input[j+1];
               input[j+1] = temp;
           }
       }
   }
   //std::cout << input;
   double SizeofInput = input.length();
   for(const char &c:input){
       map[c]++;
   }
   int SizeofSymbol = 0;
   for(auto &x:map){ // in the map count the unique charater number
       SizeofSymbol++;
   }

   //std::cout << SizeofSymbol;
   struct Shannon *Code = new Shannon[SizeofSymbol];
   pthread_t *tid = new pthread_t[SizeofSymbol];
   
   Code[0].size = SizeofSymbol;
   
   std::vector<char> sv; // create a vector to store symbol
   std::vector<double>fv;   // create a vector to store frequency
   for(auto i = map.begin(); i != map.end(); i++){
       //Code[i].s = i->first;
       //std::cout<<"char: "<<i->first<<" number: "<<i->second<<std::endl;
       sv.push_back(i->first); // push the unique charater into vector
       fv.push_back(i->second); // push the frequency of unique charater into vetcor
   }
   
   
   for(int i=SizeofSymbol-1;i>=0;i--){ // printing the reverse order
       Code[i].portno = portno;
       Code[i].hostn = hostn;
       Code[i].s = sv[i];              // set Code[i].s equal to unique symbol
       Code[i].fre = fv[i];            // set Code[i].fre equal to frequency
       Code[i].px = Code[i].fre/SizeofInput; // calculate the probabilies
       Code[i].fx = Code[i].px;
       Code[i].fx = Code[i].fx + Code[i+1].fx; // calculate the cumulative probabilies
   }
   
   for(int i=0;i<SizeofSymbol;i++){// create mutilply threads
        //Code[i].portno = portno;
        //Code[i].hostn = hostn;
        pthread_create(&tid[i],NULL,SendtoServer,&Code[i]);
    }

    for (int i=0;i<SizeofSymbol;i++) // join all the threads together 
        	pthread_join(tid[i], NULL);
        	
    std::cout << "SHANNON-FANO-ELIAS Codes:" << std::endl;
    
   for(int i=SizeofSymbol-1;i>=0;i--){
       std::cout << "Symbol  " << Code[i].s << ", Code: " << Code[i].Bin << std::endl;
   }
   
   
    return 0;
}