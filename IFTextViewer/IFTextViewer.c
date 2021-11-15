#include "IFTextViewer.h"
#include "../TextData/TextData.h"
#include "../TextAppearance/TextAppearance.h"
#include "../menu_ids.h"

struct _IFTextViewer_InstanceData
{
    int isInit;
    int isOpenningFile;
    int isFileOpened;
    TextData* textData;
    TextAppearance* textAppearance;
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
    instanceData->isOpenningFile = FALSE;
    instanceData->isFileOpened = FALSE;
    instanceData->textData = NULL;
    instanceData->textAppearance = NULL;
    return instanceData;
}

BOOL IFTextViewer_IsInit(IFTextViewer_InstanceData* This)
{
    return This->isInit;
}

static BOOL IFTextViewer_LoadFile(IFTextViewer_InstanceData* This, HWND hwnd, BOOL isFormated, const char* fileName)
{
    if (This->isOpenningFile)
        return FALSE;
    This->isOpenningFile = TRUE;
    This->isFileOpened = FALSE;
    TextAppearance_Destroy(&This->textAppearance);
    TextData_Destroy(&This->textData);
    This->textData = TextData_Get(fileName);
    if (This->textData == NULL)
    {
        This->isOpenningFile = FALSE;
        return FALSE;
    }
    This->textAppearance = TextAppearance_Get(This->textData, hwnd, isFormated);
    if (This->textAppearance == NULL)
    {
        TextData_Destroy(&This->textData);
        This->isOpenningFile = FALSE;
        return FALSE;
    }
    This->isOpenningFile = FALSE;
    This->isFileOpened = TRUE;
    return TRUE;
}

void IFTextViewer_CreateMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    CREATESTRUCT *tmpCreateStruct = (CREATESTRUCT *)lParam;
    This->hInstanse = tmpCreateStruct->hInstance;
    This->isInit = TRUE;
    if (!IFTextViewer_LoadFile(This, hwnd, TRUE, (char*)tmpCreateStruct->lpCreateParams))
    {
        MessageBox(hwnd, "Cannot open file!", "Open File Error", MB_OK);
    }
}

RETC IFTextViewer_SizeMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    if (!This->isFileOpened)
        return RETC_SUCCESS;
    return TextAppearance_Resize(This->textAppearance, hwnd, LOWORD(lParam), HIWORD(lParam));
}

RETC IFTextViewer_PaintMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    if (This->isFileOpened)
        TextAppearance_Paint(This->textAppearance, hwnd);
    return RETC_SUCCESS;
}

RETC IFTextViewer_VScrollMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    if (!This->isFileOpened)
        return RETC_SUCCESS;
    switch (LOWORD(wParam))
    {
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            TextAppearance_VScrollToPosition(This->textAppearance, hwnd, HIWORD(wParam));
            break;
        case SB_LINEDOWN:
            TextAppearance_VScrollByDelta(This->textAppearance, hwnd, 1);
            break;
        case SB_LINEUP:
            TextAppearance_VScrollByDelta(This->textAppearance, hwnd, -1);
            break;
        default:
            break;
    }
    return RETC_SUCCESS;
}

RETC IFTextViewer_HScrollMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    if (!This->isFileOpened)
        return RETC_SUCCESS;
    switch (LOWORD(wParam))
    {
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            TextAppearance_HScrollToPosition(This->textAppearance, hwnd, HIWORD(wParam));
            break;
        case SB_LINEDOWN:
            TextAppearance_HScrollByDelta(This->textAppearance, hwnd, 1);
            break;
        case SB_LINEUP:
            TextAppearance_HScrollByDelta(This->textAppearance, hwnd, -1);
            break;
        default:
            break;
    }
    return RETC_SUCCESS;
}

RETC IFTextViewer_MousewheelMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    if (!This->isFileOpened)
        return RETC_SUCCESS;
    long zDelta = -GET_WHEEL_DELTA_WPARAM(wParam) / 120;
    TextAppearance_VScrollByDelta(This->textAppearance, hwnd, zDelta);
    return RETC_SUCCESS;
}

RETC IFTextViewer_KeyDownMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam)
{

    if (This->isFileOpened) switch (wParam)
    {
        case VK_UP:
            TextAppearance_VScrollByDelta(This->textAppearance, hwnd, -1);
            break;
        case VK_DOWN:
            TextAppearance_VScrollByDelta(This->textAppearance, hwnd, 1);
            break;
        case VK_LEFT:
            TextAppearance_HScrollByDelta(This->textAppearance, hwnd, -1);
            break;
        case VK_RIGHT:
            TextAppearance_HScrollByDelta(This->textAppearance, hwnd, 1);
            break;
        case VK_PRIOR:
            TextAppearance_VScrollByDelta(This->textAppearance, hwnd, -(This->textAppearance->screenHeigthInLines - 1));
            break;
        case VK_NEXT:
            TextAppearance_VScrollByDelta(This->textAppearance, hwnd, This->textAppearance->screenHeigthInLines - 1);
            break;
    }
    return RETC_SUCCESS;
}


RETC IFTextViewer_CommandMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    switch(LOWORD(wParam))
    {
        case IDM_FILE_OPEN:
            if (!This->isOpenningFile) {
                OPENFILENAME ofn;
                char szFile[260];

                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = "*.TXT\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn) == TRUE)
                {
                    DWORD state;
                    BOOL isFormated;
                    state = GetMenuState(GetMenu(hwnd), IDM_VIEW_WRAP, MF_BYCOMMAND);
                    isFormated = state & MF_CHECKED;
                    if (!IFTextViewer_LoadFile(This, hwnd, isFormated, (char*)ofn.lpstrFile))
                    {
                        MessageBox(hwnd, "Cannot open file!", "Open File Error", MB_OK);
                    }
                }
            };
            break;
        case IDM_ABOUT:
            MessageBox(hwnd, "GSHSRRJHRS\n"
                    "5030102/90201 group\n"
                    "Year 2021", "About", MB_OK);
            break;
        case IDM_FILE_EXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case IDM_VIEW_WRAP:
            if (This->isFileOpened){
                DWORD state ;
                state = GetMenuState(GetMenu(hwnd), IDM_VIEW_WRAP, MF_BYCOMMAND);
                if (state & MF_CHECKED)
                {
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WRAP, MF_BYCOMMAND | MF_UNCHECKED);
                    return TextAppearance_SetMode(This->textAppearance, hwnd, TEXT_APPEARANCE_UNFORMATED);
                }
                else
                {
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WRAP, MF_BYCOMMAND | MF_CHECKED);
                    return TextAppearance_SetMode(This->textAppearance, hwnd, TEXT_APPEARANCE_FORMATED);
                }
            }
            break;
        default:
            break;
    }
    return RETC_SUCCESS;
}

void IFTextViewer_DestroyInstanceData(IFTextViewer_InstanceData* This, HWND hwnd)
{
    TextAppearance_Destroy(&This->textAppearance);
    TextData_Destroy(&This->textData);
    free(This);
}

