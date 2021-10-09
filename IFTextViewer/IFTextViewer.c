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

}

void IFTextViewer_PaintMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{
    TextAppearance_Paint(This->textAppearance, This->hwnd);
}

void IFTextViewer_VScrollMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{

}

void IFTextViewer_HScrollMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{

}

void IFTextViewer_MousewheelMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{

}

void IFTextViewer_KeyDownMassageHandler(IFTextViewer_InstanceData* This, WPARAM wParam, LPARAM lParam)
{

}

void IFTextViewer_DestroyInstanceData(IFTextViewer_InstanceData* This)
{
    TextAppearance_Destroy(This->textAppearance);
    TextData_Destroy(This->textData);
    free(This);
}

