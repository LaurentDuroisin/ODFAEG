include(${PROJECT_SOURCE_DIR}/cmake/Macros.cmake)

set(INCROOT ${PROJECT_SOURCE_DIR}/include/odfaeg/Window)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/odfaeg/Window)

set(SRC_WINDOW
	${INCROOT}/sdlWindow.hpp
	${SRCROOT}/sdlWindow.cpp
	${INCROOT}/x11Window.hpp
	${SRCROOT}/x11Window.cpp)

include_directories(${CMAKE_INCLUDE_PATH} ${FREETYPE_INCLUDE_DIRS})
link_directories(${CMAKE_LIBRARY_PATH})

sfgl_add_library(odfaeg-window
                 SOURCES ${SRC_WINDOW} 
				 DEPENDS odfaeg-core odfaeg-math)
target_link_libraries (odfaeg-window ${SFML_LIBRARIES})

	
	
