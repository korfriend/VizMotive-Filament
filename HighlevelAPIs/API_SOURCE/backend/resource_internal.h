#ifndef RES_INTERNAL_APP_H_
#define RES_INTERNAL_APP_H_

#include <stdint.h>

extern "C" {
    extern const uint8_t FILAMENTAPP_PACKAGE[];
    extern int FILAMENTAPP_AIDEFAULTMAT_OFFSET;
    extern int FILAMENTAPP_AIDEFAULTMAT_SIZE;
    extern int FILAMENTAPP_AIDEFAULTTRANS_OFFSET;
    extern int FILAMENTAPP_AIDEFAULTTRANS_SIZE;
    extern int FILAMENTAPP_DEPTHVISUALIZER_OFFSET;
    extern int FILAMENTAPP_DEPTHVISUALIZER_SIZE;
    extern int FILAMENTAPP_TRANSPARENTCOLOR_OFFSET;
    extern int FILAMENTAPP_TRANSPARENTCOLOR_SIZE;
}
#define FILAMENTAPP_AIDEFAULTMAT_DATA (FILAMENTAPP_PACKAGE + FILAMENTAPP_AIDEFAULTMAT_OFFSET)
#define FILAMENTAPP_AIDEFAULTTRANS_DATA (FILAMENTAPP_PACKAGE + FILAMENTAPP_AIDEFAULTTRANS_OFFSET)
#define FILAMENTAPP_DEPTHVISUALIZER_DATA (FILAMENTAPP_PACKAGE + FILAMENTAPP_DEPTHVISUALIZER_OFFSET)
#define FILAMENTAPP_TRANSPARENTCOLOR_DATA (FILAMENTAPP_PACKAGE + FILAMENTAPP_TRANSPARENTCOLOR_OFFSET)

#endif
