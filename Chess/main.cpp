#include "Echiquier.h"
#include "Menu.h"

int main()
{
    // Cr�ation de la fen�tre
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Echiquier");

    Menu menu(window);

    menu.MenuDraw();

    return 0;
}