/*
   Team 34
   Weimu Song
   Yuqi Zhou
 */

void runCommand(char*);
int stringCompare(char*, char*, int);
void changeLine();
int mod(int, int);
int div(int, int);
void printNumber(short);
void createNewFile(char*);
void getRidOfEnter(char*, char*);
void main(){
    char input[512];
    enableInterrupts();
    while(1){
        interrupt(0x21, 0, "SHELL> ", 0, 0);
        interrupt(0x21, 1, input, 0, 0);
        runCommand(input);
        changeLine();
    }
}    

void runCommand(char* input){
    if (stringCompare(input, "type", 4) == 1) {
        char buffer[13312], fileName[6];
        getRidOfEnter(input + 5, fileName);
        interrupt(0x21, 3, fileName, buffer, 0);
        interrupt(0x21, 0, buffer, 0, 0);
        buffer[0] = '\0';
    } else if (stringCompare(input, "execute", 7) == 1) {
        interrupt(0x21, 4, input + 8, 0, 0);
    } else if (stringCompare(input, "execforeground", 14) == 1) {
        interrupt(0x21, 10, input + 15, 0, 0);
    } else if (stringCompare(input, "delete", 6) == 1){
        char fileName[6];
        getRidOfEnter(input + 7, fileName);
        interrupt(0x21, 7, fileName, 0, 0); 
    } else if (stringCompare(input, "create", 6) == 1){
        createNewFile(input + 7);
    } else if (stringCompare(input, "kill", 4) == 1) {
        interrupt(0x21, 9, input[5] - '0', 0, 0);
    } else if (stringCompare(input, "dir", 3) == 1) {
        char buffer[512];
        int i, j, size;
        char fileName[7];
        interrupt(0x21, 2, buffer, 2, 0);
        for (i = 0; i < 512; i+=32){
            if (buffer[i] != 0x0){
                for (j = 0; j < 6; j++){
                    fileName[j] = buffer[i + j];
                }
                i+=6;
                size = 0;
                while(buffer[i] != 0x0){
                    size++;
                    i++;
                }
                i = i - size - 6;
                interrupt(0x21, 0, fileName, 0, 0);
                interrupt(0x21, 0, ", with size: ", 0, 0);
                printNumber(size);  
                changeLine();
            }
        }
        buffer[0] = '\0';
    } else if (stringCompare(input, "copy", 4) == 1){
        char buffer[13312];
        char fromFile[6];
        char toFile[6];
        int i, j;
        i = 5;
        for(j = 0; j < 6; j++){
            if(input[i] == ' '){
                fromFile[j] = 0x0;
            } else {
                fromFile[j] = input[i];
                i++;
            }
        }
        i++;
        for(j = 0; j < 6; j++){
            if(input[i] == ' ' || input[i] == 0xa){
                toFile[j] = 0x0;
            } else {
                toFile[j] = input[i];
                i++;
            }
        }
        interrupt(0x21, 3, fromFile, buffer, 0);
        j = 0;
        while(buffer[j] != 0x0){
            j++;
        }
        interrupt(0x21, 8, toFile, buffer, div(j, 512) + 1);
    } else if (stringCompare(input, "clear", 5) == 1){
    	int i;
    	for(i=0; i<23;i++){
    		interrupt(0x21, 0, " ", 0, 0);
    		interrupt(0x21, 0, "\n", 0, 0);
    	}
    }else {
        interrupt(0x21, 0, "Bad Command!", 0);
    }
}

void createNewFile(char* file){
    char fileName[6];
    int i, size;
    char text[512];
    char content[13312];
    getRidOfEnter(file, fileName);
    size = 0;
    while(1){
        interrupt(0x21, 0, "Enter new Text: ", 0, 0);
        interrupt(0x21, 1, text, 0, 0);
        i = 0;
        while(text[i] != 0xa){
            content[size] = text[i];
            size++;
            i++;
        }

        if (i == 0) break;
        else {
            content[size] = 0xa;
            content[size + 1] = 0xd;
            size+=2;
        }
        changeLine();
    }
    interrupt(0x21, 8, fileName, content, div(size, 512) + 1);
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
void changeLine(){
    char changeLine[3];
    changeLine[0] = 0xa;
    changeLine[1] = 0xd;
    changeLine[2] = 0x0;
    interrupt(0x21, 0, changeLine, 0, 0);    
}


int mod(int a, int b){
    while(a >= b){
        a = a - b;
    }
    return a;
}

int div(int a, int b){
    int num = 0;
    while((num + 1) * b < a){
        num++;
    }
    return num;
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
void getRidOfEnter(char* input, char* fileName){
    int i;
    for (i = 0; i < 6; i++){
        if (input[i] == 0xa){
            fileName[i] = 0x0;
            break;
        }
        fileName[i] = input[i];
    }
}

