#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include "macros.h"
using namespace std;

// Red, Yellow, Green, Blue
// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
// Specials: Reverse, Skip, Draw 2
// Wilds: Regular, Draw 4;

class Card
{
public:
	int rank;
	char color;
	bool isWild;
	bool isSpecial;
	bool status;
	Card(int r, char c, bool w, bool s, bool p) {
		rank = r;
		color = c;
		isWild = w;
		isSpecial = s;
		status = p;
	}
	string GetColor(char color) { 
		switch (color) {
		case(YELLOW):
			return "Yellow";
			break;
		case(BLUE):
			return "Blue";
			break;
		case(RED):
			return "Red";
			break;
		case(GREEN):
			return "Green";
			break;
		case(WILD_COLOR):
			return "Wild";
			break;
		default:
			break;
		}
	}
};

vector<Card> createDeck() {
	vector<Card> deck;
	char colors[] = { 'R', 'Y', 'G', 'B' };
	for (char c : colors) {
		// Deal a 0 for each color
		deck.push_back(Card(0, c, notWild, notSpecial, notPlayed));
		// Deal two of each rank for each color
		for (int i = 1; i <= 9; i++) {
			deck.push_back(Card(i, c, notWild, notSpecial, notPlayed));
			deck.push_back(Card(i, c, notWild, notSpecial, notPlayed));
		}
		// Deal two action cards of each color
		for (int i = 10; i <= 12; i++) {
			deck.push_back(Card(i, c, notWild, special, notPlayed));
			deck.push_back(Card(i, c, notWild, special, notPlayed));
		}
	}
	// Deal wilds
	for (int i = 0; i < 4; i++) {
		deck.push_back(Card(13, WILD_COLOR, wild, special, notPlayed)); // Wild
		deck.push_back(Card(14, WILD_COLOR, wild, special, notPlayed)); // Wild Draw 4
	}
	// Shuffle
	random_device rd;
	mt19937 g(rd());
	shuffle(deck.begin(), deck.end(), g);

	return deck;
}
// Special Card Ranks: 10 = Skip, 11 = Reverse, 12 = DrawTwo, 13 = Wild, 14 = WildDraw4
void displayCard(Card card) {
	switch (card.color) { // Displays cards with colors in the terminal
	case(RED):
		cout << RED_TEXT;
		break;
	case(GREEN):
		cout << GREEN_TEXT;
		break;
	case(YELLOW):
		cout << YELLOW_TEXT;
		break;
	case(BLUE):
		cout << BLUE_TEXT;
		break;
	default:
		cout << RED_TEXT;
		break;
	}
	cout << "[";
	if (card.isSpecial) {
		switch (card.rank) {
		case(skip):
			cout << "SKIP";
			break;
		case(reverse):
			cout << "REV";
			break;
		case(drawTwo):
			cout << " +2 ";
			break;
		case(wildCard):
			cout << ORANGE_TEXT << "W"
				<< YELLOW_TEXT << "I"
				<< GREEN_TEXT << "L"
				<< BLUE_TEXT << "D";
			break;
		case(wildDrawFour):
			cout << ORANGE_TEXT << "W"
				<< YELLOW_TEXT << "I"
				<< GREEN_TEXT << "L"
				<< BLUE_TEXT << "D"
				<< PURPLE_TEXT << "+4";
			break;
		default:
			cout << "[Unknown Rank: " << card.rank << "]";
			break;
		}
	}
	else {
		cout << card.rank;
	}
	cout << "]" << RESET_TEXT;
}

void reshuffle(vector<Card>& deck, vector<Card>& discardPile) {
	if (discardPile.size() <= 1) {
		return;
	}
	for (auto card : discardPile) {
		if (card.isWild) {
			card.color = WILD_COLOR;
		}
		card.status = notPlayed;
	}
	Card topCard = discardPile.back();
	discardPile.pop_back();
	deck.insert(deck.end(), discardPile.begin(), discardPile.end());
	discardPile.clear();
	discardPile.push_back(topCard);
	random_device rd;
	mt19937 g(rd());
	shuffle(deck.begin(), deck.end(), g);
	cout << "\nDiscard pile shuffled into deck (debug)\n" << endl;
}

Card draw(vector<Card>& deck, vector<Card>& hand, vector<Card>& discardPile) {
	if (deck.empty()) {
		reshuffle(deck, discardPile);
	}
	Card drawCard = deck.back();
	hand.push_back(drawCard);
	deck.pop_back();
	return drawCard;
}

void dealCards(vector<Card>& deck, vector<Card>& playerHand, vector<Card>& opponentHand) {
	for (int i = 0; i < 7; i++) {
		Card cardDrawn = draw(deck, playerHand, playerHand); // Need to remove 3rd parameter here and on other function call below, putting a bandage on this for now
	}
	for (int i = 0; i < 7; i++) {
		Card cardDrawn = draw(deck, opponentHand, opponentHand);
	}
}

bool isValidCard(Card card, Card lastPlayedCard) {
	return card.rank == lastPlayedCard.rank
		|| card.color == lastPlayedCard.color
		|| card.isWild
		? true : false;
}

char getRandomColor() {
	string validColors = "RGBY";
	random_device rd;
	mt19937 engine(rd());
	uniform_int_distribution<int> dist(0, validColors.length() - 1);
	return validColors[dist(engine)];
}

bool unoCheck(vector<Card> playerHand, bool &gamePower) {
	if (playerHand.size() == UNO) {
		cout << "UNO! One more card to win!" << endl;
	}
	else if (playerHand.size() < UNO) {
		cout << "Player wins!" << endl << "Thanks for playing!" << endl;
		gamePower = powerOff;
	}
	return gamePower;
}

void playUno() {
	int cardToPlay;
	bool gamePower = powerOn;
	bool validCardPlayed;
	string wildColor;
	string colors = "RGBY";
	vector<Card> deck = createDeck();
	vector<Card> discardPile;
	vector<Card> playerHand;
	vector<Card> opponentHand;
	dealCards(deck, playerHand, opponentHand);
	Card lastPlayedCard = draw(deck, discardPile, discardPile);
	lastPlayedCard.status = played; 
	if (lastPlayedCard.isWild) {
		lastPlayedCard.color = getRandomColor(); 
	}
	while (gamePower) {
		cout << endl << "Current Card: ";
		displayCard(lastPlayedCard);
		if (lastPlayedCard.isWild) {
			cout << "\nWild card color: " << lastPlayedCard.GetColor(lastPlayedCard.color);
		}
		cout << endl << "Your hand: " << endl;
		for (auto cards : playerHand) {
			displayCard(cards);
		}
		cout << "\nChoose a card number to play (or 0 to draw): ";
		if (!(cin >> cardToPlay)) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input! Please enter a number." << endl;
			continue; 
		}
		if (cardToPlay > 0 && cardToPlay <= playerHand.size()) {
			Card playedCard = playerHand[cardToPlay - 1];
			if (isValidCard(playedCard, lastPlayedCard)) {
				playerHand.erase(playerHand.begin() + (cardToPlay - 1));
				playedCard.status = played;
				if (playedCard.isWild) {
					cout << "WILD CARD! Select your color (R,G,B,Y): ";
					while (true) { 
						cin >> wildColor; 
						if (colors.find(toupper(wildColor[0])) != string::npos) {
							playedCard.color = toupper(wildColor[0]);
							break;
						}
						else {
							cout << "Please enter a valid color letter (R,G,B,Y): ";
						}
					}
				}
				if (playedCard.isSpecial) {
					switch (playedCard.rank) {
					case(skip): // Set skip turn var to true
						break;
					case(reverse): // Reverse turn order array or iterator
						break;
					case(drawTwo): // Force other player to draw two cards unless they can stack a draw 2. If they draw, the turn is skipped.
						break;
					case(wildDrawFour): // Force other player to draw four cards unless they can stack a draw 4. If they draw, the turn is skipped.
						break;
					default:
						break;
					}
				}
				discardPile.push_back(playedCard);
				lastPlayedCard = playedCard;
			}
			else {
				cout << "Invalid play! Select another card or draw to pass your turn." << endl;
				continue;
			}
		}
		else if (!cardToPlay) {
			draw(deck, playerHand, discardPile);
		}
		else {
			cout << "Invalid input" << endl; // Temporary way to ensure valid inputs and exit program if needed. Add "continue" below this later to avoid passing turn
			gamePower = powerOff;
		}
		unoCheck(playerHand, gamePower);
		// pass turn if hand is valid
	}
}

void mainMenu() {
	string userInput;
	bool programPower = powerOn;
	cout << "| Welcome to UNO! |" << endl << "Input \"S\" to start playing!" << endl << "At any time, input \"X\" to terminate the program." << endl;
	while (programPower) {
		cout << ">> ";
		cin >> userInput;
		switch (userInput[0]) {
		case(START):
			cout << "Game starting..." << endl;
			playUno();
			programPower = powerOff;
			break;
		case(TERMINATE):
			cout << "Terminating program... Thanks for playing!" << endl;
			programPower = powerOff;
			break;
		default:
			cout << "Unknown command!" << endl;
			break;
		}
	}
	return;
}

int main() {
	mainMenu();
	return 0;
}