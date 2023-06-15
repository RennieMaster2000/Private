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
    printf("driving to a pos(%i,%i)\n", x, y);
    ////routing
    clearNodeBoard();
    //printf("request route\n");
    Route(curX, curY,curO,x,y);
    //printf("made route\n");
    //printf("routed to the pos\n");
    routeLN = retrieveRouteInfo(&routeDir, &routeX, &routeY);
    //printf("recieved route\n");
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
int doinCOM = 0;
void send(int d){
    if(doinCOM) sendR(d);
    else sendD(d);
}
int recieve(void){
    if(doinCOM) return recieveR();
    else return recieveD();
}
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
    send(6);
    recieve();
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
    int D1[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int minesFound = 0;
    while(nodesToVisit){
        //spread
        for(int i = 0; i < 49; i++){
            board[0][i] = 0;
        }
        pureSpreading(curX, curY, board);
        //determining furthest
        int index = 99;
        int farthest = 0;
        for(int i = 0; i < 25; i++){
            if(!D1[i]){
                //printf("comp");
                if(board[X[i]][Y[i]]>farthest){
                    farthest = board[X[i]][Y[i]];
                    index = i;
                }
            }
        }
        printf("///Driving to index %i:(%i,%i)///\n",index,X[index],Y[index]);
        //drive(if successful: remove node; else: note the mine)
        /////////
        ////routing
        int result = 1;
        clearNodeBoard();
        Route(curX, curY,curO,X[index],Y[index]);
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
                result = 0;
                break;
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
                int fI = curX*5 + curY;
                if(!D1[fI]){ nodesToVisit--; D1[fI] = 1; }
                //curX = routeX[i];
                //curY = routeY[i];
            }
        }
        /////////
        if(result){
            //D1[index]=1;
            //nodesToVisit--;
        }
        else{
            printf("///mine found in direction %i of (%i,%i)///\n", (curO+2)%4, curX, curY);
            minesFound++;
            if(minesFound >= 12) break;
        }
    }
    ////searching for treasure
    send(7);
    recieve();
    int D2[25];
    int treasuresFound = 0;
    nodesToVisit = 25;
    while(nodesToVisit){
        //spread
        for(int i = 0; i < 49; i++){
            board[0][i] = 0;
        }
        pureSpreading(curX, curY, board);
        //determining furthest
        int index = 99;
        int farthest = 99;
        for(int i = 0; i < 25; i++){
            if(!D2[i]){
                if(farthest < board[X[i]][Y[i]]){
                    farthest = board[X[i]][Y[i]];
                    index = i;
                }
            }
        }
        //drive(if successful: remove node; else: note the mine)
        ////////////
        ////routing
        int result = 1;
        clearNodeBoard();
        Route(curX, curY,curO,X[index],Y[index]);
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
                result = 0;
                break;
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
                int fI = curX*5 + curY;
                if(!D2[fI]){ nodesToVisit--; D2[fI] = 1; }
                //curX = routeX[i];
                //curY = routeY[i];
            }
        }
        ////////////
        if(result){
            D2[index]=1;
            nodesToVisit--;
        }
        else{
            printf("///treasure found in direction %i of (%i,%i)///\n", (curO+2)%4, curX, curY);
            treasuresFound++;
            if(treasuresFound >= 3) break;
        }
    }
    send(4);
}

void doBetterC(void){
    send(6);
    recieve();
    ///ini playing field
    int EdgesMidX[40];
    int EdgesMinX[40];
    int EdgesMaxX[40];
    int EdgesMidY[40];
    int EdgesMinY[40];
    int EdgesMaxY[40];
    int EdgesDone[40];
    int i = 0;
    //south
    for(int x = 0; x < 5; x++){
        for(int y = 0; y < 4; y++){
            EdgesMinX[i] = x;
            EdgesMinY[i] = y;
            EdgesMaxX[i] = x;
            EdgesMaxY[i] = y+1;
            EdgesMidX[i] = 2*x;
            EdgesMidY[i] = 2*y + 1;
            EdgesDone[i] = 0;
            i++;
        }
    }
    //east
    for(int x = 0; x < 4; x++){
        for(int y = 0; y < 5; y++){
            EdgesMinX[i] = x;
            EdgesMinY[i] = y;
            EdgesMaxX[i] = x+1;
            EdgesMaxY[i] = y;
            EdgesMidX[i] = 2*x+1;
            EdgesMidY[i] = 2*y;
            EdgesDone[i] = 0;
            i++;
        }
    }
    ////loop search mines
    int minesLeft = 12;
    //minesLeft = 0;
    while(minesLeft){
        //int board[7][7];
        //spread
        //pureSpreading(curX, curY, board);
        //
        int closestEdge = 0;
        int distance = 99;
        //printf("determining closest edge\n");
        for(int j = 0; j < 40; j++){
            if(!EdgesDone[j]){
                int disX = 2*curX - EdgesMidX[j];
                if(disX<0) disX = -disX;
                int disY = 2*curY - EdgesMidY[j];
                if(disY<0) disY = -disY;
                if(disX + disY < distance){
                    distance = disX + disY;
                    closestEdge = j;
                    //printf("New closest Edge at distance %i and dir %i from (%i,%i) to point (%i,%i)\n",distance, j>20, EdgesMinX[j], EdgesMinY[j],curX,curY);
                }
                /*
                int disMin = board[EdgesMinX[j]][EdgesMinY[j]];
                int disMax = board[EdgesMaxX[j]][EdgesMaxY[j]];
                if(disMin < distance){
                    closestEdge = j;
                    distance = disMin;
                }
                else if(disMax<distance){
                    closestEdge = j;
                    distance = disMax;
                }
                */
            }
        }
        int closeX;
        int closeY;
        int farX;
        int farY;
        if(closestEdge<20){
            //south
            int dMin = EdgesMinY[closestEdge] - curY;
            if(dMin<0) dMin = -dMin;
            int dMax = EdgesMaxY[closestEdge] - curY;
            if(dMax<0) dMax = -dMax;
            if(dMin<dMax){
                closeX = EdgesMinX[closestEdge];
                closeY = EdgesMinY[closestEdge];
                farX = EdgesMaxX[closestEdge];
                farY = EdgesMaxY[closestEdge];
            }
            else{
                farX = EdgesMinX[closestEdge];
                farY = EdgesMinY[closestEdge];
                closeX = EdgesMaxX[closestEdge];
                closeY = EdgesMaxY[closestEdge];
            }
        }
        else{
            //east
            int dMin = EdgesMinX[closestEdge] - curX;
            if(dMin<0) dMin = -dMin;
            int dMax = EdgesMaxX[closestEdge] - curX;
            if(dMax<0) dMax = -dMax;
            if(dMin<dMax){
                closeX = EdgesMinX[closestEdge];
                closeY = EdgesMinY[closestEdge];
                farX = EdgesMaxX[closestEdge];
                farY = EdgesMaxY[closestEdge];
            }
            else{
                farX = EdgesMinX[closestEdge];
                farY = EdgesMinY[closestEdge];
                closeX = EdgesMaxX[closestEdge];
                closeY = EdgesMaxY[closestEdge];
            }
        }
        //moving to closest
        printf("next Edge\n");
        int L = 0;
        if(closeX == curX && closeY ==curY) L = 1;
        else L = DriveToPos(closeX,closeY);
        if(!L){
            printf("Mine not on expected edge!!!!\n");
            minesLeft--;
            //now a random edge was blocked
            int index;
            switch(curO){
                case 0:
                    //south is bomb
                    index = curX*4 + curY;
                    EdgesDone[index] = 1;
                    break;
                case 1:
                    //west is bomb
                    index = (curX-1)*5 + curY;
                    EdgesDone[index] = 1;
                    break;
                case 2:
                    //north is bomb
                    index = curX*4 + curY-1;
                    EdgesDone[index] = 1;
                    break;
                case 3:
                    //east is bomb
                    index = curX*5 + curY;
                    EdgesDone[index] = 1;
                    break;
                default:break;

            }
            continue;
        }
        else{
            EdgesDone[closestEdge] = 1;//travelled over closest edge, so certainly blocked
            if(!DriveToPos(farX,farY)){
                printf("Mine on destination edge!!!!\n");
                minesLeft--;
                continue;
            }
        }
    }
    ////tempppp
    /*
    blockEdge(2,0,0);
    blockEdge(1,1,0);
    blockEdge(3,1,0);
    blockEdge(1,1,1);
    blockEdge(2,1,1);
    blockEdge(4,1,1);
    blockEdge(0,2,1);
    blockEdge(2,2,1);
    blockEdge(4,2,1);
    blockEdge(1,3,0);
    blockEdge(1,3,1);
    blockEdge(3,3,1);
    */
    ////
    //switch modes
    printf("Ready to continue? [y/n] ");
    char awnser;
    scanf(" %c",&awnser);
    printf("Continuing\n");
    startingFrom(1);
    send(6);
    recieve();
    //reset done edges list
    for(int k = 0; k < 40; k++){
        if(k<20){
            //south
            int j = k;
            int y = j%4;
            int x = (j-y)/4;
            if(checkEdge(x,y,1)) EdgesDone[k] = 0;
            else EdgesDone[k] = 1;
        }
        else{
            //east
            int j = k-20;
            int y = j%5;
            int x = (j-y)/5;
            if(checkEdge(x,y,0)) EdgesDone[k] = 0;
            else EdgesDone[k] = 1;
        }
    }
    ////loop search treasure
    int treasureLeft = 3;
    while(treasureLeft){
        int closestEdge = 0;
        int distance = 99;
        for(int j = 0; j < 40; j++){
            if(!EdgesDone[j]){
                int disX = 2*curX - EdgesMidX[j];
                if(disX<0) disX = -disX;
                int disY = 2*curY - EdgesMidY[j];
                if(disY<0) disY = -disY;
                if(disX + disY < distance){
                    distance = disX + disY;
                    closestEdge = j;
                }
            }
        }
        int closeX;
        int closeY;
        int farX;
        int farY;
        if(closestEdge<20){
            //south
            int dMin = EdgesMinY[closestEdge] - curY;
            if(dMin<0) dMin = -dMin;
            int dMax = EdgesMaxY[closestEdge] - curY;
            if(dMax<0) dMax = -dMax;
            if(dMin<dMax){
                closeX = EdgesMinX[closestEdge];
                closeY = EdgesMinY[closestEdge];
                farX = EdgesMaxX[closestEdge];
                farY = EdgesMaxY[closestEdge];
            }
            else{
                farX = EdgesMinX[closestEdge];
                farY = EdgesMinY[closestEdge];
                closeX = EdgesMaxX[closestEdge];
                closeY = EdgesMaxY[closestEdge];
            }
        }
        else{
            //east
            int dMin = EdgesMinX[closestEdge] - curX;
            if(dMin<0) dMin = -dMin;
            int dMax = EdgesMaxX[closestEdge] - curX;
            if(dMax<0) dMax = -dMax;
            if(dMin<dMax){
                closeX = EdgesMinX[closestEdge];
                closeY = EdgesMinY[closestEdge];
                farX = EdgesMaxX[closestEdge];
                farY = EdgesMaxY[closestEdge];
            }
            else{
                farX = EdgesMinX[closestEdge];
                farY = EdgesMinY[closestEdge];
                closeX = EdgesMaxX[closestEdge];
                closeY = EdgesMaxY[closestEdge];
            }
        }
        //moving to closest
        int L = 0;
        if(closeX == curX && closeY ==curY) L = 1;
        else L = DriveToPos(closeX,closeY);
        if(!L){
            printf("treasurrreeee!!!, but unexpected\n");
            treasureLeft--;
            //now a random edge was blocked
            int index;
            switch(curO){
                case 0:
                    //south is bomb
                    index = curX*4 + curY;
                    EdgesDone[index] = 1;
                    break;
                case 1:
                    //west is bomb
                    index = (curX-1)*5 + curY+20;
                    EdgesDone[index] = 1;
                    break;
                case 2:
                    //north is bomb
                    index = curX*4 + curY-1;
                    EdgesDone[index] = 1;
                    break;
                case 3:
                    //east is bomb
                    index = curX*5 + curY+20;
                    EdgesDone[index] = 1;
                    break;
                default:break;

            }
            continue;
        }
        else{
            EdgesDone[closestEdge] = 1;//has always visited this edge
            if(!DriveToPos(farX,farY)){
                printf("treasurrreeee!!!\n");
                treasureLeft--;
                continue;
            }
        }
    }
    send(4);
}

int main(void){
    clearEdgeInfo();
    //int result = DriveToPos(getStationX(2),getStationY(2));
    //printf("result = %i\n", result);
    printf("Welcome to our EPO2: MinEvader3000!\n");
    printf("Wanna use com? [y/n] ");
    char aw;
    scanf("%c", &aw);
    if(aw=='y'){
        printf("What COM port is used? COM");
        int comport;
        scanf("%i",&comport);
        setCOM(comport);
        InitialiseCom();
        doinCOM = 1;
    }
    printf("Where are you located? station: ");
    int startingstation;
    scanf("%i", &startingstation);
    startingFrom(startingstation);
    printf("Would you like to do challenge A, B or C? ");
    char challenge;
    scanf("%c");
    scanf("%c", &challenge);
    //send(6);
    //recieve();
    //send(2);
    //recieve();
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
            //printf("Nah bro, we don't do that here\n");
            printf("We're starting up challenge C right now!\n\n\n");
            //doC();
            doBetterC();
            break;
        default:
            printf("We don't know that challenge here, so get lost!!!\n");
            break;
    }
    if(doinCOM) CloseCom();
    return 0;
}
