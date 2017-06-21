TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    $$PWD/include

SOURCES += \
    $$PWD/glad.c \
    $$PWD/nanovg/nanovg.c \
    $$PWD/demo.c \
    $$PWD/main.c

HEADERS += \
    $$PWD/include/glad/glad.h \
    $$PWD/nanovg/nanovg.h \
    $$PWD/nanovg/nanovg_gl.h \
    $$PWD/nanovg/nanovg_gl_utils.h \
    $$PWD/nanovg/fontstash.h \
    $$PWD/nanovg/stb_image.h \
    $$PWD/nanovg/stb_truetype.h \
    $$PWD/stb_image_write.h \
    $$PWD/demo.h

LIBS += glfw3.lib user32.lib gdi32.lib shell32.lib
#LIBS += glfw3.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib
