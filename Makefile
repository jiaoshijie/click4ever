CC := c++
TARGET := click4ever

SRC := ./src/main.cpp ./src/click.cpp ./src/time_utils.cpp
LIBS := -lxdo -lboost_program_options


build:
	$(CC) $(SRC) $(LIBS) -o $(TARGET)
