#include <stdio.h>
#include <stdlib.h>
#include "nodeOnly.h"
#include "COM.h"
int curO = 0;
int curX = 1;
int curY = 4;
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
    //printf("driving to a pos\n");
    ////routing
    clearNodeBoard();
    Route(curX, curY,curO,x,y);
    //printf("routed to the pos\n");
    routeLN = retrieveRouteInfo(&routeDir, &routeX, &routeY);
    ////COM Loop
    for(int i = 0; i < routeLN; i++){
        ////send
        send(routeDir[i]);
        curO = (curO+routeDir[i])%4;
        ////recieve
        //recieve
        int response = recieve();
        if(!response){
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
            //printf("moved one step close\n");
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

void send(int d){
    printf("send: %i\n",d);
    sendCOM(d);
}

int recieve(void){
    int d;
    printf("recieve: ");
    d = recieveCOM();
    printf("%i\n",d);
    //scanf("%i", &d);
    return d;
}

void sendD(int d){
    printf("send: %i\n",d);
}

int recieveD(void){
    int d;
    printf("recieve: ");
    scanf("%i", &d);
    return d;
}

void doAorB(int s1, int s2, int s3){
    //start signal
    send(0x06);
    recieve();
    //
    int stations[3] = {s1,s2,s3};
    int notVisited[3] = {1,1,1};
    int toVisit = 3;
    while(toVisit){
        //make list of stations to visit
        int* toVisitStations = (int*)malloc(toVisit*sizeof(int));
        int* toVisitX = (int*)malloc(toVisit*sizeof(int));
        int* toVisitY = (int*)malloc(toVisit*sizeof(int));
        for(int i = 0, j = 0; i < 3; i++){
            if(notVisited[i]){
                //add element to list
                toVisitStations[j] = stations[i];
                toVisitX[j] = getStationX(stations[i]);
                toVisitY[j] = getStationY(stations[i]);
                j++;
            }
        }
        //determine closest station to curPos
        int index = closestPos(curX,curY,toVisitX,toVisitY,toVisit);//function doesn't exist yet
        printf("///traveling to station %i///\n", toVisitStations[index]);

        //drive to shortest, n remove if successfully reached, else continue
        if(DriveToPos(toVisitX[index], toVisitY[index])){
            ////successfully reached
            printf("///station %i reached///\n", toVisitStations[index]);
            for(int i = 0; i < 3; i++){
                if(stations[i]==toVisitStations[index]){
                    //removing station from to-visit list
                    notVisited[i] = 0;
                    toVisit--;
                }
            }
        }
        else{
            printf("///mine found in direction %i of (%i,%i)///\n", (curO+2)%4, curX, curY);
        }
        //release resources
        free(toVisitStations);
        free(toVisitX);
        free(toVisitY);
    }
    send(4);//victory dance
    printf("ahmen!!!\n");
}

int startingGO[13] = {0, 0, 0, 0, 3, 3, 3, 2, 2, 2, 1, 1, 1};
int startingX[13] = {0, 1, 2, 3, 4, 4, 4, 3, 2, 1, 0, 0, 0};
int startingY[13] = {0, 4, 4, 4, 3, 2, 1, 0, 0, 0, 1, 2, 3};
void startingFrom(int station){
    curO = startingGO[station];
    curX = startingX[station];
    curY = startingY[station];
}

int dist(int x, int y){
    int xRes = x - curX;
    if(xRes<0) xRes = -xRes;
    int yRes = y - curY;
    if(yRes<0) yRes = -yRes;
    return xRes + yRes;
}

void doC(void){
    ////global
    //filling list of coords
    int X[25];
    int Y[25];
    int j = 0;
    for(int x = 0; x<5; x++){
        for(int y=0; y<5; y++){
            X[j] = x;
            Y[j] = y;
            j++;
        }
    }
    int nodesToVisit;
    ////searching for mines
    int board[7][7];
    nodesToVisit = 25;
    int D1[25];
    int minesFound = 0;
    while(nodesToVisit){
        //spread
        //determining furthest
        //drive(if successful: remove node; else: note the mine)
    }
    ////searching for treasure
    int D2[25];
    int treasuresFound = 0;
}
int main(void){
    clearEdgeInfo();
    //int result = DriveToPos(getStationX(2),getStationY(2));
    //printf("result = %i\n", result);
    printf("Welcome to our EPO2: MinEvader3000!\n");
    printf("What COM port is used? COM");
    int comport;
    scanf("%i",&comport);
    setCOM(comport);
    InitialiseCom();
    printf("Where are you located? station: ");
    int startingstation;
    scanf("%i", &startingstation);
    startingFrom(startingstation);
    printf("Would you like to do challenge A, B or C? ");
    char challenge;
    scanf("%c");
    scanf("%c", &challenge);
    switch(challenge){
        case 'A':
        case 'B':
            //A&B
            printf("To which stations would you like to travel? [# # #] ");
            int s1, s2, s3;
            scanf("%i %i %i", &s1, &s2, &s3);
            printf("We're starting up challenge A/B right now!\n\n\n");
            doAorB(s1,s2,s3);
            break;
        case 'C':
            //C
            printf("Nah bro, we don't do that here\n");
            break;
        default:
            printf("We don't know that challenge here, so get lost!!!\n");
            break;
    }
    CloseCom();
    return 0;
}
