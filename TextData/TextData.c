#include "TextData.h"

static int TextData_ReadFromFile(TextData* textData, const char* fileName)
{
    FILE* file = fopen(fileName, "r");
    if (file == NULL)
    {
        _MyDebugPrintln("Cannot open file.");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    textData->textSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    textData->textBuffer = (char *)malloc((textData->textSize + 1) * sizeof(char));
    if (textData->textBuffer == NULL)
    {
        _MyDebugPrintln("Cannot allocate memory\n");
        fclose(file);
        return -1;
    }
    textData->textSize = fread(textData->textBuffer, sizeof(char), textData->textSize, file);
    textData->textBuffer[textData->textSize] = '\0';

    fclose(file);
    return 0;
}

static int TextData_SplitToLines(TextData* textData)
{
    int lineIndex;
    char* ptr;;
    textData->textLinesCount = 1;
    for (ptr = textData->textBuffer; *ptr != '\0'; ptr++)
    {
        if (*ptr == '\n')
            textData->textLinesCount++;
    }
    textData->textLines = malloc(textData->textLinesCount * sizeof(char*));
    if (textData->textLines == NULL)
    {
        return -1;
    }
    textData->textLineLens = malloc(textData->textLinesCount * sizeof(int));
    if (textData->textLineLens == NULL)
    {
        free(textData->textLines);
        return -1;
    }
    textData->textLines[0] = textData->textBuffer;
    for (lineIndex = 1, ptr = textData->textBuffer; *ptr != '\0'; ptr++)
    {
        if (*ptr == '\n')
        {
            textData->textLineLens[lineIndex - 1] = ptr - textData->textLines[lineIndex - 1];
            textData->textLines[lineIndex++] = ptr + 1;
        }
    }
    textData->textLineLens[textData->textLinesCount - 1] = ptr - textData->textLines[textData->textLinesCount - 1];
    return 0;
}
TextData* TextData_Get(const char* fileName)
{
    TextData* textData = malloc(sizeof(TextData));
    if (textData == NULL)
    {
        return NULL;
    }
    if (0 != TextData_ReadFromFile(textData, fileName))
    {
        free(textData);
        return NULL;
    }
    if (0 != TextData_SplitToLines(textData))
    {
        free(textData->textBuffer);
        free(textData);
        return NULL;
    }
    return textData;
}

void TextData_Destroy(TextData* textData)
{
    free(textData->textLineLens);
    free(textData->textLines);
    free(textData->textBuffer);
    free(textData);
}
