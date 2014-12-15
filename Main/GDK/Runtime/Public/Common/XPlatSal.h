#pragma once

#ifdef WIN32

#include <sal.h>

#else

#define _Inout_
#define _Post_null_
#define _In_
#define _In_z_
#define _In_opt_z_
#define _In_opt_
#define _Outptr_
#define _Outptr_result_maybenull_
#define _Outptr_result_nullonfailure_
#define _Out_
#define _Printf_format_string_
#define _COM_Outptr_
#define _Inout_updates_to_(a, b)
#define _In_opt_z_count_(a)
#define _Use_decl_annotations_

#endif