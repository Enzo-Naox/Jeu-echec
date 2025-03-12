#pragma once
#include <cstdlib>
#include <ctime>
#include "Echiquier.h"

enum class Color;
class Echiquier;
class Piece;

class IA
{
public:
    IA(Color couleur);
    Color getCouleur() const;
    void jouer(Echiquier& echiquier);

    void effectuerMouvementStratégique(Echiquier& echiquier);

    std::vector<std::pair<int, int>> getMouvementsValidés(Piece* piece, Echiquier& echiquier);

    int evaluerMouvement(Piece* piece, const std::pair<int, int>& mouvement, Echiquier& echiquier);

    int evaluerPosition(Echiquier& echiquier);

    void simulerMouvement(Piece* piece, const std::pair<int, int>& mouvement, Echiquier& echiquier);

    int getExposurePenalty(Piece* piece);

    int getPieceValue(Piece* piece);

    bool isCentralPosition(int x, int y);

    bool estExposeeAPrendre(Piece* piece, int destX, int destY, Echiquier& echiquier);

private:
    Color couleurIA;
};

