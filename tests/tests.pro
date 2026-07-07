QT += core testlib
QT -= gui # Графика для тестов логики движка нам не нужна

TARGET = tst_enginetest
TEMPLATE = app
CONFIG += c++17 testcase

# Говорим компилятору, что нужно смотреть в папку app за заголовочными файлами
INCLUDEPATH += ../app

# Подключаем ресурсы игры (чтобы движок в тестах смог прочитать :/laws.txt)
# ЗАМЕЧАНИЕ: Замени "resources.qrc" на реальное имя твоего .qrc файла, если оно другое!
RESOURCES += ../app/resources.qrc

# Подключаем файлы движка из папки app для тестирования
HEADERS += \
    ../app/alcoholengine.h \
    ../app/itimeprovider.h \
    ../app/mocktimeprovider.h

SOURCES += \
    tst_enginetest.cpp \
    tst_enginetest.cpp \
    ../app/alcoholengine.cpp