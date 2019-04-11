include(${PROJECT_SOURCE_DIR}/cmake/Macros.cmake)

set(INCROOT ${PROJECT_SOURCE_DIR}/include/odfaeg/Window)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/odfaeg/Window)

set(SRC_WINDOW
	${INCROOT}/sdlWindow.hpp
	${SRCROOT}/sdlWindow.cpp
	${INCROOT}/x11Window.hpp
	${SRCROOT}/x11Window.cpp
	${INCROOT}/glxContext.hpp
	${SRCROOT}/glxContext.cpp
	${INCROOT}/contextImpl.hpp
	${SRCROOT}/contextImpl.cpp	
	${INCROOT}/contextSettings.hpp
	${SRCROOT}/contextSettings.cpp
	${INCROOT}/display.hpp
	${SRCROOT}/display.cpp
	${INCROOT}/x11Keyboard.hpp
	${SRCROOT}/x11Keyboard.cpp
	${INCROOT}/x11Mouse.hpp
	${SRCROOT}/x11Mouse.cpp
	${INCROOT}/iKeyboard.hpp
	${SRCROOT}/iKeyboard.cpp
	${INCROOT}/iMouse.hpp
	${SRCROOT}/iMouse.cpp
	${INCROOT}/windowImpl.hpp
	${SRCROOT}/windowImpl.cpp
	${INCROOT}/iWindow.hpp
	${INCROOT}/iContext.hpp
	${INCROOT}/windowFactory.hpp
	${SRCROOT}/windowFactory.cpp
	${INCROOT}/contextFactory.hpp
	${SRCROOT}/contextFactory.cpp
	${INCROOT}/window.hpp
	${SRCROOT}/window.cpp
	${INCROOT}/context.hpp
	${SRCROOT}/context.cpp	
	${INCROOT}/iGlResource.hpp
	${INCROOT}/glResource.hpp
	${SRCROOT}/glResource.cpp
	${SRCROOT}/glxExtensions.hpp
	${SRCROOT}/glxExtensions.cpp	
	${INCROOT}/sfmlWindowImpl.hpp
	${SRCROOT}/sfmlWindowImpl.cpp
	${INCROOT}/sfmlKeyboard.hpp
	${SRCROOT}/sfmlKeyboard.cpp
	${INCROOT}/sfmlMouse.hpp
	${SRCROOT}/sfmlMouse.cpp
	${INCROOT}/sfmlContextImpl.hpp
	${SRCROOT}/sfmlContextImpl.cpp
)

include_directories(${CMAKE_INCLUDE_PATH} ${FREETYPE_INCLUDE_DIRS})
link_directories(${CMAKE_LIBRARY_PATH})

sfgl_add_library(odfaeg-window
                 SOURCES ${SRC_WINDOW} 
				 DEPENDS odfaeg-core odfaeg-math)
target_link_libraries (odfaeg-window ${SFML_LIBRARIES})

	
	
