#include "Platform.h"
#include "Scripting.h"

BOOL IsEmpty(Operand* operand)
{
    return (operand && operand->type == OperandType_Empty);
}

BOOL IsLiteral(Operand* operand)
{
    return (operand && 
        (operand->type == OperandType_Bool ||
         operand->type == OperandType_Int ||
         operand->type == OperandType_Real));
}

BOOL IsIndex(Operand* operand)
{
    return (operand && 
        (operand->type == OperandType_Param ||
         operand->type == OperandType_Local ||
         operand->type == OperandType_Global));
}

void OperandInit(Operand* operand)
{
    if (operand)
    {
        operand->type = OperandType_Empty;
        operand->value = 0;
    }
}

value_t FromBool(BOOL value)
{
    return *(value_t*)&value;
}

BOOL ToBool(value_t value)
{
    return *(BOOL*)&value;
}

value_t FromInt(int64_t value)
{
    return *(value_t*)&value;
}

int64_t ToInt(value_t value)
{
    return *(int64_t*)&value;
}

value_t FromReal(real_t value)
{
    return *(value_t*)&value;
}

real_t ToReal(value_t value)
{
    return *(real_t*)&value;
}
