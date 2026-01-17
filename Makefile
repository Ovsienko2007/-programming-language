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

DIRSOURCETOTREE = to_tree
CFLAGSH         = -I$(DIRSOURCETOTREE)/Include
SOURCES         = $(wildcard $(DIRSOURCETOTREE)/$(DIRSOURCE)/*.cpp)
OBJECTS         = $(patsubst %.cpp, %.o, $(SOURCES))

DUMP_DIR        = DUMP

EXECUTABLE_NAME = to_tree.exe
DIR_BUILD       = Build

all: to_tree proc to_asm

to_tree: $(EXECUTABLE_NAME)

.PHONY = all clean start

$(EXECUTABLE_NAME): make_folder  $(OBJECTS)
	echo $(OBJECTS)
	$(CC) $(addprefix ./$(DIR_BUILD)/, $(OBJECTS)) -o ./$(DIR_BUILD)/$(EXECUTABLE_NAME)

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) $(CFLAGSH) -c $^ -o ./$(DIR_BUILD)/$@

start_to_tree: 
	./$(DIR_BUILD)/$(EXECUTABLE_NAME) -s expr.txt

DIRSOURCEASM = calculator

EXECUTABLE_NAME_SPU     = SPU.exe
EXECUTABLE_NAME_ASM     = ASM.exe

SOURCEDIR      = Source
INCLUDEDIR     = Include

BUILDDIR       = Build

READFILEDIR    = $(DIRSOURCEASM)/ReadFile
SOURCES_READ   = $(wildcard $(READFILEDIR)/*.cpp)
OBJECTS_READ   = $(patsubst %.cpp, $(BUILDDIR)/%.o, $(SOURCES_READ))

SPUDIR        = $(DIRSOURCEASM)/SPU
STACKDIR      = Stack
SOURCES_SPU   = $(wildcard $(SPUDIR)/*.cpp) $(SPUDIR)/$(STACKDIR)/stack.cpp 
OBJECTS_SPU   = $(patsubst %.cpp, $(BUILDDIR)/%.o, $(SOURCES_SPU))
CCFLAGSH_SPU = -I$(SPUDIR)/$(STACKDIR) -DSHOW_DUMP

ASMDIR        = $(DIRSOURCEASM)/ASM
SOURCES_ASM   = $(wildcard $(ASMDIR)/$(SOURCEDIR)/*.cpp)
OBJECTS_ASM   = $(patsubst %.cpp, $(BUILDDIR)/%.o, $(SOURCES_ASM))
INCLUDE_ASM   = $(ASMDIR)/$(INCLUDEDIR)
CCFLAGSH_ASM = -I$(READFILEDIR) -I$(INCLUDE_ASM)

proc: make_folder $(OBJECTS_READ) $(EXECUTABLE_NAME_ASM) $(EXECUTABLE_NAME_SPU)

#____________________________READ_______________________________
$(BUILDDIR)/$(READFILEDIR)/%.o: $(READFILEDIR)/%.cpp
	$(CC) $(CFLAGS) $(CCFLAGSH_ASM) -c $< -o $@

#____________________________SPU________________________________
$(EXECUTABLE_NAME_SPU): $(OBJECTS_SPU)
	$(CC) $(OBJECTS_SPU) -o $(BUILDDIR)/$@

$(BUILDDIR)/$(SPUDIR)/%.o: $(SPUDIR)/%.cpp
	$(CC) $(CFLAGS) $(CCFLAGSH_SPU) -c $< -o $@

$(BUILDDIR)/$(SPUDIR)/$(STACKDIR)/%.o: $(SPUDIR)/$(STACKDIR)/%.cpp
	$(CC) $(CFLAGS) $(CCFLAGSH_SPU) -c $< -o $@

#____________________________ASM________________________________
$(EXECUTABLE_NAME_ASM): $(OBJECTS_ASM) $(OBJECTS_READ)
	$(CC) $(OBJECTS_ASM) $(OBJECTS_READ) -o $(BUILDDIR)/$@

$(BUILDDIR)/$(ASMDIR)/$(SOURCEDIR)/%.o: $(ASMDIR)/$(SOURCEDIR)/%.cpp
	$(CC) $(CFLAGS) $(CCFLAGSH_ASM) -c $< -o $@

DIRSOURCE_TO_ASM       = to_asm/Source

CFLAGSH_TO_ASM         = -Ito_asm/Include
SOURCES_TO_ASM         = $(wildcard $(DIRSOURCE_TO_ASM)/*.cpp)
OBJECTS_TO_ASM         = $(patsubst %.cpp, %.o, $(SOURCES_TO_ASM))

DUMP_DIR_TO_ASM        = DUMP

EXECUTABLE_NAME_TO_ASM = to_asm.exe
DIR_BUILD       = Build

to_asm: $(EXECUTABLE_NAME_TO_ASM)


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

#________________________STARTS_________________________________
start_spu:
	./$(BUILDDIR)/$(EXECUTABLE_NAME_SPU)

start_asm:
	./$(BUILDDIR)/$(EXECUTABLE_NAME_ASM) result_asm.asm

start:
	./$(DIR_BUILD)/$(EXECUTABLE_NAME) -s expr.txt
	./$(DIR_BUILD)/$(EXECUTABLE_NAME_TO_ASM) -s result_tree.txt
	./$(BUILDDIR)/$(EXECUTABLE_NAME_ASM) result_asm.asm
	./$(BUILDDIR)/$(EXECUTABLE_NAME_SPU)

start_to_asm:
	valgrind ./$(DIR_BUILD)/$(EXECUTABLE_NAME_TO_ASM) -s result_tree.txt
	@for file_dot in $(wildcard $(DUMP_DIR_TO_ASM)/*.dot); do \
		dot -Tpng $$file_dot -o $${file_dot%.dot}.png; \
	done

#_____________________CREAT FOLDERS_____________________________
make_folder:
	mkdir -p $(BUILDDIR)/$(READFILEDIR)
	mkdir -p $(BUILDDIR)/$(SPUDIR)/$(STACKDIR)
	mkdir -p $(BUILDDIR)/$(ASMDIR)/$(SOURCEDIR)
	mkdir -p $(DIR_BUILD)/$(DIRSOURCETOTREE)/$(DIRSOURCE)/
	mkdir -p $(DIR_BUILD)/$(DIRSOURCE_TO_ASM)/

	mkdir -p $(DUMP_DIR)/
	@echo "Folders were created!"

clean:
	rm -rf ./$(DIR_BUILD)/
	rm -rf ./$(DUMP_DIR)/