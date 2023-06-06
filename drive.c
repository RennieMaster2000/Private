#include <stdio.h>
#include <stdlib.h>
#include "nodeOnly.h"
#include "COM.h"
int curO = 0;//getStationGO(1);
int curX = 1;//getStationX(1);
int curY = 5;//getStationY(1);
int* routeX;
int* routeY;
int* routeDir;
int routeLN;

void send(int d);
int recieve(void);
//drives from a station out of it
int DriveToFrom(int start, int end){
    ////routing
    clearNodeBoard();
    Route(getStationX(start), getStationY(start),getStationGO(start),getStationX(end),getStationY(end));
    routeLN = retrieveRouteInfo(&routeDir, &routeX, &routeY);
    ////drive out
    int driveOutLocDir = toMod4(getStationGO(start)-curO);
    send(driveOutLocDir);
    //recieve ok
    recieve();
    ////COM Loop
    for(int i = 0; i < routeLN; i++){
        ////send
        send(routeDir[i]);
        curO = (curO+routeDir[i])%4;
        ////recieve
        //recieve
        int response = recieve();
        if(response){
            ////mine
            //apply new orientation, pos stays the same
            curO = (curO+2)%2;
            //blocking edge
            switch(curO){
                case 0://mine on south
                    blockEdge(curX,curY,1);
                    break;
                case 1://mine on west
                    blockEdge(curX-1,curY,0);
                    break;
                case 2://mine on north
                    blockEdge(curX,curY-1,1);
                    break;
                case 3://mine on east
                    blockEdge(curX,curY,0);
                    break;
                default://impossible
                    break;
            }
            //signal failure
            return 0;
        }
        else{
            ////ok
            //apply new position
            curX = routeX[i];
            curY = routeY[i];
        }
    }
    ////drive in
    int driveInLocDir = toMod4(getStationGO(end) + 2 - curO);
    send(driveInLocDir);
    //wait for response? do we get an ok when reaching end staion? y:we send final then, n: we should prob do that
    //!!final is send by the main control, not this function
    recieve();
    return 1;
}

//Drives from current position to destination position, returns 1 if successful, returns 0 otherwise(leaving curPos on last node, and turning orientation away from mine)
int DriveToPos(int x, int y){
    ////routing
    clearNodeBoard();
    Route(curX, curY,curO,x,y);
    routeLN = retrieveRouteInfo(&routeDir, &routeX, &routeY);
    ////COM Loop
    for(int i = 0; i < routeLN; i++){
        ////send
        send(routeDir[i]);
        curO = (curO+routeDir[i])%4;
        ////recieve
        //recieve
        int response = recieve();
        if(response){
            //printf("mine\n");
            ////mine
            //apply new orientation, pos stays the same
            curO = (curO+2)%4;
            //printf("curO:%i\n",curO);
            //blocking edge
            switch(curO){
                case 0://mine on south
                    blockEdge(curX,curY,1);
                    break;
                case 1://mine on west
                    blockEdge(curX-1,curY,0);
                    break;
                case 2://mine on north
                    blockEdge(curX,curY-1,1);
                    break;
                case 3://mine on east
                    blockEdge(curX,curY,0);
                    break;
                default://impossible
                    break;
            }
            //printf("blocked mine path\n");
            //signal failure
            return 0;
        }
        else{
            ////ok
            //apply new position
            switch(curO){
                case 0:curY--;break;
                case 1:curX++;break;
                case 2:curY++;break;
                case 3:curX--;break;
                default:break;
            }
            //curX = routeX[i];
            //curY = routeY[i];
        }
    }
    return 1;
}
//function for position to position(C)
//function for position to station(B)
//possibly seperate station from onboard movement

void sendR(int d){
    printf("send: %i\n",d);
    sendCOM(d);
}

int recieveR(void){
    int d;
    printf("recieve: ");
    d = recieveCOM();
    printf("%i\n",d);
    //scanf("%i", &d);
    return d;
}

void send(int d){
    printf("send: %i\n",d);
}

int recieve(void){
    int d;
    printf("recieve: ");
    scanf("%i", &d);
    return d;
}

void doAorB(int s1, int s2, int s3){
    int stations[3] = {s1,s2,s3};
    int notVisited[3] = {1,1,1};
    int toVisit = 3;
    while(toVisit){
        //make list of stations to visit
        int* toVisitStations = (int*)malloc(toVisit*sizeof(int));
        for(int i = 0, j = 0; i < 3; i++){
            if(notVisited[i]){
                //add element to list
                toVisitStations[j] = stations[i];
                j++;
            }
        }
        //determine closest station to curPos
        int shortest = closestPos();//function doesn't exist yet
        printf("///traveling to station %i///\n", shortest);

        //drive to shortest, n remove if successfully reached, else continue
        if(DriveToPos(getStationX(shortest), getStationY(shortest))){
            ////successfully reached
            for(int i = 0; i < 3; i++){
                printf("///station %i reached///\n", shortest);
                if(stations[i]==shortest){
                    //removing station from to-visit list
                    notVisited[i] = 0;
                    toVisit--;
                }
            }
        }
    }
    send(4);//victory dance
}

int main(void){
    //InitialiseCom();
    clearEdgeInfo();
    int result = DriveToPos(getStationX(2),getStationY(2));
    printf("result = %i\n", result);
    //CloseCom();
    return 0;
}
