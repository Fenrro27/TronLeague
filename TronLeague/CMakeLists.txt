cmake_minimum_required(VERSION 3.20)
project(TronLeague LANGUAGES CXX)

# Dependencias del sistema y vcpkg
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_path(STB_INCLUDE_DIR "stb_image.h")

if(NOT STB_INCLUDE_DIR)
    message(FATAL_ERROR "No se encontró stb_image.h. Asegúrate de instalar stb mediante vcpkg.")
endif()

# Recopilar todos los archivos fuente
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "*.cpp" "*.h")

# Crear el ejecutable principal
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Directorios de inclusión (rutas planas para resolver headers en subcarpetas)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Render
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry
    ${CMAKE_CURRENT_SOURCE_DIR}/Objects
    ${STB_INCLUDE_DIR}
)

# Enlazar librerías
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
    glm::glm
    imgui::imgui
)

# POST-BUILD: Copiar shaders y texturas junto al binario
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures"
    
    # Copiar shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
        
    # Copiar texturas
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/textures" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures"
        
    COMMENT "Desplegando shaders y texturas al directorio del ejecutable..."
)

# Ocultar terminal de consola en compilaciones Release en Windows
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
        LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
        LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
endif()
