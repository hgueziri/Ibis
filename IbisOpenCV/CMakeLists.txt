
SET( VTKOPENCV_SRC
	vtkopencvbridge.cpp
        ibisopencvmath.cpp
)

SET( VTKOPENCV_HDR
	vtkopencvbridge.h
        ibisopencvmath.h
)

#================================
# Define output
#================================
IF (BUILD_SHARED_LIBS)
ADD_LIBRARY( ibisVTKOpenCVBridge SHARED ${VTKOPENCVS_SRC} ${VTKOPENCV_HDR} )
ELSE( BUILD_SHARED_LIBS )
ADD_LIBRARY( ibisVTKOpenCVBridge ${VTKOPENCV_SRC} ${VTKOPENCV_HDR} )
ENDIF( BUILD_SHARED_LIBS )

target_link_libraries( ibisVTKOpenCVBridge svl vtkExtensions )
target_include_directories( ibisVTKOpenCVBridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS} )
