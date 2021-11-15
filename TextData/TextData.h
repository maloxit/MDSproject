#ifndef __IF_TEXT_DATA_H_
#define __IF_TEXT_DATA_H_

#include "../defines.h"

typedef struct _TextData
{
    char* textBuffer;
    char** textLines;
    int textLinesCount;
    long textSize;
} TextData;

TextData* TextData_Get(const char* fileName);

int TextData_LineLen(TextData* This, int index);

void TextData_Destroy(TextData** pThis);

#endif // __IF_TEXT_DATA_H_
