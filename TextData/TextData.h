#ifndef __IF_TEXT_DATA_H_
#define __IF_TEXT_DATA_H_

#include "../defines.h"

typedef struct _TextData
{
    char* textBuffer;
    int textSize;
    char** textLines;
    int* textLineLens;
    int textLinesCount;
} TextData;

TextData* TextData_Get(const char* fileName);

void TextData_Destroy(TextData* textData);

#endif // __IF_TEXT_DATA_H_
