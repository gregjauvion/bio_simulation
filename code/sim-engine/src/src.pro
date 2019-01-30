include(../defaults.pri)
CONFIG -= qt

TARGET = sim-engine
TEMPLATE = lib

SOURCES += modeling/model.cpp \
    modeling/molecule.cpp \
    modeling/reaction.cpp \
    modeling/cell.cpp \
    simulation/simulation.cpp \
    parsing/func.cpp \
    parsing/parser.cpp \
    parsing/utils.cpp \
    modeling/growth.cpp \
    modeling/division.cpp \
    parsing/parameter.cpp \
    modeling/cell_factory.cpp \
    simulation/outputs.cpp \
    simulation/generator.cpp \
    simulation/stop_condition.cpp \
    simulation/settings.cpp \
    interface.cpp \
    simulation/lineage.cpp

HEADERS += \
    modeling/model.h \
    modeling/molecule.h \
    modeling/reaction.h \
    modeling/parameter.h \
    modeling/cell.h \
    simulation/simulation.h \
    parsing/func.h \
    parsing/parser.h \
    parsing/utils.h \
    modeling/growth.h \
    modeling/division.h \
    modeling/cell_factory.h \
    simulation/outputs.h \
    json.h \
    parsing/evaluable.h \
    simulation/random.h \
    simulation/generator.h \
    simulation/stop_condition.h \
    simulation/settings.h \
    simulation/lineage.h
