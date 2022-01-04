/*
Tsega Tsewameskel
VM II
*/
#include <stdio.h>   // io
#include <stdlib.h>  // malloc
#include <string.h>  // for strlen, strcpy, strcmp
#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

void file();
char* noSpace(char*);
char* breakBefore(char[], char);
char* breakAfter(char[], char);
char* detectNum(char []);
char* intoAsm(char [],char,char [],char,char [], char []);
char* popOrPush(char [], char [], char [], char [], char[]);
char* arithmetic(char [], char []);
char* branching(char [],char [], char []);
char* functionOrCall(char [], char [], char [], char []);
char* returnC(char [], char []);
int stringToInt(char*);

int eqInc = -1;
int ltInc = -1;
int gtInc = -1;
int callInc = -1;

int main()
{
    file();
    return 0;
}

void file(){
    //Files are declared using file pointers
    FILE* inputFile = NULL;
    FILE* outputFile = NULL;

    const int bufferSize = 256;
    //256 is an arbitrary value
    char inputFileName[bufferSize];
    //Get file name from keyboard
    printf("Please enter an input filename: ");
    fgets(inputFileName, bufferSize, stdin);
     //removes the \n
    inputFileName[strcspn(inputFileName, "\n")] = 0;
    //Open files with fopen, read only
    inputFile = fopen(inputFileName, "r");

    int inputLength = strlen(inputFileName);
    inputFileName[inputLength-3]='\0';
    char outputFileName[inputLength];
    strcpy(outputFileName, inputFileName);
    strcat(outputFileName, ".asm");


    //Open files with fopen, write only
    outputFile = fopen(outputFileName, "w");

    //If the file pointer are none, the file didn't open
    if(inputFile == NULL){
        puts("Error opening input file");
    }
    //makes sure code doesn't keep running if there no input file
    assert(inputFile != NULL);
    char buffer[bufferSize];
    char *line = (char*)malloc(sizeof(char) * bufferSize);

    char* x = NULL;
    char xCommandT = '-';


    char* y = NULL;
    char yCommandT = '-';

    char* number = NULL;
    char* result = NULL;

    //Use fgets to read from the file, null when end of file
    while(fgets(buffer, bufferSize, inputFile) != NULL){
        buffer[strcspn(buffer, "\r\n")] = 0;

        line = noSpace(buffer);
        if(strlen(line)>0){
            //printf("Debug line: %s\n",line);
            // push constant 9
            // x.     y.     number
            x = breakBefore(line, ' ');
            y = breakAfter(line, ' ');
            y = breakBefore(y, ' ');
            number = detectNum(line);

            if(x[0]=='p')
            {
                xCommandT='M';
            }
            else if((x[0] == 'l' && x[1] == 'a')||
                    (x[0] == 'g' && x[1] == 'o')||
                    (x[0] == 'i' && x[1] == 'f')){
                xCommandT = 'B';
            }
            else if(x[0] == 'f'|| x[0] == 'c' || x[0] == 'r'){
              xCommandT = 'F';
            }

            else{
                xCommandT='A';
            }

            if(islower(y[0])){
                yCommandT='s';
            }
            else if(isupper(y[0]) && isupper(y[1])){
              yCommandT = 'L';
            }
            else if(isupper(y[0]) && islower(y[1])){
              yCommandT = 'N';
            }
            else{
                yCommandT='E';
            }

            result = intoAsm(x,xCommandT,y,yCommandT,number,inputFileName);
            puts(result);
            fputs(result,outputFile);
        }

    }
    fclose(inputFile);
    fputs("(END) \n @END \n 0;JMP",outputFile);
    fputs("\n", outputFile);
    fclose(outputFile);
    free(line);
    // free(x);
    // free(y);
    // free(number);
}

char* noSpace(char *input){
    int len = strlen(input) + 1;

    int begin = 0;
    char compare[2];
    compare[1] = '\0';
    char comment[] = "/\0";
    char *spaceless = (char*)malloc(sizeof(char) * len);

    if (input == NULL){
        return NULL;
    }

    for(int i = 0; i < len; i++){
            compare[0] = input[i];
            if(strcmp(compare,comment) == 0){
                i=len;
            }
            if(input[i]==' ' && begin==0){
                i++;
            }
            else{
                spaceless[begin] = input[i];
                begin++;
            }
    }
    int lastInt = begin ;
    spaceless[lastInt] = '\0';
    return spaceless;
}

char* breakBefore(char string[], char symbol){
    int len = strlen(string) ;
    int count = 0;
    char *result = (char*)malloc(sizeof(char) * len);
    //char compare[2];
    //compare[1] = '\0';

    if(len > 0){
      for(int i =0; i<len; i++){
        if(string[i] == symbol){
          i = len;
        }
        result[count] = string[i];
        count++;
      }
    }
    int newInt = count;
    result[newInt] = '\0';
    return result;
}

char* breakAfter(char string[], char breakChar){
  int position =-1;
  int stringLen = strlen(string)-1;

  for(int x = 0; x < stringLen; x++ ){
    if(string[x] == breakChar){
      position = x;
      break;
    }
  }
  if(position > -1){
    int newStringLen = stringLen - position + 1;
    char *newString = malloc(sizeof(char) * newStringLen);
    int begin = 0 ;
    for(int oldBegin = position+1; oldBegin <= stringLen;
    oldBegin++){
      newString[begin] = string[oldBegin];
      begin++;
    }

    newString[newStringLen] = '\0';
    return newString;
  }
  return "";
}

char* detectNum(char string[]){
  int x = 0;
  int len = strlen(string);
  char* num = (char*)malloc(len * sizeof(char));
  for(int i=0; i<len; i++){
    if(isdigit(string[i])){
      num[x] = string[i];
      x++;
    }

  }
    num[x] = '\0';
    return num;

}

char* intoAsm(char x[],char xCommandT, char y[], char yCommandT, char z[], char inputFileName[]){
  char arithmeticAsm[200];
  char pushPopAsm[300];
  char branchingAsm[100];
  char functionCallAsm[100];
  char returnAsm[100];
  char* finalAsm =  (char*)malloc(sizeof(char) * 400);

  if(xCommandT == 'A' ){
    strcpy(finalAsm, arithmetic(x,arithmeticAsm));
  }
  else if(xCommandT == 'M'){
    strcpy(finalAsm, popOrPush(x,y,z, pushPopAsm, inputFileName));
  }
  else if(xCommandT == 'B' && yCommandT == 'L'){
    strcpy(finalAsm, branching(x,y,branchingAsm));
  }
  else if(xCommandT == 'F' && yCommandT == 'N'){
    strcpy(finalAsm, functionOrCall(x,y,z,functionCallAsm));
  }
  else if( xCommandT == 'F' && yCommandT == 'E'){
    strcpy(finalAsm, returnC(x,returnAsm));
  }
  else{
    puts("Error intoAsm");
  }
  return finalAsm;
}

char* popOrPush(char x[],char y[],char number[],char pushPopAsm[], char inputFileName[]){
  char temp[100];
  char buffer[100];
  int num ;

  if(x[0] == 'p' && x[1] == 'o'){
    if(memcmp(y, "argument", 7) == 0 || memcmp(y, "local", 4) == 0 ||
       memcmp(y, "this", 3) == 0 || memcmp(y, "that", 3) == 0){
         char segment[10];

        //why 3 instead of 7
         if(memcmp(y, "argument", 7) == 0){
           strcpy(segment, "ARG");
         }
         else if(memcmp(y, "local", 3) == 0){
           strcpy(segment, "LCL");
         }
         else if(memcmp(y, "this", 3) == 0){
           strcpy(segment, "THIS");
         }
         else{
           strcpy(segment, "THAT");
         }

         strcpy(pushPopAsm, "@");
         strcat(pushPopAsm, number);
         strcat(pushPopAsm, "\n D=A \n @");
         strcat(pushPopAsm, segment);
         strcat(pushPopAsm, "\n A=M \n D=D+A \n @");
         strcat(pushPopAsm, segment);
         strcat(pushPopAsm, "\n M=D \n @SP \n M=M-1 \n D=M \n @");
         strcat(pushPopAsm, segment);
         strcat(pushPopAsm, "\n A=M \n M=D \n @");
         strcat(pushPopAsm, segment);
         strcat(pushPopAsm, "\n D=A \n @");
         strcat(pushPopAsm, segment);
         strcat(pushPopAsm, "\n A=M \n D=A-D \n @");
         strcat(pushPopAsm, segment);
         strcat(pushPopAsm, "\n M=D \n");
      }
     else if(memcmp(y, "pointer", 6) == 0||memcmp(y, "static", 5) == 0||
                memcmp(y, "temp", 3) == 0){

                  if(memcmp(y, "pointer", 6) == 0){
                    num = stringToInt(number) + 3;
                    sprintf(buffer, "%i", num);
                    strcpy(temp, "R");
                    strcat(temp, buffer);
                    puts(buffer);
                  }
                  else if(memcmp(y, "static", 5) == 0){
                    strcpy(temp, inputFileName);
                    strcat(temp, ".");
                    strcat(temp, number);
                  }
                  else{
                    num = stringToInt(number) + 5;
                    sprintf(buffer, "%d", num);
                    strcpy(temp, "R");
                    strcat(temp, buffer);
                    puts(buffer);
                  }

                  strcpy(pushPopAsm, "\n @SP \n AM=M-1 \n D=M \n @");
                  strcat(pushPopAsm, temp);
                  strcat(pushPopAsm, "\n M=D \n");
                }

                else{
                  puts("Error in Static or Pointer");
                }
  }

  else{
    if(memcmp(y, "argument", 7) == 0 || memcmp(y, "local", 4) == 0 ||
       memcmp(y, "this", 4) == 0 || memcmp(y, "that", 3) == 0){
         char segment[10];
         if(memcmp(y, "argument", 7) == 0){
           strcpy(segment, "ARG");
         }
         else if(memcmp(y, "local", 4) == 0){
           strcpy(segment, "LCL");
         }
         else if(memcmp(y, "this", 3) == 0){
           strcpy(segment, "THIS");
         }
         else{
           strcpy(segment, "THAT");
         }

         strcpy(pushPopAsm, "@");
         strcat(pushPopAsm, number);
         strcat(pushPopAsm, "\n D=A \n @");
         strcat(pushPopAsm, segment);
         strcat(pushPopAsm, "\n A=M \n D=D+A \n A=D \n D=M \n @SP \n A=M \n M=D \n @SP \n M=M+1 \n");
    }
    else if(memcmp(y, "pointer", 6) == 0||memcmp(y, "static", 5) == 0||
            memcmp(y, "temp", 3) == 0){
            if(memcmp(y, "pointer", 6) == 0){
                   num = stringToInt(number) + 3;
                   sprintf(buffer, "%i", num);
                   strcpy(temp, "R");
                   strcat(temp, buffer);
            }
            else if(memcmp(y, "static", 5) == 0){
              strcpy(temp, inputFileName);
              strcat(temp, ".");
              strcat(temp, number);
            }
            else{
              num = stringToInt(number) + 5;
              sprintf(buffer, "%d", num);
              strcpy(temp, "R");
              strcat(temp, buffer);
            }

            strcpy(pushPopAsm, "@");
            strcat(pushPopAsm, temp);
            strcat(pushPopAsm, "\n D=M \n @SP \n A=M \n M=D \n @SP \n M=M+1 \n");
    }
    else if(memcmp(y, "constant",8) == 0){
            strcpy(pushPopAsm, "@");
            strcat(pushPopAsm, number);
            strcat(pushPopAsm, "\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
        }
    else{
      puts("Error in Pop Or Push");
    }
  }

  return pushPopAsm;

}

char* arithmetic(char command[],char arithmeticAsm[]){
  char temp[100];
  char counter[100];
  char stringBegin[100];
  char stringEnd[100];


  if(memcmp(command, "add", 3) == 0||memcmp(command, "sub", 3) == 0||
      memcmp(command, "or", 2) == 0||memcmp(command, "and", 3) ==0){
        if(strcmp(command, "add\0") == 0){
          puts(command);
          strcpy(temp, "D=D+M");
        }
        else if(strcmp(command, "sub\0") == 0){
          puts(command);
          strcpy(temp, "D=D-M");
        }
        else if(strcmp(command, "or\0") == 0){
          puts(command);
          strcpy(temp, "D=D|M");
        }
        else{
          puts(command);
          strcpy(temp, "D=D&M");
        }

        strcpy(arithmeticAsm, "@SP\n M=M-1 \n A=M \n D=M \n A=A-1 \n");
        strcat(arithmeticAsm, temp);
        strcat(arithmeticAsm, "@SP \n A=M \n M=0 \n");
      }

  else if(memcmp(command, "not", 3) == 0||memcmp(command, "neg", 3) ==0){
    if(strcmp(command, "not\0") == 0){
      puts(command);
      strcpy(command, "M=!M");
    }
    else{
      puts(command);
      strcpy(command, "M=-M");
    }

    strcpy(arithmeticAsm, "@SP \n A=M-1 \n");
    strcat(arithmeticAsm, temp);
    strcat(arithmeticAsm, "\n");
  }
  else if(memcmp(command, "eq",2) == 0||memcmp(command, "gt",2) == 0||
          memcmp(command, "lt",2) == 0){


      if(strcmp(command, "eq\0") == 0){
          puts(command);
          eqInc = eqInc + 1;
          strcpy(temp, "D;JEQ");
          sprintf(counter, "%d", eqInc);

          strcat(stringBegin, "EQST");
          strcat(stringBegin, counter);

          strcat(stringEnd, "EQEND");
          strcat(stringEnd, counter);

      }
      else if(strcmp(command, "gt\0") == 0){
          puts(command);
          gtInc = gtInc + 1;
          strcpy(temp, "D;JGT");
          sprintf(counter, "%d", gtInc);

          strcat(stringBegin, "GTST");
          strcat(stringBegin, counter);

          strcat(stringEnd, "GTEND");
          strcat(stringEnd, counter);
      }
      else{
          puts(command);
          ltInc = ltInc + 1;
          strcpy(temp, "D;JLT");
          sprintf(counter, "%d", ltInc);

          strcat(stringBegin, "LTST");
          strcat(stringBegin, counter);

          strcat(stringEnd, "LTEND");
          strcat(stringEnd, counter);
      }

      strcpy(arithmeticAsm, "@SP\nA=M-1 \n A=M \n D=M \n A=A-1 \n D=D-M \n @NEQ \n");
      strcat(arithmeticAsm, stringBegin);
      strcat(arithmeticAsm, "\n");
      strcat(arithmeticAsm, temp);

      strcat(arithmeticAsm, "\n @SP \n A=M-1 \n M=-1 \n @");
      strcat(arithmeticAsm, stringEnd);

      strcat(arithmeticAsm, "\n");
      strcat(arithmeticAsm, "0;JMP\n(");
      strcat(arithmeticAsm, stringBegin);

      strcat(arithmeticAsm, ")");
      strcat(arithmeticAsm, "\n @SP \n A=M-1 \n M=-1 \n (");
      strcat(arithmeticAsm, stringEnd);
      strcat(arithmeticAsm, ")\n");
  }
  return arithmeticAsm;
}

char* branching(char x[], char y[], char branchingAsm[]){
  if(memcmp(x, "label",5) == 0){
      strcpy(branchingAsm, "(");
      strcat(branchingAsm, y);
      strcat(branchingAsm, ")");
      strcat(branchingAsm, "\n");
  }
  else if(memcmp(x, "goto",4) == 0){
      strcpy(branchingAsm, "@");
      strcat(branchingAsm, y);
      strcat(branchingAsm, "\n0;JMP\n");
  }
  else if(memcmp(x, "if-goto",7) == 0){
      strcpy(branchingAsm, "@SP\nAM=M-1\nD=M\n@");
      strcat(branchingAsm, y);
      strcat(branchingAsm, "\nD;JNE\n");
  }
  else{
      puts("Line 575");
  }
  return branchingAsm;
}

char* functionOrCall(char x[], char y[], char z[], char functionCallAsm[]){
  int funLoop;
  char counter[10];
  char temp[10];

  if(memcmp(x, "function",8) == 0){
      strcpy(functionCallAsm, "(");
      strcat(functionCallAsm, y);
      strcat(functionCallAsm, ")");
      strcat(functionCallAsm, "\n");
      funLoop = stringToInt(z);
      for(int i=0; i<funLoop; i=i+1){
          strcat(functionCallAsm, "@SP\nA=M\nM=0\n@SP\nM=M+1\n");
      }
  }
  else if(memcmp(x, "call",4) == 0){
      callInc = callInc+1 ;
      sprintf(counter, "%d", callInc);
      strcpy(temp, "CALL");
      strcat(temp, counter);


      strcpy(functionCallAsm, "@");
      strcat(functionCallAsm, temp);
      strcat(functionCallAsm, "\nD=A\n@SP\nA=M\nM=D\n@SP\n"
                                     "M=M+1\n@LCL\nD=M\n@SP\nA=M\n"
                                     "M=D\n@SP\nM=M+1\n@ARG\nD=M\n"
                                     "@SP\nA=M\nM=D\n@SP\nM=M+1\n@"
                                     "THIS\nD=M\n@SP\nA=M\nM=D\n@SP"
                                     "\nM=M+1\n@THAT\nD=M\n@SP\nA=M"
                                     "\nM=D\n@SP\nM=M+1\n@SP\nD=M\n"
                                     "@");
      strcat(functionCallAsm, temp);
      strcat(functionCallAsm, "\nD=D-A\n@5\nD=D-A\n@ARG\nM=D\n"
                                     "@SP\nD=M\n@LCL\nM=D\n");
      strcat(functionCallAsm, "@");
      strcat(functionCallAsm, y);
      strcat(functionCallAsm, "\n0;JMP\n");
      strcat(functionCallAsm, "(");
      strcat(functionCallAsm, temp);
      strcat(functionCallAsm, ")");
      strcat(functionCallAsm, "\n");
  }
  else{
      puts("Error");
  }
  return functionCallAsm;
}

char* returnC(char x[], char returnAsm[]){
  if (memcmp(x, "return",6) == 0){
      strcpy(returnAsm, "@LCL\nD=M\n@");
      strcat(returnAsm, "var");
      strcat(returnAsm, "\nM=D\n@5\nD=D+A\nA=D\nD=M\n@");
      strcat(returnAsm, "var1");
      strcat(returnAsm, "\nM=D\n@SP\nM=M-1\nA=M\nD=M\n"
                             "@ARG\nA=M\nM=D\n@ARG\nD=M+1\n@SP"
                             "\nM=D\n@");
      strcat(returnAsm, "var");
      strcat(returnAsm, "\nD=M\n@1\nD=D-A\nA=D\nD=M\n@THAT\n"
                             "M=D\n@");
      strcat(returnAsm, "var");
      strcat(returnAsm, "\nD=M\n@2\nD=D-A\nA=D\nD=M\n@THIS\nM=D\n"
                             "@");
      strcat(returnAsm, "var");
      strcat(returnAsm, "\nD=M\n@3\nD=D-A\nA=D\nD=M\n@ARG\nM=D\n@");
      strcat(returnAsm, "var");
      strcat(returnAsm, "\nD=M\n@4\nD=D-A\nA=D\nD=M\n@LCL\nM=D\n@");
      strcat(returnAsm, "var1");
      strcat(returnAsm, "\nA=M\n0;JMP\n");
  }
  else{
      puts("Error");
  }
  return returnAsm;
}

int charToDigit(char digit){
    return digit - '0';
}
int stringToInt(char* string){
    unsigned int len = strlen(string) - 1;

    int sum = 0;
    int power = 0;

    for (int i= len; i>-1; i--){
        sum += (charToDigit(string[i]) * floor(pow(10,power++)));
    }
    return sum;
}
