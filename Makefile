NAME = emu_nes.so

CC = gcc

CFLAGS = -std=c99 -Wall -Wextra -Ofast -pipe -g -fPIC -march=native

LD = ld

LDFLAGS = -shared -static-libgcc -Wl,--version-script=link.T -Wl,--no-undefined

SRCS_PATH = src/

SRCS_NAME = nes.c \
            apu.c \
            cpu.c \
            gpu.c \
            mem.c \
            mbc.c \
            cpu/instr.c \

LIBRETRO = 1

ifeq ($(LIBRETRO), 1)

SRCS_NAME+= libretro/libretro.c \

endif

SRCS = $(addprefix $(SRCS_PATH), $(SRCS_NAME))

OBJS_PATH = obj/

OBJS_NAME = $(SRCS_NAME:.c=.o)

OBJS = $(addprefix $(OBJS_PATH), $(OBJS_NAME))

all: $(NAME)

$(NAME): $(OBJS)
	@echo "LD $(NAME)"
	@$(CC) -o $(NAME) $(OBJS) $(LDFLAGS)

$(OBJS_PATH)%.o: $(SRCS_PATH)%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -o $@ -c $<

clean:
	@rm -f $(OBJS)
	@rm -f $(NAME)

.PHONY: all clean
