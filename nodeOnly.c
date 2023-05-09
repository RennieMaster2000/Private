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
int stationX[13] = {0, 1, 2, 3, 4, 4, 4, 3, 2, 1, 0, 0, 0};
int stationY[13] = {0, 4, 4, 4, 3, 2, 1, 0, 0, 0, 1, 2, 3};
int stationGO[13] = {0, 0, 0, 0, 3, 3, 3, 2, 2, 2, 1, 1, 1};//global orientation when coming out of station

//holds distances associated with nodes(x,y)
int nodeboard[5][5];

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

//clears distances associated with nodes
void clearNodeBoard(void){
    for(int x = 0; x < 5; x++){
        for(int y = 0; y < 5; y++){
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
    if(s) edgesS[x][y] = 0;
    else edgesE[x][y] = 0;
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
    nodeboard[endX][endY] = 1;
    int iteration = 1;
    LTNode* startNode;
    int lengthList = 0;

    ////infection algorithm
    while(!nodeboard[startX][startY]){
        //spread
        iteration++;
        for(int i = 0; i < spreadersLN; i++){
            LTNode* curS = spreaders[i];
            //left
            if(Inbound(curS->x-1, curS->y)){
                //node exists
                if(edgesE[curS->x-1][curS->y]){
                    //not blocked
                    if(!nodeboard[curS->x-1][curS->y]){
                        //gets infected
                        LTNode* new = (LTNode*) malloc(sizeof(LTNode));
                        new->x = curS->x-1;
                        new->y = curS->y;
                        new->parent = curS;
                        new->next = 0;
                        new->go = 1;
                        endOfChain->next = new;
                        endOfChain = new;
                        nodeboard[new->x][new->y] = iteration;
                        infected[infectedLN] = new;
                        infectedLN++;
                        //check if not startNode
                        if(new->x == startX && new->y == startY){
                            startNode = new;
                            //printf("found\n");
                        }
                        printf("new: %i at c%i%i left\n", iteration, new->y, new->x);
                    }
                }
            }
            //right
            if(Inbound(curS->x+1, curS->y)){
                //node exists
                if(edgesE[curS->x][curS->y]){
                    //not blocked
                    if(!nodeboard[curS->x+1][curS->y]){
                        //gets infected
                        LTNode* new = (LTNode*) malloc(sizeof(LTNode));
                        new->x = curS->x+1;
                        new->y = curS->y;
                        new->parent = curS;
                        new->next = 0;
                        new->go = 3;
                        endOfChain->next = new;
                        endOfChain = new;
                        nodeboard[new->x][new->y] = iteration;
                        infected[infectedLN] = new;
                        infectedLN++;
                        //check if not startNode
                        if(new->x == startX && new->y == startY){
                            startNode = new;
                            //printf("found\n");
                        }
                        printf("new: %i at c%i%i right\n", iteration, new->y, new->x);
                    }
                }
            }
            //up
            if(Inbound(curS->x, curS->y-1)){
                //node exists
                if(edgesS[curS->x][curS->y-1]){
                    //not blocked
                    if(!nodeboard[curS->x][curS->y-1]){
                        //gets infected
                        LTNode* new = (LTNode*) malloc(sizeof(LTNode));
                        new->x = curS->x;
                        new->y = curS->y-1;
                        new->parent = curS;
                        new->next = 0;
                        new->go = 2;
                        endOfChain->next = new;
                        endOfChain = new;
                        nodeboard[new->x][new->y] = iteration;
                        infected[infectedLN] = new;
                        infectedLN++;
                        //check if not startNode
                        if(new->x == startX && new->y == startY){
                            startNode = new;
                            //printf("found\n");
                        }
                        printf("new: %i at c%i%i up\n", iteration, new->y, new->x);
                    }
                }
            }
            //down
            if(Inbound(curS->x, curS->y+1)){
                //node exists
                if(edgesS[curS->x][curS->y]){
                    //not blocked
                    if(!nodeboard[curS->x][curS->y+1]){
                        //gets infected
                        LTNode* new = (LTNode*) malloc(sizeof(LTNode));
                        new->x = curS->x;
                        new->y = curS->y+1;
                        new->parent = curS;
                        new->next = 0;
                        new->go = 0;
                        endOfChain->next = new;
                        endOfChain = new;
                        nodeboard[new->x][new->y] = iteration;
                        infected[infectedLN] = new;
                        infectedLN++;
                        //check if not startNode
                        if(new->x == startX && new->y == startY){
                            startNode = new;
                            //printf("found\n");
                        }
                        printf("new: %i at c%i%i down\n", iteration, new->y, new->x);
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
    //
    if(routeX) free(routeX);
    if(routeY) free(routeY);
    if(routeDir) free(routeDir);
    routeX = (int*)malloc(routeLength*sizeof(int));
    routeY = (int*)malloc(routeLength*sizeof(int));
    routeDir = (int*)malloc((routeLength-1)*sizeof(int));
    LTNode* curTNode = startNode;
    LTNode placeholderStartOrientation;
    placeholderStartOrientation.go = startDir;
    LTNode* prevTNode = &placeholderStartOrientation;
    //determining route
    for(int i = 0; i < routeLength; i++){
        routeX[i] = curTNode->x;
        routeY[i] = curTNode->y;
        if(i < routeLength-1){ routeDir[i] = toMod4(curTNode->go - prevTNode->go); /*printf("c%i%i: dir=%i-%i=%i\n",routeY[i],routeX[i], curTNode->go, prevTNode->go, routeDir[i]);*/}
        //printf("c%i%i at go %i\n",routeY[i],routeX[i], curTNode->go);
        prevTNode = curTNode;
        curTNode = curTNode->parent;
    }
    //clearing memory elements
    LTNode* curCNode = base.next;
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

////executing region
/*
int main(void){
    clearNodeBoard();
    clearEdgeInfo();
    blockEdge(1,0,1);
    blockEdge(1,1,1);
    blockEdge(1,2,1);
    blockEdge(1,3,1);
    blockEdge(3,1,1);
    blockEdge(3,2,1);
    blockEdge(3,3,1);
    blockEdge(3,0,1);
    blockEdge(1,4,0);
    blockEdge(2,0,0);
    Route(stationX[1],stationY[1], stationGO[1],stationX[7],stationY[7]);
    printRoute();
}
*/