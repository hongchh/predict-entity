SRCS = src/DataStructure.cpp src/Util.cpp src/BTree.cpp src/BlockFile.cpp src/MedRank.cpp
OBJS = $(SRCS:.cpp=.o)
CP = g++

predict: $(OBJS)
	$(CP) -o predict $(OBJS) src/main.cpp

DataStructure.o: src/DataStructure.h src/DataStructure.cpp
	$(CP) -c src/DataStructure.cpp

Util.o: src/Util.h src/Util.cpp
	$(CP) -c src/Util.cpp

BlockFile.o: src/BlockFile.h src/BlockFile.cpp
	$(CP) -c src/BlockFile.cpp

BTree.o: src/BTree.h src/BTree.cpp
	$(CP) -c src/BTree.cpp

MedRank.o: src/MedRank.h src/MedRank.cpp
	$(CP) -c src/MedRank.cpp

clean:
	-rm $(OBJS) predict