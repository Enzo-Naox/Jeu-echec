#include "Piece.h"

Piece::Piece(Color color, PieceType type, int x, int y)
	:color(color), type(type), x(x), y(y) {
}

void Piece::draw(sf::RenderWindow& window, float offsetX, float offsetY, int tileSize)
{
	sf::Transform transform;
	float offset = (tileSize - 64) / 2.0f;
	sprite.setPosition(x * tileSize + offsetX + offset, y * tileSize + offsetY + offset);
	window.draw(sprite);
}

PieceType Piece::getType() const
{
	return type;
}

Color Piece::getColor() const
{
	return color;
}

int Piece::getX() const
{
	return x;
}

int Piece::getY() const
{
	return y;
}

void Piece::setPosition(int x, int y)
{
	this->x = x;
	this->y = y;
}

bool Pion::isValidMove(int newX, int newY, Echiquier& echiquier) const {
	// V�rifiez l'orientation de l'�chiquier (si le joueur blanc est en bas ou en haut)
	int direction = (color == Color::Noir) ? 1 : -1;  // Direction par d�faut : blanc vers le bas, noir vers le haut

	// Si l'�chiquier est tourn�, inversez la direction
	if (echiquier.isBoardRotated()) {
		direction = (color == Color::Noir) ? -1 : 1;  // Si l'�chiquier est tourn�, la direction s'inverse
	}

	// D�placement en avant d'une case
	if (newY == y + direction && newX == x && !echiquier.getPieceAt(newX, newY)) {
		return true;
	}

	// D�placement de deux cases si le pion n'a pas boug�
	// Ajustez la ligne de d�part selon la rotation de l'�chiquier
	int initialY = (color == Color::Noir) ? (echiquier.isBoardRotated() ? 6 : 1) : (echiquier.isBoardRotated() ? 1 : 6);

	if (newY == y + 2 * direction && newX == x && !echiquier.getPieceAt(newX, newY) && !echiquier.getPieceAt(newX, y + direction) && y == initialY) {
		return true;
	}

	// Capture en diagonale
	if (newY == y + direction && (newX == x - 1 || newX == x + 1)) {
		Piece* targetPiece = echiquier.getPieceAt(newX, newY);
		if (targetPiece && targetPiece->getColor() != color) {
			return true;
		}
	}

	return false;
}


bool Roi::isValidMove(int newX, int newY, Echiquier& echiquier) const {
	// Calculer la diff�rence entre les positions de d�part et d'arriv�e
	int dx = abs(newX - x);
	int dy = abs(newY - y);
	std::vector<Piece*>& pieces = echiquier.getPieces();

	// V�rifier que le roi se d�place d'une seule case dans n'importe quelle direction
	if ((dx <= 1 && dy <= 1) && (dx + dy > 0)) {
		// V�rifier si la case cible est vide ou contient une pi�ce adverse
		Piece* targetPiece = echiquier.getPieceAt(newX, newY);
		if (!targetPiece || targetPiece->getColor() != color) {
			// Simuler le mouvement pour v�rifier si le roi reste en �chec
			int originalX = x;
			int originalY = y;
			Piece* capturedPiece = nullptr;

			// Si une pi�ce ennemie est pr�sente sur la case cible, simuler sa capture
			if (targetPiece) {
				capturedPiece = targetPiece;
				auto it = std::find(pieces.begin(), pieces.end(), capturedPiece);
				if (it != pieces.end()) {
					pieces.erase(it);
				}
			}

			// D�placer temporairement le roi � la nouvelle position
			const_cast<Roi*>(this)->setPosition(newX, newY);

			// V�rifier si le roi est toujours en �chec apr�s le mouvement
			bool stillInCheck = echiquier.isKingInCheck(color);

			// Annuler le mouvement du roi
			const_cast<Roi*>(this)->setPosition(originalX, originalY);

			// Restaurer la pi�ce captur�e si n�cessaire
			if (capturedPiece) {
				pieces.push_back(capturedPiece);
			}

			// Si le roi n'est pas en �chec apr�s le mouvement, il s'agit d'un mouvement valide
			return !stillInCheck;
		}
	}

	return false;
}


bool Reine::isValidMove(int newX, int newY, Echiquier& echiquier) const {
	int dx = newX - x;
	int dy = newY - y;

	// V�rifier si le mouvement est en ligne droite (comme une tour)
	if (dx == 0 || dy == 0) {
		int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
		int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

		int currentX = x + stepX;
		int currentY = y + stepY;

		// V�rifier les cases interm�diaires
		while (currentX != newX || currentY != newY) {
			if (echiquier.getPieceAt(currentX, currentY)) {
				return false; // Une pi�ce bloque le chemin
			}
			currentX += stepX;
			currentY += stepY;
		}
	}
	// V�rifier si le mouvement est en diagonale (comme un fou)
	else if (abs(dx) == abs(dy)) {
		int stepX = (dx > 0) ? 1 : -1;
		int stepY = (dy > 0) ? 1 : -1;

		int currentX = x + stepX;
		int currentY = y + stepY;

		// V�rifier les cases interm�diaires
		while (currentX != newX || currentY != newY) {
			if (echiquier.getPieceAt(currentX, currentY)) {
				return false; // Une pi�ce bloque le chemin
			}
			currentX += stepX;
			currentY += stepY;
		}
	}
	// Si le mouvement n'est ni en ligne droite ni en diagonale, il est invalide
	else {
		return false;
	}

	// V�rifier la case de destination (doit �tre vide ou contenir une pi�ce adverse)
	Piece* targetPiece = echiquier.getPieceAt(newX, newY);
	if (!targetPiece || targetPiece->getColor() != color) {
		return true;
	}

	return false;
}


bool Fou::isValidMove(int newX, int newY, Echiquier& echiquier) const {
	// V�rifier que le mouvement est en diagonale
	if (abs(newX - x) != abs(newY - y)) {
		return false; // Le fou doit se d�placer en diagonale
	}

	// D�terminer la direction du d�placement
	int dx = (newX > x) ? 1 : -1;
	int dy = (newY > y) ? 1 : -1;

	int currentX = x + dx;
	int currentY = y + dy;

	// V�rifier les cases entre la position actuelle et la case de destination
	while (currentX != newX && currentY != newY) {
		if (currentX < 0 || currentX >= echiquier.getBoardSize() || currentY < 0 || currentY >= echiquier.getBoardSize()) {
			return false;
		}

		if (echiquier.getPieceAt(currentX, currentY)) {
			return false; // Une pi�ce bloque le chemin
		}

		currentX += dx;
		currentY += dy;
	}

	// V�rifier la case de destination (doit �tre vide ou contenir une pi�ce adverse)
	Piece* targetPiece = echiquier.getPieceAt(newX, newY);
	if (!targetPiece || targetPiece->getColor() != color) {
		return true;
	}

	return false;
}


bool Cavalier::isValidMove(int newX, int newY, Echiquier& echiquier) const {
	// Calculer la diff�rence entre les positions de d�part et d'arriv�e
	int dx = abs(newX - x);
	int dy = abs(newY - y);

	// V�rifier si le mouvement correspond � une forme en "L"
	if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
		// V�rifier si la case cible est vide ou contient une pi�ce adverse
		Piece* targetPiece = echiquier.getPieceAt(newX, newY);
		if (!targetPiece || targetPiece->getColor() != color) {
			return true;
		}
	}

	return false;
}


bool Tour::isValidMove(int newX, int newY, Echiquier& echiquier) const {
	// V�rifier que le mouvement est en ligne droite (horizontal ou vertical)
	if (newX != x && newY != y) {
		return false; // La tour ne peut pas se d�placer en diagonale
	}

	// V�rifier les cases entre la position actuelle et la case de destination
	int dx = (newX > x) ? 1 : (newX < x) ? -1 : 0;
	int dy = (newY > y) ? 1 : (newY < y) ? -1 : 0;

	int currentX = x + dx;
	int currentY = y + dy;

	while (currentX != newX || currentY != newY) {
		if (echiquier.getPieceAt(currentX, currentY)) {
			return false; // Une pi�ce bloque le chemin
		}
		currentX += dx;
		currentY += dy;
	}

	// V�rifier la case de destination (doit �tre vide ou contenir une pi�ce adverse)
	Piece* targetPiece = echiquier.getPieceAt(newX, newY);
	if (!targetPiece || targetPiece->getColor() != color) {
		return true;
	}

	return false;
}


Pion::Pion(Color color, int x, int y) : Piece(color, PieceType::Pion, x, y)
{
	std::string textureFile = (color == Color::Blanc) ? "img/textures/white_Pion.png" : "img/textures/black_Pion.png";
	if (!texture.loadFromFile(textureFile)) {
		std::cerr << "Erreur: impossible de charger la texture " << textureFile << std::endl;
	}
	sprite.setTexture(texture);
}

Roi::Roi(Color color, int x, int y) : Piece(color, PieceType::Roi, x, y)
{
	std::string textureFile = (color == Color::Blanc) ? "img/textures/white_Roi.png" : "img/textures/black_Roi.png";
	if (!texture.loadFromFile(textureFile)) {
		std::cerr << "Erreur: impossible de charger la texture " << textureFile << std::endl;
	}
	sprite.setTexture(texture);
}

Reine::Reine(Color color, int x, int y) : Piece(color, PieceType::Reine, x, y)
{
	std::string textureFile = (color == Color::Blanc) ? "img/textures/white_Reine.png" : "img/textures/black_Reine.png";
	if (!texture.loadFromFile(textureFile)) {
		std::cerr << "Erreur: impossible de charger la texture " << textureFile << std::endl;
	}
	sprite.setTexture(texture);
}

Fou::Fou(Color color, int x, int y) : Piece(color, PieceType::Fou, x, y)
{
	std::string textureFile = (color == Color::Blanc) ? "img/textures/white_Fou.png" : "img/textures/black_Fou.png";
	if (!texture.loadFromFile(textureFile)) {
		std::cerr << "Erreur: impossible de charger la texture " << textureFile << std::endl;
	}
	sprite.setTexture(texture);
}

Cavalier::Cavalier(Color color, int x, int y) : Piece(color, PieceType::Cavalier, x, y)
{
	std::string textureFile = (color == Color::Blanc) ? "img/textures/white_Cavalier.png" : "img/textures/black_Cavalier.png";
	if (!texture.loadFromFile(textureFile)) {
		std::cerr << "Erreur: impossible de charger la texture " << textureFile << std::endl;
	}
	sprite.setTexture(texture);
}

Tour::Tour(Color color, int x, int y) : Piece(color, PieceType::Tour, x, y)
{
	std::string textureFile = (color == Color::Blanc) ? "img/textures/white_Tour.png" : "img/textures/black_Tour.png";
	if (!texture.loadFromFile(textureFile)) {
		std::cerr << "Erreur: impossible de charger la texture " << textureFile << std::endl;
	}
	sprite.setTexture(texture);
}
