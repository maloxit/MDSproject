#include "TextAppearance.h"

TextAppearance* TextAppearance_Get(TextData* textData, HWND hwnd)
{
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
    This->screenWidthInChars = 0;
    This->screenHeigthInLines = 0;
    This->screenLinesCount = 1;
    This->textData = textData;
    This->firstScreenLine->textLineIndex = 0;
    This->firstScreenLine->lineBeginOffset = 0;
    This->firstScreenLine->nextLine = This->firstScreenLine;
    This->firstScreenLine->prevLine = This->firstScreenLine;
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

int TextAppearance_Resize(TextAppearance* This, long windowWidth, long windowHeigth)
{
    //TODO
}

int TextAppearance_VScroll(TextAppearance* This, long delta)
{
    return -1;
}

int TextAppearance_Paint(TextAppearance* This, HWND hwnd)
{
    HDC hDC;
    PAINTSTRUCT paintStruct;
    hDC = BeginPaint(hwnd, &paintStruct);
    ScreenLine* curLine = This->firstScreenLine;
    RECT screenRect;
    int i = 0;
    GetClientRect(hwnd, &screenRect);
    do
    {
        if (curLine->textLineIndex < This->textData->textLinesCount && curLine->textLineIndex >= 0)
        {
            TextOut(
                    hDC,
                    screenRect.left,
                    screenRect.top + This->betweenLinesDist * i,
                    This->textData->textLines[curLine->textLineIndex] + curLine->lineBeginOffset,
                    MIN(This->screenWidthInChars, This->textData->textLineLens[curLine->textLineIndex] - curLine->lineBeginOffset)
                    );
        }
        i++;
        curLine = curLine->nextLine;
    } while (curLine != This->firstScreenLine);
    EndPaint(hwnd, &paintStruct);
    return 0;
}
