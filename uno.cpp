#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include "macros.h"


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
	std::string GetColor(char color) { // For debugging/testing purposes
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
		case('W'):
			return "Wild";
			break;
		default:
			break;
		}
	}
};

std::vector<Card> createDeck() {
	std::vector<Card> deck;
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
		deck.push_back(Card(13, 'W', wild, special, notPlayed)); // Wild
		deck.push_back(Card(14, 'W', wild, special, notPlayed)); // Wild Draw 4
	}

	// Shuffle
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(deck.begin(), deck.end(), g);

	return deck;
}



// Special Card Ranks: 10 = Skip, 11 = Reverse, 12 = DrawTwo, 13 = Wild, 14 = WildDraw4

void displayCard(Card card) {
	switch (card.color) { // Displays cards with colors in the terminal
	case(RED):
		std::cout << RED_TEXT;
		break;
	case(GREEN):
		std::cout << GREEN_TEXT;
		break;
	case(YELLOW):
		std::cout << YELLOW_TEXT;
		break;
	case(BLUE):
		std::cout << BLUE_TEXT;
		break;
	default:
		std::cout << RED_TEXT;
		break;
	}
	std::cout << "[";
	if (card.isSpecial) {
		switch (card.rank) {
		case(skip):
			std::cout << "SKIP";
			break;
		case(reverse):
			std::cout << "REV";
			break;
		case(drawTwo):
			std::cout << " +2 ";
			break;
		case(wildCard):
			std::cout << ORANGE_TEXT << "W"
				<< YELLOW_TEXT << "I"
				<< GREEN_TEXT << "L"
				<< BLUE_TEXT << "D";
			break;
		case(wildDrawFour):
			std::cout << ORANGE_TEXT << "W"
				<< YELLOW_TEXT << "I"
				<< GREEN_TEXT << "L"
				<< BLUE_TEXT << "D"
				<< PURPLE_TEXT << "+4";
			break;
		default:
			std::cout << "[Unknown Rank: " << card.rank << "]";
			break;
		}
	}
	else {
		std::cout << card.rank;
	}
	std::cout << "]" << RESET_TEXT;
}

void reshuffle(std::vector<Card>& deck, std::vector<Card>& discardPile) {
	if (discardPile.size() <= 1) {
		// Failsafe in the event that all cards are in hands
		return;
	}
	Card topCard = discardPile.back();
	discardPile.pop_back();
	deck.insert(deck.end(), discardPile.begin(), discardPile.end());
	discardPile.clear();
	discardPile.push_back(topCard);
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(deck.begin(), deck.end(), g);
	std::cout << "\nDiscard pile shuffled into deck (debug)\n" << std::endl;
}

Card draw(std::vector<Card>& deck, std::vector<Card>& hand, std::vector<Card>& discardPile) {
	if (deck.empty()) {
		reshuffle(deck, discardPile);
	}
	Card drawCard = deck.back();
	hand.push_back(drawCard);
	deck.pop_back();
	return drawCard;
}

void dealCards(std::vector<Card>& deck, std::vector<Card>& playerHand, std::vector<Card>& opponentHand) {
	for (int i = 0; i < 7; i++) {
		Card cardDrawn = draw(deck, playerHand, playerHand); // Need to remove 3rd parameter here and on other function call below, putting a bandage on this for now
	}
	for (int i = 0; i < 7; i++) {
		Card cardDrawn = draw(deck, opponentHand, opponentHand);
	}
}

void mainMenu(bool& gamePower) {
	char userInput;
	while (powerOn) {
		std::cout << ">> ";
		std::cin.get(userInput);
		if (userInput == 'S') {
			std::cout << "Game starting..." << std::endl;
			gamePower = powerOn;
			break;
		}
		else if (userInput == 'X') {
			std::cout << "Terminating program... Thanks for playing!" << std::endl;
			break;
		}
	}
	return;
}

int main() {
	int cardToPlay;
	bool gamePowerOn = true;
	std::vector<Card> deck = createDeck();
	std::vector<Card> discardPile;
	std::vector<Card> playerHand;
	std::vector<Card> opponentHand;
	// Below lines are for a menu option once the game is functional. Not critical to functionality, as of now game automatically starts
	//std::cout << "| Welcome to UNO! |" << std::endl << "Input \"S\" to start playing!" << std::endl << "At any time, input \"X\" to terminate the program." << std::endl;
	//mainMenu(gamePowerOn);
	//if (gamePowerOn) {
	//std::cout << "Game has started!" << std::endl;
	dealCards(deck, playerHand, opponentHand);
	// Below will likely be moved outside of main later
	while (gamePowerOn) {
		Card lastPlayedCard = draw(deck, discardPile, discardPile);
		lastPlayedCard.status = played;
		std::cout << std::endl << "Current Card: ";
		displayCard(lastPlayedCard);
		std::cout << std::endl << "Your hand: " << std::endl;
		for (auto cards : playerHand) {
			displayCard(cards);
		}
		std::cout << "\nChoose a card number to play (or 0 to draw): ";
		std::cin >> cardToPlay;
		if (cardToPlay > 0 && cardToPlay <= playerHand.size() && (cardToPlay * -1) < 0) {
			Card playedCard = playerHand[cardToPlay - 1];
			playerHand.erase(playerHand.begin() + (cardToPlay - 1));
			std::cout << "You selected: ";
			displayCard(playedCard);
			playedCard.status = played;
			discardPile.push_back(playedCard);
		}
		else if (!cardToPlay) {
			draw(deck, playerHand, discardPile);
		}
		else {
			std::cout << "Invalid input" << std::endl; // Temporary way to ensure valid inputs and exit program if needed
			gamePowerOn = false;
			break;
		}
	}
	//}
	return 0;
}