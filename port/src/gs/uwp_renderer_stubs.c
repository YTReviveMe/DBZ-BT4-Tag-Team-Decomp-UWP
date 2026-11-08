#ifdef BT3_UWP
#include "gs_draw.h"

GsBackend sGlBackend = {0};
GsBackend sDirect3D9Backend = {0};
void GsGl_Vsync(int enabled) { (void)enabled; }
#endif
