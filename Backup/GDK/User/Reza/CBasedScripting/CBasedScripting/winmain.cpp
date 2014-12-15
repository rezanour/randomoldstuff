#include "StdAfx.h"
#include "Scripting.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    Operand o;
    OperandInit(&o);

    o.type = OperandType_Int;

    o.value = FromInt(32);
    int64_t i = ToInt(o.value);

    o.value = FromReal(42.123);
    real_t r = ToReal(o.value);

    o.value = FromBool(TRUE);
    BOOL b = ToBool(o.value);

    char message[100] = {0};
    sprintf_s(message, "i=%d, r=%f, b=%d\n", i, r, b);
    OutputDebugStringA(message);

    return 0;
}
