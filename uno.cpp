#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include "macros.h"
using namespace std;

string colors = "RGBY";

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

class Player
{
public:
	string Name;
	bool isCPU;
	vector<Card> hand;

	Player(string whatName, bool whatType)
	{
		Name = whatName;
		isCPU = whatType;
	}

	~Player()
	{
		//cout << "Destructor called" << endl; 
	}
};

void initPlayers(vector<Player>& players, string playName, int numOpp = 2)
{
	cout << "PLAYERS\n";
	cout << "-----------------\n";
	players.emplace_back(playName, userPlayer);
	string oppName = "CPU";
	cout << "> " << playName << endl;
	for (int x = 1; x <= numOpp; x++)
	{
		oppName += to_string(x);
		cout << "> " << oppName << endl;
		players.emplace_back(oppName, botPlayer);
		oppName.pop_back();
	}
	cout << "-----------------\n";
}

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

void dealCards(vector<Card>& deck, Player& humanPlayer, vector<Player>& players, int numOpp, vector<Card>& discardPile) {
	
	if (devMode) {
		Card devWildCard = Card(wildCard, WILD_COLOR, wild, special, notPlayed);
		//Card devWildDrawFour = Card(wildDrawFour, WILD_COLOR, wild, special, notPlayed);
		//Card devDrawTwo = Card(drawTwo, BLUE, notWild, special, notPlayed);
		for (int i = 0; i < STARTING_HAND_SIZE; i++) {
			humanPlayer.hand.push_back(devWildCard);
			//humanPlayer.hand.push_back(devDrawTwo);
		}
		
	}
	else {
		for (int i = 0; i < STARTING_HAND_SIZE; i++) {
			draw(deck, humanPlayer.hand, discardPile);
		}
	}
	for (int x = 0; x < numOpp; x++) {
		for (int i = 0; i < STARTING_HAND_SIZE; i++) {
			draw(deck, players[x + 1].hand, discardPile);
		}
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

void clearScreen() {
	// \033[2J clears the screen
	// \033[1;1H moves the cursor to the top-left corner
	cout << CLEAR_SCREEN;
}

bool unoCheck(Player currentPlayer, bool& gamePower) {
	if (currentPlayer.hand.size() == UNO) {
		cout << "UNO! " << currentPlayer.Name << " only needs to play one more card to win! " << endl;
	}
	else if (currentPlayer.hand.size() < UNO) {
		if (!currentPlayer.isCPU) {
			cout << "========================================" << endl;
			cout << "       CONGRATULATIONS! YOU WIN!        \n";
			cout << "========================================" << endl;
			gamePower = powerOff;
		}
		else {
			cout << "========================================" << endl;
			cout << "    CPU WINS! BETTER LUCK NEXT TIME!  \n";
			cout << "========================================" << endl;
			gamePower = powerOff;
		}

	}
	return gamePower;
}

Card initRound(string &playerName, int &playerCount, vector<Player> &players, vector<Card>& deck, vector<Card>& discardPile, int &potentialDraw) {
	cout << "Enter your name: ";
	while (!(cin >> playerName))
	{
		cout << "Invalid input. Try again with a valid name";
		cin.clear();
		cin.ignore(10000, '\n');
	}

	cout << "Enter the total amount of opponents you want" << endl;
	while (!(cin >> playerCount))
	{
		cout << "Invalid input. Try again with ";
		cin.clear();
		cin.ignore(10000, '\n');
	}
	clearScreen();
	cout << "A total of " << playerCount + 1 << " players are competing. Good luck!\n" << endl;

	initPlayers(players, playerName, playerCount);
	dealCards(deck, players[0], players, playerCount, discardPile);
	Card lastPlayedCard = draw(deck, discardPile, discardPile);
	lastPlayedCard.status = played;
	switch (lastPlayedCard.rank) {
	case(drawTwo):
		potentialDraw += 2;
		break;
	case(wildCard):
		lastPlayedCard.color = getRandomColor();
		break;
	case(wildDrawFour):
		lastPlayedCard.color = getRandomColor();
		potentialDraw += 4;
		break;
	}
	return lastPlayedCard;
}

void welcomeScreen() {

	cout << "========================================" << endl;
	// Raw String Literal R"( ... )" handles the spacing automatically
	cout << R"(
       UU   UU   NN   NN    OOOOO
       UU   UU   NNN  NN   OO   OO
       UU   UU   NN N NN   OO   OO
       UU   UU   NN  NNN   OO   OO
        OUUUO    NN   NN    OOOOO
					)" << endl;

	cout << "========================================" << endl;
	cout << "            WELCOME TO UNO!             " << endl;
	cout << "========================================" << endl;
	//cout << "Player wins!" << endl << "Thanks for playing!" << endl;
	return;
}

void topDiscardInfo(Card lastPlayedCard, int potentialDraw, bool canRespond) {
	cout << endl << "Current Card: ";
	displayCard(lastPlayedCard);
	if (lastPlayedCard.isWild) {
		cout << "\nWild card color: " << lastPlayedCard.GetColor(lastPlayedCard.color);
	}
	if (canRespond) {
		switch (lastPlayedCard.rank) {
		case(drawTwo):
			cout << "\nOpponent played a draw two! You must play a draw two from your hand or input 0 to draw " << potentialDraw << " cards and pass your turn.\n";
			break;
		case(wildDrawFour):
			cout << "\nOpponent played a draw four! You must play a draw four from your hand or input 0 to draw " << potentialDraw << " cards and pass your turn.\n";
			break;
		default:
			break;
		}
	}

}

void wildCardLogic(Player player, Card &playedCard, string colors, string &wildColor) {
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

void specialCardLogic(Card playedCard, int &turnCount, int &iterator, int &potentialDraw, vector<Player> &players,
						int &nextPlayer, bool &canRespond, vector<Card> &deck, vector<Card> &discardPile) {
	switch (playedCard.rank) {
	case(skip): // Increment iterator so next player is skipped
		turnCount += iterator;
		break;
	case(reverse): // Reverse turn order iterator
		iterator *= -1;
		break;
	case(drawTwo): // Force other player to draw two cards unless they can stack a draw 2. If they draw, the turn is skipped.
		potentialDraw += 2;
		for (auto card : players[nextPlayer].hand) {
			if (card.rank == drawTwo) {
				canRespond = true;
				break;
			}
		}
		if (!canRespond) {
			cout << endl << players[nextPlayer - iterator].Name << " played a draw 2 on " << players[nextPlayer].Name << "! " << players[nextPlayer].Name << " does not have a card to stack and has drawn " << potentialDraw << " cards and lost their turn for this round.\n";
			canRespond = false;
			for (int i = 0; i < potentialDraw; i++) {
				draw(deck, players[nextPlayer].hand, discardPile);
			}
			potentialDraw = 0;
			turnCount += iterator;
		}
		break;
	case(wildDrawFour): // Same logic as draw 2 
		potentialDraw += 4;
		for (auto card : players[nextPlayer].hand) {
			if (card.rank == wildDrawFour) {
				canRespond = true;
				break;
			}
		}
		if (!canRespond) {
			cout << endl << players[nextPlayer - iterator].Name << " played a draw 4 on " << players[nextPlayer].Name << "! " << players[nextPlayer].Name << " does not have a card to stack and has drawn " << potentialDraw << " cards and lost their turn for this round.\n";
			canRespond = false;
			for (int i = 0; i < potentialDraw; i++) {
				draw(deck, players[nextPlayer].hand, discardPile);
			}
			potentialDraw = 0;
			turnCount += iterator;
		}
		break;
	default:
		break;
	}
}

void playerCardLogic(vector<Player> &players, string colors, Card &playedCard, 
				int &cardToPlay, int &turnCount, int &iterator, 
				int &potentialDraw, bool &canRespond, vector<Card> &deck, 
				vector<Card> &discardPile, string &wildColor, Card &lastPlayedCard,
				int &nextPlayer) {
	players[0].hand.erase(players[0].hand.begin() + (cardToPlay - 1));
	playedCard.status = played;
	if (playedCard.isWild) {
		cout << "WILD CARD! Select your color (R,G,B,Y): ";
		wildCardLogic(players[0], playedCard, colors, wildColor);
	}
	if (playedCard.isSpecial) {
		specialCardLogic(playedCard, turnCount, iterator, potentialDraw, players, nextPlayer, canRespond, deck, discardPile);
	}
	discardPile.push_back(playedCard);
	lastPlayedCard = playedCard;
}

void drawCardLogic(bool &canRespond, int &potentialDraw, vector<Card> &deck, vector<Player> &players, vector<Card> &discardPile, int currentPlayer) {
	if (canRespond) {
		cout << "Drawing multiple cards. potentialDraw val = " << potentialDraw << endl;
		for (int i = 0; i < potentialDraw; i++) {
			draw(deck, players[currentPlayer].hand, discardPile);
		}
		potentialDraw = 0;
		canRespond = false;
	}
	else {
		cout << "Drawing one card." << endl;
		draw(deck, players[currentPlayer].hand, discardPile);
	}
}

void cpuTurn(int currentPlayer, Card &lastPlayedCard, bool &canRespond, int &potentialDraw, vector<Card> &deck, vector<Card> &discardPile, vector<Player> &players, int &nextPlayer, int &iterator, int &turnCount) {
	Card playedCard = Card(0, 0, false, false, false);
	int RGBY[] = { 0, 0, 0, 0 };
	int cardToPlay = 0;
	int max = 0;
	char preferredColor;
	bool hasValidCard = false;
	for (auto card : players[currentPlayer].hand) {
		switch (card.color) {
		case('R'):
			RGBY[0]++;
			break;
		case('G'):
			RGBY[1]++;
			break;
		case('B'):
			RGBY[2]++;
		case('Y'):
			RGBY[3]++;
			break;
		default:
			break;
		}
	}
	for (int i = 0; i < 4; i++) {
		switch (i) {
		case(0):
			if (RGBY[i] > max) {
				max = RGBY[i];
				preferredColor = 'R';
			}
			break;
		case(1):
			if (RGBY[i] > max) {
				max = RGBY[i];
				preferredColor = 'G';
			}
			break;
		case(2):
			if (RGBY[i] > max) {
				max = RGBY[i];
				preferredColor = 'B';
			}
			break;
		case(3):
			if (RGBY[i] > max) {
				max = RGBY[i];
				preferredColor = 'Y';
			}
			break;
		default:
			break;
		}
	}
	//cout << "CPU's preferred color is " << preferredColor << endl;

	if (canRespond) { 
		for (auto card : players[currentPlayer].hand) {
			cardToPlay++;
			if (card.rank == lastPlayedCard.rank) {
				playedCard = card;
				specialCardLogic(card, turnCount, iterator, potentialDraw, players, nextPlayer, canRespond, deck, discardPile);
				break;
			}
		}
		canRespond = false;
	}
	else {
		//cout << "Not can respond" << endl;
		for (int i = 0; i < 3; i++) { // 3 steps of logic: 1. Do I have a color matching the current card? 2. Do I have a rank matching the current card? 3. Do I have a wild card?
			for (auto card : players[currentPlayer].hand) {
				cardToPlay++;
				if (card.color == lastPlayedCard.color && !card.isWild && i == 0) { // Matching color, not necessarily matching rank, not wild
					//cout << "Rule 1\n";
					hasValidCard = true;
					playedCard = card;
					break;
				}
				else if (card.rank == lastPlayedCard.rank && !hasValidCard && !card.isWild && i == 1) { // Can't be a matching color at this point, can be a matching rank, not wild
					//cout << "Rule 2\n";
					hasValidCard = true;
					playedCard = card;
					break;
				}
				else if (isValidCard(card, lastPlayedCard) && !hasValidCard && i == 2) { // Can't be a matching rank or color at this point, only a wild.
					//cout << "Rule 3\n";
					hasValidCard = true;
					playedCard = card;
					break;
				}
			}
			if (hasValidCard) {
				break;
			}
			else {
				cardToPlay = 0;
			}
		}
		if (!hasValidCard) {
			cout << players[currentPlayer].Name <<" does not have a valid card to play. They have drawn a card.\n";
			drawCardLogic(canRespond, potentialDraw, deck, players, discardPile, currentPlayer);
			return;
		}
		else {

			if (playedCard.isSpecial) {
				specialCardLogic(playedCard, turnCount, iterator, potentialDraw, players, nextPlayer, canRespond, deck, discardPile);
			}
			if (playedCard.isWild) {
				playedCard.color = preferredColor;
			}
			//cout << "Card to play: " << cardToPlay << endl;
			//players[currentPlayer].hand.erase(players[currentPlayer].hand.begin() + (cardToPlay - 1));
			//playedCard.status = played;
			////cout << players[currentPlayer].Name << " played : ";
			//displayCard(playedCard);
			//discardPile.push_back(playedCard);
			//lastPlayedCard = playedCard;
		}
	}
	players[currentPlayer].hand.erase(players[currentPlayer].hand.begin() + (cardToPlay - 1));
	playedCard.status = played;
	cout << players[currentPlayer].Name << " played : ";
	displayCard(playedCard);
	discardPile.push_back(playedCard);
	lastPlayedCard = playedCard;
	
}

void playUno() {
	welcomeScreen();
	int potentialDraw = 0;
	int cardToPlay;
	bool gamePower = powerOn;
	bool validCardPlayed;
	string wildColor;
	vector<Card> deck = createDeck();
	vector<Card> discardPile;
	vector<Card> playerHand;
	vector<Card> opponentHand;
	int iterator = 1;
	bool canRespond = false;
	string playerName;
	int playerCount = 0;
	vector<Player> players;
	int turnCount = players.size() * 50;
	int nextPlayer = 0;
	int currentPlayer = 0;
	Card lastPlayedCard = initRound(playerName, playerCount, players, deck, discardPile, potentialDraw);

	while (gamePower) {
		currentPlayer = turnCount % players.size();
		nextPlayer = (turnCount + iterator) % players.size();
		if (turnCount % players.size() == 0) {
			cout << "| CURRENT HAND SIZES |" << endl;
			for (auto player : players) {
				cout << player.Name << ": " << player.hand.size() << endl;
			}
			topDiscardInfo(lastPlayedCard, potentialDraw, canRespond); 
			cout << endl << "Your hand: " << endl; 
			for (auto cards : players[0].hand) { 
				displayCard(cards); 
			}
			if (devMode) {
				
				for (auto player : players) {
					if (player.isCPU) {
						cout << "\nHand of " << player.Name << " (devMode on)\n";
						for (auto cards : player.hand) {
							displayCard(cards);
						}
					}
				}
			}
			cout << "\nChoose a card number to play (or 0 to draw): ";
			if (!(cin >> cardToPlay)) { 
				cin.clear(); 
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Invalid input! Please enter a number." << endl;
				continue;
			}
			
			if (canRespond && cardToPlay > 0) { 
				Card playedCard = players[0].hand[cardToPlay - 1];
				if (playedCard.rank == lastPlayedCard.rank) {
					canRespond = false;
				}
			}
			if (cardToPlay > 0 && cardToPlay <= players[0].hand.size() && !canRespond) {
				Card playedCard = players[0].hand[cardToPlay - 1];
				if (isValidCard(playedCard, lastPlayedCard)) {
					playerCardLogic(players, colors, playedCard, cardToPlay, turnCount, iterator, potentialDraw, canRespond, deck, discardPile, wildColor, lastPlayedCard, nextPlayer); // 
				}
				else {
					cout << "Invalid play! Select another card or draw to pass your turn." << endl;
					continue;
				}
			}
			else if (!cardToPlay) {
				drawCardLogic(canRespond, potentialDraw, deck, players, discardPile, currentPlayer);
			}
			
			else {
				if (canRespond) {
					cout << "\nInvalid play!\nYou must play a card with the same draw effect as the one just played or input 0 to draw " << potentialDraw << "cards!\n";
				}
				else {
					cout << "\nInvalid play!\n"; 
				}
				continue;
			}
			//unoCheck(players[0].hand, gamePower);
			//turnCount += iterator;
		}
		else
		{
			cout << "CPU's turn" << endl;
			cpuTurn(turnCount % players.size(), lastPlayedCard, canRespond, potentialDraw, deck, discardPile, players, nextPlayer, iterator, turnCount);
			cout << endl;
		}
		turnCount += iterator;
		unoCheck(players[currentPlayer], gamePower);
		if (!players[currentPlayer].isCPU && gamePower) {
			clearScreen();
		}
	}
}

void mainMenu() {
	string userInput;
	bool programPower = powerOn;
	cout << "Input \"S\" to start playing!" << endl << "At any time, input \"X\" to terminate the program.\n Input \"D\" to launch the game in dev mode (hand is all wilds by default, can be changed in deal function.)" << endl;
	while (programPower) {
		cout << ">> ";
		cin >> userInput;
		switch (userInput[0]) {
		case(START):
			clearScreen();
			cout << "Game starting..." << endl;
			playUno();
			programPower = powerOff;
			break;
		case(TERMINATE):
			cout << "Terminating program... Thanks for playing!" << endl;
			programPower = powerOff;
			break;
		case(DEV_MODE):
			clearScreen();
			cout << "Game starting in dev mode..." << endl;
			devMode = true;
			playUno();
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