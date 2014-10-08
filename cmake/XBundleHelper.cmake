macro(add_gui_executable exe_name icon_file app_name app_version app_ident app_vendor app_string)
	set(${exe_name}_SRCS ${ARGN})
	if (APPLE)
		set(${exe_name}_TGT ${app_name})
	else()
		set(${exe_name}_TGT ${exe_name})
	endif()
	
	add_executable("${${exe_name}_TGT}" MACOSX_BUNDLE ${${exe_name}_SRCS})
	INSTALL(TARGETS "${${exe_name}_TGT}"
	    	BUNDLE DESTINATION . COMPONENT Runtime
	    	RUNTIME DESTINATION bin COMPONENT Runtime
   	)
	if(APPLE)
		set( MACOSX_BUNDLE_INFO_STRING ${app_string} )
		set( MACOSX_BUNDLE_BUNDLE_VERSION ${app_version} )
		set( MACOSX_BUNDLE_GUI_IDENTIFIER ${app_ident} )
		set( MACOSX_BUNDLE_BUNDLE_NAME ${app_name} )
		set( MACOSX_BUNDLE_ICON_FILE ${icon_file} )
		add_custom_target( ${exe_name}_bundle_dirs
			COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${${exe_name}_TGT}.app/Contents/Resources"
			COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${${exe_name}_TGT}.app/Contents/MacOS"
			COMMAND cp ${CMAKE_SOURCE_DIR}/pixmaps/${MACOSX_BUNDLE_ICON_FILE}
				"${CMAKE_CURRENT_BINARY_DIR}/${${exe_name}_TGT}.app/Contents/Resources/${MACOSX_BUNDLE_ICON_FILE}"
	# qt aplikacie
    #COMMAND cp *.qm ${CMAKE_CURRENT_BINARY_DIR}/ProjectName.app/Contents/Resources/
		)
		add_dependencies( "${${exe_name}_TGT}" ${exe_name}_bundle_dirs )
		# This tells cmake where to place files inside the bundle
		set("${${exe_name}_TGT}_MACOSX_RSRCS" ${CMAKE_CURRENT_SRC_DIR}/pixmaps/${icon_file})
		set_source_files_properties(${${${exe_name}_TGT}_MACOSX_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		list (APPEND ${exe_name}_SRCS ${${${exe_name}_TGT}_MACOSX_RESOURCES})
		# set how it shows up in the Info.plist file
		SET(APPS "${CMAKE_CURRENT_BINARY_DIR}/${${exe_name}_TGT}.app")
		SET(DIRS "\${wxWidgets_LIBRARY_DIRS}")
		# set where in the bundle to put the icns file

		INSTALL(CODE "
		include(BundleUtilities)
		fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")
		" COMPONENT Runtime)
	ELSE(APPLE)
		INSTALL(TARGETS ${${exe_name}_TGT} RUNTIME DESTINATION bin)
	ENDIF(APPLE)
endmacro()
	

macro(gui_target_link_libraries target)
	target_link_libraries(${${target}_TGT} ${ARGN})
endmacro()