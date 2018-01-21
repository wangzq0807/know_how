CC = gcc
CFLAGS = -g -O0 -fPIC

LDFLAGS = -lglib-2.0 -lgobject-2.0
CFLAGS += -I ./ -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/

objects = main.o bird_object.o animal_object.o
CFLAGS += $(DEFS)

# 目标
## $^ : 表示所有的依赖文件
test_gobject : $(objects)
	$(CC) $(LDFLAGS) -o test_gobject $^

# 静态模式
## $< : 表示第一个依赖文件
## $@ : 表示目标文件
$(objects) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm test_gobject $(objects)
