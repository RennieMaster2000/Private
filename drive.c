#include <stdio.h>
#include "nodeOnly.h"
int curO = 0;//getStationGO(1);
int curX;
int curY;
int* routeX;
int* routeY;
int* routeDir;
int routeLN;

void send(int d);
int recieve(void);
//drives from a station out of it
void DriveToFrom(int start, int end){
    ////routing
    clearNodeBoard();
    Route(getStationX(start), getStationY(start),getStationGo(start),getStationX(end),getStationY(end));
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
        recieve();
        //apply only when recieving ok
        curX = routeX[i];
        curY = routeY[i];
        //

        //apply when recieving mine
        //curO = (curO+2)%2
        //return 0
        //
    }
    ////drive in
    int driveInLocDir = toMod4(getStationGO(end) + 2 - curO);
    send(driveInLocDir);
    //wait for response? do we get an ok when reaching end staion? y:we send final then, n: we should prob do that
    //!!final is send by the main control, not this function

    //return 1
}

void send(int d){
    printf("send: %i\n",d);
}

int recieve(void){
    int d;
    scanf("%i", &d);
    return d;
}

int main(void){
    clearEdgeInfo();
    DriveToFrom(1,2);
    return 0;
}