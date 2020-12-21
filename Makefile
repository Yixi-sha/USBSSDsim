.PHONY : all clean rebuild  compile  link test 

TYPE := cpp
CC := g++

ifeq ($(DEBUG),true)
FLAGS_CC += -g
FLAGS_LD += -g
endif

Moudles := base
Target := app.out


Build_dir := build

Moudle_libs := $(addsuffix .a, $(Moudles))
Moudle_libs := $(addprefix $(Build_dir)/, $(Moudle_libs))

Moudle_dirs := $(addprefix $(Build_dir)/, $(Moudles))
Dirs := $(Build_dir) $(Moudle_dirs)
FLAGS_LD += -lusb-1.0 -lpthread
FLAGS_CC += -lusb-1.0

all : $(Target)

$(Target) : compile link 

link : $(Moudle_libs) $(Build_dir)/main.o
	$(CC) -o $(Target) -Xlinker "-(" $^  -Xlinker "-)" $(FLAGS_LD)
$(Build_dir)/main.o : main.$(TYPE)
	$(CC) ${FLAGS_CC} -o  $@ -c  $^

compile : $(Build_dir) $(Moudle_dirs)
	set -e ; \
	for dir in $(Moudles) ; \
	do \
		cd $$dir && \
		$(MAKE) all Moudles:=$${dir} DEBUG:=$(DEBUG) && \
		cd .. ; \
	done


$(Dirs) :
	mkdir $@

test:
	@echo "Moudle_dirs => $(Moudle_dirs)"

clean :
	rm *.out $(Build_dir) -rf 

rebuild : clean all