#include "windows.h"
#include "stdio.h"
#include "stdbool.h"

HHOOK keyboard_hook;

bool left_win_down = false;
bool right_win_down = false;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode != HC_ACTION )
        return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
    
    KBDLLHOOKSTRUCT* input_event = (KBDLLHOOKSTRUCT*)lParam;
    
    // save state of win keys manually... (needs to be tested some more)
    
    if(wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
    {
        bool log = false;
        bool capture = false;
        
        if(input_event->vkCode == VK_LWIN)
        {
            capture = true;
            if(left_win_down == false)
            {
                left_win_down = true;
                log = true;
            }
        }
        else if(input_event->vkCode == VK_RWIN)
        {
            capture = true;
            if(right_win_down == false)
            {
                right_win_down = true;
                log = true;
            }
        }
        
        if(log)
        {
            int key = (input_event->scanCode << 16) + (input_event->flags << 24);
            char key_name[256];
            GetKeyNameText(key, key_name, 100);
            printf("You pressed %s!\n", key_name); 
        }
        
        if(capture)
            return 1;
    }
    
    if(wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
    {
        if(input_event->vkCode == VK_LWIN)
            left_win_down = false;
        else if(input_event->vkCode == VK_RWIN)
            right_win_down = false;
    }
    return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

int main()
{
    HINSTANCE hInstance = GetModuleHandle(0);
    keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    
    MSG msg;
    BOOL bRet;
    
    while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
    { 
        if (bRet == -1)
        {
            printf("unknown error\n");
            exit(-1);
        }
        else
        {
            TranslateMessage(&msg); 
            DispatchMessage(&msg);
        } 
    } 
    return msg.wParam;
}