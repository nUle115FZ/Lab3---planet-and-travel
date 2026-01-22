QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Папки для временных файлов
MOC_DIR = build/moc
OBJECTS_DIR = build/obj
UI_DIR = build/ui
RCC_DIR = build/rcc

# Определяем пути
INCLUDEPATH += include \
               include/core \
               include/graph \
               include/game \
               include/ui

# Исходные файлы
SOURCES += \
    src/ui/main.cpp \
    src/ui/mainwindow.cpp \
    src/ui/graphview.cpp \
    src/ui/addplanetdialog.cpp \
    src/ui/addedgedialog.cpp \
    src/ui/piratebattle.cpp \
    src/graph/StarGraph.cpp \
    src/graph/Dijkstra.cpp \
    src/game/Trader.cpp \
    src/game/BlackHole.cpp

# Заголовочные файлы
HEADERS += \
    include/ui/mainwindow.h \
    include/ui/graphview.h \
    include/ui/addplanetdialog.h \
    include/ui/addedgedialog.h \
    include/ui/piratebattle.h \
    include/core/Sequence.h \
    include/core/DynamicArray.h \
    include/core/PriorityQueue.h \
    include/graph/StarGraph.h \
    include/graph/Dijkstra.h \
    include/game/Trader.h \
    include/game/BlackHole.h \
    include/ui/visual_effects.h

# UI формы
FORMS += \
    forms/mainwindow.ui \
    forms/addplanetdialog.ui \
    forms/addedgedialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
