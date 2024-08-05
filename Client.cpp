#include <string.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>

using namespace std;

int main (int argc, char* argv[])
{
    int listenFd, portNo;
    bool loop = false;
    struct sockaddr_in svrAdd;
    struct hostent *server;
    
    if(argc < 3)
    {
        cerr<<"Syntax : ./client <host name> <port>"<<endl;
        return 0;
    }
    
    portNo = atoi(argv[2]);
    
    if((portNo > 65535) || (portNo < 2000))
    {
        cerr<<"Please enter port number between 2000 - 65535"<<endl;
        return 0;
    }       
    
    //create client skt
    listenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if(listenFd < 0)
    {
        cerr << "Cannot open socket" << endl;
        return 0;
    }
    
    server = gethostbyname(argv[1]);
    
    if(server == NULL)
    {
        cerr << "Host does not exist" << endl;
        return 0;
    }
    
    bzero((char *) &svrAdd, sizeof(svrAdd));
    svrAdd.sin_family = AF_INET;
    
    bcopy((char *) server -> h_addr, (char *) &svrAdd.sin_addr.s_addr, server -> h_length);
    
    svrAdd.sin_port = htons(portNo);
    
    int checker = connect(listenFd,(struct sockaddr *) &svrAdd, sizeof(svrAdd));
    
    if (checker < 0)
    {
        cerr << "Cannot connect!" << endl;
        return 0;
    }
    //prints out the message needed like in assignment description
    cout << " Connection established" << endl;
    cout << "  IP: " << argv[1] << " Port " << portNo << endl; 
    //send stuff to server
    for(;;)
    {
        char s[500];
        cout << "Enter a valid file name on the server: " << endl;
        bzero(s, sizeof(s));
        cin.getline(s, sizeof(s));
        //sends messsage to server containing name, dir, or exit command
        write(listenFd, s, strlen(s));

		//Created a response buffer
        char response[500];
        memset(response,0,sizeof(response));
        int bytesRead = read(listenFd, response, sizeof(response));
        //Checks whether response is goodbye to close the port and communication.
        if(strcmp(response,"Goodbye!") == 0){
        	close(listenFd);
        	break;
        }
        //Checks if the server sent a response saying DNE to send file contents to server
        //if it does exist the server will send message saying file exists.
        //Otherwirse we send the info to the server using the else statement.
        else if(strcmp(response, "DNE") != 0){
        	cout << response << endl;
        	bzero(response,sizeof(response));
        }
       else{
       		//Creates a file to open the file to send to server
        	ifstream file(s, ios::binary);
        	if(file){
        		file.seekg(0, ios::end);
        		int msgSize = file.tellg();
        		file.seekg(0,ios::beg);
      			
        		memset(response,0,sizeof(response));
        		char buffer[msgSize];
        		file.read(buffer,msgSize);
        		//sends the file contents to server
        		write(listenFd, buffer, msgSize);
        		//awaits a sresponse to prevent a backlog.
        		read(listenFd, response, sizeof(response));
        	}
        	else{
        		cout << "No such file" << endl;
        	}
        	
        }

        memset(s,0,sizeof(s));
        memset(response,0,sizeof(response));
    }
}
