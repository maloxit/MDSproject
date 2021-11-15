#include "TextAppearance.h"

static void SetNextLine_Formated(TextAppearance* This, ScreenLine* curLine, ScreenLine* nextLine)
{
    int prevLineIndex = curLine->textLineIndex;
    if (prevLineIndex >= This->textData->textLinesCount || (TextData_LineLen(This->textData, prevLineIndex) - curLine->lineBeginOffset) <= This->screenWidthInChars)
    {
        nextLine->textLineIndex = prevLineIndex + 1;
        nextLine->lineBeginOffset = 0;
    }
    else
    {
        nextLine->textLineIndex = prevLineIndex;
        nextLine->lineBeginOffset = curLine->lineBeginOffset + This->screenWidthInChars;
    }
}

static void SetNextLine_Unformated(TextAppearance* This, ScreenLine* curLine, ScreenLine* nextLine)
{
    nextLine->textLineIndex = curLine->textLineIndex + 1;
    nextLine->lineBeginOffset = 0;
}

static void SetPrevLine_Formated(TextAppearance* This, ScreenLine* prevLine, ScreenLine* curLine)
{
    int curLineIndex = curLine->textLineIndex;
    if (curLineIndex > This->textData->textLinesCount)
    {
        prevLine->textLineIndex = curLineIndex - 1;
        prevLine->lineBeginOffset = 0;
    }
    else if (curLine->lineBeginOffset == 0)
    {
        int lineLen = TextData_LineLen(This->textData, curLineIndex - 1);
        prevLine->textLineIndex = curLineIndex - 1;
        prevLine->lineBeginOffset = lineLen / This->screenWidthInChars * This->screenWidthInChars;
        if (prevLine->lineBeginOffset == lineLen && prevLine->lineBeginOffset != 0)
            prevLine->lineBeginOffset -= This->screenWidthInChars;
    }
    else
    {
        prevLine->textLineIndex = curLineIndex;
        prevLine->lineBeginOffset = curLine->lineBeginOffset - This->screenWidthInChars;
    }
}

static void SetPrevLine_Unformated(TextAppearance* This, ScreenLine* prevLine, ScreenLine* curLine)
{
    prevLine->textLineIndex = curLine->textLineIndex - 1;
    prevLine->lineBeginOffset = 0;
}

static void RecalculatePosition_Formated(TextAppearance* This)
{
    int i;
    unsigned long position = 0;
    This->horizontalPosition = 0;
    This->horizontalMaxPosition = 0;
    This->verticalPosition = 0;
    if (This->screenWidthInChars == 0)
    {
        This->verticalMaxPosition = 0;
        return;
    }
    for (i = 0; i < This->textData->textLinesCount; i++)
    {
        if (i == This->firstScreenLine->textLineIndex)
            This->verticalPosition = position;
        ++position;
        if (TextData_LineLen(This->textData, i) != 0)
            position += (TextData_LineLen(This->textData, i) - 1) / This->screenWidthInChars;
    }
    if (position != 0)
        --position;
    This->verticalMaxPosition = position;
}

static void RecalculatePosition_Unformated(TextAppearance* This)
{
    int i;
    unsigned long position = 0;
    This->verticalMaxPosition = This->textData->textLinesCount;
    if (This->verticalMaxPosition != 0)
        --This->verticalMaxPosition;
    This->verticalPosition = This->firstScreenLine->textLineIndex;
    for (i = 0; i < This->textData->textLinesCount; i++)
    {
        if (TextData_LineLen(This->textData, i) > position)
            position = TextData_LineLen(This->textData, i);
    }
    if (position != 0)
        --position;
    This->horizontalMaxPosition = position;
}

static ScreenLine* AddLineBefore(ScreenLine* line)
{
    ScreenLine* newLine = (ScreenLine*)malloc(sizeof(ScreenLine));
    if (newLine == NULL)
        return NULL;
    newLine->prevLine = line->prevLine;
    newLine->nextLine = line;
    line->prevLine->nextLine = newLine;
    line->prevLine = newLine;
    return newLine;
}

static RETC ResetScreenLines(TextAppearance* This)
{
    int i;
    ScreenLine* line;
    line = This->firstScreenLine->nextLine;
    if (This->mode == TEXT_APPEARANCE_FORMATED)
        This->firstScreenLine->lineBeginOffset = 0;
    for (i = 1; i < This->screenHeigthInLines; i++)
    {
        if (line == This->firstScreenLine)
        {
            ScreenLine* newLine = AddLineBefore(line);
            if (newLine == NULL)
                return RETC_MEMALLOC_ERROR;
            _MyDebugPrintln("Add line.");
            line = newLine;
        }
        This->setNextLine(This, line->prevLine, line);
        line = line->nextLine;
    }
    if (line != This->firstScreenLine)
    {
        line->prevLine->nextLine = This->firstScreenLine;
        This->firstScreenLine->prevLine = line->prevLine;
        do
        {
            _MyDebugPrintln("Delete line.");
            ScreenLine* tmp = line->nextLine;
            free(line);
            line = tmp;
        }
        while (line != This->firstScreenLine);
    }
    return RETC_SUCCESS;
}

static void SetVScrollPos(TextAppearance* This, HWND hwnd)
{
    if (This->verticalMaxPosition > MAX_SCROLL_VALUE)
        SetScrollPos(hwnd, SB_VERT, This->verticalPosition / (double)This->verticalMaxPosition * MAX_SCROLL_VALUE, TRUE);
    else
        SetScrollPos(hwnd, SB_VERT, This->verticalPosition, TRUE);
}

static void SetVScrollRange(TextAppearance* This, HWND hwnd)
{
    if (This->verticalMaxPosition > MAX_SCROLL_VALUE)
    {
        SetScrollRange(hwnd, SB_VERT, 0, MAX_SCROLL_VALUE, FALSE);
        SetScrollPos(hwnd, SB_VERT, This->verticalPosition / (double)This->verticalMaxPosition * MAX_SCROLL_VALUE, TRUE);
    }
    else
    {
        SetScrollRange(hwnd, SB_VERT, 0, This->verticalMaxPosition, FALSE);
        SetScrollPos(hwnd, SB_VERT, This->verticalPosition, TRUE);
    }
}

static void SetHScrollPos(TextAppearance* This, HWND hwnd)
{
    if (This->horizontalMaxPosition > MAX_SCROLL_VALUE)
        SetScrollPos(hwnd, SB_HORZ, This->horizontalPosition / (double)This->horizontalMaxPosition * MAX_SCROLL_VALUE, TRUE);
    else
        SetScrollPos(hwnd, SB_HORZ, This->horizontalPosition, TRUE);
}

static void SetHScrollRange(TextAppearance* This, HWND hwnd)
{
    if (This->horizontalMaxPosition > MAX_SCROLL_VALUE)
    {
        SetScrollRange(hwnd, SB_HORZ, 0, MAX_SCROLL_VALUE, FALSE);
        SetScrollPos(hwnd, SB_HORZ, This->horizontalPosition / (double)This->horizontalMaxPosition * MAX_SCROLL_VALUE, TRUE);
    }
    else
    {
        SetScrollRange(hwnd, SB_HORZ, 0, This->horizontalMaxPosition, FALSE);
        SetScrollPos(hwnd, SB_HORZ, This->horizontalPosition, TRUE);
    }
}

static RETC SetFormatedMode(TextAppearance* This, HWND hwnd)
{
    RETC code;
    This->mode = TEXT_APPEARANCE_FORMATED;
    This->setNextLine = SetNextLine_Formated;
    This->setPrevLine = SetPrevLine_Formated;
    This->firstScreenLine->lineBeginOffset = 0;
    code = ResetScreenLines(This);
    if (code != RETC_SUCCESS)
        return code;
    RecalculatePosition_Formated(This);
    InvalidateRect(hwnd, NULL, TRUE);
    ShowScrollBar(hwnd, SB_VERT, TRUE);
    ShowScrollBar(hwnd, SB_HORZ, FALSE);
    SetVScrollRange(This, hwnd);
    return RETC_SUCCESS;
}

static RETC SetUnformatedMode(TextAppearance* This, HWND hwnd)
{
    RETC code;
    This->mode = TEXT_APPEARANCE_UNFORMATED;
    This->setNextLine = SetNextLine_Unformated;
    This->setPrevLine = SetPrevLine_Unformated;
    This->firstScreenLine->lineBeginOffset = 0;
    code = ResetScreenLines(This);
    if (code != RETC_SUCCESS)
        return code;
    RecalculatePosition_Unformated(This);
    InvalidateRect(hwnd, NULL, TRUE);
    ShowScrollBar(hwnd, SB_VERT, TRUE);
    ShowScrollBar(hwnd, SB_HORZ, TRUE);
    SetVScrollRange(This, hwnd);
    return RETC_SUCCESS;
}

TextAppearance* TextAppearance_Get(TextData* textData, HWND hwnd, BOOL isFormated)
{
    TextAppearance* This = malloc(sizeof(TextAppearance));
    if (This == NULL)
        return NULL;
    This->firstScreenLine = malloc(sizeof(ScreenLine));
    if (This->firstScreenLine == NULL)
    {
        free(This);
        return NULL;
    }

    This->hFont = CreateFont(FONT_CHAR_HEIGHT, FONT_CHAR_WIDTH, 0, 0, FONT_CHAR_WEIGHT, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FIXED_PITCH | FF_DONTCARE, "Consolas");
    HDC hDC;
    hDC = GetDC(hwnd);
    SelectObject(hDC, This->hFont);
    SetBkMode(hDC, TRANSPARENT);
    This->charWidth = FONT_CHAR_WIDTH;
    This->charHeigth = FONT_CHAR_HEIGHT;
    This->betweenLinesDist = FONT_BETWEEN_LINES_DIST;

    This->screenWidthInChars = 1;
    This->screenHeigthInLines = 1;
    This->textData = textData;
    This->firstScreenLine->nextLine = This->firstScreenLine;
    This->firstScreenLine->prevLine = This->firstScreenLine;
    This->firstScreenLine->textLineIndex = 0;
    This->firstScreenLine->lineBeginOffset = 0;

    This->mode = TEXT_APPEARANCE_NONE;
    if (TextAppearance_SetMode(This, hwnd, isFormated ? TEXT_APPEARANCE_FORMATED : TEXT_APPEARANCE_UNFORMATED) != RETC_SUCCESS)
    {
        TextAppearance_Destroy(&This);
        return NULL;
    }

    RECT rect;
    GetWindowRect(hwnd, &rect);
    long width = rect.right - rect.left;
    long height = rect.bottom - rect.top;
    if (TextAppearance_Resize(This, hwnd, width, height) != RETC_SUCCESS)
    {
        TextAppearance_Destroy(&This);
        return NULL;
    }

    return This;
}

void TextAppearance_Destroy(TextAppearance** pThis)
{
    ScreenLine* curLine;
    if (pThis == NULL || *pThis == NULL)
        return;

    curLine = (*pThis)->firstScreenLine;
    do
    {
        ScreenLine* tmp = curLine->nextLine;
        free(curLine);
        curLine = tmp;
    }
    while (curLine != (*pThis)->firstScreenLine);
    DeleteObject((*pThis)->hFont);
    free((*pThis));
    (*pThis) = NULL;
}

RETC TextAppearance_Resize(TextAppearance* This, HWND hwnd, long windowWidth, long windowHeigth)
{
    unsigned long prevHeigthInLines = This->screenHeigthInLines;
    unsigned long prevWidthInChars = This->screenWidthInChars;
    This->screenHeigthInLines = (windowHeigth + This->betweenLinesDist - 1) / This->betweenLinesDist;
    This->screenWidthInChars = (windowWidth) / This->charWidth;
    if (This->screenHeigthInLines != prevHeigthInLines || This->screenWidthInChars != prevWidthInChars)
    {
        if (This->mode == TEXT_APPEARANCE_FORMATED || This->screenHeigthInLines != prevHeigthInLines)
        {
            RETC code = ResetScreenLines(This);
            if (code != RETC_SUCCESS)
                return code;
            if (This->mode == TEXT_APPEARANCE_FORMATED)
            {
                RecalculatePosition_Formated(This);
            }
        }
        InvalidateRect(hwnd, NULL, TRUE);
        SetVScrollRange(This, hwnd);
        SetHScrollRange(This, hwnd);
    }
    return RETC_SUCCESS;
}

void TextAppearance_VScrollToPosition(TextAppearance* This, HWND hwnd, unsigned long position)
{
    long delta;
    if (This->verticalMaxPosition > MAX_SCROLL_VALUE)
        delta =  position / (double)MAX_SCROLL_VALUE * This->verticalMaxPosition - This->verticalPosition;
    else
        delta = position - This->verticalPosition;
    TextAppearance_VScrollByDelta(This, hwnd, delta);
}

void TextAppearance_VScrollByDelta(TextAppearance* This, HWND hwnd, long delta)
{
    long i = 0;
    if (delta > 0)
        for (i = 0; i < delta && This->verticalPosition < This->verticalMaxPosition; i++)
        {
            _MyDebugPrintln("Scroll down.");
            This->verticalPosition++;
            This->firstScreenLine = This->firstScreenLine->nextLine;
            This->setNextLine(This, This->firstScreenLine->prevLine->prevLine, This->firstScreenLine->prevLine);
        }
    if (delta < 0)
        for (i = 0; i > delta && This->verticalPosition > 0; i--)
        {
            _MyDebugPrintln("Scroll up.");
            This->verticalPosition--;
            This->firstScreenLine = This->firstScreenLine->prevLine;
            This->setPrevLine(This, This->firstScreenLine, This->firstScreenLine->nextLine);
        }
    if (i != 0)
    {
        InvalidateRect(hwnd, NULL, TRUE);
        SetVScrollPos(This, hwnd);
    }
}

void TextAppearance_HScrollToPosition(TextAppearance* This, HWND hwnd, unsigned long position)
{
    long delta;
    if (This->horizontalMaxPosition > MAX_SCROLL_VALUE)
        delta =  position / (double)MAX_SCROLL_VALUE * This->horizontalMaxPosition - This->horizontalPosition;
    else
        delta = position - This->horizontalPosition;
    TextAppearance_HScrollByDelta(This, hwnd, delta);
}

void TextAppearance_HScrollByDelta(TextAppearance* This, HWND hwnd, long delta)
{
    long i = 0;
    if (delta > 0)
        for (i = 0; i < delta && This->horizontalPosition < This->horizontalMaxPosition; i++)
        {
            _MyDebugPrintln("Scroll right.");
            This->horizontalPosition++;
        }
    if (delta < 0)
        for (i = 0; i > delta && This->horizontalPosition > 0; i--)
        {
            _MyDebugPrintln("Scroll left.");
            This->horizontalPosition--;
        }
    if (i != 0)
    {
        InvalidateRect(hwnd, NULL, TRUE);
        SetHScrollPos(This, hwnd);
    }
}

RETC TextAppearance_SetMode(TextAppearance* This, HWND hwnd, enum TEXT_APPEARANCE_MODE mode)
{
    if (This->mode != mode)
        switch(mode)
        {
        case TEXT_APPEARANCE_FORMATED:
            return SetFormatedMode(This, hwnd);
        case TEXT_APPEARANCE_UNFORMATED:
            return SetUnformatedMode(This, hwnd);
        default:
            return RETC_LOGIC_ERROR;
        }
    return RETC_SUCCESS;
}

void TextAppearance_Paint(TextAppearance* This, HWND hwnd)
{
    HDC hDC;
    PAINTSTRUCT paintStruct;
    hDC = BeginPaint(hwnd, &paintStruct);
    ScreenLine* curLine = This->firstScreenLine;
    RECT screenRect;
    int i = 0;
    long maxShownlen = This->screenWidthInChars;
    GetClientRect(hwnd, &screenRect);
    if (This->mode == TEXT_APPEARANCE_UNFORMATED)
        maxShownlen += 1;
    do
    {
        if (curLine->textLineIndex < This->textData->textLinesCount && curLine->textLineIndex >= 0)
        {
            long pos = (This->mode == TEXT_APPEARANCE_UNFORMATED) ? (This->horizontalPosition) : (curLine->lineBeginOffset);
            long len = TextData_LineLen(This->textData, curLine->textLineIndex) - pos;
            len = MIN(len, maxShownlen);
            TextOut(
                hDC,
                screenRect.left,
                screenRect.top + This->betweenLinesDist * i,
                This->textData->textLines[curLine->textLineIndex] + pos,
                len
            );
        }
        i++;
        curLine = curLine->nextLine;
    }
    while (curLine != This->firstScreenLine);
    EndPaint(hwnd, &paintStruct);
}
