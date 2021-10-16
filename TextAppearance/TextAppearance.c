#include "TextAppearance.h"

static void TextAppearance_SetNextLine_Formated(TextAppearance* This, ScreenLine* curLine, ScreenLine* nextLine)
{
    int prevLineIndex = curLine->textLineIndex;
    if (prevLineIndex >= This->textData->textLinesCount || (This->textData->textLineLens[prevLineIndex] - curLine->lineBeginOffset) <= This->screenWidthInChars)
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

static void TextAppearance_SetNextLine_Unformated(TextAppearance* This, ScreenLine* curLine, ScreenLine* nextLine)
{
    nextLine->textLineIndex = curLine->textLineIndex + 1;
    nextLine->lineBeginOffset = 0;
}

static void TextAppearance_SetPrevLine_Formated(TextAppearance* This, ScreenLine* prevLine, ScreenLine* curLine)
{
    int curLineIndex = curLine->textLineIndex;
    if (curLineIndex > This->textData->textLinesCount)
    {
        prevLine->textLineIndex = curLineIndex - 1;
        prevLine->lineBeginOffset = 0;
    }
    else if (curLine->lineBeginOffset == 0)
    {
        prevLine->textLineIndex = curLineIndex - 1;
        prevLine->lineBeginOffset = (This->textData->textLineLens[curLineIndex - 1]) / This->screenWidthInChars * This->screenWidthInChars;
        if (prevLine->lineBeginOffset == This->textData->textLineLens[curLineIndex - 1] && prevLine->lineBeginOffset != 0)
            prevLine->lineBeginOffset -= This->screenWidthInChars;
    }
    else
    {
        prevLine->textLineIndex = curLineIndex;
        prevLine->lineBeginOffset = curLine->lineBeginOffset - This->screenWidthInChars;
    }
}

static void TextAppearance_SetPrevLine_Unformated(TextAppearance* This, ScreenLine* prevLine, ScreenLine* curLine)
{
    prevLine->textLineIndex = curLine->textLineIndex - 1;
    prevLine->lineBeginOffset = 0;
}

static void TextAppearance_RecalculatePosition_Formated(TextAppearance* This)
{
    int i;
    unsigned long position = 0;
    This->horizontalPosition = 0;
    This->horizontalMaxPosition = 0;
    This->verticalPosition = 0;
    for (i = 0; i < This->textData->textLinesCount; i++)
    {
        if (i == This->firstScreenLine->textLineIndex)
        {
            This->verticalPosition = position;
        }
        ++position;
        if (This->textData->textLineLens[i] != 0)
            position += (This->textData->textLineLens[i] - 1) / This->screenWidthInChars;
    }
    if (position != 0)
        --position;
    This->verticalMaxPosition = position;
}

static void TextAppearance_RecalculatePosition_Unformated(TextAppearance* This)
{
    int i;
    unsigned long position = 0;
    This->verticalMaxPosition = This->textData->textLinesCount;
    if (This->verticalMaxPosition != 0)
        --This->verticalMaxPosition;
    This->verticalPosition = This->firstScreenLine->textLineIndex;
    for (i = 0; i < This->textData->textLinesCount; i++)
    {
        if (This->textData->textLineLens[i] > position)
            position = This->textData->textLineLens[i];
    }
    if (position != 0)
        --position;
    This->horizontalMaxPosition = position;
}

static ScreenLine* TextAppearance_AddLineBefore(ScreenLine* line)
{
    ScreenLine* newLine = (ScreenLine*)malloc(sizeof(ScreenLine));
    if (newLine == NULL)
    {
        return NULL;
    }
    newLine->prevLine = line->prevLine;
    newLine->nextLine = line;
    line->prevLine->nextLine = newLine;
    line->prevLine = newLine;
    return newLine;
}

static int TextAppearance_ResetScreenLines(TextAppearance* This)
{
    int i;

    ScreenLine* line;
    line = This->firstScreenLine->nextLine;
    if (This->mode == TEXT_APPEARANCE_FORMATED)
    {
        This->firstScreenLine->lineBeginOffset = 0;
    }
    for (i = 1; i < This->screenHeigthInLines; i++)
    {
        if (line == This->firstScreenLine)
        {
            ScreenLine* newLine = TextAppearance_AddLineBefore(line);
            if (newLine == NULL)
            {
                return -1;
            }
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
        } while (line != This->firstScreenLine);
    }
    return 0;
}

static int TextAppearance_SetFormatedMode(TextAppearance* This, HWND hwnd)
{
    int code;
    This->mode = TEXT_APPEARANCE_FORMATED;

    This->setNextLine = TextAppearance_SetNextLine_Formated;
    This->setPrevLine = TextAppearance_SetPrevLine_Formated;

    This->firstScreenLine->lineBeginOffset = 0;
    code = TextAppearance_ResetScreenLines(This);
    if (code != 0)
        return code;
    TextAppearance_RecalculatePosition_Formated(This);


    InvalidateRect(hwnd, NULL, TRUE);
    ShowScrollBar(hwnd, SB_VERT, TRUE);
    ShowScrollBar(hwnd, SB_HORZ, FALSE);

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

    return 0;
}

static int TextAppearance_SetUnformatedMode(TextAppearance* This, HWND hwnd)
{
    This->mode = TEXT_APPEARANCE_UNFORMATED;

    This->setNextLine = TextAppearance_SetNextLine_Unformated;
    This->setPrevLine = TextAppearance_SetPrevLine_Unformated;

    This->firstScreenLine->lineBeginOffset = 0;
    TextAppearance_ResetScreenLines(This);
    TextAppearance_RecalculatePosition_Unformated(This);


    InvalidateRect(hwnd, NULL, TRUE);
    ShowScrollBar(hwnd, SB_VERT, TRUE);
    ShowScrollBar(hwnd, SB_HORZ, TRUE);

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

    return 0;
}

TextAppearance* TextAppearance_Get(TextData* textData, HWND hwnd)
{
    int code;
    TextAppearance* This = malloc(sizeof(TextAppearance));
    if (This == NULL)
    {
        return NULL;
    }
    This->firstScreenLine = malloc(sizeof(ScreenLine));
    if (This->firstScreenLine == NULL)
    {
        free(This);
        return NULL;
    }
    This->hFont = CreateFont(20, 10, 0, 0, 400, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FIXED_PITCH | FF_DONTCARE, "Consolas");
    HDC hDC;
    hDC = GetDC(hwnd);
    SelectObject(hDC, This->hFont);
    SetBkMode(hDC, TRANSPARENT);
    This->charWidth = 10;
    This->charHeigth = 20;
    This->betweenLinesDist = 20;
    This->screenWidthInChars = 1;
    This->screenHeigthInLines = 1;
    This->textData = textData;
    This->firstScreenLine->nextLine = This->firstScreenLine;
    This->firstScreenLine->prevLine = This->firstScreenLine;
    This->firstScreenLine->textLineIndex = 0;
    This->firstScreenLine->lineBeginOffset = 0;

    code = TextAppearance_SetFormatedMode(This, hwnd);
    if (code != 0)
    {
        TextAppearance_Destroy(This);
        return NULL;
    }


    return This;
}

void TextAppearance_Destroy(TextAppearance* This)
{
    ScreenLine* curLine = This->firstScreenLine;

    do
    {
        ScreenLine* tmp = curLine->nextLine;
        free(curLine);
        curLine = tmp;
    } while (curLine != This->firstScreenLine);
    DeleteObject(This->hFont);
    free(This);
}

int TextAppearance_Resize(TextAppearance* This, HWND hwnd, long windowWidth, long windowHeigth)
{
    unsigned long prevHeigthInLines = This->screenHeigthInLines;
    unsigned long prevWidthInChars = This->screenWidthInChars;
    This->screenHeigthInLines = (windowHeigth + This->betweenLinesDist - 1) / This->betweenLinesDist;
    This->screenWidthInChars = (windowWidth) / This->charWidth;
    if (This->screenHeigthInLines != prevHeigthInLines || This->screenWidthInChars != prevWidthInChars)
    {
        if (This->mode == TEXT_APPEARANCE_FORMATED || This->screenHeigthInLines != prevHeigthInLines)
        {
            int code = TextAppearance_ResetScreenLines(This);
            if (code != 0)
                return code;
            if (This->mode == TEXT_APPEARANCE_FORMATED)
            {
                TextAppearance_RecalculatePosition_Formated(This);
            }

        }


        InvalidateRect(hwnd, NULL, TRUE);

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
    return 0;
}

int TextAppearance_VScrollToPosition(TextAppearance* This, HWND hwnd, unsigned long position)
{
    long delta;
    if (This->verticalMaxPosition > MAX_SCROLL_VALUE)
    {
        delta =  position / (double)MAX_SCROLL_VALUE * This->verticalMaxPosition - This->verticalPosition;
    }
    else
    {
        delta = position - This->verticalPosition;
    }
    return TextAppearance_VScrollByDelta(This, hwnd, delta);
}

int TextAppearance_VScrollByDelta(TextAppearance* This, HWND hwnd, long delta)
{

    long i = 0;
    if (delta > 0)
    {
        for (i = 0; i < delta && This->verticalPosition < This->verticalMaxPosition; i++)
        {
            _MyDebugPrintln("Scroll down.");
            This->verticalPosition++;
            This->firstScreenLine = This->firstScreenLine->nextLine;
            This->setNextLine(This, This->firstScreenLine->prevLine->prevLine, This->firstScreenLine->prevLine);
        }
    }
    if (delta < 0)
    {
        for (i = 0; i > delta && This->verticalPosition > 0; i--)
        {
            _MyDebugPrintln("Scroll up.");
            This->verticalPosition--;
            This->firstScreenLine = This->firstScreenLine->prevLine;
            This->setPrevLine(This, This->firstScreenLine, This->firstScreenLine->nextLine);
        }
    }
    if (i != 0)
    {
        InvalidateRect(hwnd, NULL, TRUE);
        if (This->verticalMaxPosition > MAX_SCROLL_VALUE)
        {
            SetScrollPos(hwnd, SB_VERT, (int)(This->verticalPosition / (double)This->verticalMaxPosition * MAX_SCROLL_VALUE), TRUE);
        }
        else
        {
            SetScrollPos(hwnd, SB_VERT, This->verticalPosition, TRUE);
        }
    }

    return 0;
}

int TextAppearance_HScrollToPosition(TextAppearance* This, HWND hwnd, unsigned long position)
{
    long delta;
    if (This->horizontalMaxPosition > MAX_SCROLL_VALUE)
    {
        delta =  position / (double)MAX_SCROLL_VALUE * This->horizontalMaxPosition - This->horizontalPosition;
    }
    else
    {
        delta = position - This->horizontalPosition;
    }
    return TextAppearance_HScrollByDelta(This, hwnd, delta);
}

int TextAppearance_HScrollByDelta(TextAppearance* This, HWND hwnd, long delta)
{

    long i = 0;
    if (delta > 0)
    {
        for (i = 0; i < delta && This->horizontalPosition < This->horizontalMaxPosition; i++)
        {
            _MyDebugPrintln("Scroll right.");
            This->horizontalPosition++;
        }
    }
    if (delta < 0)
    {
        for (i = 0; i > delta && This->horizontalPosition > 0; i--)
        {
            _MyDebugPrintln("Scroll left.");
            This->horizontalPosition--;
        }
    }
    if (i != 0)
    {
        InvalidateRect(hwnd, NULL, TRUE);
        if (This->horizontalMaxPosition > MAX_SCROLL_VALUE)
        {
            SetScrollPos(hwnd, SB_HORZ, (int)(This->horizontalPosition / (double)This->horizontalMaxPosition * MAX_SCROLL_VALUE), TRUE);
        }
        else
        {
            SetScrollPos(hwnd, SB_HORZ, This->horizontalPosition, TRUE);
        }
    }

    return 0;
}

int TextAppearance_SetMode(TextAppearance* This, HWND hwnd, enum TEXT_APPEARANCE_MODE mode)
{

    if (This->mode != mode) switch(mode)
    {
    case TEXT_APPEARANCE_FORMATED:
        TextAppearance_SetFormatedMode(This, hwnd);
        break;
    case TEXT_APPEARANCE_UNFORMATED:
        TextAppearance_SetUnformatedMode(This, hwnd);
        break;
    default:
        return -1;
    }
    return 0;
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
    {
        maxShownlen += 1;
        do
        {
            if (curLine->textLineIndex < This->textData->textLinesCount && curLine->textLineIndex >= 0)
            {
                long len = This->textData->textLineLens[curLine->textLineIndex] - This->horizontalPosition;
                len = MIN(len, maxShownlen);
                TextOut(
                        hDC,
                        screenRect.left,
                        screenRect.top + This->betweenLinesDist * i,
                        This->textData->textLines[curLine->textLineIndex] + This->horizontalPosition,
                        len
                        );
            }
            i++;
            curLine = curLine->nextLine;
        } while (curLine != This->firstScreenLine);

    }
    else
    {
        do
        {
            if (curLine->textLineIndex < This->textData->textLinesCount && curLine->textLineIndex >= 0)
            {
                long len = This->textData->textLineLens[curLine->textLineIndex] - curLine->lineBeginOffset;
                len = MIN(len, maxShownlen);
                TextOut(
                        hDC,
                        screenRect.left,
                        screenRect.top + This->betweenLinesDist * i,
                        This->textData->textLines[curLine->textLineIndex] + curLine->lineBeginOffset,
                        len
                        );
            }
            i++;
            curLine = curLine->nextLine;
        } while (curLine != This->firstScreenLine);
    }

    EndPaint(hwnd, &paintStruct);
}
