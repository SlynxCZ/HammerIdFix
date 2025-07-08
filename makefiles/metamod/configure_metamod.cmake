if (WIN32)
	set(HammerIdFix_VDF_PLATFORM "win64")
else()
	set(HammerIdFix_VDF_PLATFORM "linuxsteamrt64")
endif()

configure_file(
		${CMAKE_CURRENT_LIST_DIR}/DemoVoiceFix.vdf.in
	${PROJECT_SOURCE_DIR}/configs/addons/metamod/DemoVoiceFix.vdf
)
configure_file(
		${CMAKE_CURRENT_LIST_DIR}/HammerIdFix.vdf.in
		${PROJECT_SOURCE_DIR}/configs/addons/metamod/HammerIdFix.vdf
)