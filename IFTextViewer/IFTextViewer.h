#ifndef __IF_TEXT_VIEWER_H_
#define __IF_TEXT_VIEWER_H_

#include "../defines.h"


typedef struct _IFTextViewer_InstanceData IFTextViewer_InstanceData;

IFTextViewer_InstanceData* IFTextViewer_GetInstanceData();

BOOL IFTextViewer_IsInit(IFTextViewer_InstanceData* pInstanceData);

void IFTextViewer_CreateMassageHandler(IFTextViewer_InstanceData* pInstanceData, HWND hwnd, WPARAM wParam, LPARAM lParam);

void IFTextViewer_SizeMassageHandler(IFTextViewer_InstanceData* pInstanceData, WPARAM wParam, LPARAM lParam);

void IFTextViewer_PaintMassageHandler(IFTextViewer_InstanceData* pInstanceData, WPARAM wParam, LPARAM lParam);

void IFTextViewer_VScrollMassageHandler(IFTextViewer_InstanceData* pInstanceData, WPARAM wParam, LPARAM lParam);

void IFTextViewer_HScrollMassageHandler(IFTextViewer_InstanceData* pInstanceData, WPARAM wParam, LPARAM lParam);

void IFTextViewer_MousewheelMassageHandler(IFTextViewer_InstanceData* pInstanceData, WPARAM wParam, LPARAM lParam);

void IFTextViewer_KeyDownMassageHandler(IFTextViewer_InstanceData* pInstanceData, WPARAM wParam, LPARAM lParam);

void IFTextViewer_DestroyInstanceData(IFTextViewer_InstanceData* pInstanceData);

#endif // __IF_TEXT_VIEWER_H_
