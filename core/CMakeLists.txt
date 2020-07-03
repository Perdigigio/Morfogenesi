add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

add_executable(Morfogenesi.Main
	main.cpp
	imgui_impl_glfw.cpp
	imgui_impl_opengl3.cpp)

add_library(imgui STATIC
	${CMAKE_SOURCE_DIR}/ext/imgui/imgui.cpp
	${CMAKE_SOURCE_DIR}/ext/imgui/imgui_draw.cpp
	${CMAKE_SOURCE_DIR}/ext/imgui/imgui_widgets.cpp)

target_link_libraries(Morfogenesi.Main glfw)
target_link_libraries(Morfogenesi.Main glad)
target_link_libraries(Morfogenesi.Main imgui)

target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../ext/imgui")