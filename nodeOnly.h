//////Improved maths

//converts numbers elliptically to modulus 4 space
int toMod4(int num);//--hidden--
//////Mapping

//station starting coordinates
//x coord of stations nearest node
int getStationX(int i);

//y coord of stations nearest node
int getStationY(int i);

//global orientation when driving out of station
int getStationGO(int i);//global orientation when coming out of station

//holds distances associated with nodes(x,y)
//int nodeboard[5][5];--hidden--

//holds edge information(1 open, 0 blocked) (x,y)
//int edgesS[5][4];--hidden--
//int edgesE[4][5];--hidden--

//returns 1 if node is inbound
//int Inbound(int x, int y);--hidden--

//clears distances associated with nodes
void clearNodeBoard(void);

//clears edge information
void clearEdgeInfo(void);

//blocks edge
void blockEdge(int x, int y, int s);

//returns 1 if Edge is accesible
int checkEdge(int x, int y, int s);
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
/*
int* routeX;
int* routeY;
int* routeDir;
int routeLength;
*/
/*add left, right, forward info*/
/*maybe add fake nodes after running algoritm to simulate moving into station(or just required directions)*/

//moves dir pointer to routeDir array, and x/y to x/y array(starting from the second note). also returns route direction array length
int retrieveRouteInfo(int** dir, int** x, int** y);

//stores tree base-node
//LTNode base; --hidden--

//routing algoritm
void Route(int startX, int startY, int startDir, int endX, int endY);

//closest station to a start station
int closestStation(int startStation, int* endStations, int lnEndStations);

//spreads and gives distances from ref in board
void pureSpreading(int refX, int refY, int* board);

//returns index of closest point from dest to ref
int closestPos(int refX, int refY, int* destX, int* destY, int destLn);

////Display region

//print the nodes on route and the directions inbetween
void printRoute(void);