#================================
# Define sources
#================================
SET( IBIS_ITK_NONRIGID_REGISTRATION_SRC
    itkCannyEdgeDetectionImageFilter2.hxx  
    itkGradientOrientationAlignmentCostFunction.hxx  
    itkVelocityFieldTransformCustom.hxx
    itkCombinationGOACostFunction.hxx      
    itkRescaledGaussianGradientImageFilter.hxx
    itkGOAGradientAscent.hxx               
    itkRescaledImageFilter.hxx
    itkSymbaRegistrationMethod.hxx
    itkVectorLinearInterpolateImageFunction2.hxx    
    vnl_sd_matrix_tools.hxx    
    itkGradientOrientationDifferenceFilter.hxx    
)

SET( IBIS_ITK_NONRIGID_REGISTRATION_HDR
    itkCannyEdgeDetectionImageFilter2.h  
    itkGradientOrientationAlignmentCostFunction.h  
    itkVelocityFieldTransformCustom.h
    itkCombinationGOACostFunction.h      
    itkRescaledGaussianGradientImageFilter.h
    itkGOAGradientAscent.h               
    itkRescaledImageFilter.h
    itkSymbaRegistrationMethod.h
    itkVectorLinearInterpolateImageFunction2.h    
    vnl_sd_matrix_tools.h   
    itkGradientOrientationDifferenceFilter.h
)

#================================
# Define include dir for
# dependent projects
#================================
# SET( ITK_NONRIGID_REGISTRATION_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Include directory for itkNonRigidRegistration .h files" )
# MARK_AS_ADVANCED( ITK_NONRIGID_REGISTRATION_INCLUDE_DIR )

#================================
# Define output
#================================
IF (BUILD_SHARED_LIBS)
   ADD_LIBRARY( itkNonRigidRegistration SHARED ${IBIS_ITK_NONRIGID_REGISTRATION_SRC} ${IBIS_ITK_NONRIGID_REGISTRATION_HDR} )
ELSE( BUILD_SHARED_LIBS )
   ADD_LIBRARY( itkNonRigidRegistration ${IBIS_ITK_NONRIGID_REGISTRATION_SRC} ${IBIS_ITK_NONRIGID_REGISTRATION_HDR} )
ENDIF( BUILD_SHARED_LIBS )

target_link_libraries( itkNonRigidRegistration ${ITK_LIBRARIES} )

#================================
# Define include dir for
# dependent projects
#================================
target_include_directories( itkNonRigidRegistration PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )
set_property(TARGET itkNonRigidRegistration PROPERTY POSITION_INDEPENDENT_CODE ON)