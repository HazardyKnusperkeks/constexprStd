TEMPLATE	 = app
CONFIG		+= console c++1z strict_c++ testcase
INCLUDEPATH	+= include
QT			+= testlib

gcc {
	QMAKE_CFLAGS_DEBUG		*= -ggdb3
	QMAKE_CXXFLAGS_DEBUG	*= -ggdb3
	QMAKE_CXXFLAGS_RELEASE	-= -O2
	QMAKE_CXXFLAGS_RELEASE	*= -O3
	
	#Ab Qt 5.9 QT_GCC_*VERSION soll die Versionsnummer mit der Qt gebaut wurde angeben (scheint aber leer zu sein) und
	#die aktuelle Versionsnummer soll in QMAKE_GCC_*VERSION sein.
	#Es scheint jedoch, dass das verhalten (noch?) inkonsistent ist, nehme daher den Wert von QMAKE_GCC_*VERSION, falls
	#dieser nicht leer ist, ansonsten QT_GCC_*VERSION.
	isEmpty(QMAKE_GCC_MAJOR_VERSION) {
		ALG_GCC_MAJOR_VERSION = $$QT_GCC_MAJOR_VERSION
		ALG_GCC_MINOR_VERSION = $$QT_GCC_MINOR_VERSION
		ALG_GCC_PATCH_VERSION = $$QT_GCC_PATCH_VERSION
	} #isEmpty(QMAKE_GCC_MAJOR_VERSION)
	else {
		ALG_GCC_MAJOR_VERSION = $$QMAKE_GCC_MAJOR_VERSION
		ALG_GCC_MINOR_VERSION = $$QMAKE_GCC_MINOR_VERSION
		ALG_GCC_PATCH_VERSION = $$QMAKE_GCC_PATCH_VERSION
	} #else -> isEmpty(QMAKE_GCC_MAJOR_VERSION)
	isEmpty(ALG_GCC_MAJOR_VERSION):warning(Konnte GCC Version nicht ermitteln!)
}

gcc {
	extraWarnings = -pedantic -Wformat=2 -Wfloat-equal -Wconversion -Wzero-as-null-pointer-constant \
	                -Wsign-conversion -Wlogical-op -Wshadow
	greaterThan(ALG_GCC_MAJOR_VERSION, 5):extraWarnings += -Wnull-dereference -Wduplicated-cond -Wplacement-new=2
	greaterThan(ALG_GCC_MAJOR_VERSION, 6):extraWarnings += -Wdangling-else
	QMAKE_CFLAGS	*= $$extraWarnings
	QMAKE_CXXFLAGS	*= $$extraWarnings
	
	for ( modul, QT ) {
		modul = $$replace(modul,testlib,test)
		liste = $$split(modul,)
		erstesZeichen = $$take_first(liste)
		QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]/Qt$$upper($$erstesZeichen)$$join(liste,,,)
		QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS/src]/Qt$$upper($$erstesZeichen)$$join(liste,,,)
	}
}

SOURCES		 = test_main.cpp\
			   test_files/*.cpp

HEADERS		 = test.hpp\
			   include/constexprStd/*\
			   include/constexprStd/details/*\
			   test_files/test_constants.hpp\
			   test_files/test_container.hpp\
			   test_files/test_count_instances.hpp\
			   test_files/test_helper_functions.hpp\
			   test_files/test_move_int.hpp

OTHER_FILES	+= constexprStd.pri\
			   LICENSE\
			   README.md
