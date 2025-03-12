#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <array>

#include "Piece.h"
#include "IA.h"

class Piece;
class IA;
enum class Color;

struct Coup {
    int startX;  // Position de départ sur l'axe X
    int startY;  // Position de départ sur l'axe Y
    int endX;    // Position d'arrivée sur l'axe X
    int endY;    // Position d'arrivée sur l'axe Y

    Coup(int sx, int sy, int ex, int ey) : startX(sx), startY(sy), endX(ex), endY(ey) {}
};

class Echiquier
{
public:
    Echiquier(sf::RenderWindow& window);
    void drawBoard();
    void capturePiece(Piece* capturedPiece);
    Piece* getPieceAt(int x, int y) const;
    int getBoardSize();
    bool isVSIA() const;
    bool isBoardRotated() const;
    void setVSIA(bool ia);
    void setBoardRotation(bool rotated);
    bool isKingInCheck(Color kingColor);
    bool isGameOver();
    bool estEnEchec(Color couleur);
    std::vector<Piece*> getPiecesBlanches();
    std::vector<Piece*> getPiecesNoires();
    std::vector<std::pair<int, int>> getAllPossibleMoves(Piece& piece);
    bool isCheckmate(Color kingColor);
    std::vector<Piece*>& getPieces() {
        return pieces;
    }

private:
    float offsetX, offsetY;
    int TILE_SIZE, BOARD_SIZE;
    bool isPlayerVSIa = false;
    bool isRotated = false;
    std::vector<Piece*> pieces;
    std::vector<Piece*> capturedPieces;
    sf::RenderWindow& window;
    sf::Color cgris, fgris;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    Color currentPlayer;
    void initialisePieces();
    void drawBackground();
};


