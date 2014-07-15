all:
	@make -C modules
	@make -C core 

clean:
	@make -C core clean
	@make -C modules clean

