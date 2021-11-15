#ifndef __IF_TEXT_VIEWER_H_
#define __IF_TEXT_VIEWER_H_

#include "../defines.h"


typedef struct _IFTextViewer_InstanceData IFTextViewer_InstanceData;

IFTextViewer_InstanceData* IFTextViewer_GetInstanceData();

BOOL IFTextViewer_IsInit(IFTextViewer_InstanceData* This);

void IFTextViewer_CreateMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam);

void IFTextViewer_DestroyInstanceData(IFTextViewer_InstanceData* This, HWND hwnd);


typedef RETC IFTextViewer_MessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam);


RETC IFTextViewer_SizeMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam);

RETC IFTextViewer_PaintMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam);

RETC IFTextViewer_VScrollMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam);

RETC IFTextViewer_HScrollMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam);

RETC IFTextViewer_MousewheelMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam);

RETC IFTextViewer_KeyDownMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam);

RETC IFTextViewer_CommandMessageHandler(IFTextViewer_InstanceData* This, HWND hwnd, WPARAM wParam, LPARAM lParam);
#endif // __IF_TEXT_VIEWER_H_
