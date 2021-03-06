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
void writeSector(char*, int);
void readFile(char*, char*);
void deleteFile(char*, char*);
void writeFile(char*, char*, int);
int stringLength(char*);
int stringCompare(char*, char*, int);
void executeProgram(char*, int);
void terminate();
void printNumber(short);
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


void writeSector(char* buffer, int sector){
    int relativeSector = mod(sector, 18) + 1;
    int head = mod(div(sector, 18), 2);
    int track = div(sector, 36);
    interrupt(0x13, 769, buffer, track * 256 + relativeSector, head * 256);
}
void readFile(char* file, char* buffer){   
    char dir[512];
    int i;
    int addAddr, fileSize;
    addAddr = 0;
    readSector(dir, 2); 
    fileSize = stringLength(file);
    if (fileSize > 6) fileSize = 6;
    for(i = 0; i < 512; i+=32){
        if(stringCompare(file, dir + i, fileSize) != 0){
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
void deleteFile(char* file){
    char dir[512];
    char map[512];
    int i;
    int sectorNum, fileSize;
    readSector(dir, 2);
    readSector(map, 1);
    fileSize = stringLength(file);
    if (fileSize > 6) fileSize = 6;
    for(i = 0; i < 512; i+=32){
        if(stringCompare(file, dir + i, fileSize) != 0){
            dir[i] = 0x0;
            i+=6;
            while(dir[i] != 0x0){
                sectorNum = dir[i];
                map[sectorNum - 1] = 0x0;
                dir[i] = 0x0;
                i++;
            }
        }
    }
    writeSector(dir, 2);
    writeSector(map, 1);
}

void writeFile(char* file, char* buffer, int numberOfSectors){
    char dir[512];
    char map[512];
    int i, j;
    int sectorNum, sectorLeft;
    readSector(dir, 2);
    readSector(map, 1);
    for(i = 0; i < 512; i+=32){
        if(dir[i] == 0x0){
            for(j = 0; j < 6; j++){
                dir[i + j] = file[j];
                if (file[j] == 0x0){
                    break;    
                }
            }
            break;
        }
    }
    if (i == 512){
        return;
    }
    i+=6;
    for(sectorLeft = 0; sectorLeft < numberOfSectors; sectorLeft++){
        for(sectorNum = 0; sectorNum < 512; sectorNum++){
            if(map[sectorNum] == 0x0){
                map[sectorNum] = 0xFF;
                dir[i] = sectorNum;
                i++;
                writeSector(buffer + 512 * sectorLeft, sectorNum);
                break;
            }
        }
    }
    while(mod(i, 32) != 0){
        dir[i] = 0x0;
        i++;
    }
    writeSector(dir, 2);
    writeSector(map, 1);
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
    } else if (ax == 6) {
        writeSector(bx, cx);
    } else if (ax == 7) {
        deleteFile(bx);
    } else if (ax == 8) {
        writeFile(bx, cx, dx);
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

int stringLength(char* string){
    int i;
    i = 0;
    while(string[i] != 0x0){
        i++;
    }
    return i;
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
void printNumber(short number){
     char tmp[7];
     char output[7];
     char c;
     short orig, i, j;
 
     for(i = 0; i < 7; i++){
         output[i] = 0;
         tmp[i] = 0;
     }
 
     i = 0;
     orig = number;
     if (number < 0) number = -number;
     do{
         tmp[i++] = mod(number, 10) + '0';}
     while ((number = div(number, 10)) > 0);
 
     if (orig < 0) tmp[i++] = '-';
     i--;
     for (j = 0; i >= 0; i--, j++){
        output[j] = tmp[i];
     }
     interrupt(0x21, 0, output, 0, 0);
}

