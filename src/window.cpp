#include "window.h"

Window::Window(T_Update updateFunction, T_Create createFunction, uint32_t w, uint32_t h, uint32_t px, uint32_t py, bool fullscreen, bool vsync):
    updateFunction(updateFunction), createFunction(createFunction) {
        sAppName = "Application Name";

        if(Construct(w, h, px, py, fullscreen, vsync)){
			Start();
		}
    }

bool Window::OnUserCreate() {
    DebugPoint::pge = this; // give debugger pge access
    return createFunction(this);
}

bool Window::OnUserUpdate(float elapsedTime)  { return updateFunction(this, elapsedTime); }

bool Window::OnUserDestroy() {
    DebugPoint::pge = nullptr;
    return true;
}