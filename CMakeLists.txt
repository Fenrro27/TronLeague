cmake_minimum_required(VERSION 3.20)

project(TronLeagueWorkspace CXX)

# Coherencia con el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################################################
# Rutas de Salida (Output Directories)
################################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin")

################################################################################
# Configuración del Subproyecto
################################################################################
add_subdirectory(TronLeague)

# --- FUNCIÓN PARA EJECUTAR DESDE TERMINAL ---
# Esto permite hacer: cmake --build --preset linux-debug --target run
add_custom_target(run
    COMMAND $<TARGET_FILE:TronLeague>
    DEPENDS TronLeague
    WORKING_DIRECTORY $<TARGET_FILE_DIR:TronLeague>
    COMMENT "Lanzando TronLeague (OpenGL)..."
)
