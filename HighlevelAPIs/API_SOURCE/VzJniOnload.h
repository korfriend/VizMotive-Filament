
#pragma once
#include "VizComponentAPIs.h"

#include <jni.h>

namespace vzm
{
    // This must be called before using engine APIs
    extern "C" API_EXPORT void JNI_Onload(JavaVM* vm);
}