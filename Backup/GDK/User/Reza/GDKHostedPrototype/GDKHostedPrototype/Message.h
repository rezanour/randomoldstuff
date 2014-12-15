#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _GDK_MESSAGE_H_
#define _GDK_MESSAGE_H_

////////////////////////////////////////////////////////////////////////////////
// messages and related interfaces and methods

namespace GDK
{
    enum MessageType
    {
        MessageType_System = 0,
        MessageType_Debug,
        MessageType_Screen,
        MessageType_Object,
    };

    enum MessageCategory
    {
        MessageCategory_Application = 0,
        MessageCategory_Debug,
        MessageCategory_Update,
        MessageCategory_Input,
        MessageCategory_Physics,
        MessageCategory_Collision,
        MessageCategory_Trigger,
    };

    // messages are used heavily, and so must be 
    // lightweight and prefer living/copying on the stack
    struct Message
    {
        MessageType     Type : 8;
        MessageCategory Category : 8;
        uint16_t        Subject;        // interpretation specific to Category
        uint64_t        Data;           // interpretation specific to Subject
    };
} // GDK

#endif // _GDK_MESSAGE_H_
