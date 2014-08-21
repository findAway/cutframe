#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARAM_NUM 5    //max parameter number
#define NAME_MAX_LEN  20   //max name length of destination file

#define PRT_ERROR(format, argv...) fprintf(stderr, "[%s] line:%d #### ",\
        __FUNCTION__, __LINE__);fprintf(stderr, format, ##argv)

#define SAFE_CLOSE_FILE(file) if(file != NULL){fclose(file); file=NULL;}

int main(int argc, char** argv)
{
    if (argc != MAX_PARAM_NUM + 1)
    {
        PRT_ERROR("param num error\n");
        return 1;
    }

    const char* pStrFrameData = argv[1];
    const char* pStrFrameLen = argv[2];
    const char* pStrDest = argv[3];
    const int nStartLine = atoi(argv[4]);
    const int nEndLine = atoi(argv[5]);

    char achDestFrameData[NAME_MAX_LEN+5];
    char achDestFrameLen[NAME_MAX_LEN+5];
    
    int nNameLen = strlen(pStrDest);
    if (nNameLen > NAME_MAX_LEN)
    {
        nNameLen = NAME_MAX_LEN;
    }

    memcpy(achDestFrameData, pStrDest, nNameLen);
    memcpy(achDestFrameLen, pStrDest, nNameLen);
    sprintf(&achDestFrameData[nNameLen], ".dat");
    sprintf(&achDestFrameLen[nNameLen], ".txt");


    //print information
    printf("src frame data file: %s\n", pStrFrameData);
    printf("src frame length file: %s\n", pStrFrameLen);
    printf("dest frame data file: %s\n", achDestFrameData);
    printf("dest frame length file: %s\n", achDestFrameLen);
    printf("cut from line:%d to %d\n", nStartLine, nEndLine);

    //check parameter
    if (nStartLine < 0 || nEndLine < 0 || nEndLine < nStartLine)
    {
        PRT_ERROR("parameter start position or end position error\n");
        return 1;
    }

    FILE* pfSrcFrmFile = NULL;
    FILE* pfSrcLenFile = NULL;
    FILE* pfDestFrmFile = NULL;
    FILE* pfDestLenFile = NULL;

    int nFlag = 1;
    while(1)
    {
        pfSrcFrmFile = fopen(pStrFrameData, "r");
        if (pfSrcFrmFile == NULL)
        {
            PRT_ERROR("open %s error\n", pStrFrameData);
            nFlag = 0;
            break;
        }

        pfSrcLenFile = fopen(pStrFrameLen, "r");
        if (pfSrcLenFile == NULL)
        {
            PRT_ERROR("open %s error\n", pStrFrameLen);
            nFlag = 0;
            break;
        }

        pfDestFrmFile = fopen(achDestFrameData, "w");
        if (pfDestFrmFile == NULL)
        {
            PRT_ERROR("open %s error\n", achDestFrameData);
            nFlag = 0;
            break;
        }

        pfDestLenFile = fopen(achDestFrameLen, "w");
        if (pfDestLenFile == NULL)
        {
            PRT_ERROR("open %s error\n", achDestFrameLen);
            nFlag = 0;
            break;
        }

	    break;
    }

    if (nFlag == 0)
    {
        SAFE_CLOSE_FILE(pfSrcFrmFile);
        SAFE_CLOSE_FILE(pfSrcLenFile);
        SAFE_CLOSE_FILE(pfDestFrmFile);
        SAFE_CLOSE_FILE(pfDestLenFile);
        return 1;
    }

    //search start position
    nFlag = 1;
    int nStartPos = 0;
    int nReadLine = 0;
    
    while (nReadLine < nStartLine - 1)
    {
	    int nReadSize = 0;
        int nRead = fscanf(pfSrcLenFile, "%d", &nReadSize);
	    if (nReadSize <= 0 || nRead <= 0)
	    {
            PRT_ERROR("get frame size error\n");
            nFlag = 0;
            break;
	    }

        printf("frame size:%d\n", nReadSize);
        nReadLine += 1;
        nStartPos += nReadSize;
    }

    if (nFlag == 0)
    {
        SAFE_CLOSE_FILE(pfSrcFrmFile);
        SAFE_CLOSE_FILE(pfSrcLenFile);
        SAFE_CLOSE_FILE(pfDestFrmFile);
        SAFE_CLOSE_FILE(pfDestLenFile);
        return 1;
    }

    //start to cut frames
    nFlag = 1;
    fseek(pfSrcFrmFile, nStartPos, SEEK_SET);
    char* pBuf = malloc(1<<20);
    while (1)
    {
        if (nReadLine >= nEndLine)
        {
            break;
        }

        int nReadSize = 0;
        int nRead = fscanf(pfSrcLenFile, "%d", &nReadSize);
        if (nReadSize <= 0 || nRead <= 0)
        {
            PRT_ERROR("get frame size error\n");
            nFlag = 0;
            break;
        }

        printf("frame size:%d\n", nReadSize);
        nReadLine += 1;

        int nDataRead = fread(pBuf, 1, nReadSize, pfSrcFrmFile);
        if (nDataRead > 0)
        {
            fwrite(pBuf, 1, nDataRead, pfDestFrmFile);
            fprintf(pfDestLenFile, "%d\n", nDataRead);
        }

        if (nDataRead < nReadSize)
        {
            PRT_ERROR("file read over unexpectly, expect read:%d, actual read:%d\n", nReadSize, nDataRead);
            nFlag = 0;
            break;
        }
    }

    SAFE_CLOSE_FILE(pfSrcFrmFile);
    SAFE_CLOSE_FILE(pfSrcLenFile);
    SAFE_CLOSE_FILE(pfDestFrmFile);
    SAFE_CLOSE_FILE(pfDestLenFile);
    free(pBuf);

    return 0;
}
