#ifndef __IF_TEXT_APPEARANCE_H_
#define __IF_TEXT_APPEARANCE_H_

#include "../defines.h"
#include "../TextData/TextData.h"

typedef struct _ScreenLine
{
    int textLineIndex;
    int lineBeginOffset;
    struct _ScreenLine* prevLine;
    struct _ScreenLine* nextLine;
} ScreenLine;

typedef struct
{
    HFONT hFont;
    unsigned long charWidth;
    unsigned long charHeigth;
    unsigned long betweenLinesDist;
    ScreenLine* firstScreenLine;
    unsigned long screenWidthInChars;
    unsigned long screenHeigthInLines;
    int screenLinesCount;
    TextData* textData;

} TextAppearance;

TextAppearance* TextAppearance_Get(TextData* textData, HWND hwnd);

void TextAppearance_Destroy(TextAppearance* This);

int TextAppearance_Resize(TextAppearance* This, long windowWidth, long windowHeigth);

int TextAppearance_VScroll(TextAppearance* This, long delta);

int TextAppearance_Paint(TextAppearance* This, HWND hwnd);

#endif // __IF_TEXT_APPEARANCE_H_
