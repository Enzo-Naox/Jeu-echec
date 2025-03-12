#include "Menu.h"

Menu::Menu(sf::RenderWindow& window) : window(window), echiquier(window), currentState(MenuState::MainMenu) {

    startButtonRect.setSize(sf::Vector2f(300, 150));
    startButtonRect.setFillColor(sf::Color(50, 50, 50, 0));
    startButtonRect.setPosition(650, 260);
   
    exitButtonRect.setSize(sf::Vector2f(300, 150));
    exitButtonRect.setFillColor(sf::Color(50, 50, 50, 0));
    exitButtonRect.setPosition(970, 260);

    WhiteButtonRect.setSize(sf::Vector2f(300, 150));
    WhiteButtonRect.setFillColor(sf::Color(50, 50, 50, 0));
    WhiteButtonRect.setPosition(650, 260);

    BlackButtonRect.setSize(sf::Vector2f(300, 150));
    BlackButtonRect.setFillColor(sf::Color(50, 50, 50, 0));
    BlackButtonRect.setPosition(970, 260);
}

void Menu::DrawBackground()
{
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("img/background/fond.png")) {
        // Gérer l'erreur si le fichier ne peut pas être chargé
        return;
    }

    // Créer un sprite et lui assigner la texture
    sf::Sprite  backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    window.draw(backgroundSprite);
}

void Menu::textBackground()
{
    sf::Texture textTexture;
    if (!textTexture.loadFromFile("img/background/mainText.png")) {
        // Gérer l'erreur si le fichier ne peut pas être chargé
        return;
    }

    // Créer un sprite et lui assigner la texture
    sf::Sprite textSprite;
    textSprite.setTexture(textTexture);

    window.draw(textSprite);
}

void Menu::colorTextBackground()
{
    sf::Texture colorTextTexture;
    if (!colorTextTexture.loadFromFile("img/background/colorText.png")) {
        // Gérer l'erreur si le fichier ne peut pas être chargé
        return;
    }

    // Créer un sprite et lui assigner la texture
    sf::Sprite colorTextSprite;
    colorTextSprite.setTexture(colorTextTexture);

    window.draw(colorTextSprite);
}

void Menu::MenuDraw() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Gérer les événements du menu
            handleEvents(event);
        }

        // Dessiner en fonction de l'état du menu
        if (currentState == MenuState::MainMenu) {
            drawMain();  // Dessiner le menu principal
        }
        else if (currentState == MenuState::ColorChoice) {
            drawColorChoice();  // Dessiner le choix de couleur
        }
    }
}

void Menu::handleEvents(sf::Event event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        int mouseX = event.mouseButton.x;
        int mouseY = event.mouseButton.y;

        // Si on est sur le menu principal
        if (currentState == MenuState::MainMenu) {
            // Vérifier si le bouton "Start" (Player vs Player) a été cliqué
            if (isStartButtonHovered()) {
                launchPVP();  // Lancer le mode PVP
            }

            // Vérifier si le bouton "Exit" (Player vs IA) a été cliqué
            if (isExitButtonHovered()) {
                echiquier.setVSIA(true);
                currentState = MenuState::ColorChoice;  // Passer à l'écran de choix de couleur
            }
        }

        // Si on est dans l'écran de choix de couleur
        else if (currentState == MenuState::ColorChoice) {
            if (isWhiteButtonHovered()) {
                launchPVP();
            }
            else if (isBlackButtonHovered()) {
                echiquier.setBoardRotation(true);
                launchPVP();
            }
        }
    }
}

void Menu::launchPVP() {
    window.clear(sf::Color::Black);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        echiquier.drawBoard();
    }
}


void Menu::drawMain() {
    window.clear(sf::Color::Black);

    // Dessiner les boutons
    DrawBackground();
    textBackground();
    window.draw(startButtonRect);
    window.draw(exitButtonRect);

    window.display();
}

void Menu::drawColorChoice() {
    window.clear(sf::Color::Black);

    // Dessiner les boutons
    DrawBackground();
    colorTextBackground();
    window.draw(WhiteButtonRect);
    window.draw(BlackButtonRect);

    window.display();
}

bool Menu::isStartButtonHovered() {
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::FloatRect bounds = startButtonRect.getGlobalBounds();
    return bounds.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
}
bool Menu::isExitButtonHovered() {
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::FloatRect bounds = exitButtonRect.getGlobalBounds();
    return bounds.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
}
bool Menu::isBlackButtonHovered() {
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::FloatRect bounds = BlackButtonRect.getGlobalBounds();
    return bounds.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
}
bool Menu::isWhiteButtonHovered() {
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    sf::FloatRect bounds = WhiteButtonRect.getGlobalBounds();
    return bounds.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
}


