//////Improved maths

//converts numbers elliptically to modulus 4 space
//int toMod4(int num);--hidden--
//////Mapping

//station starting coordinates
//x coord of stations nearest node
int stationX[13];
//y coord of stations nearest node
int stationY[13];
//global orientation when driving out of station
int stationGO[13];//global orientation when coming out of station

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
//LTNode base; --hidden--

//routing algoritm
void Route(int startX, int startY, int startDir, int endX, int endY);

////Display region

//print the nodes on route and the directions inbetween
void printRoute(void);