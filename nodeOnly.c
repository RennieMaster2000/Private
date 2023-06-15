#include <stdlib.h>
#include <stdio.h>
//////Improved maths

//converts numbers elliptically to modulus 4 space
int toMod4(int num){
    num = num%4;
    if (num<0) num+=4;
    return num;
}
//////Mapping

//station starting coordinates
int stationX[13] = {0, 1, 2, 3, 5, 5, 5, 3, 2, 1, -1, -1, -1};
int stationY[13] = {0, 5, 5, 5, 3, 2, 1, -1, -1, -1, 1, 2, 3};
int stationGO[13] = {0, 0, 0, 0, 3, 3, 3, 2, 2, 2, 1, 1, 1};//global orientation when coming out of station

int getStationX(int i){return stationX[i];};
int getStationY(int i){return stationY[i];};
int getStationGO(int i){return stationGO[i];};

//holds distances associated with nodes(x,y)
int nodeboard[7][7];//

//holds edge information(1 open, 0 blocked) (x,y)
int edgesS[5][4];
int edgesE[4][5];

//returns 1 if node is inbound
int Inbound(int x, int y){
    int result = 0;
    if(x >= 0){
        if(x<5){
            if(y >= 0){
                if(y<5){
                    result = 1;
                }
            }
        }
    }
    return result;
}

//returns 1 if node is within imaginative bounds
int InboundIm(int x, int y){
    int result = 0;
    if(x >= 0){
        if(x<5){
            if(y >= 0){
                if(y<5){
                    //only real result
                    result = 1;
                }
                else if(y == 5){
                    if(x >= 1){
                        if(x < 4){
                            result = 1;
                        }
                    }
                }
            }
            else if(y == -1){
                if(x >= 1){
                    if(x < 4){
                        result = 1;
                    }
                }
            }
        }
        else if(x == 5){
            if(y >= 1){
                if(y < 4){
                    result = 1;
                }
            }
        }
    }
    else if(x == -1){
        if(y >= 1){
            if(y < 4){
                result = 1;
            }
        }
    }
    return result;
}

//clears distances associated with nodes
void clearNodeBoard(void){
    for(int x = 0; x < 7; x++){
        for(int y = 0; y < 7; y++){
            nodeboard[x][y] = 0;
        }
    }
}

//clears edge information
void clearEdgeInfo(void){
    for(int a = 0; a < 5; a++){
        for(int b = 0; b < 4; b++){
            edgesE[b][a] = 1;
            edgesS[a][b] = 1;
        }
    }
}

//blocks edge
void blockEdge(int x, int y, int s){
    //printf("blocking x:%i y:%i s:%i\n",x,y,s);
    if(s) edgesS[x][y] = 0;
    else edgesE[x][y] = 0;
}

//returns 1 if Edge is accesible
int checkEdge(int x, int y, int s){
    int result = 0;
    if(s){
        if(x >= 0 && x < 5){
            //not x==-1 or x==5 columns
            if(y == -1 || y == 4){
                if(x >0 && x <4) result = 1;
            }
            else result = edgesS[x][y];
        }
    }
    else{
        if(y >= 0 && y < 5){
            //not y==-1 or y==5 rows
            if(x == -1 || x == 4){
                if(y >0 && y <4) result = 1;
            }
            else result = edgesE[x][y];
        }
    }
    return result;
}
//////end Mapping

//////Routing

//linked tree node struct
typedef struct LTNode LTNode;
struct LTNode{
    LTNode* next;
    LTNode* parent;
    int x;
    int y;
    int go;//global orientation
};


//stores final route
int* routeX;
int* routeY;
int* routeDir;
int routeLength;
/*add left, right, forward info*/
/*maybe add fake nodes after running algoritm to simulate moving into station(or just required directions)*/

//moves dir pointer to routeDir array, and x/y to x/y array(starting from the second note). also returns route direction array length
int retrieveRouteInfo(int** dir, int** x, int** y){
    *dir = routeDir;
    *x = routeX + sizeof(int);
    *y = routeY + sizeof(int);
    return routeLength - 1;
}

//stores tree base-node
LTNode base;

//routing algoritm
void Route(int startX, int startY, int startDir, int endX, int endY){
    //assign basevalues
    base.x = endX;
    base.y = endY;
    base.parent = 0;
    base.next = 0;
    base.go = 4;//unused
    
    ////prepping for infection algorithm
    LTNode* endOfChain = &base;//used to track last entry in linked chain
    //buffer for infection
    LTNode* buff1[128];
    LTNode* buff2[128];
    int buffSwitch = 0;
    //
    LTNode** spreaders = buff1;
    int spreadersLN = 0;
    LTNode** infected = buff2;
    int infectedLN = 0;
    //
    spreaders[0] = &base;
    spreadersLN++;
    //
    nodeboard[endX+1][endY+1] = 1;
    int iteration = 1;
    LTNode* startNode;
    int lengthList = 0;

    //collecting final nodes
    LTNode* topOfTree[16];
    int lnTOT = 0;
    
    //printf("starting spread\n");
    ////infection algorithm
    while(!nodeboard[startX+1][startY+1]){
        //printf("starting route loop\n");
        //check if spread is occurring
        if(!spreadersLN){
            //something gone wrong(inaccessible destination or start)
            printf("\n////\nRouting Error:\n\tno route available, path is inaccessible\n////\n");
            break;
        }
        //spread
        iteration++;
        for(int i = 0; i < spreadersLN; i++){
            LTNode* curS = spreaders[i];
            //printf("spreading from: c%i%i\n", curS->y,curS->x);
            //left
            if(InboundIm(curS->x-1, curS->y)){
                //printf("inbound\n");
                //node exists
                if(checkEdge(curS->x-1,curS->y,0)){
                    //printf("edge open\n");
                    //not blocked
                    if((!nodeboard[curS->x][curS->y+1])|| nodeboard[curS->x][curS->y+1]==iteration){
                        //gets infected
                        LTNode* new = (LTNode*) malloc(sizeof(LTNode));
                        new->x = curS->x-1;
                        new->y = curS->y;
                        new->parent = curS;
                        new->next = 0;
                        new->go = 1;
                        endOfChain->next = new;
                        endOfChain = new;
                        nodeboard[new->x+1][new->y+1] = iteration;
                        infected[infectedLN] = new;
                        infectedLN++;
                        //check if not startNode
                        if(new->x == startX && new->y == startY){
                            startNode = new;
                            topOfTree[lnTOT] = new;
                            lnTOT++;
                            //printf("found\n");
                        }
                        //printf("new: %i at c%i%i left\n", iteration, new->y, new->x);
                    }
                }
            }
            //right
            if(InboundIm(curS->x+1, curS->y)){
                //node exists
                if(checkEdge(curS->x,curS->y,0)){
                    //not blocked
                    if((!nodeboard[curS->x+2][curS->y+1])||nodeboard[curS->x+2][curS->y+1]==iteration){
                        //gets infected
                        LTNode* new = (LTNode*) malloc(sizeof(LTNode));
                        new->x = curS->x+1;
                        new->y = curS->y;
                        new->parent = curS;
                        new->next = 0;
                        new->go = 3;
                        endOfChain->next = new;
                        endOfChain = new;
                        nodeboard[new->x+1][new->y+1] = iteration;
                        infected[infectedLN] = new;
                        infectedLN++;
                        //check if not startNode
                        if(new->x == startX && new->y == startY){
                            startNode = new;
                            topOfTree[lnTOT] = new;
                            lnTOT++;
                            //printf("found\n");
                        }
                        //printf("new: %i at c%i%i right\n", iteration, new->y, new->x);
                    }
                }
            }
            //up
            if(InboundIm(curS->x, curS->y-1)){
                //node exists
                if(checkEdge(curS->x,curS->y-1,1)){
                    //not blocked
                    if((!nodeboard[curS->x+1][curS->y])||nodeboard[curS->x+1][curS->y]==iteration){
                        //gets infected
                        LTNode* new = (LTNode*) malloc(sizeof(LTNode));
                        new->x = curS->x;
                        new->y = curS->y-1;
                        new->parent = curS;
                        new->next = 0;
                        new->go = 2;
                        endOfChain->next = new;
                        endOfChain = new;
                        nodeboard[new->x+1][new->y+1] = iteration;
                        infected[infectedLN] = new;
                        infectedLN++;
                        //check if not startNode
                        if(new->x == startX && new->y == startY){
                            startNode = new;
                            topOfTree[lnTOT] = new;
                            lnTOT++;
                            //printf("found\n");
                        }
                        //printf("new: %i at c%i%i up\n", iteration, new->y, new->x);
                    }
                }
            }
            //down
            if(InboundIm(curS->x, curS->y+1)){
                //printf("down inbound\n");
                //node exists
                if(checkEdge(curS->x,curS->y,1)){
                    //printf("down edge\n");
                    //not blocked
                    if((!nodeboard[curS->x+1][curS->y+2])||nodeboard[curS->x+1][curS->y+2]==iteration){
                        //printf("down node\n");
                        //gets infected
                        LTNode* new = (LTNode*) malloc(sizeof(LTNode));
                        new->x = curS->x;
                        new->y = curS->y+1;
                        new->parent = curS;
                        new->next = 0;
                        new->go = 0;
                        endOfChain->next = new;
                        endOfChain = new;
                        nodeboard[new->x+1][new->y+1] = iteration;
                        infected[infectedLN] = new;
                        infectedLN++;
                        //check if not startNode
                        if(new->x == startX && new->y == startY){
                            startNode = new;
                            topOfTree[lnTOT] = new;
                            lnTOT++;
                            //printf("found\n");
                        }
                        //printf("new: %i at c%i%i down\n", iteration, new->y, new->x);
                    }
                }
            }
        }
        //switch buffer
        if(buffSwitch){
            spreaders = buff1;
            infected = buff2;
            buffSwitch = 0;
        }
        else{
            spreaders = buff2;
            infected = buff1;
            buffSwitch = 1;
        }
        spreadersLN = infectedLN;
        infectedLN = 0;
    }
    //preping for determining route
    routeLength = iteration;
    //printf("amount of possible paths of length %i: %i\n", routeLength, lnTOT);
    //determining path of least turns
    int index = 0;
    int sumOfTurns = 99;
    LTNode startplaceholder;
    startplaceholder.go = startDir;
    for(int i = 0; i < lnTOT; i++){
        int localSOT = 0;
        LTNode* prevTNode = &startplaceholder;
        LTNode* curTNode = topOfTree[i];
        for(int j = 0; j < routeLength - 1; j++){
            //printf("instruction %i\n...",i);
            int newInstru = (curTNode->go - prevTNode->go+4)%4;
            if(newInstru == 3) newInstru = 1;
            localSOT += newInstru;
            prevTNode = curTNode;
            curTNode = curTNode->parent;
            //printf("gives turn value %i\n",localSOT);
        }
        if(localSOT < sumOfTurns){
            sumOfTurns = localSOT;
            index = i;
        } 
    }
    //printf("pre-a\n");
    startNode = topOfTree[index];
    //printf("a");
    //
    /*
    if(routeX) free(routeX);
    if(routeY) free(routeY);
    if(routeDir) free(routeDir);
    */
    //printf("b");
    routeX = (int*)malloc(routeLength*sizeof(int));
    routeY = (int*)malloc(routeLength*sizeof(int));
    routeDir = (int*)malloc((routeLength-1)*sizeof(int));
    LTNode* curTNode = startNode;
    LTNode placeholderStartOrientation;
    placeholderStartOrientation.go = startDir;
    LTNode* prevTNode = &placeholderStartOrientation;
    //printf("c");
    //determining route
    //printf("calcing instrus\n");
    for(int i = 0; i < routeLength; i++){
        routeX[i] = curTNode->x;
        routeY[i] = curTNode->y;
        if(i < routeLength-1){ routeDir[i] = toMod4(curTNode->go - prevTNode->go); /*printf("c%i%i: dir=%i-%i=%i\n",routeY[i],routeX[i], curTNode->go, prevTNode->go, routeDir[i]);*/}
        //printf("c%i%i at go %i\n",routeY[i],routeX[i], curTNode->go);
        prevTNode = curTNode;
        curTNode = curTNode->parent;
    }
    //printf("calced instrus\n");
    //clearing memory elements
    LTNode* curCNode = base.next;
    //printf("biepbapboep\n%p\nladada", curCNode);
    while(1){
        LTNode* nextNode = curCNode->next;
        //printf("Next node: (%i, %i) at %p\n", nextNode->x,nextNode->y,nextNode);
        free(curCNode);
        if(!nextNode) break;
        curCNode = nextNode;
    }
}

////Display region

//print the nodes on route and the directions inbetween
void printRoute(void){
    for(int i = 0; i < routeLength; i++){
        printf("c%i%i ", routeY[i],routeX[i]);
        if( i < routeLength-1) {
            switch(routeDir[i]){
                case 0:printf("forward ");break;
                case 1:printf("right ");break;
                case 2:printf("backwards ");break;
                default:printf("left ");break;
            }
        }
    }
    printf("\b\n");
}

int closestStation(int startStation, int* endStations, int lnEndStations){
    //spread entire board
    int totalInfected = 1;//if tI=25, or infectedLN=0: stop spreading

    int board[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
    int buff1x[128];
    int buff1y[128];
    int buff2x[128];
    int buff2y[128];

    int* spreadersX = buff1x;
    int* spreadersY = buff1y;
    int spreadersLN = 1;
    int* infectedX = buff2x;
    int* infectedY = buff2y;
    int infectedLN = 0;

    spreadersX[0] = stationX[startStation];
    spreadersY[0] = stationY[startStation];
    int iteration = 1;//
    board[stationX[startStation]][stationY[startStation]] = iteration;
    while(1){
        iteration++;
        //printf("\niteration %i:\n", iteration);
        for(int i = 0; i < spreadersLN; i++){
            int curSX = spreadersX[i];
            int curSY = spreadersY[i];
            //printf("\n\tspread from (%i,%i)\n\t\t", curSX, curSY);
            //left
            if(Inbound(curSX-1, curSY)){
                //node exists
                if(edgesE[curSX-1][curSY]){
                    //not blocked
                    if(!board[curSX-1][curSY]){
                        //gets infected
                        board[curSX-1][curSY] = iteration;
                        infectedX[infectedLN] = curSX - 1;
                        infectedY[infectedLN] = curSY;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-left ");
                    }
                }
                else{
                    //printf("block-left ");
                }
            }
            else{
                //printf("out-left ");
            }
            //right
            if(Inbound(curSX+1, curSY)){
                //node exists
                if(edgesE[curSX][curSY]){
                    //not blocked
                    if(!board[curSX+1][curSY]){
                        //gets infected
                        board[curSX+1][curSY] = iteration;
                        infectedX[infectedLN] = curSX+1;
                        infectedY[infectedLN] = curSY;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-right ");
                    }
                }
                else{
                    //printf("block-right ");
                }
            }
            else{
                //printf("out-right ");
            }
            //up
            if(Inbound(curSX, curSY-1)){
                //node exists
                if(edgesS[curSX][curSY-1]){
                    //not blocked
                    if(!board[curSX][curSY-1]){
                        //gets infected
                        board[curSX][curSY-1] = iteration;
                        infectedX[infectedLN] = curSX;
                        infectedY[infectedLN] = curSY-1;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-up ");
                    }
                }
                else{
                    //printf("block-up ");
                }
            }
            else{
                //printf("out-up ");
            }
            //down
            if(Inbound(curSX, curSY+1)){
                //node exists
                if(edgesS[curSX][curSY]){
                    //not blocked
                    if(!board[curSX][curSY+1]){
                        //gets infected
                        board[curSX][curSY+1] = iteration;
                        infectedX[infectedLN] = curSX;
                        infectedY[infectedLN] = curSY+1;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-down ");
                    }
                }
                else{
                    //printf("block-down ");
                }
            }
            else{
                //printf("out-down ");
            }

        }
        //switch buffer
        int* aX;
        int* aY;
        aX = spreadersX;
        aY = spreadersY;
        spreadersX = infectedX;
        spreadersY = infectedY;
        infectedX = aX;
        infectedY = aY;
        spreadersLN = infectedLN;
        infectedLN = 0;
        if(spreadersLN == 0) break;//stopped spreading
    }
    //determine shortest route
    int shortest = 0;
    int length = 0xFF;
    for(int i = 0; i < lnEndStations; i++){
        //check if shortest
        if(board[stationX[endStations[i]]][stationY[endStations[i]]] < length){
            shortest = endStations[i];
            length = board[stationX[endStations[i]]][stationY[endStations[i]]];
        }
    }
    return shortest;
}

void pureSpreading(int refX, int refY, int* board){
    //spread entire board
    int totalInfected = 1;//if tI=25, or infectedLN=0: stop spreading

    int buff1x[128];
    int buff1y[128];
    int buff2x[128];
    int buff2y[128];

    int* spreadersX = buff1x;
    int* spreadersY = buff1y;
    int spreadersLN = 1;
    int* infectedX = buff2x;
    int* infectedY = buff2y;
    int infectedLN = 0;

    spreadersX[0] = refX;
    spreadersY[0] = refY;
    int iteration = 1;//
    board[(refX+1)*7+refY+1] = iteration;
    while(1){
        iteration++;
        //printf("\niteration %i:\n", iteration);
        for(int i = 0; i < spreadersLN; i++){
            int curSX = spreadersX[i];
            int curSY = spreadersY[i];
            //printf("\n\tspread from (%i,%i)\n\t\t", curSX, curSY);
            //left
            if(InboundIm(curSX-1, curSY)){
                //node exists
                if(checkEdge(curSX-1,curSY,0)){
                    //not blocked
                    if(!board[curSX*7 + curSY+1]){
                        //gets infected
                        board[curSX*7 + curSY+1] = iteration;
                        infectedX[infectedLN] = curSX - 1;
                        infectedY[infectedLN] = curSY;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-left ");
                    }
                }
                else{
                    //printf("block-left ");
                }
            }
            else{
                //printf("out-left ");
            }
            //right
            if(InboundIm(curSX+1, curSY)){
                //node exists
                if(checkEdge(curSX,curSY,0)){
                    //not blocked
                    if(!board[(curSX+2)*7 + curSY+1]){
                        //gets infected
                        board[(curSX+2)*7 + curSY+1] = iteration;
                        infectedX[infectedLN] = curSX+1;
                        infectedY[infectedLN] = curSY;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-right ");
                    }
                }
                else{
                    //printf("block-right ");
                }
            }
            else{
                //printf("out-right ");
            }
            //up
            if(InboundIm(curSX, curSY-1)){
                //node exists
                if(checkEdge(curSX,curSY-1,1)){
                    //not blocked
                    if(!board[(curSX+1)*7 + curSY]){
                        //gets infected
                        board[(curSX+1)*7 + curSY] = iteration;
                        infectedX[infectedLN] = curSX;
                        infectedY[infectedLN] = curSY-1;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-up ");
                    }
                }
                else{
                    //printf("block-up ");
                }
            }
            else{
                //printf("out-up ");
            }
            //down
            if(InboundIm(curSX, curSY+1)){
                //node exists
                if(checkEdge(curSX,curSY,1)){
                    //not blocked
                    if(!board[(curSX+1)*7+curSY+2]){
                        //gets infected
                        board[(curSX+1)*7+curSY+2] = iteration;
                        infectedX[infectedLN] = curSX;
                        infectedY[infectedLN] = curSY+1;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-down ");
                    }
                }
                else{
                    //printf("block-down ");
                }
            }
            else{
                //printf("out-down ");
            }

        }
        //switch buffer
        int* aX;
        int* aY;
        aX = spreadersX;
        aY = spreadersY;
        spreadersX = infectedX;
        spreadersY = infectedY;
        infectedX = aX;
        infectedY = aY;
        spreadersLN = infectedLN;
        infectedLN = 0;
        if(spreadersLN == 0) break;//stopped spreading
    }
}
//returns index of closest point from dest to ref
int closestPos(int refX, int refY, int* destX, int* destY, int destLn){
    //spread entire board
    int totalInfected = 1;//if tI=25, or infectedLN=0: stop spreading

    int board[7][7] = {{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};
    int buff1x[128];
    int buff1y[128];
    int buff2x[128];
    int buff2y[128];

    int* spreadersX = buff1x;
    int* spreadersY = buff1y;
    int spreadersLN = 1;
    int* infectedX = buff2x;
    int* infectedY = buff2y;
    int infectedLN = 0;

    spreadersX[0] = refX;
    spreadersY[0] = refY;
    int iteration = 1;//
    board[refX+1][refY+1] = iteration;
    while(1){
        iteration++;
        //printf("\niteration %i:\n", iteration);
        for(int i = 0; i < spreadersLN; i++){
            int curSX = spreadersX[i];
            int curSY = spreadersY[i];
            //printf("\n\tspread from (%i,%i)\n\t\t", curSX, curSY);
            //left
            if(InboundIm(curSX-1, curSY)){
                //node exists
                if(checkEdge(curSX-1,curSY,0)){
                    //not blocked
                    if(!board[curSX][curSY+1]){
                        //gets infected
                        board[curSX][curSY+1] = iteration;
                        infectedX[infectedLN] = curSX - 1;
                        infectedY[infectedLN] = curSY;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-left ");
                    }
                }
                else{
                    //printf("block-left ");
                }
            }
            else{
                //printf("out-left ");
            }
            //right
            if(InboundIm(curSX+1, curSY)){
                //node exists
                if(checkEdge(curSX,curSY,0)){
                    //not blocked
                    if(!board[curSX+2][curSY+1]){
                        //gets infected
                        board[curSX+2][curSY+1] = iteration;
                        infectedX[infectedLN] = curSX+1;
                        infectedY[infectedLN] = curSY;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-right ");
                    }
                }
                else{
                    //printf("block-right ");
                }
            }
            else{
                //printf("out-right ");
            }
            //up
            if(InboundIm(curSX, curSY-1)){
                //node exists
                if(checkEdge(curSX,curSY-1,1)){
                    //not blocked
                    if(!board[curSX+1][curSY]){
                        //gets infected
                        board[curSX+1][curSY] = iteration;
                        infectedX[infectedLN] = curSX;
                        infectedY[infectedLN] = curSY-1;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-up ");
                    }
                }
                else{
                    //printf("block-up ");
                }
            }
            else{
                //printf("out-up ");
            }
            //down
            if(InboundIm(curSX, curSY+1)){
                //node exists
                if(checkEdge(curSX,curSY,1)){
                    //not blocked
                    if(!board[curSX+1][curSY+2]){
                        //gets infected
                        board[curSX+1][curSY+2] = iteration;
                        infectedX[infectedLN] = curSX;
                        infectedY[infectedLN] = curSY+1;
                        //printf("(%i,%i)",infectedX[infectedLN],infectedY[infectedLN]);
                        infectedLN++;
                    }
                    else{
                        //printf("immune-down ");
                    }
                }
                else{
                    //printf("block-down ");
                }
            }
            else{
                //printf("out-down ");
            }

        }
        //switch buffer
        int* aX;
        int* aY;
        aX = spreadersX;
        aY = spreadersY;
        spreadersX = infectedX;
        spreadersY = infectedY;
        infectedX = aX;
        infectedY = aY;
        spreadersLN = infectedLN;
        infectedLN = 0;
        if(spreadersLN == 0) break;//stopped spreading
    }
    //determine shortest route
    int shortest = 0;
    int length = 0xFF;
    for(int i = 0; i < destLn; i++){
        //check if shortest
        if(board[destX[i]+1][destY[i]+1] < length){
            shortest = i;
            length = board[destX[i]+1][destY[i]+1];
        }
    }
    return shortest;
}

////executing region

/*
int main(void){
    clearNodeBoard();
    clearEdgeInfo();
    
    Route(stationX[1],stationY[1],stationGO[1],stationX[2], stationY[2]);
    printRoute();
   int ends[] = {3, 4 ,5 ,6 ,7 ,8, 9, 10, 11, 12};
   int endsLN = 10;
   int shortestTo1 = closestStation(1, ends, endsLN);
   printf("closest station to 1 is %i\n", shortestTo1);
}
*/