#include "VzJniOnload.h" 


#include "private/backend/VirtualMachineEnv.h"

namespace vzm{

    void JNI_Onload(JavaVM* vm); {
      ::filament::VirtualMachineEnv::JNI_OnLoad(vm);
    }

}