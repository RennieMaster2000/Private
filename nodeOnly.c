#include <stdlib.h>
#include <stdio.h>
//////Mapping
//station starting coordinates
int stationX[13] = {0, 1, 2, 3, 4, 4, 4, 3, 2, 1, 0, 0, 0};
int stationY[13] = {0, 4, 4, 4, 3, 2, 1, 0, 0, 0, 1, 2, 3};

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
//////end Mapping

//////Routing
//linked tree node struct
typedef struct LTNode LTNode;
struct LTNode{
    LTNode* next;
    LTNode* parent;
    int x;
    int y;
};


//stores final route
int* routeX;
int* routeY;
int routeLength;
/*add left, right, forward info*/
/*maybe add fake nodes after running algoritm to simulate moving into station(or just required directions)*/

//stores tree base-node
LTNode base;

//routing algoritm
void Route(int startX, int startY, int endX, int endY){
    //assign basevalues
    base.x = endX;
    base.y = endY;
    base.parent = 0;
    base.next = 0;
    
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
                        //printf("new: %i at c%i%i\n", iteration, new->x, new->y);
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
                        //printf("new: %i at c%i%i\n", iteration, new->x, new->y);
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
                        //printf("new: %i at c%i%i\n", iteration, new->x, new->y);
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
                        //printf("new: %i at c%i%i\n", iteration, new->x, new->y);
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
    routeX = (int*)malloc(routeLength*sizeof(int));
    routeY = (int*)malloc(routeLength*sizeof(int));
    LTNode* curTNode = startNode;
    //determining route
    for(int i = 0; i < routeLength; i++){
        routeX[i] = curTNode->x;
        routeY[i] = curTNode->y;
        //printf("c%i%i\n",routeX[i],routeY[i]);
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

////executing region
int main(void){
    clearNodeBoard();
    clearEdgeInfo();
    Route(stationX[1],stationY[1],stationX[9],stationY[9]);
    for(int i = 0; i < routeLength; i++){
        printf("c%i%i ", routeX[i],routeY[i]);
    }
    printf("\b\n");
}