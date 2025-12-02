CC = g++
CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef \
   -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
   -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ \
   -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion\
   -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2\
   -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers\
   -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo\
   -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits\
   -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE -DCOLOR_PRINT

DIRSOURCE       = Source
CFLAGSH         = -IInclude
SOURCES         = $(wildcard $(DIRSOURCE)/*.cpp)
OBJECTS         = $(patsubst %.cpp, %.o, $(SOURCES))

DUMP_DIR        = DUMP

EXECUTABLE_NAME = main.exe
DIR_BUILD       = Build

all: $(EXECUTABLE_NAME)


.PHONY = all clean start

$(EXECUTABLE_NAME): make_folder  $(OBJECTS)
	echo $(OBJECTS)
	$(CC) $(addprefix ./$(DIR_BUILD)/, $(OBJECTS)) -o ./$(DIR_BUILD)/$(EXECUTABLE_NAME)

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) $(CFLAGSH) -c $^ -o ./$(DIR_BUILD)/$@

make_folder:
	mkdir -p $(DIR_BUILD)/$(DIRSOURCE)/
	mkdir -p $(DUMP_DIR)/

dump:
	@for file_dot in $(wildcard $(DUMP_DIR)/*.dot); do \
		dot -Tpng $$file_dot -o $${file_dot%.dot}.png; \
	done

start:
	./$(DIR_BUILD)/$(EXECUTABLE_NAME)

clean:
	rm -rf ./$(DIR_BUILD)/
	rm -rf ./$(DUMP_DIR)/