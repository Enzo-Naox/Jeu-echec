#include "Echiquier.h"

Echiquier::Echiquier(sf::RenderWindow& window)
    : window(window), offsetX(50), offsetY(50), TILE_SIZE(80), BOARD_SIZE(8),
    cgris(sf::Color(168, 168, 168)), fgris(sf::Color(76, 76, 76)), currentPlayer(Color::Blanc) {
}

void Echiquier::drawBackground()
{
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("img/background/fond.png")) {
        // G�rer l'erreur si le fichier ne peut pas �tre charg�
        return;
    }

    // Cr�er un sprite et lui assigner la texture
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    window.draw(backgroundSprite);
}


void Echiquier::initialisePieces()
{
    int blackRow = (isBoardRotated()) ? 0 : 7;
    int whiteRow = (isBoardRotated()) ? 7 : 0;
    int blackPawnRow = (isBoardRotated()) ? 1 : 6;
    int whitePawnRow = (isBoardRotated()) ? 6 : 1;

    // Ajouter les pieces Noires (ou noires selon l'inversion)
    pieces.push_back(new Tour(Color::Noir, 0, whiteRow));
    pieces.push_back(new Cavalier(Color::Noir, 1, whiteRow));
    pieces.push_back(new Fou(Color::Noir, 2, whiteRow));
    pieces.push_back(new Reine(Color::Noir, 3, whiteRow));
    pieces.push_back(new Roi(Color::Noir, 4, whiteRow));
    pieces.push_back(new Fou(Color::Noir, 5, whiteRow));
    pieces.push_back(new Cavalier(Color::Noir, 6, whiteRow));
    pieces.push_back(new Tour(Color::Noir, 7, whiteRow));
    for (int col = 0; col < BOARD_SIZE; ++col) {
        pieces.push_back(new Pion(Color::Noir, col, whitePawnRow));
    }

    // Ajouter les pieces Blanches (ou blanches selon l'inversion)
    pieces.push_back(new Tour(Color::Blanc, 0, blackRow));
    pieces.push_back(new Cavalier(Color::Blanc, 1, blackRow));
    pieces.push_back(new Fou(Color::Blanc, 2, blackRow));
    pieces.push_back(new Reine(Color::Blanc, 3, blackRow));
    pieces.push_back(new Roi(Color::Blanc, 4, blackRow));
    pieces.push_back(new Fou(Color::Blanc, 5, blackRow));
    pieces.push_back(new Cavalier(Color::Blanc, 6, blackRow));
    pieces.push_back(new Tour(Color::Blanc, 7, blackRow));
    for (int col = 0; col < BOARD_SIZE; ++col) {
        pieces.push_back(new Pion(Color::Blanc, col, blackPawnRow));
    }
}

std::vector<std::pair<int, int>> Echiquier::getAllPossibleMoves(Piece& piece) {
    std::vector<std::pair<int, int>> possibleMoves;

    // Parcourir toutes les cases de l'�chiquier
    for (int newX = 0; newX < BOARD_SIZE; ++newX) {
        for (int newY = 0; newY < BOARD_SIZE; ++newY) {
            if (piece.isValidMove(newX, newY, *this)) {
                // Ajouter la position valide aux mouvements possibles
                possibleMoves.push_back({ newX, newY });
            }
        }
    }

    return possibleMoves;
}

void Echiquier::capturePiece(Piece* capturedPiece) {
    pieces.erase(std::remove(pieces.begin(), pieces.end(), capturedPiece), pieces.end());
    capturedPieces.push_back(capturedPiece);
}

void Echiquier::drawBoard() {
    Piece* selectedPiece = nullptr;
    IA* ia = nullptr;
    initialisePieces();

    if (isVSIA()) {
        ia = new IA(isBoardRotated() ? Color::Blanc : Color::Noir);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;
                int tileX = (mouseX - offsetX) / TILE_SIZE;
                int tileY = (mouseY - offsetY) / TILE_SIZE;

                if (isVSIA() && !ia) {
                    ia = new IA(isBoardRotated() ? Color::Blanc : Color::Noir);
                }

                if (!isVSIA() || (isVSIA() && currentPlayer != ia->getCouleur())) {
                    // S�lection de la pi�ce
                    if (!selectedPiece) {
                        Piece* piece = getPieceAt(tileX, tileY);
                        if (piece && piece->getColor() == currentPlayer) {
                            selectedPiece = piece;
                        }
                    }
                    else {
                        // Si le mouvement est valide
                        if (selectedPiece->isValidMove(tileX, tileY, *this)) {
                            Piece* targetPiece = getPieceAt(tileX, tileY);

                            int oldX = selectedPiece->getX();
                            int oldY = selectedPiece->getY();

                            // Sauvegarder l'�tat de la pi�ce captur�e (mais ne pas la supprimer de suite)
                            bool pieceCaptured = false;
                            int targetOldX = -1, targetOldY = -1;
                            if (targetPiece && targetPiece->getColor() != selectedPiece->getColor()) {
                                pieceCaptured = true;
                                targetOldX = targetPiece->getX();
                                targetOldY = targetPiece->getY();

                                // Simuler la capture sans supprimer imm�diatement
                                targetPiece->setPosition(-1, -1); // On place temporairement hors du plateau
                            }

                            // Mise � jour de la position de la pi�ce
                            selectedPiece->setPosition(tileX, tileY);

                            if (isKingInCheck(currentPlayer)) {
                                // Annuler le mouvement
                                std::cout << "D�placement invalide, le roi est toujours en �chec !" << std::endl;
                                selectedPiece->setPosition(oldX, oldY);

                                // Restaurer la position de la pi�ce captur�e si n�cessaire
                                if (pieceCaptured && targetPiece) {
                                    targetPiece->setPosition(targetOldX, targetOldY);
                                }
                            }
                            else {
                                // Si le mouvement est valide, effectuer la capture r�ellement
                                if (pieceCaptured && targetPiece) {
                                    capturePiece(targetPiece);
                                }

                                // Changer le joueur
                                currentPlayer = (currentPlayer == Color::Blanc) ? Color::Noir : Color::Blanc;

                                // V�rifier si le roi adverse est en �chec
                                if (isKingInCheck(currentPlayer)) {
                                    std::cout << "Le roi " << (currentPlayer == Color::Blanc ? "Blanc" : "Noir") << " est en �chec !" << std::endl;
                                }
                            }
                        }

                        // R�initialiser la s�lection de pi�ce
                        selectedPiece = nullptr;
                    }
                }
            }
        }

        if (isVSIA() && currentPlayer == ia->getCouleur()) {
            ia->jouer(*this);
            // Changer le joueur apr�s que l'IA ait jou�
            currentPlayer = (currentPlayer == Color::Blanc) ? Color::Noir : Color::Blanc;
        }

        if (isGameOver()) {
            
        }

        // Affichage et surlignage
        window.clear();
        drawBackground();

        // Dessiner l'�chiquier
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                tile.setPosition(col * TILE_SIZE + offsetX, row * TILE_SIZE + offsetY);
                tile.setFillColor((row + col) % 2 == 0 ? cgris : fgris);
                window.draw(tile);
            }
        }

        int whiteCount = 0;
        int ligneWhite = 0;
        int blackCount = 0;
        int ligneBlack = 0;

        for (auto& piece : capturedPieces) {
            if (piece->getColor() == Color::Blanc) {
                piece->setPosition(whiteCount, ligneWhite);
                piece->draw(window, 700, 40, 70);
                whiteCount++;
                if (whiteCount == 8) {
                    ligneWhite = 1;
                    whiteCount = 0;
                }
            }
            else {
                piece->setPosition(blackCount, ligneBlack);
                piece->draw(window, 700, 535, 70);
                blackCount++;
                if (blackCount == 8) {
                    ligneBlack = 1;
                    blackCount = 0;
                }
            }
        }

        for (auto& piece : pieces) {
            if (piece->getType() == PieceType::Roi && isKingInCheck(piece->getColor())) {
                sf::RectangleShape checkHighlight(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                checkHighlight.setPosition(piece->getX() * TILE_SIZE + offsetX, piece->getY() * TILE_SIZE + offsetY);
                checkHighlight.setFillColor(sf::Color(150, 0, 200, 180)); // Violet
                window.draw(checkHighlight);
            }
        }

        // Surlignage des cases de d�placement valide
        if (selectedPiece) {
            for (int row = 0; row < BOARD_SIZE; ++row) {
                for (int col = 0; col < BOARD_SIZE; ++col) {
                    if (selectedPiece->isValidMove(col, row, *this)) {
                        Piece* targetPiece = getPieceAt(col, row);
                        sf::RectangleShape moveHighlight(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));
                        moveHighlight.setPosition(col * TILE_SIZE + offsetX + 1, row * TILE_SIZE + offsetY + 1);

                        if (targetPiece && targetPiece->getColor() != selectedPiece->getColor()) {
                            moveHighlight.setFillColor(sf::Color(242, 61, 65, 255)); // Rouge pour les captures
                        }
                        else {
                            moveHighlight.setFillColor(sf::Color(184, 203, 53, 255)); // Vert pour les d�placements
                        }
                        window.draw(moveHighlight);
                    }
                }
            }
        }

        // Surlignage de la pi�ce s�lectionn�e
        if (selectedPiece) {
            sf::RectangleShape highlight(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            highlight.setPosition(selectedPiece->getX() * TILE_SIZE + offsetX, selectedPiece->getY() * TILE_SIZE + offsetY);
            highlight.setFillColor(sf::Color(244, 244, 103, 255));
            window.draw(highlight);
        }

        // Affichage du texte du tour courant
        sf::Font font;
        if (font.loadFromFile("fonts/Coffee_Town.ttf")) {
            sf::Text turnText;
            turnText.setFont(font);
            turnText.setString(currentPlayer == Color::Blanc ? "Tour des Blancs" : "Tour des Noirs");
            turnText.setCharacterSize(70);
            turnText.setFillColor(currentPlayer == Color::Blanc ? sf::Color::White : sf::Color::Black);
            turnText.setPosition(700, 320);
            window.draw(turnText);
        }

        std::vector<std::pair<int, int>> possibleMoves;
        for (auto& piece : pieces) {
            std::vector<std::pair<int, int>> moves = getAllPossibleMoves(*piece);
            possibleMoves.insert(possibleMoves.end(), moves.begin(), moves.end());
        }

        // Dessiner les pi�ces
        for (const auto& piece : pieces) {
            piece->draw(window, offsetX, offsetY, TILE_SIZE);
        }

        window.display();
    }
}

Piece* Echiquier::getPieceAt(int x, int y) const {
    for (Piece* piece : pieces) {
        if (piece->getX() == x && piece->getY() == y) {
            return piece;
        }
    }
    return nullptr; // Aucun pi�ce trouv�e
}

int Echiquier::getBoardSize()
{
    return BOARD_SIZE;
}

bool Echiquier::isVSIA() const
{
    return isPlayerVSIa;
}

void Echiquier::setVSIA(bool ia)
{
    isPlayerVSIa = ia;
}

bool Echiquier::isBoardRotated() const
{
    return isRotated;
}

void Echiquier::setBoardRotation(bool rotated)
{
    isRotated = rotated;
}

bool Echiquier::isCheckmate(Color kingColor) {
    // V�rifier si le roi est en �chec
    if (!isKingInCheck(kingColor)) {
        return false;  // Pas d'�chec, donc pas d'�chec et mat
    }

    // Parcourir toutes les pi�ces du joueur
    for (Piece* piece : pieces) {
        if (piece->getColor() == kingColor) {
            int originalX = piece->getX();
            int originalY = piece->getY();

            // Tester tous les mouvements possibles pour cette pi�ce
            for (int row = 0; row < BOARD_SIZE; ++row) {
                for (int col = 0; col < BOARD_SIZE; ++col) {
                    Piece* targetPiece = getPieceAt(col, row);

                    // V�rifier si le mouvement est valide et simuler le mouvement
                    if (piece->isValidMove(col, row, *this)) {
                        // Simuler le mouvement
                        piece->setPosition(col, row);

                        // **Simulation de capture sans suppression r�elle**
                        bool wasCaptured = false;
                        if (targetPiece && targetPiece->getColor() != kingColor) {
                            wasCaptured = true;
                            // Retirer temporairement la pi�ce de la liste (sans la d�truire)
                            auto it = std::find(pieces.begin(), pieces.end(), targetPiece);
                            if (it != pieces.end()) {
                                pieces.erase(it);
                            }
                        }

                        // V�rifier si le roi est toujours en �chec apr�s le mouvement
                        bool stillInCheck = isKingInCheck(kingColor);

                        // Annuler le mouvement
                        piece->setPosition(originalX, originalY);

                        // Restaurer la pi�ce captur�e si n�cessaire
                        if (wasCaptured) {
                            pieces.push_back(targetPiece);
                        }

                        // Si le roi n'est plus en �chec apr�s un mouvement, ce n'est pas un �chec et mat
                        if (!stillInCheck) {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;  // Aucune pi�ce ne peut sauver le roi, �chec et mat
}


bool Echiquier::isKingInCheck(Color kingColor) {
    // Trouver le roi du joueur courant
    Piece* king = nullptr;
    for (Piece* piece : pieces) {
        if (piece->getType() == PieceType::Roi && piece->getColor() == kingColor) {
            king = piece;
            break;
        }
    }

    // Si le roi n'a pas �t� trouv� (ce qui ne devrait pas arriver), retourner false
    if (!king) return false;

    int kingX = king->getX();
    int kingY = king->getY();

    // V�rifier si une pi�ce adverse peut se d�placer sur la position du roi
    for (Piece* piece : pieces) {
        if (piece->getColor() != kingColor) {
            if (piece->isValidMove(kingX, kingY, *this)) {
                return true;  // Le roi est en �chec
            }
        }
    }

    return false;  // Le roi n'est pas en �chec
}

bool Echiquier::isGameOver() {
    // V�rifier si un des rois a �t� captur�
    Piece* whiteKing = nullptr;
    Piece* blackKing = nullptr;

    for (Piece* piece : pieces) {
        if (piece->getType() == PieceType::Roi) {
            if (piece->getColor() == Color::Blanc) {
                whiteKing = piece;
            }
            else {
                blackKing = piece;
            }
        }
    }

    if (!whiteKing || !blackKing) {
        // Si l'un des rois a �t� captur�, l'autre joueur gagne
        std::cout << "Le roi a �t� captur� !"<< std::endl;
        return true;
    }

    // V�rifier si le roi adverse est en �chec et mat
    if (isCheckmate(Color::Blanc)) {
        std::cout << "�chec et mat ! Le joueur Noir gagne !" << std::endl;
        return true;
    }
    if (isCheckmate(Color::Noir)) {
        std::cout << "�chec et mat ! Le joueur Blanc gagne !" << std::endl;
        return true;
    }

    return false; // La partie n'est pas termin�e
}

bool Echiquier::estEnEchec(Color couleur) {
    // Trouver le roi de la couleur sp�cifi�e
    Piece* roi = nullptr;
    for (Piece* piece : pieces) {
        if (piece->getColor() == couleur && piece->getType() == PieceType::Roi) {
            roi = piece;
            break;
        }
    }

    if (!roi) {
        std::cout << "Erreur: Le roi de la couleur sp�cifi�e n'a pas �t� trouv�." << std::endl;
        return false;
    }

    // V�rifier si une pi�ce ennemie peut attaquer la position du roi
    int roiX = roi->getX();
    int roiY = roi->getY();

    for (Piece* piece : pieces) {
        if (piece->getColor() != couleur) {
            // V�rifier si cette pi�ce peut se d�placer sur la position du roi
            if (piece->isValidMove(roiX, roiY, *this)) {
                return true;  // Le roi est en �chec
            }
        }
    }

    return false;  // Le roi n'est pas en �chec
}

std::vector<Piece*> Echiquier::getPiecesBlanches() {
    std::vector<Piece*> piecesBlanches;
    for (Piece* piece : pieces) { // pieces est un vecteur de toutes les pi�ces sur l'�chiquier
        if (piece->getColor() == Color::Blanc) {
            piecesBlanches.push_back(piece);
        }
    }
    return piecesBlanches;
}

// Vous pouvez aussi ajouter une m�thode pour obtenir les pi�ces noires
std::vector<Piece*> Echiquier::getPiecesNoires() {
    std::vector<Piece*> piecesNoires;
    for (Piece* piece : pieces) {
        if (piece->getColor() == Color::Noir) {
            piecesNoires.push_back(piece);
        }
    }
    return piecesNoires;
}


