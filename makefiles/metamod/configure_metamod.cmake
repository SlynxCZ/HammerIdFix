if (WIN32)
	set(PROJECT_VDF_PLATFORM "win64")
else()
	set(PROJECT_VDF_PLATFORM "linuxsteamrt64")
endif()

configure_file(
		${CMAKE_CURRENT_LIST_DIR}/HammerIdFix.vdf.in
		${PROJECT_SOURCE_DIR}/configs/addons/metamod/HammerIdFix.vdf
)