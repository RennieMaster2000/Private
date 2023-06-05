#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <time.h>

#define COMPORT "COM2"
#define BAUDRATE CBR_9600
#define DLAY 10
#define TIMEOUT 10

HANDLE hSerial;
//--------------------------------------------------------------
// Function: initSio
// Description: intializes the parameters as Baudrate, Bytesize, 
//           Stopbits, Parity and Timeoutparameters of
//           the COM port
//--------------------------------------------------------------
void initSio(void){

    COMMTIMEOUTS timeouts ={0};
    DCB dcbSerialParams = {0};

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        //error getting state
        printf("error getting state \n");
    }

    dcbSerialParams.BaudRate = BAUDRATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if(!SetCommState(hSerial, &dcbSerialParams)){
        //error setting serial port state
        printf("error setting state \n");
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if(!SetCommTimeouts(hSerial, &timeouts)){
    //error occureed. Inform user
        printf("error setting timeout state \n");
    }
}

//--------------------------------------------------------------
// Function: readByte
// Description: reads a single byte from the COM port into
//              buffer buffRead
//--------------------------------------------------------------
int readByte(char *buffRead) {

    DWORD dwBytesRead = 0;

    if (!ReadFile(hSerial, buffRead, 1, &dwBytesRead, NULL))
    {
        printf("error reading byte from input buffer \n");
    }
    //printf("Byte read from read buffer is: %c \n", buffRead[0]);
    return(0);
}

//--------------------------------------------------------------
// Function: writeByte
// Description: writes a single byte stored in buffRead to
//              the COM port 
//--------------------------------------------------------------
int writeByte(char *buffWrite){

    DWORD dwBytesWritten = 0;

    if (!WriteFile(hSerial, buffWrite, 1, &dwBytesWritten, NULL))
    {
        printf("error writing byte to output buffer \n");
    }
    //printf("Byte written to write buffer is: %c \n", buffWrite[0]);

    return(0);
}


int InitialiseCom(void)
{


    char byteBuffer[BUFSIZ+1];

    //----------------------------------------------------------
    // Open COMPORT for reading and writing
    //----------------------------------------------------------
    hSerial = CreateFile(COMPORT,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    if(hSerial == INVALID_HANDLE_VALUE){
        if(GetLastError()== ERROR_FILE_NOT_FOUND){
            //serial port does not exist. Inform user.
            printf(" serial port does not exist \n");
        }
        //some other error occurred. Inform user.
        printf(" some other error occured. Inform user.\n");
        return 1;
    }

    //----------------------------------------------------------
    // Initialize the parameters of the COM port
    //----------------------------------------------------------

    initSio();

    printf("Opened COM\n");
    return 0;

}

void CloseCom(void){
    CloseHandle(hSerial);
}

//sends the first 8 bits of an int
void sendCOM(int d){
    //could be d has no adress?
    writeByte(&d);
}

//waits till it recieves a byte, and returns it in an int
int recieveCOM(void){
    //test if it already waits to recieve something
    char d = 119;
    int i = 0;
    while(d==119){
        readByte(&d);
        //delay(DLAY);
        i++;
    }
    return (int)d;
}