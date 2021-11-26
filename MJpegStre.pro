TEMPLATE = app
TARGET = MyVideoSurface

QT += widgets multimedia multimediawidgets

HEADERS += \
    MyVideoSurface.h \

SOURCES += \
    main.cpp \
    MyVideoSurface.cpp \

copyVideo.commands = $(COPY_DIR) $$PWD/cam7.avi $$OUT_PWD
QMAKE_EXTRA_TARGETS += copyVideo
PRE_TARGETDEPS += copyVideo
