# TronLeague - Simulación Gráfica e Interactiva con OpenGL

**TronLeague** es un juego y simulación gráfica 3D interactiva desarrollada en **C++** utilizando la API de **OpenGL** (Core Profile). Combina la icónica moto *Tron Light Cycle* con la dinámica de juego vehicular inspirada en *Rocket League*, incluyendo físicas de conducción, colisión elástica con un balón, iluminación Phong, sombras dinámicas mediante *Shadow Mapping*, *Skybox* cúbico y una **interfaz gráfica moderna integrada con Dear ImGui**.

El proyecto es totalmente **multiplataforma** (Windows, Linux y macOS), utilizando **CMake**, **CMakePresets** y gestión declarativa de dependencias con **vcpkg**.

---

## Características Principales

- **API Gráfica**: OpenGL (Core Profile 3.3 / 4.0) con soporte multiplataforma.
- **Interfaz Gráfica Integrada (Dear ImGui)**:
  - **Marcador Superior de Goles**: HUD centrado que contabiliza y muestra en tiempo real los goles anotados de forma persistente.
  - **Aviso de Gol**: Banner festivo animado (*¡¡¡GOOOOL!!!*) con saque de centro automático tras cada anotación sin reiniciar la aplicación.
  - **Panel de Controles y Telemetría**: Ventana translúcida con la guía de controles, velocímetro en tiempo real y monitor de FPS.
  - **Estética Tron**: Tema visual oscuro con bordes y acentos cian/neón.
- **Sombreadores (GLSL 400)**: Shaders modulares con subrutinas (`recordDepth` y `shadeWithShadow`).
- **Iluminación y Sombras**: Iluminación Phong direccional combinada con mapas de sombras (*Shadow Mapping*) con filtrado PCF 3x3 y bias adaptativo.
- **Entorno Skybox**: Mapeo cúbico (*Cubemap*) 360° en espacio infinito.
- **Físicas y Colisiones Continuas**:
  - Control de moto con aceleración, marcha atrás, desaceleración por fricción y giro.
  - Dinámica del balón con gravedad, rebotes en muros/suelo y transferencia de momento al colisionar con la moto.
  - Detección de gol y reposicionamiento instantáneo del balón y el jugador.
- **Cámara en 3ª Persona**: Seguimiento suave con órbita mediante ratón y cambio de objetivo (*Target Lock*) entre la moto y el balón.
- **Gestor de Dependencias**: `vcpkg` en modo manifiesto (`vcpkg.json`) para configuración automatizada y reproducible (`glfw3`, `glm`, `glew`, `stb`, `imgui`).
- **Sistema de Construcción**: CMake moderno con perfiles `CMakePresets.json` y objetivo `run` para compilación y ejecución directa en un solo comando.

---

## Requisitos Previos

1. **vcpkg**: Gestor de paquetes configurado con la variable de entorno `VCPKG_ROOT`.
2. **CMake**: Versión 3.20 o superior.
3. **Compilador C++17**:
   - **Windows**: Visual Studio 2022 (MSVC) o superior con soporte C++ y CMake.
   - **Linux**: GCC 9+ o Clang 10+ con soporte OpenGL y librerías de desarrollo (`pkg-config`, `libgl1-mesa-dev`, `libglu1-mesa-dev`, `libx11-dev`, `libxrandr-dev`, `libxi-dev`, `libxinerama-dev`, `libxcursor-dev`, `libxmu-dev`, `xorg-dev`).
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
  export VCPKG_ROOT=$HOME/vcpkg
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

#### Windows
```powershell
# Configurar
cmake --preset win-debug

# Compilar y Ejecutar directamente
cmake --build --preset win-debug --target run
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

```
TronLeague/
├── CMakeLists.txt              # Configuración global de CMake y target 'run'
├── CMakePresets.json           # Presets de CMake para Windows, Linux y macOS
├── vcpkg.json                  # Manifiesto de dependencias (glfw3, glm, glew, stb, imgui)
├── README.md                   # Documentación principal del proyecto
├── .gitignore                  # Exclusiones de build, caché y binarios
└── TronLeague/                 # Subproyecto del juego
    ├── CMakeLists.txt          # Configuración del ejecutable, dependencias y copiado de assets
    ├── main.cpp                # Punto de entrada de la aplicación
    ├── stb_image_impl.cpp      # Unidad de traducción para stb_image
    ├── Core/                   # Lógica principal y control de la aplicación
    │   ├── CGApplication.h/.cpp # Gestión de ventana GLFW, ImGui y bucle principal
    │   ├── CGCamera.h/.cpp      # Cámara orbital en 3ª persona y seguimiento
    │   ├── CGScene.h/.cpp       # Escena con terreno, muros, portería, moto y luces
    │   ├── CGSkybox.h/.cpp      # Cubemap de fondo espacial
    │   └── CGModel.h/.cpp       # Controlador del juego, marcador, físicas e interfaz HUD
    ├── Render/                 # Infraestructura de sombreadores y materiales
    │   ├── CGShaderProgram.h/.cpp # Compilación, enlace y uniforms de shaders GLSL
    │   ├── CGMaterial.h/.cpp    # Materiales Phong y carga de texturas con stb_image
    │   └── CGLight.h/.cpp       # Luz direccional y parámetros de iluminación
    ├── Geometry/               # Primitivas geométricas (VAO/VBOs)
    │   ├── CGFigure.h/.cpp      # Clase base abstracta para figuras geométricas
    │   ├── CGCube.h/.cpp        # Geometría de muros
    │   ├── CGSphere.h/.cpp      # Geometría del balón
    │   ├── CGGround.h/.cpp      # Geometría del suelo con repetición de textura
    │   └── Porteria.h/.cpp      # Geometría de la portería
    ├── Objects/                # Objetos jerárquicos y mallas compuestas
    │   ├── CGObject.h/.cpp      # Clase base abstracta para objetos compuestos
    │   ├── CGPiece.h/.cpp       # Pieza individual con VAO/VBO y matriz de modelo
    │   ├── TronLightCycle.h/.cpp # Ensamblado de la moto Tron Light Cycle
    │   └── TronLightCycle_pieces.h/.cpp # Datos de vértices e índices de la moto
    ├── shaders/                # Código fuente de los shaders GLSL
    │   ├── VertexShader.glsl        # Vertex shader principal con soporte de sombras
    │   ├── FragmentShader.glsl      # Fragment shader con iluminación Phong, PCF y subrutinas
    │   ├── SkyboxVertexShader.glsl  # Vertex shader para el skybox
    │   └── SkyboxFragmentShader.glsl# Fragment shader para el cubemap del skybox
    └── textures/               # Texturas del juego (JPG/PNG)
```
