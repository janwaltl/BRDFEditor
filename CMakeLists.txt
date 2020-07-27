# CMakeList.txt : CMake project for BRDF Editor, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
#set output 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#only compile the library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw-3.2.1)
#Make libraries(GLFW,ImGui,GLAD) available
include_directories(external/ glfw-3.2.1/include ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (BRDFEditor "src/main.cpp")

#Add GLFW library
target_link_libraries(BRDFEditor glfw)

set_property(TARGET BRDFEditor PROPERTY CXX_STANDARD 17)
set_property(TARGET BRDFEditor PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET BRDFEditor PROPERTY VS_STARTUP_PROJECT BRDFEditor)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


#######################    EXTERNAL   SOURCE FILES    #############################
set(CLOAD_SOURCES 
	"external/cload/cl.hpp"
	"external/cload/cl_platform.h"
	"external/cload/cload.h"
	"external/cload/cload.cpp"
)
source_group("external\\cload\\" FILES ${CLOAD_SOURCES})
target_sources(BRDFEditor PUBLIC ${CLOAD_SOURCES})

set(GLAD_SOURCES 
	"external/glad/glad.h"
	"external/glad/glad.c"
)
source_group("external\\glad\\" FILES ${GLAD_SOURCES})
target_sources(BRDFEditor PUBLIC ${GLAD_SOURCES})

set(STB_SOURCES 
	"external/stb/stb_image.h"
	"external/stb/stb_image.c"
)
source_group("external\\stb\\" FILES ${STB_SOURCES})
target_sources(BRDFEditor PUBLIC ${STB_SOURCES})

set(TINY_OBJ_LOADER_SOURCES 
	"external/tiny_obj_loader/tiny_obj_loader.h"
	"external/tiny_obj_loader/tiny_obj_loader.cc"
)
source_group("external\\tiny_obj_loader\\" FILES ${TINY_OBJ_LOADER_SOURCES})
target_sources(BRDFEditor PUBLIC ${TINY_OBJ_LOADER_SOURCES})

set(IMGUI_SOURCES 
	"external/ImGui/imconfig.h"
	"external/ImGui/imgui.h"
	"external/ImGui/imgui.cpp"
	"external/ImGui/imgui_draw.cpp"
	"external/ImGui/imgui_demo.cpp"
	"external/ImGui/imgui_widgets.cpp"
	"external/ImGui/imgui_internal.h"
	"external/ImGui/imstb_rectpack.h"
	"external/ImGui/imstb_textedit.h"
	"external/ImGui/imstb_truetype.h"
)
source_group("external\\ImGui\\" FILES ${IMGUI_SOURCES})
target_sources(BRDFEditor PUBLIC ${IMGUI_SOURCES})

add_subdirectory(external/json11)
target_link_libraries(BRDFEditor json11)

#######################    BRDFEditor SOURCE FILES    #############################
set(BRDFEditor_CORE_SOURCES 
	"src/Core/Exception.hpp"
	"src/Core/ImGuiManager.hpp"
	"src/Core/ImGuiManager.cpp"
	"src/Core/Input.hpp"
	"src/Core/Input.cpp"
	"src/Core/Program.hpp"
	"src/Core/Program.cpp"
	"src/Core/WindowManager.hpp"
	"src/Core/WindowContext.hpp"
	"src/Core/WindowManager.cpp")
source_group("src\\Core\\" FILES ${BRDFEditor_CORE_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_CORE_SOURCES})

set(BRDFEditor_GL_SOURCES 
	"src/GL/GLException.hpp"
	"src/GL/GLException.cpp"
	"src/GL/Shader.hpp"
	"src/GL/Shader.cpp"
	"src/GL/ShaderException.hpp"
	"src/GL/ShaderProgram.hpp"
	"src/GL/ShaderProgram.cpp")
source_group("src\\GL\\" FILES ${BRDFEditor_GL_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_GL_SOURCES})
 
 set(BRDFEditor_GUI_SOURCES 
	"src/GUI/Console.hpp"
	"src/GUI/Console.cpp"
	"src/GUI/CommonElements.hpp"
	"src/GUI/CommonElements.cpp"
	"src/GUI/GUI.hpp"
	"src/GUI/GUI.cpp"
	"src/GUI/GUIParams.hpp"
	"src/GUI/GUIParams.cpp"
	"src/GUI/SceneEvents.hpp"
	"src/GUI/SceneEvents.cpp"
	"src/GUI/TwoPanelLayout.hpp"
	"src/GUI/TwoPanelLayout.cpp"
	)
source_group("src\\GUI\\" FILES ${BRDFEditor_GUI_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_GUI_SOURCES})

 set(BRDFEditor_GUI_SCENEREN_SOURCES 
	"src/GUI/SceneRendering/Materials.hpp"
	"src/GUI/SceneRendering/Materials.cpp"
	"src/GUI/SceneRendering/SceneRendering.hpp"
	"src/GUI/SceneRendering/SceneRendering.cpp"
	"src/GUI/SceneRendering/SceneTools.hpp"
	"src/GUI/SceneRendering/SceneTools.cpp"
	"src/GUI/SceneRendering/SceneView.hpp"
	"src/GUI/SceneRendering/SceneView.cpp"
	"src/GUI/SceneRendering/RenderSettings.hpp"
	"src/GUI/SceneRendering/RenderSettings.cpp"
	)
source_group("src\\GUI\\SceneRendering\\" FILES ${BRDFEditor_GUI_SCENEREN_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_GUI_SCENEREN_SOURCES})

set(BRDFEditor_GUI_EDITING_SOURCES 
	"src/GUI/BRDFEditing/Editor.hpp"
	"src/GUI/BRDFEditing/Editor.cpp"
	"src/GUI/BRDFEditing/EditorParams.hpp"
	"src/GUI/BRDFEditing/EditorParams.cpp"
	"src/GUI/BRDFEditing/BRDFMatList.hpp"
	"src/GUI/BRDFEditing/BRDFMatList.cpp"
	"src/GUI/BRDFEditing/BRDFMatParams.hpp"
	"src/GUI/BRDFEditing/BRDFMatParams.cpp"
	"src/GUI/BRDFEditing/SourceEditor.hpp"
	"src/GUI/BRDFEditing/SourceEditor.cpp"
	)
source_group("src\\GUI\\BRDFEditing\\" FILES ${BRDFEditor_GUI_EDITING_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_GUI_EDITING_SOURCES})

set(BRDFEditor_GUI_GRAPHS_SOURCES 
	"src/GUI/Graphs/Graphs.hpp"
	"src/GUI/Graphs/Graphs.cpp"
	"src/GUI/Graphs/Graph.hpp"
	"src/GUI/Graphs/Graph.cpp"
	)
source_group("src\\GUI\\Graphs\\" FILES ${BRDFEditor_GUI_GRAPHS_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_GUI_GRAPHS_SOURCES})


set(BRDFEditor_OPENCL_RENDERER_SOURCES 
	"src/OpenCLRenderer/Camera.hpp"
	"src/OpenCLRenderer/Camera.cpp"
	"src/OpenCLRenderer/CLHeaders.hpp"
	"src/OpenCLRenderer/EnvMap.hpp"
	"src/OpenCLRenderer/EnvMap.cpp"
	"src/OpenCLRenderer/RendererExceptions.hpp"
	"src/OpenCLRenderer/Scene.hpp"
	"src/OpenCLRenderer/Scene.cpp"
	"src/OpenCLRenderer/SceneLoader.hpp"
	"src/OpenCLRenderer/SceneLoader.cpp"
	)
source_group("src\\OpenCLRenderer\\" FILES ${BRDFEditor_OPENCL_RENDERER_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_OPENCL_RENDERER_SOURCES})

set(BRDFEditor_OPENCL_RENDERER_RENDERER_SOURCES 
	"src/OpenCLRenderer/Renderer/DeviceInfo.hpp"
	"src/OpenCLRenderer/Renderer/PlatformInfo.hpp"
	"src/OpenCLRenderer/Renderer/Renderer.hpp"
	"src/OpenCLRenderer/Renderer/Renderer.cpp"
	)
source_group("src\\OpenCLRenderer\\Renderer" FILES ${BRDFEditor_OPENCL_RENDERER_RENDERER_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_OPENCL_RENDERER_RENDERER_SOURCES})

set(BRDFEditor_OPENCL_RENDERER_KERNEL_SOURCES 
	"src/OpenCLRenderer/Kernel/BRDF.hpp"
	"src/OpenCLRenderer/Kernel/BRDF.cpp"
	"src/OpenCLRenderer/Kernel/BRDFParams.h"
	"src/OpenCLRenderer/Kernel/BRDFParams.cpp"
	"src/OpenCLRenderer/Kernel/CustomKernel.hpp"
	"src/OpenCLRenderer/Kernel/CustomKernel.cpp"
	"src/OpenCLRenderer/Kernel/Kernel.hpp"
	"src/OpenCLRenderer/Kernel/Kernel.cpp"
	"src/OpenCLRenderer/Kernel/KernelSource.hpp"
	"src/OpenCLRenderer/Kernel/KernelSource.cpp"
	"src/OpenCLRenderer/Kernel/ToneMappingKernel.hpp"
	"src/OpenCLRenderer/Kernel/ToneMappingKernel.cpp"
	"src/OpenCLRenderer/Kernel/ToneMappingKernelSource.hpp"
	"src/OpenCLRenderer/Kernel/ToneMappingKernelSource.cpp"
	)
source_group("src\\OpenCLRenderer\\Kernel" FILES ${BRDFEditor_OPENCL_RENDERER_KERNEL_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_OPENCL_RENDERER_KERNEL_SOURCES})


set(BRDFEditor_OPENCL_RENDERER_OBJECTS_SOURCES 
	"src/OpenCLRenderer/Objects/BVH/BVH.hpp"
	"src/OpenCLRenderer/Objects/BVH/BVH.cpp"
	"src/OpenCLRenderer/Objects/Object.hpp"
	"src/OpenCLRenderer/Objects/Object.cpp"
	"src/OpenCLRenderer/Objects/Sphere.hpp"
	"src/OpenCLRenderer/Objects/Sphere.cpp"
	"src/OpenCLRenderer/Objects/Plane.hpp"
	"src/OpenCLRenderer/Objects/Plane.cpp"
	"src/OpenCLRenderer/Objects/Cuboid.hpp"
	"src/OpenCLRenderer/Objects/Cuboid.cpp"
	"src/OpenCLRenderer/Objects/TriangleMesh.hpp"
	"src/OpenCLRenderer/Objects/TriangleMesh.cpp")
source_group("src\\OpenCLRenderer\\Objects" FILES ${BRDFEditor_OPENCL_RENDERER_OBJECTS_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_OPENCL_RENDERER_OBJECTS_SOURCES})


set(BRDFEditor_OPENCL_RENDERER_MATERIALS_SOURCES 
	"src/OpenCLRenderer/Materials/Material.hpp"
	"src/OpenCLRenderer/Materials/Material.cpp"
	"src/OpenCLRenderer/Materials/MaterialsLibrary.hpp"
	"src/OpenCLRenderer/Materials/MaterialsLibrary.cpp")
source_group("src\\OpenCLRenderer\\Materials" FILES ${BRDFEditor_OPENCL_RENDERER_MATERIALS_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_OPENCL_RENDERER_MATERIALS_SOURCES})

set(BRDFEditor_OPENCL_RENDERER_LIGHTS_SOURCES 
	"src/OpenCLRenderer/Lights/Light.hpp"
	"src/OpenCLRenderer/Lights/Light.cpp"
	"src/OpenCLRenderer/Lights/AreaLight.hpp"
	"src/OpenCLRenderer/Lights/AreaLight.cpp"
	"src/OpenCLRenderer/Lights/PointLight.hpp"
	"src/OpenCLRenderer/Lights/PointLight.cpp"
	"src/OpenCLRenderer/Lights/SphereLight.hpp"
	"src/OpenCLRenderer/Lights/SphereLight.cpp")
source_group("src\\OpenCLRenderer\\Lights" FILES ${BRDFEditor_OPENCL_RENDERER_LIGHTS_SOURCES})
target_sources(BRDFEditor PUBLIC ${BRDFEditor_OPENCL_RENDERER_LIGHTS_SOURCES})
#######################    COPY RESOURCES TO BINARY    #############################

add_custom_command(TARGET BRDFEditor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:BRDFEditor>)