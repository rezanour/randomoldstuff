#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_SCRIPTING_H_
#define _GDK_SCRIPTING_H_

#ifdef __cplusplus
extern "C" {
#endif

// storage for encoding values. We use a 64bit integer instead of void* 
// so that it remains constant across different build architectures
typedef uint64_t value_t;

// Real values are currently represented as doubles, but we may want to tweak this
typedef double real_t;


/*-------------------------------------------------------------------------------------------
    OperandType
    Identifies how the operand should be interpreted.
-------------------------------------------------------------------------------------------*/
typedef enum
{
    OperandType_Empty = 0,
    OperandType_Bool,
    OperandType_Int,
    OperandType_Real,
    OperandType_Param,
    OperandType_Local,
    OperandType_Global,
} OperandType;


/*-------------------------------------------------------------------------------------------
    Operand
    Holds the type and value of a single operand.
-------------------------------------------------------------------------------------------*/
typedef struct
{
    OperandType type;
    value_t     value;
} Operand;


/*-------------------------------------------------------------------------------------------
    Methods to operate on Operands
-------------------------------------------------------------------------------------------*/
BOOL IsEmpty(Operand* operand);
BOOL IsLiteral(Operand* operand);
BOOL IsIndex(Operand* operand);

void OperandInit(Operand* operand);

value_t FromBool(BOOL value);
BOOL ToBool(value_t value);

value_t FromInt(int64_t value);
int64_t ToInt(value_t value);

value_t FromReal(real_t value);
real_t ToReal(value_t value);


/*-------------------------------------------------------------------------------------------
    Methods to operate on the stack pointers.
-------------------------------------------------------------------------------------------*/
HRESULT StackInit(size_t maxStackElements, uint64_t** stackPointer);
HRESULT StackDestroy(uint64_t* stackPointer);

#ifdef __cplusplus
}
#endif

#endif // _GDK_SCRIPTING_H_
