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
void readFile(char*, char*);
int stringCompare(char*, char*, int);
void executeProgram(char*, int);
void terminate();
int main(){ 
    makeInterrupt21();
    interrupt(0x21, 4, "shell\0", 0x2000, 0);
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

void readFile(char* file, char* buffer){   
    char dir[512];
    int i;
    int addAddr;
    addAddr = 0;
    readSector(dir, 2); 
    //printString(dir + 1);
    for(i = 0; i < 512; i+=32){
        if(stringCompare(file, dir + i, 6) != 0){
            i+=6;
            while(dir[i] != 0x0){
                readSector(buffer + addAddr, dir[i]);
                i++;
                addAddr += 512;
            }
            return;
        }
    }
}

void executeProgram(char* name, int segment){
    char buffer[13312];
    int i;
    readFile(name, buffer);
    for(i = 0; i < 13312; i++){
        putInMemory(segment, i, buffer[i]);
    }
    launchProgram(segment);
}

void terminate(){
    char shell[6];
    shell[0] = 's';
    shell[1] = 'h';
    shell[2] = 'e';
    shell[3] = 'l';
    shell[4] = 'l';
    shell[5] = '\0';
    interrupt(0x21, 4, shell, 0x2000, 0);
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
    if (ax == 0) {
        printString(bx);
    } else if (ax == 1) {
        readString(bx);
    } else if (ax == 2) {
        readSector(bx, cx);
    } else if (ax == 3) {
        readFile(bx, cx);
    } else if (ax == 4) {
        executeProgram(bx, cx);
    } else if (ax == 5) {
        terminate();
    } else {
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

int stringCompare(char* str1, char* str2, int size){
    int i;
    for (i = 0; i < size; i++){
        if (str1[i] != str2[i]){
            return 0;
        }
    }
    return 1;
}
