#pragma once
#include <SFML/Graphics.hpp>

#include "Echiquier.h"

class Echiquier;

enum class MenuState {
    MainMenu,
    ColorChoice
};

class Menu {
public:
    Menu(sf::RenderWindow& window);
    void DrawBackground();
    void textBackground();
    void colorTextBackground();
    void MenuDraw();  // Dessiner le menu principal ou l'écran de choix de couleur
    void handleEvents(sf::Event event);
    bool isStartButtonHovered();
    bool isExitButtonHovered();
    bool isWhiteButtonHovered();
    bool isBlackButtonHovered();
    void launchPVP();
    void drawMain();
    void drawColorChoice();  // Afficher le choix de couleur

private:
    Echiquier echiquier;
    sf::RenderWindow& window;
    sf::Font font;
    sf::RectangleShape startButtonRect;
    sf::RectangleShape exitButtonRect;
    sf::RectangleShape BlackButtonRect;
    sf::RectangleShape WhiteButtonRect;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture textTexture;
    sf::Sprite textSprite;
    sf::Texture colorTextTexture;
    sf::Sprite colorTextSprite;
    MenuState currentState;  // L'état actuel du menu
};

