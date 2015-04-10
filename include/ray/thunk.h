#ifndef _H_THUNK_H_
#define _H_THUNK_H_

#include <Windows.h>

class Thunk
{
    #pragma pack(push, 1)

    typedef	unsigned char	BYTE;
    typedef	unsigned long	DWORD;

    typedef	struct tagTHUNK_THISCALL
    {
	    BYTE	Mov;	// 0xB9
	    DWORD	This;	// this pointer
	    BYTE	Jmp;	// 0xE9
	    DWORD	Addr;	// relative jmp
    }THUNK_THISCALL, *PTHUNK_THISCALL;

    typedef struct tagTHUNK_STDCALL
    {
	    BYTE	Push[3]; // push dword ptr[esp] ;
	    DWORD	Mov;	 // mov dword ptr [esp + 4], this;
	    DWORD	This;    // this pointer
	    BYTE	Jmp;     // 0xE9
	    DWORD	Addr;    // relative jmp

    }THUNK_STDCALL, *PTHUNK_STDCALL;

    #pragma pack(pop)

public:
    Thunk()
    {
        pThunk = (Thunk *)VirtualAlloc(nullptr, sizeof(Thunk), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    }

    virtual	~Thunk()
    {
        if (pThunk)
        {
            VirtualFree(pThunk, 0, MEM_RELEASE);
        }
    }

    template<typename T>
    static DWORD getMemberAddress(T Member)
    {
        union
        {
            T		From;
            DWORD	To;
        }union_cast;

        union_cast.From = Member;

        return union_cast.To;
    }

    template<typename T>
    WNDPROC	ThisCall(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, PVOID window, T MainProc)
    {
        _thunk->_this_call.Mov = 0xB9;
        _thunk->_this_call.This= (DWORD)window;
        _thunk->_this_call.Jmp = 0xE9;
        _thunk->_this_call.Addr = getMemberAddress(MainProc) - (DWORD)_thunk->_this_call - sizeof(THUNK_THISCALL);

        ::FlushInstructionCache(getCurrentProcess(),_thunk, sizeof(Thunk));

        return	(WNDPROC)&_thunk->_this_call;
    }

    template<typename T>
    WNDPROC	StdCall(PVOID window,T MainProc)
    {
        _thunk->_std_call.Push[0] = 0xFF;
        _thunk->_std_call.Push[1] = 0x34;
        _thunk->_std_call.Push[2] = 0x24;
        _thunk->_std_call.Mov     = 0x042444C7;
        _thunk->_std_call.This    = (DWORD)window;
        _thunk->_std_call.Jmp     = 0xE9;
        _thunk->_std_call.Addr    = getMemberAddress(MainProc) - (DWORD)&_thunk->_std_call - sizeof(THUNK_STDCALL);

        ::FlushInstructionCache(GetCurrentProcess(),_thunk, sizeof(Thunk));

        return	(WNDPROC)&_thunk->_std_call;
    }

private:
    THUNK_THISCALL	_this_call;
    THUNK_STDCALL	_std_call;
    Thunk			*_thunk;
};

#endif