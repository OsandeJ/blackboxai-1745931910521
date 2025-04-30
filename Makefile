# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Diretórios
SRC_DIR = src
GUI_DIR = $(SRC_DIR)/gui
BACKEND_DIR = $(SRC_DIR)/backend
BUILD_DIR = build

# Arquivos fonte
SOURCES = main.c \
          $(BACKEND_DIR)/backend.c \
          $(GUI_DIR)/gui.c \
          $(GUI_DIR)/views/produtos_view.c \
          $(GUI_DIR)/views/clientes_view.c \
          $(GUI_DIR)/views/pedidos_view.c \
          $(GUI_DIR)/widgets/custom_button.c \
          $(GUI_DIR)/widgets/custom_dialog.c \
          $(GUI_DIR)/widgets/custom_header.c

# Objetos
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

# Nome do executável
TARGET = sistema_gestao

# Regras
all: $(BUILD_DIR)/$(TARGET) copy-assets

copy-assets:
	@mkdir -p $(BUILD_DIR)/src/gui/styles
	cp $(SRC_DIR)/gui/styles/theme.css $(BUILD_DIR)/src/gui/styles/

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

.PHONY: all clean run copy-assets

# Dependências
-include $(OBJECTS:.o=.d)

# Gerar arquivos de dependência
$(BUILD_DIR)/%.d: %.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MM -MT '$(@:.d=.o)' $< > $@

# Instalar dependências (Ubuntu/Debian)
deps:
	sudo apt-get update
	sudo apt-get install -y build-essential pkg-config libgtk-3-dev

# Criar diretórios necessários
setup:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)
	mkdir -p $(BUILD_DIR)/$(GUI_DIR)/views
	mkdir -p $(BUILD_DIR)/$(GUI_DIR)/widgets
	mkdir -p $(BUILD_DIR)/$(BACKEND_DIR)
