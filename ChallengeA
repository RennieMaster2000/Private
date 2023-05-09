#include <stdio.h>
#include "nodeOnly.h"


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