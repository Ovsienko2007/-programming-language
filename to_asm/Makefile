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

DIRSOURCE_TO_ASM       = Source

CFLAGSH_TO_ASM         = -IInclude
SOURCES_TO_ASM         = $(wildcard $(DIRSOURCE_TO_ASM)/*.cpp)
OBJECTS_TO_ASM         = $(patsubst %.cpp, %.o, $(SOURCES_TO_ASM))

DUMP_DIR_TO_ASM        = DUMP

EXECUTABLE_NAME_TO_ASM = to_asm.exe
DIR_BUILD       = Build

all: $(EXECUTABLE_NAME_TO_ASM)


.PHONY = all clean start

$(EXECUTABLE_NAME_TO_ASM): make_folder  $(OBJECTS_TO_ASM)
	echo $(OBJECTS_TO_ASM)
	$(CC) $(addprefix ./$(DIR_BUILD)/, $(OBJECTS_TO_ASM)) -o ./$(DIR_BUILD)/$(EXECUTABLE_NAME_TO_ASM)

$(OBJECTS_TO_ASM): %.o: %.cpp
	$(CC) $(CFLAGS) $(CFLAGSH_TO_ASM) -c $^ -o ./$(DIR_BUILD)/$@

dump:
	@for file_dot in $(wildcard $(DUMP_DIR_TO_ASM)/*.dot); do \
		dot -Tpng $$file_dot -o $${file_dot%.dot}.png; \
	done

make_folder:
	mkdir -p $(DIR_BUILD)/$(DIRSOURCE_TO_ASM)/
	mkdir -p $(DUMP_DIR_TO_ASM)/
	@echo "Folders were created!"


start:
	valgrind ./$(DIR_BUILD)/$(EXECUTABLE_NAME_TO_ASM) -s result_tree.txt
	@for file_dot in $(wildcard $(DUMP_DIR_TO_ASM)/*.dot); do \
		dot -Tpng $$file_dot -o $${file_dot%.dot}.png; \
	done

clean:
	rm -rf ./$(DIR_BUILD)/
	rm -rf ./$(DUMP_DIR_TO_ASM)/