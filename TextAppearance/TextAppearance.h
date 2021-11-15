#ifndef __IF_TEXT_APPEARANCE_H_
#define __IF_TEXT_APPEARANCE_H_

#include "../defines.h"
#include "../TextData/TextData.h"

#define MAX_SCROLL_VALUE 65535
#define FONT_CHAR_HEIGHT 20
#define FONT_CHAR_WIDTH 10
#define FONT_CHAR_WEIGHT 400
#define FONT_BETWEEN_LINES_DIST 20



typedef struct _ScreenLine
{
    int textLineIndex;
    int lineBeginOffset;
    struct _ScreenLine* prevLine;
    struct _ScreenLine* nextLine;
} ScreenLine;

typedef struct _TextAppearance TextAppearance;

typedef void (*SetNextLineFunc)(TextAppearance* This, ScreenLine* curLine, ScreenLine* nextLine);
typedef void (*SetPrevLineFunc)(TextAppearance* This, ScreenLine* prevLine, ScreenLine* curLine);

struct _TextAppearance
{
    HFONT hFont;
    TextData* textData;
    enum TEXT_APPEARANCE_MODE
    {
        TEXT_APPEARANCE_FORMATED,
        TEXT_APPEARANCE_UNFORMATED,
        TEXT_APPEARANCE_NONE
    } mode;
    SetNextLineFunc setNextLine;
    SetNextLineFunc setPrevLine;
    ScreenLine* firstScreenLine;
    unsigned long charWidth;
    unsigned long charHeigth;
    unsigned long betweenLinesDist;
    unsigned long screenWidthInChars;
    unsigned long screenHeigthInLines;
    unsigned long verticalMaxPosition;
    unsigned long horizontalMaxPosition;
    unsigned long verticalPosition;
    unsigned long horizontalPosition;

};

TextAppearance* TextAppearance_Get(TextData* textData, HWND hwnd, BOOL isFormated);

void TextAppearance_Destroy(TextAppearance** pThis);

RETC TextAppearance_Resize(TextAppearance* This, HWND hwnd, long windowWidth, long windowHeigth);

void TextAppearance_VScrollToPosition(TextAppearance* This, HWND hwnd, unsigned long position);

void TextAppearance_VScrollByDelta(TextAppearance* This, HWND hwnd, long delta);

void TextAppearance_HScrollToPosition(TextAppearance* This, HWND hwnd, unsigned long position);

void TextAppearance_HScrollByDelta(TextAppearance* This, HWND hwnd, long delta);

RETC TextAppearance_SetMode(TextAppearance* This, HWND hwnd, enum TEXT_APPEARANCE_MODE mode);

void TextAppearance_Paint(TextAppearance* This, HWND hwnd);

#endif // __IF_TEXT_APPEARANCE_H_
