### Specify your qt install location here
### Ex. /opt/qt/5.12.0/gcc_64
QT_DIR        = /opt/qt/5.12.1/gcc_64

CXX           = g++
UIC           = $(QT_DIR)/bin/uic
MOC           = $(QT_DIR)/bin/moc
BUILD_DIR     = ./build/
DEFINES       = -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
CXXFLAGS      = -pipe -O2 -std=gnu++11 -Wall -W -D_REENTRANT -fPIC $(DEFINES)
INCPATH       = -I. -I$(QT_DIR)/include -I$(QT_DIR)/include/QtWidgets -I$(QT_DIR)/include/QtGui -I$(QT_DIR)/include/QtCore
LFLAGS        = -Wl,-O1 -Wl,-rpath,$(QT_DIR)/lib
LIBS          = -L$(QT_DIR)/lib -lQt5Widgets -lQt5Gui -lQt5Core -lGL -lpthread
SOURCES       = main.cpp interface.cpp moc_interface.cpp
OBJECTS       = $(patsubst %.cpp, %.o, $(SOURCES))
EXEOJBS       = $(addprefix $(BUILD_DIR), $(OBJECTS))
UIHEADER      = ui_interface.h
MOCCXX        = moc_interface.cpp
TARGET        = Basic_image_processing

all: build_dir $(TARGET)

$(TARGET): $(UIHEADER) $(EXEOJBS)  
	$(CXX) $(LFLAGS) -o $(TARGET) $(EXEOJBS) $(LIBS)

$(BUILD_DIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c $< -o $@

$(MOCCXX): interface.h
	$(MOC) $(DEFINES) $(INCPATH) interface.h -o $@

$(UIHEADER): interface.ui
	$(UIC) interface.ui -o $@

build_dir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(MOCCXX) $(UIHEADER)

.PHONY: clean
