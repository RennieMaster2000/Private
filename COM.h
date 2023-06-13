void setCOM(int com);
int InitialiseCom(void);

void CloseCom(void);

//sends the first 8 bits of an int
void sendCOM(int d);

//waits till it recieves a byte, and returns it in an int
int recieveCOM(void);