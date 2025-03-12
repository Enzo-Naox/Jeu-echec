#pragma once
#include <SFML/Graphics.hpp>
#include "Echiquier.h"

#ifndef PIECES_H
#define PIECES_H 

enum class Color { Blanc, Noir };
enum class PieceType { Pion, Roi, Reine, Fou, Cavalier, Tour };
class Echiquier;

class Piece {
public:
    Piece(Color color, PieceType type, int x, int y);
    virtual void draw(sf::RenderWindow& window, float offsetX, float offsetY, int tileSize);
    virtual bool isValidMove(int newX, int newY, Echiquier& echiquier) const = 0;
    Color getColor() const;
    PieceType getType() const;
    int getX() const;
    int getY() const;
    void setPosition(int x, int y);
    int getValeur() const {
        switch (type) {
        case PieceType::Pion:   return 1;
        case PieceType::Tour:   return 5;
        case PieceType::Cavalier: return 3;
        case PieceType::Fou:    return 3;
        case PieceType::Reine:   return 9;
        case PieceType::Roi:    return 0;  // Le roi n'a pas de valeur spécifique dans le calcul
        default: return 0;
        }
    }

protected:
    Color color;
    PieceType type;
    int x;
    int y;
    sf::Sprite sprite;
    sf::Texture texture;
};

class Pion : public Piece {
public:
    bool isValidMove(int newX, int newY, Echiquier& echiquier) const override;
	Pion(Color color, int x, int y);
};

class Roi : public Piece {
public:
    bool isValidMove(int newX, int newY, Echiquier& echiquier) const override;
	Roi(Color color, int x, int y);
};

class Reine : public Piece {
public:
    bool isValidMove(int newX, int newY, Echiquier& echiquier) const override;
	Reine(Color color, int x, int y);
};

class Tour : public Piece {
public:
    bool isValidMove(int newX, int newY, Echiquier& echiquier) const override;
	Tour(Color color, int x, int y);
};

class Cavalier : public Piece {
public:
    bool isValidMove(int newX, int newY, Echiquier& echiquier) const override;
	Cavalier(Color color, int x, int y);
};

class Fou : public Piece {
public:
    bool isValidMove(int newX, int newY, Echiquier& echiquier) const override;
	Fou(Color color, int x, int y);
};

#endif
