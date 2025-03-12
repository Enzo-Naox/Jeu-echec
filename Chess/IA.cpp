#include "IA.h"

IA::IA(Color couleur) : couleurIA(couleur) {
    std::srand(std::time(nullptr));
}

Color IA::getCouleur() const
{
    return couleurIA;
}

void IA::jouer(Echiquier& echiquier) {
    std::cout << "IA joue son tour" << std::endl;
    effectuerMouvementStratégique(echiquier);
}

void IA::effectuerMouvementStratégique(Echiquier& echiquier) {
    std::vector<Piece*> piecesIA;

    // Récupérer toutes les pièces de l'IA
    for (Piece* piece : echiquier.getPieces()) {
        if (piece->getColor() == couleurIA) {
            piecesIA.push_back(piece);
        }
    }

    std::pair<int, int> meilleurMouvement;
    Piece* meilleurePiece = nullptr;
    int scoreMax = -INT_MAX; // Un score très bas pour la comparaison

    // Évaluer chaque pièce et son mouvement
    for (Piece* piece : piecesIA) {
        std::vector<std::pair<int, int>> mouvementsValides = getMouvementsValidés(piece, echiquier);

        for (const auto& mouvement : mouvementsValides) {
            int scoreMouvement = evaluerMouvement(piece, mouvement, echiquier);

            // Si ce mouvement est meilleur que les précédents
            if (scoreMouvement > scoreMax) {
                scoreMax = scoreMouvement;
                meilleurePiece = piece;
                meilleurMouvement = mouvement;
            }
        }
    }

    // Si un bon mouvement est trouvé, on le joue
    if (meilleurePiece) {
        int oldX = meilleurePiece->getX();
        int oldY = meilleurePiece->getY();
        int destX = meilleurMouvement.first;
        int destY = meilleurMouvement.second;

        // Effectuer le mouvement
        Piece* targetPiece = echiquier.getPieceAt(destX, destY);
        if (targetPiece) {
            echiquier.capturePiece(targetPiece);
        }
        meilleurePiece->setPosition(destX, destY);

        std::cout << "IA a déplacé une pièce de (" << oldX << ", " << oldY << ") à (" << destX << ", " << destY << ")." << std::endl;
    }
    else {
        std::cout << "Aucun mouvement stratégique trouvé pour l'IA." << std::endl;
    }
}

// Récupérer tous les mouvements valides pour une pièce
std::vector<std::pair<int, int>> IA::getMouvementsValidés(Piece* piece, Echiquier& echiquier) {
    std::vector<std::pair<int, int>> mouvementsValides;
    for (int row = 0; row < echiquier.getBoardSize(); ++row) {
        for (int col = 0; col < echiquier.getBoardSize(); ++col) {
            if (piece->isValidMove(col, row, echiquier)) {
                mouvementsValides.emplace_back(col, row);
            }
        }
    }
    return mouvementsValides;
}

int IA::evaluerMouvement(Piece* piece, const std::pair<int, int>& mouvement, Echiquier& echiquier) {
    int score = 0;

    int destX = mouvement.first;
    int destY = mouvement.second;
    Piece* targetPiece = echiquier.getPieceAt(destX, destY);

    // Capture d'une pièce ennemie
    if (targetPiece && targetPiece->getColor() != piece->getColor()) {
        score += getPieceValue(targetPiece);  // Ajouter la valeur de la pièce capturée
    }

    // Bonus pour contrôle du centre
    if (isCentralPosition(destX, destY)) {
        score += 10;
    }

    // Vérification si la pièce est exposée à la capture
    if (estExposeeAPrendre(piece, destX, destY, echiquier)) {
        // Ajouter une pénalité en fonction de la valeur de la pièce exposée
        score -= getExposurePenalty(piece);
    }

    // Vérifier si le roi est en échec
    if (echiquier.estEnEchec(couleurIA)) {
        score -= 1000; // Le roi en échec est une situation désastreuse
    }

    // Ajouter une pénalité si une pièce de l'IA est exposée à la capture après le mouvement
    for (Piece* adversaire : echiquier.getPieces()) {
        if (adversaire->getColor() != couleurIA) {
            // Vérifier si l'adversaire peut capturer la pièce de l'IA
            if (adversaire->isValidMove(destX, destY, echiquier)) {
                score -= getExposurePenalty(piece);  // Pénalité en fonction du type de la pièce exposée
            }
        }
    }

    return score;
}

// Fonction pour obtenir la pénalité en fonction du type de la pièce exposée
int IA::getExposurePenalty(Piece* piece) {
    switch (piece->getType()) {
    case PieceType::Pion:     return 10;  // Pion moins pénalisé
    case PieceType::Cavalier: return 30;  // Cavalier modérément pénalisé
    case PieceType::Fou:      return 30;  // Fou modérément pénalisé
    case PieceType::Tour:     return 50; // Tour assez pénalisée
    case PieceType::Reine:    return 100; // Reine très pénalisée
    case PieceType::Roi:      return 1000; // Roi extrêmement pénalisé
    default: return 0;         // Autres pièces (non définies) ne sont pas pénalisées
    }
}

int IA::getPieceValue(Piece* piece) {
    switch (piece->getType()) {
    case PieceType::Pion: return 10;
    case PieceType::Cavalier: return 30;
    case PieceType::Fou: return 30;
    case PieceType::Tour: return 50;
    case PieceType::Reine: return 100;
    case PieceType::Roi: return 0; // Le roi n'a pas de valeur pour la capture, mais il doit être protégé
    default: return 0;
    }
}


// Vérifier si une position est près du centre du plateau
bool IA::isCentralPosition(int x, int y) {
    return (x == 3 || x == 4) && (y == 3 || y == 4);
}

bool IA::estExposeeAPrendre(Piece* piece, int destX, int destY, Echiquier& echiquier) {
    // Vérifier si une pièce ennemie peut atteindre la case de destination
    for (Piece* adversaire : echiquier.getPieces()) {
        if (adversaire->getColor() != piece->getColor()) {
            if (adversaire->isValidMove(destX, destY, echiquier)) {
                return true; // La pièce est exposée à la capture
            }
        }
    }
    return false; // La pièce n'est pas exposée
}