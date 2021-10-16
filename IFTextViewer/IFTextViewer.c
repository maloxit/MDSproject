#include "IFTextViewer.h"
#include "../TextData/TextData.h"
#include "../TextAppearance/TextAppearance.h"

struct _IFTextViewer_InstanceData
{
    int isInit;
    TextData* textData;
    TextAppearance* textAppearance;
    HWND hwnd;
    HFONT hFont;
    HINSTANCE hInstanse;
};

IFTextViewer_InstanceData* IFTextViewer_GetInstanceData()
{
    IFTextViewer_InstanceData* instanceData = malloc(sizeof(IFTextViewer_InstanceData));
    if (instanceData == NULL)
    {
        return NULL;
    }
    instanceData->isInit = FALSE;
    return instanceData;
}

WINBOOL IFTextViewer_IsInit(IFTextViewer_InstanceData* This)
{
    return This->isInit;
}

void IFTextViewer_CreateMassageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    CREATESTRUCT *tmpCreateStruct = (CREATESTRUCT *)lParam;
    This->hwnd = hwnd;
    This->hInstanse = tmpCreateStruct->hInstance;
    This->textData = TextData_Get((char*)tmpCreateStruct->lpCreateParams);
    if (This->textData == NULL)
    {
        This->isInit = FALSE;
        return;
    }
    This->textAppearance = TextAppearance_Get(This->textData, This->hwnd);
    if (This->textAppearance == NULL)
    {
        TextData_Destroy(This->textData);
        This->isInit = FALSE;
        return;
    }
    This->isInit = TRUE;
}

void IFTextViewer_SizeMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{
    if (!This->isInit)
        return;
    if (0 != TextAppearance_Resize(This->textAppearance, This->hwnd, LOWORD(lParam), HIWORD(lParam)))
    {
        IFTextViewer_DestroyInstanceData(This);
        PostQuitMessage (0);
    }
}

void IFTextViewer_PaintMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{
    if (!This->isInit)
        return;
    TextAppearance_Paint(This->textAppearance, This->hwnd);
}

void IFTextViewer_VScrollMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{
    if (!This->isInit)
        return;
    switch (LOWORD(wParam))
    {
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            TextAppearance_VScrollToPosition(This->textAppearance, This->hwnd, HIWORD(wParam));
            break;
        case SB_LINEDOWN:
            TextAppearance_VScrollByDelta(This->textAppearance, This->hwnd, 1);
            break;
        case SB_LINEUP:
            TextAppearance_VScrollByDelta(This->textAppearance, This->hwnd, -1);
            break;
        default:
            break;
    }
}

void IFTextViewer_HScrollMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{
    if (!This->isInit)
        return;
    switch (LOWORD(wParam))
    {
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            TextAppearance_HScrollToPosition(This->textAppearance, This->hwnd, HIWORD(wParam));
            break;
        case SB_LINEDOWN:
            TextAppearance_HScrollByDelta(This->textAppearance, This->hwnd, 1);
            break;
        case SB_LINEUP:
            TextAppearance_HScrollByDelta(This->textAppearance, This->hwnd, -1);
            break;
        default:
            break;
    }
}

void IFTextViewer_MousewheelMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{
    if (!This->isInit)
        return;
    long zDelta = -GET_WHEEL_DELTA_WPARAM(wParam) / 120;
    TextAppearance_VScrollByDelta(This->textAppearance, This->hwnd, zDelta);
}

void IFTextViewer_KeyDownMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{
    if (!This->isInit)
        return;
    switch (wParam)
    {
        case VK_UP:
            TextAppearance_VScrollByDelta(This->textAppearance, This->hwnd, -1);
            break;
        case VK_DOWN:
            TextAppearance_VScrollByDelta(This->textAppearance, This->hwnd, 1);
            break;
        case VK_LEFT:
            TextAppearance_HScrollByDelta(This->textAppearance, This->hwnd, -1);
            break;
        case VK_RIGHT:
            TextAppearance_HScrollByDelta(This->textAppearance, This->hwnd, 1);
            break;
        case VK_PRIOR:
            TextAppearance_VScrollByDelta(This->textAppearance, This-> hwnd, -(This->textAppearance->screenHeigthInLines - 1));
            break;
        case VK_NEXT:
            TextAppearance_VScrollByDelta(This->textAppearance, This->hwnd, This->textAppearance->screenHeigthInLines - 1);
            break;
        case VK_TAB:
            TextAppearance_SetMode(This->textAppearance, This-> hwnd, TEXT_APPEARANCE_UNFORMATED);
            break;
        case VK_SHIFT:
            TextAppearance_SetMode(This->textAppearance, This-> hwnd, TEXT_APPEARANCE_FORMATED);
            break;
        default:
            break;
    }
}

void IFTextViewer_DestroyInstanceData(IFTextViewer_InstanceData* This)
{
    TextAppearance_Destroy(This->textAppearance);
    TextData_Destroy(This->textData);
    free(This);
}

