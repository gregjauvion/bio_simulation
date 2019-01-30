include(../defaults.pri)

TEMPLATE = app
CONFIG   -= app_bundle
CONFIG   -= qt

SOURCES += main.cpp \
    parsing_tests.cpp \
    modeling_tests.cpp \
    simulation_tests.cpp

LIBS += -L../src -lsim-engine

HEADERS +=
