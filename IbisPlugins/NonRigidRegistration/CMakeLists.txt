# define sources
set( PluginSrc 
	nonRigidRegistrationplugininterface.cpp 
	nonRigidRegistrationwidget.cpp 
	)

set( PluginHdrMoc 
	nonRigidRegistrationwidget.h
	nonRigidRegistrationplugininterface.h
	qdebugstream.h 
	)

set( PluginUi nonRigidRegistrationwidget.ui )

add_subdirectory( itkNonRigidRegistration )

# Create plugin
DefinePlugin( "${PluginSrc}" "${PluginHdr}" "${PluginHdrMoc}" "${PluginUi}" )

#add library specific to this plugin
target_link_libraries( ${PluginName} itkNonRigidRegistration )