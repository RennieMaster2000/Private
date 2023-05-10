#include <stdio.h>
#include "nodeOnly.h"
//test text

//executes Challenge A, starts at station 1 and moves to all stations
void Execute(int s1, int s2, int s3){
    int* routeDir;
    int routeLen;
    ////station 1 to station s1
    clearNodeBoard();
    clearEdgeInfo();
    Route(getStationX(1),getStationY(1),getStationGO(1),getStationX(s1),getStationY(s1));
    routeLen = retrieveRouteDir(&routeDir);
    //send forward
    //send other instructions
    //move into station
    //move out of station

    ////station s1 to station s2
    clearNodeBoard();
    Route(getStationX(s1),getStationY(s1),getStationGO(s1),getStationX(s2),getStationY(s2));
    routeLen = retrieveRouteDir(&routeDir);
    //send forward
    //send other instructions
    //move into station
    //move out of station

    ////station s2 to station s3
    clearNodeBoard();
    Route(getStationX(s2),getStationY(s2),getStationGO(s2),getStationX(s3),getStationY(s3));
    routeLen = retrieveRouteDir(&routeDir);
    //send forward
    //send other instructions
    //move into station
    //move out of station
}

int main(void){
}