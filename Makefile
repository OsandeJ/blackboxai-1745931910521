CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -g
LIBS = `pkg-config --libs gtk+-3.0`

# Diretórios
SRC_DIR = src
GUI_DIR = $(SRC_DIR)/gui
WIDGETS_DIR = $(GUI_DIR)/widgets
VIEWS_DIR = $(GUI_DIR)/views
BACKEND_DIR = $(SRC_DIR)/backend

# Arquivos fonte
SOURCES = main.c \
          $(GUI_DIR)/gui.c \
          $(WIDGETS_DIR)/custom_header.c \
          $(WIDGETS_DIR)/custom_button.c \
          $(WIDGETS_DIR)/custom_dialog.c \
          $(VIEWS_DIR)/produtos_view.c \
          $(VIEWS_DIR)/clientes_view.c \
          $(VIEWS_DIR)/pedidos_view.c \
          $(BACKEND_DIR)/backend.c

# Objetos
OBJECTS = $(SOURCES:.c=.o)

# Nome do executável
TARGET = sistema_gestao

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

# Dependências
main.o: main.c $(GUI_DIR)/gui.h $(BACKEND_DIR)/backend.h
$(GUI_DIR)/gui.o: $(GUI_DIR)/gui.c $(GUI_DIR)/gui.h $(WIDGETS_DIR)/custom_header.h $(WIDGETS_DIR)/custom_button.h $(WIDGETS_DIR)/custom_dialog.h
$(WIDGETS_DIR)/custom_header.o: $(WIDGETS_DIR)/custom_header.c $(WIDGETS_DIR)/custom_header.h
$(WIDGETS_DIR)/custom_button.o: $(WIDGETS_DIR)/custom_button.c $(WIDGETS_DIR)/custom_button.h
$(WIDGETS_DIR)/custom_dialog.o: $(WIDGETS_DIR)/custom_dialog.c $(WIDGETS_DIR)/custom_dialog.h
$(VIEWS_DIR)/produtos_view.o: $(VIEWS_DIR)/produtos_view.c $(VIEWS_DIR)/produtos_view.h
$(VIEWS_DIR)/clientes_view.o: $(VIEWS_DIR)/clientes_view.c $(VIEWS_DIR)/clientes_view.h
$(VIEWS_DIR)/pedidos_view.o: $(VIEWS_DIR)/pedidos_view.c $(VIEWS_DIR)/pedidos_view.h
$(BACKEND_DIR)/backend.o: $(BACKEND_DIR)/backend.c $(BACKEND_DIR)/backend.h
