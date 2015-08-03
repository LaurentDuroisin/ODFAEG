# include the ODFAEG specific macros
include(${PROJECT_SOURCE_DIR}/cmake/Macros.cmake)

set(INCROOT ${PROJECT_SOURCE_DIR}/include/odfaeg/BoundingAreas)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/odfaeg/BoundingAreas)
set(SRC
	${INCROOT}/segment.h
	${SRCROOT}/segment.cpp
	${INCROOT}/boundingAreas.h
	${INCROOT}/boundingCircle.h
	${SRCROOT}/boundingCircle.cpp
	${INCROOT}/boundingEllipse.h
	${SRCROOT}/boundingEllipse.cpp
	${INCROOT}/boundingRectangle.h
	${SRCROOT}/boundingRectangle.cpp
	${INCROOT}/orientedBoundingRect.h
	${SRCROOT}/orientedBoundingRect.cpp
	${INCROOT}/boundingPolygon.h
	${SRCROOT}/boundingPolygon.cpp
	${INCROOT}/axisAlignedBoundingBox.h)
include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})
find_package (SFML 2 REQUIRED)
sfgl_add_library(sfgl-boundingAreas
				 SOURCES ${SRC}
				 DEPENDS sfgl-core sfgl-math)
target_link_libraries (sfgl-boundingAreas ${SFML_LIBRARIES})
	