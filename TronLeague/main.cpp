#include "CGApplication.h"
#include <iostream>
#include <cstdlib>
#include <exception>

//
// PROYECTO: TronLeague
// 
// DESCRIPCIÓN: Aplicación gráfica interactiva en OpenGL con físicas, shaders e iluminación
//
int main()
{
    char opc = 'n';
    do {
        CGApplication app;

        try
        {
            app.run();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Excepción no controlada: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "¿Quieres jugar otra vez? (Y/N): ";
        if (!(std::cin >> opc)) {
            break;
        }
    } while (opc == 'y' || opc == 'Y');

    return EXIT_SUCCESS;
}
