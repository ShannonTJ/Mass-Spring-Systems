CC=g++
OBJDIR=./obj
SRCDIR=./src

INCDIR=-I/usr/local/include -I/usr/include -I/usr/X11/inlcude -Iinclude -Imiddleware/glad/include
LIBDIR=-L/usr/X11R6/lib -L/usr/local/lib -L/usr/X11R6/lib64

CFLAGS=-c -std=c++0x -O3 -Wall
#LIBS=\
	 -lglfw3 \
	 -lGLEW \
	 -framework Cocoa \
	 -framework OpenGL \
	 -framework IOKit \
	-framework CoreVideo

LIBS = `pkg-config --libs glfw3 gl` -ldl

SOURCES=$(wildcard $(SRCDIR)/*cpp) 
OBJECTS=$(addprefix $(OBJDIR)/,$(notdir $(SOURCES:.cpp=.o)))

EXECUTABLE=A3

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) ./obj/glad.o
	$(CC) $(LINKFLAGS) $(OBJECTS) ./obj/glad.o -o $@ $(LIBS) $(LIBDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@ $(INCDIR)

# GLAD Specific Stuff
$(OBJDIR)/glad.o: middleware/glad/src/glad.c 
	$(CC) $(CFLAGS) $< -o $@ $(INCDIR)

clean:
	rm $(OBJDIR)/*.o $(EXECUTABLE)

