#-------------------------------------------------
#
# Project created by QtCreator 2018-11-13T15:00:32
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RetroArchOverlayEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
INCLUDEPATH += libretro-common/include

SOURCES += \
    about.cpp \
    main.cpp \
    mainwindow.cpp \
    overlaydisplay.cpp \
    overlayeditor.cpp \
    libretro-common/compat/compat_posix_string.c \
    libretro-common/compat/compat_strcasestr.c \
    libretro-common/compat/compat_strl.c \
    libretro-common/compat/fopen_utf8.c \
    libretro-common/encodings/encoding_utf.c \
    libretro-common/file/config_file.c \
    libretro-common/file/file_path.c \
    libretro-common/lists/string_list.c \
    libretro-common/memmap/memmap.c \
    libretro-common/streams/file_stream.c \
    libretro-common/vfs/vfs_implementation.c

HEADERS += \
    about.h \
    libretro-common/include/compat/msvc/stdint.h \
    libretro-common/include/compat/fopen_utf8.h \
    libretro-common/include/compat/msvc.h \
    libretro-common/include/compat/posix_string.h \
    libretro-common/include/compat/strcasestr.h \
    libretro-common/include/compat/strl.h \
    libretro-common/include/encodings/utf.h \
    libretro-common/include/file/config_file.h \
    libretro-common/include/file/file_path.h \
    libretro-common/include/lists/string_list.h \
    libretro-common/include/streams/file_stream.h \
    libretro-common/include/string/stdstring.h \
    libretro-common/include/vfs/vfs_implementation.h \
    libretro-common/include/boolean.h \
    libretro-common/include/libretro.h \
    libretro-common/include/memmap.h \
    libretro-common/include/retro_assert.h \
    libretro-common/include/retro_common_api.h \
    libretro-common/include/retro_inline.h \
    libretro-common/include/retro_miscellaneous.h \
    mainwindow.h \
    overlaydisplay.h \
    overlayeditor.h

FORMS += \
    about.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
