# include the ODFAEG specific macros
include(${PROJECT_SOURCE_DIR}/cmake/Macros.cmake)

set(INCROOT ${PROJECT_SOURCE_DIR}/include/odfaeg/Sound)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/odfaeg/Sound)
set(SRC
	${INCROOT}/export.hpp
	${INCROOT}/player.h
	${SRCROOT}/player.cpp
	${INCROOT}/stream.h
	${SRCROOT}/stream.cpp)
	
include_directories(${CMAKE_INCLUDE_PATH})
link_directories (${CMAKE_LIBRARY_PATH})
find_package (SFML 2 REQUIRED)
sfgl_add_library(odfaeg-sound
				 SOURCES ${SRC}
				 DEPENDS sfgl-core)
target_link_libraries (odfaeg-sound ${SFML_LIBRARIES})

