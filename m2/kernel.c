/*
Team 34
Weimu Song
Yuqi Zhou
*/
void printString(char*);
void readString(char*);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
int main(){ 
    char line[80];
    char buffer[512];
    makeInterrupt21();
    interrupt(0x21, 1, line, 0, 0);
    interrupt(0x21, 0, line, 0, 0);
    interrupt(0x21, 2, buffer, 30, 0);
    interrupt(0x21, 0, buffer, 0, 0);
    /*printString("enter: \0");
    readString(line); 
    printString(line);*/
    while(1){

    }
}

void printString(char* chars){
    int i;
    i = 0;
    while(*(chars + i) != '\0'){
        interrupt(0x10, 0xe * 256 + *(chars + i), 0, 0, 0); 
        i++;
    }
}

void readString(char* chars){
    int i;
    i = 0;
    while(1){
        char returnedValue = interrupt(0x16, 0, 0, 0, 0);
        if (returnedValue == 0xd){
            interrupt(0x10, 0xe * 256 + 0xa, 0, 0, 0);
            interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0);
            break;
        } else if (returnedValue == 0x8){
            interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
            interrupt(0x10, 0xe * 256 + ' ', 0, 0, 0);
            interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
            if(i > 0){
                i--;
            }        
        } else {
            *(chars + i) = returnedValue;
            interrupt(0x10, 0xe * 256 + returnedValue, 0, 0, 0);
            i++;
        }
    }
    *(chars + i) = 0xa;
    *(chars + i + 1) = 0x0;
}

void readSector(char* buffer, int sector){
    int relativeSector = mod(sector, 18) + 1;
    int head = mod(div(sector, 18), 2);
    int track = div(sector, 36);
    interrupt(0x13, 513, buffer, track * 256 + relativeSector, head * 256);
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
    if (ax == 0){
        printString(bx);
    } else if (ax == 1){
        readString(bx);
    } else if (ax == 2){
        readSector(bx, cx);
    } else{
        printString("The second argument should be less than 3\0");
    }
}
int mod(int a, int b){
    while(a >= b){
        a = a - b;
    }
    return a;
}

int div(int a, int b){
    int quotient = 0;
    while ((quotient + 1) * b <= a) {
        quotient++;
    }
    return quotient;
}
