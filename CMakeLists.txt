CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
SET(CMAKE_CXX_STANDARD 14)
PROJECT(memorypool)
ADD_EXECUTABLE(${PROJECT_NAME} main.cpp MemoryPool.cpp rbtree.cpp
        Logger.cpp
        Logger.h)
#ADD_EXECUTABLE(rbtree rbtree.cpp)
