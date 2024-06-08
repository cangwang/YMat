

#pragma once
#include <EGL/egl.h>
#include <GLES3/gl3.h>
//#include <GLES2/gl2.h>
#include <EGL/eglext.h>
#include <utils/ymlog.h>
#include <android/native_window.h>

/**
 * cangwang 2018.12.1
 */
typedef EGLBoolean (EGLAPIENTRYP EGL_PRESENTATION_TIME_ANDROIDPROC)(EGLDisplay display, EGLSurface surface, khronos_stime_nanoseconds_t time);
class EGLCore {
public:
    EGLCore();

    ~EGLCore();

    void start(ANativeWindow *window);

    //使用共享context，用于录制
    void start(EGLContext context, ANativeWindow *window);

    EGLConfig chooseConfig();

    EGLConfig chooseRecordConfig();

    EGLint* getAttributes();

    EGLContext createContext(EGLDisplay eglDisplay, EGLConfig eglConfig);

    //使用共享context，用于录制
    EGLContext createContext(EGLDisplay eglDisplay, EGLConfig eglConfig, EGLContext context);

    GLboolean buildContext(ANativeWindow *window);

    void setPresentationTime(uint64_t nsecs);

    void swapBuffer();

    void release();

    EGLContext getEglContext();

protected:

private:
    EGLDisplay mDisplay;
    EGLSurface mSurface;
    EGLContext mContext;
    EGL_PRESENTATION_TIME_ANDROIDPROC eglPresentationTimeANDROID = NULL;
};