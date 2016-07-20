
PROJECT(bataille)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#add definitions, compiler switches, etc.
#ADD_DEFINITIONS(-Wall -O0 -g) #debug
ADD_DEFINITIONS(-Wall -O3 -Wall) #release
#ADD_DEFINITIONS(-g -DNDEBUG -Wall -O0 -ffast-math -march=athlon-tbird -mtune=athlon-tbird -mfpmath=sse -msse -mmmx)
#ADD_DEFINITIONS(-Wall -O3 -ffast-math -march=athlon-tbird -mtune=athlon-tbird -mfpmath=sse -msse)
#ADD_DEFINITIONS(-Wall -O3 -ffast-math -march=athlon-tbird -mtune=athlon-tbird -mfpmath=sse -msse -g)

#INCLUDE(FindSDL)

#MESSAGE(STATUS SDL_FOUND=${SDL_FOUND})
#IF(${SDL_FOUND} MATCHES "YES")
#  MESSAGE(STATUS SDL_INCLUDE_DIR=${SDL_INCLUDE_DIR})
#  MESSAGE(STATUS SDL_LIBRARY=${SDL_LIBRARY})
#  INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
#ENDIF(${SDL_FOUND} MATCHES "YES")

#list all source files here
AUX_SOURCE_DIRECTORY(src SRC_FILES_CPP)
ADD_EXECUTABLE(bataille ${SRC_FILES_CPP})

#need to link to some other libraries ? just add them here
#TARGET_LINK_LIBRARIES(mastermind pthread)
#TARGET_LINK_LIBRARIES(labyrinthe png jpeg)
#TARGET_LINK_LIBRARIES(mastermind ${SDL_LIBRARY})
 
