#include "defines.h"

int _MyDebugPrintln(const char* str)
{
    //return printf("%s\n", str);
}

#include "IFTextViewer/IFTextViewer.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("IFTextViewer");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the window class */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;                /* This function is called by windows */
    wincl.style = CS_DBLCLKS | CS_CLASSDC | CS_OWNDC;   /* Catch double-clicks and request own DC*/
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilities for variation */
           szClassName,         /* Class name */
           _T("Interfaces: Text Viewer"),       /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VSCROLL, /* default window with scroll */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           lpszArgument         /* Argument string as Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    if (_CrtDumpMemoryLeaks()) Beep(200,200);

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static IFTextViewer_InstanceData* pInstanceData = NULL;
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            _MyDebugPrintln("WM_CREATE\n");
            pInstanceData = IFTextViewer_GetInstanceData();
            if (pInstanceData == NULL)
            {
                PostQuitMessage (0);
                break;
            }
            IFTextViewer_CreateMassageHandler(pInstanceData, hwnd, wParam, lParam);
            break;
        case WM_SIZE:
            _MyDebugPrintln("WM_SIZE\n");
            IFTextViewer_SizeMassageHandler(pInstanceData, wParam, lParam);
            break;
        case WM_PAINT:
            _MyDebugPrintln("WM_PAINT\n");
            IFTextViewer_PaintMassageHandler(pInstanceData, wParam, lParam);
            break;
        case WM_VSCROLL:
            _MyDebugPrintln("WM_VSCROLL\n");
            IFTextViewer_VScrollMassageHandler(pInstanceData, wParam, lParam);
            break;
        case WM_HSCROLL:
            _MyDebugPrintln("WM_HSCROLL\n");
            IFTextViewer_HScrollMassageHandler(pInstanceData, wParam, lParam);
            break;
        case WM_MOUSEWHEEL:
            _MyDebugPrintln("WM_MOUSEWHEEL\n");
            IFTextViewer_MousewheelMassageHandler(pInstanceData, wParam, lParam);
            break;
        case WM_KEYDOWN:
            _MyDebugPrintln("WM_KEYDOWN\n");
            IFTextViewer_KeyDownMassageHandler(pInstanceData, wParam, lParam);
            break;
        case WM_DESTROY:
            _MyDebugPrintln("WM_DESTROY\n");
            IFTextViewer_DestroyInstanceData(pInstanceData);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
