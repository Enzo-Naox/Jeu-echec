#include "Echiquier.h"
#include "Menu.h"

int main()
{
    // Création de la fenêtre
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Echiquier");

    Menu menu(window);

    menu.MenuDraw();

    return 0;
}