

void runCommand(char*);
int stringCompare(char*, char*, int);
void main(){
    char input[512];
    char changeLine[3];

    changeLine[0] = 0xa;
    changeLine[1] = 0xd;
    changeLine[2] = 0x0;

    while(1){
        interrupt(0x21, 0, "SHELL> ", 0, 0);
        interrupt(0x21, 1, input, 0, 0);
        runCommand(input);
        interrupt(0x21, 0, changeLine, 0, 0);    
    }
}    

void runCommand(char* input){
    if (stringCompare(input, "type", 4) == 1) {
        char buffer[13312];
        interrupt(0x21, 3, input + 5, buffer, 0);
        interrupt(0x21, 0, buffer, 0, 0);
    } else if (stringCompare(input, "execute", 7) == 1) { 
        interrupt(0x21, 4, input + 8, 0x2000, 0);
    }
    else {
        interrupt(0x21, 0, "Bad Command!", 0);
    }
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
