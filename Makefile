#
# Makefile para Linux e macOS
#

SOURCE  = main.c quadtree.c lib/SOIL/image_DXT.c lib/SOIL/image_helper.c lib/SOIL/SOIL.c lib/SOIL/stb_image_aug.c
OBJECTS = $(SOURCE:.c=.o)

PROG = quadtree

UNAME = `uname`

all: $(TARGET)
	-@make $(UNAME)

Darwin: $(OBJECTS)
	gcc $(OBJECTS) -Wno-deprecated -framework OpenGL -framework GLUT -lm -o $(PROG)

Linux: $(OBJECTS)
	gcc $(OBJECTS) -lGL -lGLU -lglut -lm -o $(PROG)

clean:
	-@ rm -f $(OBJECTS) $(PROG)
