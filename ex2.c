//313268393 Yoav Berger

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//finding the right line feed of the file according to its name and BOM of src file
//in windows getting back the first one - \r
void findLineFeed(char* osName, int littleBom, char realLinfFeed[2]){
   
     if (strcmp(osName,"-win") == 0)
    {
        //in windows case we will recive the first letter
        //little
        if (littleBom == 1)
        {
            realLinfFeed[0] = 0x0d;
            realLinfFeed[1] = 0x00;
        }
        //big
        if (littleBom == 0)
        {
            realLinfFeed[0] = 0x00;
            realLinfFeed[1] = 0x0d;
        }

    }
    if (strcmp(osName,"-unix") == 0)
    {
       //little
        if (littleBom == 1)
        {
            realLinfFeed[0] = 0x0a;
            realLinfFeed[1] = 0x00;
        }
        //big
        if (littleBom == 0)
        {
            realLinfFeed[0] = 0x00;
            realLinfFeed[1] = 0x0a;
        }
    }
      if (strcmp(osName,"-mac") == 0)
    {
        //little
        if (littleBom == 1)
        {
            realLinfFeed[0] = 0x0d;
            realLinfFeed[1] = 0x00;
        }
        //big
        if (littleBom == 0)
        {
            realLinfFeed[0] = 0x00;
            realLinfFeed[1] = 0x0d;
        }
    }
}

//returns the char /n according to BOM
void slashN(int littleBom, char slashNBuffer[2]){
    //little
        if (littleBom == 1)
        {
            slashNBuffer[0] = 0x0a;
            slashNBuffer[1] = 0x00;
        }
        //big
        if (littleBom == 0)
        {
            slashNBuffer[0] = 0x00;
            slashNBuffer[1] = 0x0a;
        }
}

//swaping cells in the buffer
void bufferSwap(char buffer[2]){
    char swapBuffer[1];
    swapBuffer[0] = buffer[1];
    buffer[1] = buffer[0];
    buffer[0] = swapBuffer[0];
}

//check if need to swap or keep the same BOM
int isSwapKeep(char* swapOrKeep){
    if (strcmp(swapOrKeep,"-swap") == 0){
        return 1;
    }
    if (strcmp(swapOrKeep,"-keep") == 0){
        return 0;
    }
}

//check if the given OS is windows
int isWindows(char* osName){
    if (strcmp(osName,"-win") == 0){
        return 1;
    }
    return 0;
}


int isLittle(char * srcFile){
    //create pointer to File to read and one to write
    FILE* srcPtr;
    //reading the src file and checking if there is a file
    srcPtr = fopen(srcFile,"rb");
    if (srcPtr == NULL)
    {
        exit (-1);
    }
    //creating buffer for reading and iterate
    char buffer[2];
    char bomLittle[2];
    char bomBig[2];
    bomLittle[0] = 0xFF;
    bomLittle[1] = 0xFE;
    bomBig[0] = bomLittle[1];
    bomBig[1] = bomLittle[0];

    fread(buffer,2,1,srcPtr);
    if (buffer[0] == bomLittle[0] && buffer[1] == bomLittle[1])
    {
        //closing file
        fclose(srcPtr);
        return 1;
    }
    if (buffer[0] == bomBig[0] && buffer[1] == bomBig[1])
    {
        //closing file
        fclose(srcPtr);
        return 0;
    }
}

//copy the bytes of a file and create a new file with same bytes
void createFile(char * srcFile, char * dstFile){
    //create pointer to File to read and one to write
    FILE* srcPtr;
    FILE* dstPtr;
    //reading the src file and checking if there is a file
    srcPtr = fopen(srcFile,"rb");
    if (srcPtr == NULL)
    {
        return;
    }

    //creating a new dst file
    dstPtr =fopen(dstFile,"wb");

    //creating buffer for reading and iterate
    char buffer[2];

    //reading and writing to a new file
    while (fread(buffer,2,1,srcPtr) == 1)
    { 
      fwrite(buffer,2,1,dstPtr);
    }
    
    //closing both files
    fclose(srcPtr);
    fclose(dstPtr);
    return;
} 

//converting file  according to OS and changing BOM if needed
void convertFile(char * srcFile, char * dstFile, char srcLF[2], char dstLF[2],int isFromWindows, int isToWindows,int isNeedSwap,char slashNChar[2] ){
    //create pointer to File to read and one to write
    FILE* srcPtr;
    FILE* dstPtr;
    //reading the src file and checking if there is a file
    srcPtr = fopen(srcFile,"rb");
    if (srcPtr == NULL)
    {
        return;
    }

    //creating a new dst file
    dstPtr =fopen(dstFile,"wb");

    //checking ig we need to swap the chars
    if (isNeedSwap == 1)
    {
        bufferSwap(dstLF);
        bufferSwap(slashNChar);
    }
    

    //creating buffer for reading and iterate, and start to read
    char buffer[2];
    while (fread(buffer,2,1,srcPtr) == 1)
    {
        //in case we found line feed
        if (buffer[0] == srcLF[0] && buffer[1] == srcLF[1])
        {
            //in case we are reading from windows we need to write another char
            if (isFromWindows == 1)
            {
                fread(buffer,2,1,srcPtr);
            }
            //in case we are writing to windows we need to write /r/n
            if (isToWindows == 1)
            {
                fwrite(dstLF,2,1,dstPtr);
                fwrite(slashNChar,2,1,dstPtr);
            }
            //in other cases we write normally
            if (isToWindows == 0)
            {
                fwrite(dstLF,2,1,dstPtr);
            }
            
        }
        else
        {
            //in cases we in regular chars and we need to swap
            if (isNeedSwap == 1)
            {
                bufferSwap(buffer);
                fwrite(buffer,2,1,dstPtr);
            }
            if (isNeedSwap == 0)
            {
                fwrite(buffer,2,1,dstPtr);
            }
            
            
        }
        
    }
    
    //closing both files
    fclose(srcPtr);
    fclose(dstPtr);
    return;
} 

void main(int argc, char *argv[]){
    //in case we have 2 arguments we want to copy the file
    if (argc == 3)
    {
        char * srcFileName = argv[1];
        char * dstFileName = argv[2];
        
        createFile(srcFileName, dstFileName);

    }
    //in case we have 4 arguments we want to convert to diffrent OS
    if (argc == 5)
    {
        char * srcFileName = argv[1];
        char * dstFileName = argv[2];
        char * srcFileOS = argv[3];
        char * dstFileOS = argv[4];

        //argument check
        if (strcmp(srcFileOS,"-win") != 0 && strcmp(srcFileOS,"-mac") != 0 && strcmp(srcFileOS,"-unix") != 0)
        {
            return;
        }
        if (strcmp(dstFileOS,"-win") != 0 && strcmp(dstFileOS,"-mac") != 0 && strcmp(dstFileOS,"-unix") != 0)
        {
            return;
        }
        
        
        //checking BOM and line feed according to it
        int isLittleBOM = isLittle(srcFileName);
        char srcLineFeed[2];
        char dstLineFeed[2];
        findLineFeed(srcFileOS, isLittleBOM, srcLineFeed);
        findLineFeed(dstFileOS, isLittleBOM, dstLineFeed);
        
        //checking from and to windows and making the /n buffer
        int isFromWindows = isWindows(srcFileOS);
        int isToWindows = isWindows(dstFileOS);
        char slashNChar[2];
        slashN(isLittleBOM, slashNChar);

        convertFile(srcFileName, dstFileName, srcLineFeed, dstLineFeed,isFromWindows, isToWindows, 0,slashNChar);

    }
    if (argc == 6)
    {
        char * srcFileName = argv[1];
        char * dstFileName = argv[2];
        char * srcFileOS = argv[3];
        char * dstFileOS = argv[4];
        char * swapOrKeep = argv[5];

        //argument check
        if (strcmp(srcFileOS,"-win") != 0 && strcmp(srcFileOS,"-mac") != 0 && strcmp(srcFileOS,"-unix") != 0)
        {
            return;
        }
        if (strcmp(dstFileOS,"-win") != 0 && strcmp(dstFileOS,"-mac") != 0 && strcmp(dstFileOS,"-unix") != 0)
        {
            return;
        }
        
        
        //checking BOM and line feed according to it
        int isLittleBOM = isLittle(srcFileName);
        char srcLineFeed[2];
        char dstLineFeed[2];
        findLineFeed(srcFileOS, isLittleBOM, srcLineFeed);
        findLineFeed(dstFileOS, isLittleBOM, dstLineFeed);
        
        //checking from and to windows and making the /n buffer
        int isFromWindows = isWindows(srcFileOS);
        int isToWindows = isWindows(dstFileOS);
        char slashNChar[2];
        slashN(isLittleBOM, slashNChar);

        //checking if need to swap
        int isSwap = isSwapKeep(swapOrKeep);
        
        convertFile(srcFileName, dstFileName, srcLineFeed, dstLineFeed,isFromWindows, isToWindows, isSwap,slashNChar);

    }
}