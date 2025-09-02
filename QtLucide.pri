# QtLucide.pri - qmake integration file
# Include this file in your qmake project to use QtLucide

QT += core gui widgets svg

INCLUDEPATH += $$PWD/include

# Link against QtLucide library
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/lib -lQtLucided
} else {
    LIBS += -L$$PWD/lib -lQtLucide
}

# Add headers
HEADERS += \
    $$PWD/include/QtLucide/QtLucide.h \
    $$PWD/include/QtLucide/QtLucideIconEngine.h \
    $$PWD/include/QtLucide/QtLucideIconPainter.h \
    $$PWD/include/QtLucide/QtLucideEnums.h \
    $$PWD/include/QtLucide/QtLucideStrings.h
