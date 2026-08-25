# TronLeague - Simulación Gráfica e Interactiva con OpenGL

**TronLeague** es un juego y simulación gráfica 3D interactiva desarrollada en **C++** utilizando la API de **OpenGL** (Core Profile). Combina la icónica moto *Tron Light Cycle* con la dinámica de juego vehicular inspirada en *Rocket League*, incluyendo físicas de conducción, colisión elástica con un balón, iluminación Phong, sombras dinámicas mediante *Shadow Mapping* y un *Skybox* cúbico.

El proyecto es totalmente **multiplataforma** (Windows, Linux y macOS), utilizando **CMake**, **CMakePresets** y gestión declarativa de dependencias con **vcpkg**.

---

## Características Principales

- **API Gráfica**: OpenGL (Core Profile 3.3 / 4.0) con soporte multiplataforma.
- **Sombreadores (GLSL 400)**: Shaders modulares con subrutinas (`recordDepth` y `shadeWithShadow`).
- **Iluminación y Sombras**: Iluminación Phong direccional combinada con mapas de sombras (*Shadow Mapping*) basados en Framebuffer Objects (FBO).
- **Entorno Skybox**: Mapeo cúbico (*Cubemap*) 360° en espacio infinito.
- **Físicas y Colisiones**:
  - Control de moto con aceleración, marcha atrás, desaceleración por fricción y giro.
  - Dinámica del balón con gravedad, rebotes en muros/suelo y transferencia de momento al colisionar con la moto.
  - Detección de gol al introducir el balón en la portería.
- **Cámara en 3ª Persona**: Seguimiento suave con órbita mediante ratón y cambio de objetivo (*Target Lock*) entre la moto y el balón.
- **Gestor de Dependencias**: `vcpkg` en modo manifiesto (`vcpkg.json`) para configuración automatizada y reproducible.
- **Sistema de Construcción**: CMake moderno con perfiles `CMakePresets.json` y objetivo `run` para compilación y ejecución directa en un solo comando.
- **Carga de Texturas Portátil**: Uso de `stb_image` integrado, eliminando dependencias de librerías legacy como FreeImage.

---

## Requisitos Previos

1. **vcpkg**: Gestor de paquetes configurado con la variable de entorno `VCPKG_ROOT`.
   - *Historial de vcpkg actualizado* (compatible con baseline moderno).
2. **CMake**: Versión 3.20 o superior.
3. **Compilador C++17**:
   - **Windows**: Visual Studio 2022 (MSVC) o superior con soporte C++ y CMake.
   - **Linux**: GCC 9+ o Clang 10+ con soporte OpenGL y librerías de desarrollo (`libgl1-mesa-dev`, `libx11-dev`, `libxrandr-dev`, `libxi-dev`, `libxinerama-dev`, `libxcursor-dev`).
   - **macOS**: Clang / Xcode Command Line Tools.
4. **Hardware**: Tarjeta gráfica compatible con OpenGL 3.3 o superior.

---

## Instrucciones de Compilación y Ejecución

### 1. Configuración Inicial (Solo la primera vez)

Define la ruta hacia tu instalación de `vcpkg`:

- **Windows (PowerShell)**:
  ```powershell
  $env:VCPKG_ROOT = "C:\ruta\a\vcpkg"
  ```
- **Linux / macOS (Bash o Zsh)**:
  ```bash
  export VCPKG_ROOT=/ruta/a/vcpkg
  ```

---

### 2. Comandos por Plataforma

#### Linux
```bash
# Configurar
cmake --preset linux-debug

# Compilar y Ejecutar directamente
cmake --build --preset linux-debug --target run
```

Para compilación en modo optimizado (*Release*):
```bash
cmake --preset linux-release
cmake --build --preset linux-release --target run
```

#### Windows
```powershell
# Configurar
cmake --preset win-debug

# Compilar y Ejecutar directamente
cmake --build --preset win-debug --target run
```

Para compilación en modo optimizado (*Release*):
```powershell
cmake --preset win-release
cmake --build --preset win-release --target run
```

#### macOS
```bash
# Configurar
cmake --preset macos-debug

# Compilar y Ejecutar directamente
cmake --build --preset macos-debug --target run
```

---

## Controles del Juego

| Tecla / Entrada | Acción |
| :--- | :--- |
| **`S`** | Acelerar moto hacia adelante |
| **`W`** | Frenar / Marcha atrás |
| **`A`** | Girar moto a la izquierda |
| **`D`** | Girar moto a la derecha |
| **Ratón** | Rotar orientación y órbita de la cámara |
| **`C`** | Alternar objetivo de la cámara (Moto $\leftrightarrow$ Balón) |
| **`F12`** | Alternar modo pantalla completa / ventana |
| **`ESC`** | Salir del juego |

---

## Estructura del Proyecto

El código fuente está modularizado en subcarpetas temáticas que se integran automáticamente en el sistema de construcción:

```
TronLeague/
├── CMakeLists.txt              # Configuración global de CMake y target 'run'
├── CMakePresets.json           # Presets de CMake para Windows, Linux y macOS
├── vcpkg.json                  # Manifiesto de dependencias (glfw3, glm, glew, stb)
├── README.md                   # Documentación principal del proyecto
├── .gitignore                  # Exclusiones de build, caché y binarios
└── TronLeague/                 # Subproyecto del juego
    ├── CMakeLists.txt          # Configuración del ejecutable, dependencias y copiado de assets
    ├── main.cpp                # Punto de entrada de la aplicación
    ├── stb_image_impl.cpp      # Unidad de traducción para stb_image
    ├── Core/                   # Lógica principal y control de la aplicación
    │   ├── CGApplication.h/.cpp # Gestión de ventana GLFW, callbacks y bucle de render
    │   ├── CGCamera.h/.cpp      # Cámara orbital en 3ª persona y seguimiento
    │   ├── CGScene.h/.cpp       # Escena con terreno, muros, portería, moto y luces
    │   ├── CGSkybox.h/.cpp      # Cubemap de fondo espacial
    │   └── CGModel.h/.cpp       # Controlador del juego: físicas, colisiones y ShadowMap
    ├── Render/                 # Infraestructura de sombreadores y materiales
    │   ├── CGShaderProgram.h/.cpp # Compilación, enlace y uniforms de shaders GLSL
    │   ├── CGMaterial.h/.cpp    # Materiales Phong y carga de texturas con stb_image
    │   └── CGLight.h/.cpp       # Luz direccional y parámetros de iluminación
    ├── Geometry/               # Primitivas geométricas (VAO/VBOs)
    │   ├── CGFigure.h/.cpp      # Clase base abstracta para figuras geométricas
    │   ├── CGCube.h/.cpp        # Geometría de cubos/paralelepípedos (muros del campo)
    │   ├── CGSphere.h/.cpp      # Geometría esférica paramétrica (balón)
    │   ├── CGGround.h/.cpp      # Geometría del suelo con repetición de textura
    │   └── Porteria.h/.cpp      # Geometría de la portería
    ├── Objects/                # Objetos jerárquicos y mallas compuestas
    │   ├── CGObject.h/.cpp      # Clase base abstracta para objetos compuestos
    │   ├── CGPiece.h/.cpp       # Pieza individual con VAO/VBO y matriz de modelo
    │   ├── TronLightCycle.h/.cpp # Ensamblado de la moto Tron Light Cycle
    │   └── TronLightCycle_pieces.h/.cpp # Datos de vértices e índices de la moto
    ├── shaders/                # Código fuente de los shaders GLSL
    │   ├── VertexShader.glsl        # Vertex shader principal con soporte de sombras
    │   ├── FragmentShader.glsl      # Fragment shader con iluminación Phong y subrutinas
    │   ├── SkyboxVertexShader.glsl  # Vertex shader para el skybox
    │   └── SkyboxFragmentShader.glsl# Fragment shader para el cubemap del skybox
    └── textures/               # Texturas del juego (JPG/PNG)
```

---

## Solución de Problemas Frecuentes

### 1. `VCPKG_ROOT` no está definido
- **Causa**: CMake no sabe dónde encontrar la instalación de `vcpkg`.
- **Solución**: Define la variable de entorno `VCPKG_ROOT` antes de ejecutar CMake (`export VCPKG_ROOT=/ruta/a/vcpkg` en Linux/macOS o `$env:VCPKG_ROOT="C:\vcpkg"` en Windows).

### 2. Error al inicializar GLEW o el contexto OpenGL
- **Causa**: Controladores gráficos desactualizados o entorno sin aceleración OpenGL 3.3+.
- **Solución**: Asegúrate de tener instalados los drivers propietarios o controladores Mesa actualizados de tu tarjeta gráfica.

### 3. Faltan librerías de desarrollo en Linux (X11 / OpenGL)
- En distribuciones basadas en Debian/Ubuntu:
  ```bash
  sudo apt-get install build-essential cmake ninja-build libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxinerama-dev libxcursor-dev
  ```

---

## Licencia y Créditos

Proyecto desarrollado como parte de la asignatura de **Informática Gráfica / Computer Graphics**.
- **Modelos y Texturas**: Recursos adaptados para la simulación interactiva Tron.
- **Librerías**: GLFW, GLEW, GLM, stb_image.
