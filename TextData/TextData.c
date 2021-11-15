#include "TextData.h"

static RETC TextData_ReadFromFile(TextData* This, const char* fileName)
{
    int textSize;
    FILE* file = fopen(fileName, "r");
    if (file == NULL)
    {
        return RETC_INPUT_ERROR;
    }
    fseek(file, 0, SEEK_END);
    textSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    This->textBuffer = (char *)malloc((textSize + 1) * sizeof(char));
    if (This->textBuffer == NULL)
    {
        fclose(file);
        return RETC_MEMALLOC_ERROR;
    }
    textSize = fread(This->textBuffer, sizeof(char), textSize, file);
    This->textBuffer[textSize] = '\0';
    This->textSize = textSize + 1;
    fclose(file);
    return RETC_SUCCESS;
}

static RETC TextData_SplitToLines(TextData* This)
{
    int lineIndex;
    char* ptr;;
    This->textLinesCount = 1;
    for (ptr = This->textBuffer; *ptr != '\0'; ptr++)
    {
        if (*ptr == '\n')
            This->textLinesCount++;
    }
    This->textLines = malloc(This->textLinesCount * sizeof(char*));
    if (This->textLines == NULL)
    {
        return RETC_MEMALLOC_ERROR;
    }
    This->textLines[0] = This->textBuffer;
    for (lineIndex = 1, ptr = This->textBuffer; *ptr != '\0'; ptr++)
    {
        if (*ptr == '\n')
        {
            This->textLines[lineIndex++] = ptr + 1;
        }
    }
    return RETC_SUCCESS;
}

int TextData_LineLen(TextData* This, int index)
{
    if (index < 0 || index >= This->textLinesCount)
        return 0;
    else if (index == This->textLinesCount - 1)
        return This->textSize - (This->textLines[index] - This->textBuffer) - 1;
    else
        return This->textLines[index + 1] - This->textLines[index] - 1;
}


TextData* TextData_Get(const char* fileName)
{
    TextData* textData = malloc(sizeof(TextData));
    if (textData == NULL)
    {
        return NULL;
    }
    if (RETC_SUCCESS != TextData_ReadFromFile(textData, fileName))
    {
        free(textData);
        return NULL;
    }
    if (RETC_SUCCESS != TextData_SplitToLines(textData))
    {
        free(textData->textBuffer);
        free(textData);
        return NULL;
    }
    return textData;
}

void TextData_Destroy(TextData** pThis)
{
    if (pThis == NULL || (*pThis) == NULL)
        return;
    free((*pThis)->textLines);
    free((*pThis)->textBuffer);
    free((*pThis));
    (*pThis) = NULL;
}
