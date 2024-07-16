
#include "CGApplication.h"
#include <iostream>
#include <stdexcept>

//
// PROYECTO: Project6
// 
// DESCRIPCIÓN: Aplicación gráfica que introduce las texturas
//
int main()
{
    char opc;
    do {
        CGApplication app;

        try
        {
            app.run();
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Quiere jugar otra vez(Y/N)?" << std::endl;
        std::cin >> opc;
    } while (opc == 'y' || opc == 'Y');

    return EXIT_SUCCESS;
}