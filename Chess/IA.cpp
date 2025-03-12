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
    effectuerMouvementStrat�gique(echiquier);
}

void IA::effectuerMouvementStrat�gique(Echiquier& echiquier) {
    std::vector<Piece*> piecesIA;

    // R�cup�rer toutes les pi�ces de l'IA
    for (Piece* piece : echiquier.getPieces()) {
        if (piece->getColor() == couleurIA) {
            piecesIA.push_back(piece);
        }
    }

    std::pair<int, int> meilleurMouvement;
    Piece* meilleurePiece = nullptr;
    int scoreMax = -INT_MAX; // Un score tr�s bas pour la comparaison

    // �valuer chaque pi�ce et son mouvement
    for (Piece* piece : piecesIA) {
        std::vector<std::pair<int, int>> mouvementsValides = getMouvementsValid�s(piece, echiquier);

        for (const auto& mouvement : mouvementsValides) {
            int scoreMouvement = evaluerMouvement(piece, mouvement, echiquier);

            // Si ce mouvement est meilleur que les pr�c�dents
            if (scoreMouvement > scoreMax) {
                scoreMax = scoreMouvement;
                meilleurePiece = piece;
                meilleurMouvement = mouvement;
            }
        }
    }

    // Si un bon mouvement est trouv�, on le joue
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

        std::cout << "IA a d�plac� une pi�ce de (" << oldX << ", " << oldY << ") � (" << destX << ", " << destY << ")." << std::endl;
    }
    else {
        std::cout << "Aucun mouvement strat�gique trouv� pour l'IA." << std::endl;
    }
}

// R�cup�rer tous les mouvements valides pour une pi�ce
std::vector<std::pair<int, int>> IA::getMouvementsValid�s(Piece* piece, Echiquier& echiquier) {
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

    // Capture d'une pi�ce ennemie
    if (targetPiece && targetPiece->getColor() != piece->getColor()) {
        score += getPieceValue(targetPiece);  // Ajouter la valeur de la pi�ce captur�e
    }

    // Bonus pour contr�le du centre
    if (isCentralPosition(destX, destY)) {
        score += 10;
    }

    // V�rification si la pi�ce est expos�e � la capture
    if (estExposeeAPrendre(piece, destX, destY, echiquier)) {
        // Ajouter une p�nalit� en fonction de la valeur de la pi�ce expos�e
        score -= getExposurePenalty(piece);
    }

    // V�rifier si le roi est en �chec
    if (echiquier.estEnEchec(couleurIA)) {
        score -= 1000; // Le roi en �chec est une situation d�sastreuse
    }

    // Ajouter une p�nalit� si une pi�ce de l'IA est expos�e � la capture apr�s le mouvement
    for (Piece* adversaire : echiquier.getPieces()) {
        if (adversaire->getColor() != couleurIA) {
            // V�rifier si l'adversaire peut capturer la pi�ce de l'IA
            if (adversaire->isValidMove(destX, destY, echiquier)) {
                score -= getExposurePenalty(piece);  // P�nalit� en fonction du type de la pi�ce expos�e
            }
        }
    }

    return score;
}

// Fonction pour obtenir la p�nalit� en fonction du type de la pi�ce expos�e
int IA::getExposurePenalty(Piece* piece) {
    switch (piece->getType()) {
    case PieceType::Pion:     return 10;  // Pion moins p�nalis�
    case PieceType::Cavalier: return 30;  // Cavalier mod�r�ment p�nalis�
    case PieceType::Fou:      return 30;  // Fou mod�r�ment p�nalis�
    case PieceType::Tour:     return 50; // Tour assez p�nalis�e
    case PieceType::Reine:    return 100; // Reine tr�s p�nalis�e
    case PieceType::Roi:      return 1000; // Roi extr�mement p�nalis�
    default: return 0;         // Autres pi�ces (non d�finies) ne sont pas p�nalis�es
    }
}

int IA::getPieceValue(Piece* piece) {
    switch (piece->getType()) {
    case PieceType::Pion: return 10;
    case PieceType::Cavalier: return 30;
    case PieceType::Fou: return 30;
    case PieceType::Tour: return 50;
    case PieceType::Reine: return 100;
    case PieceType::Roi: return 0; // Le roi n'a pas de valeur pour la capture, mais il doit �tre prot�g�
    default: return 0;
    }
}


// V�rifier si une position est pr�s du centre du plateau
bool IA::isCentralPosition(int x, int y) {
    return (x == 3 || x == 4) && (y == 3 || y == 4);
}

bool IA::estExposeeAPrendre(Piece* piece, int destX, int destY, Echiquier& echiquier) {
    // V�rifier si une pi�ce ennemie peut atteindre la case de destination
    for (Piece* adversaire : echiquier.getPieces()) {
        if (adversaire->getColor() != piece->getColor()) {
            if (adversaire->isValidMove(destX, destY, echiquier)) {
                return true; // La pi�ce est expos�e � la capture
            }
        }
    }
    return false; // La pi�ce n'est pas expos�e
}