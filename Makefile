objects= edit.o edit_func_node.o edit_func_operate.o

myvim:$(objects)
	cc -o myvim $(objects) -g

$(objects):%.o:%.c
	cc -c $< -o $@ -g

.PHONY:clean

clean:
	-rm myvim $(objects)