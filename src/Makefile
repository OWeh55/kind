# Makefile for kind, the backup tool
# 

OBJECTS = kind.o filetools.o FileName.o DateTime.o stringtools.o KindConfig.o Lexer.o rulecomp.o expiretools.o Image.o

LOPT := $(OPT) 
#-static
COPT := $(OPT) -std=c++11
LIBS := 

COPT := $(COPT) -g -Wall  -fno-strict-aliasing 

# source code beautifier
ASTYLE := astyle
# format options for beautifier
ASTYLE_OPT = --style=gnu --unpad-paren --pad-header --pad-oper --indent-namespaces --indent=spaces=2 --convert-tabs --align-pointer=type --align-reference=type --remove-brackets

%.o:%.c
	$(CC) $(COPT) $(INCLUDE) -c $*.c

%.o:%.cpp
	$(CXX) $(COPT) $(INCLUDE) -c $*.cpp

%: %.o
	$(CXX) $(COPT) $(LOPT) $(LIBRARY) -o $* $*.o $(LIBS)

%:%.cpp
	$(CXX) $(COPT) $(INCLUDE) -c $*.cpp
	$(CXX) $(LIBRARY) $(LOPT) -o $* $*.o $(LIBS)

%.cpp:%.ag
	AppGen $*.ag $*.cpp

kind: $(OBJECTS)
	$(CXX) $(LOPT) $(LIBRARY) -o kind $(OBJECTS) $(LIBS)

static: kind
	$(CXX) $(LOPT) $(LIBRARY) -static -o kind_static $(OBJECTS) $(LIBS)
	strip kind_static
	cp kind_static ../bin/kind

clean:
	-rm -f *.o depend *~ *orig

dep:	kind.cpp
	$(CXX) -M $(COPT) $(INCLUDE) *.cpp > depend	

format:
	$(ASTYLE) $(ASTYLE_OPT) *.ag *.h *.cpp

-include depend
