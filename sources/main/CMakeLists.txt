link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_executable(appmain appmain.cc)

target_link_libraries(appmain ${LIB_PREFIX}tatsy_pppm${LIB_POSTFIX})

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
  set_property(TARGET appmain APPEND PROPERTY LINK_FLAGS "/DEBUG /PROFILE")
  configure_file(../project.vcxproj.user.in "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/appmain.vcxproj.user" @ONLY)
endif()
