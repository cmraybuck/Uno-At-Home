
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include "macros.h"
#include <chrono>
#include <thread>
using namespace std;
using namespace chrono_literals;
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
	cout << "\n| Discard pile shuffled into deck |\n" << endl;
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
		Card devWildDrawFour = Card(wildDrawFour, WILD_COLOR, wild, special, notPlayed);
		Card devDrawTwo = Card(drawTwo, BLUE, notWild, special, notPlayed);
		for (int i = 0; i < STARTING_HAND_SIZE; i++) {
			humanPlayer.hand.push_back(devWildDrawFour);
		}
		for (int x = 0; x < numOpp; x++) {
			for (int i = 0; i < STARTING_HAND_SIZE; i++) {
				//players[x + 1].hand.push_back(devWildDrawFour);
				draw(deck, players[x + 1].hand, discardPile);
			}
		}
	}
	else {
		for (int i = 0; i < STARTING_HAND_SIZE; i++) {
			draw(deck, humanPlayer.hand, discardPile);
		}
		for (int x = 0; x < numOpp; x++) {
			for (int i = 0; i < STARTING_HAND_SIZE; i++) {
				draw(deck, players[x + 1].hand, discardPile);
			}
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
		if (!currentPlayer.isCPU && !botMode) {
			cout << "========================================" << endl;
			cout << "              YOU WIN!                \n";
			cout << "========================================" << endl;
			gamePower = powerOff;
		}
		else {
			cout << "========================================" << endl;
			cout << "              " << currentPlayer.Name << " WINS!      \n";
			cout << "========================================" << endl;
			gamePower = powerOff;
		}

	}
	return gamePower;
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
	cout << endl << "Last played card: ";
	displayCard(lastPlayedCard);
	if (lastPlayedCard.isWild) {
		cout << "\nCard color: " << lastPlayedCard.GetColor(lastPlayedCard.color);
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

void wildCardLogic(Player player, Card& playedCard, string colors, string& wildColor) {
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

void specialCardLogic(Card playedCard, int& turnCount, int& iterator, int& potentialDraw, vector<Player>& players,
	int& nextPlayer, bool& canRespond, vector<Card>& deck, vector<Card>& discardPile, int currentPlayer) {
	switch (playedCard.rank) {
	case(skip): // Increment iterator so next player is skipped
		turnCount += iterator;
		cout << players[currentPlayer].Name << " has skipped " << players[nextPlayer].Name << "'s turn! \n";
		break;
	case(reverse): // Reverse turn order iterator
		cout << players[currentPlayer].Name << " has reversed the turn order!\n";
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
			cout << endl << players[currentPlayer].Name << " played a draw 2 on " << players[nextPlayer].Name << "! \n" << players[nextPlayer].Name << " does not have a card to stack and has drawn " << potentialDraw << " cards and lost their turn for this round.\n";
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
			cout << endl << players[currentPlayer].Name << " played a draw 4 on " << players[nextPlayer].Name << "! \n" << players[nextPlayer].Name << " does not have a card to stack and has drawn " << potentialDraw << " cards and lost their turn for this round.\n";
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

void playerCardLogic(vector<Player>& players, string colors, Card& playedCard,
	int& cardToPlay, int& turnCount, int& iterator,
	int& potentialDraw, bool& canRespond, vector<Card>& deck,
	vector<Card>& discardPile, string& wildColor, Card& lastPlayedCard,
	int& nextPlayer) {
	players[0].hand.erase(players[0].hand.begin() + (cardToPlay - 1));
	playedCard.status = played;
	if (playedCard.isWild) {
		cout << "WILD CARD! Select your color (R,G,B,Y): ";
		wildCardLogic(players[0], playedCard, colors, wildColor);
	}
	if (playedCard.isSpecial) {
		specialCardLogic(playedCard, turnCount, iterator, potentialDraw, players, nextPlayer, canRespond, deck, discardPile, HUMAN);
	}
	discardPile.push_back(playedCard);
	lastPlayedCard = playedCard;
}

void drawCardLogic(bool& canRespond, int& potentialDraw, vector<Card>& deck, vector<Player>& players, vector<Card>& discardPile, int currentPlayer) {
	if (canRespond) {
		for (int i = 0; i < potentialDraw; i++) {
			draw(deck, players[currentPlayer].hand, discardPile);
		}
		potentialDraw = 0;
		canRespond = false;
	}
	else {
		draw(deck, players[currentPlayer].hand, discardPile);
	}
}

void cpuTurn(int currentPlayer, Card& lastPlayedCard, bool& canRespond, int& potentialDraw, vector<Card>& deck, vector<Card>& discardPile, vector<Player>& players, int& nextPlayer, int& iterator, int& turnCount) {
	Card playedCard = Card(0, 0, false, false, false);
	int RGBY[] = { 0, 0, 0, 0 };
	int cardToPlay = 0;
	int max = 0;
	char preferredColor = getRandomColor();
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

	if (canRespond) {
		canRespond = false;
		for (auto card : players[currentPlayer].hand) {
			cardToPlay++;
			if (card.rank == lastPlayedCard.rank) {
				playedCard = card;
				specialCardLogic(card, turnCount, iterator, potentialDraw, players, nextPlayer, canRespond, deck, discardPile, currentPlayer);
				if (playedCard.isWild) {
					playedCard.color = preferredColor;
				}
				break;
			}
		}
	}
	else {
		for (int i = 0; i < 3; i++) { // 3 steps of logic: 1. Do I have a color matching the current card? 2. Do I have a rank matching the current card? 3. Do I have a wild card?
			for (auto card : players[currentPlayer].hand) {
				cardToPlay++;
				if (card.color == lastPlayedCard.color && !card.isWild && i == 0) { // Rule 1: Matching color, not necessarily matching rank, not wild
					hasValidCard = true;
					playedCard = card;
					break;
				}
				else if (card.rank == lastPlayedCard.rank && !hasValidCard && !card.isWild && i == 1) { // Rule 2: Not matching color, matching rank, not wild
					hasValidCard = true;
					playedCard = card;
					break;
				}
				else if (isValidCard(card, lastPlayedCard) && !hasValidCard && i == 2) { // Rule 3: Not matching color or rank, card is wild
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
			cout << players[currentPlayer].Name << " does not have a valid card to play. " << players[currentPlayer].Name << " has drawn a card.\n";
			drawCardLogic(canRespond, potentialDraw, deck, players, discardPile, currentPlayer);
			this_thread::sleep_for(DELAY);
			return;
		}
		else {
			if (playedCard.isSpecial) {
				specialCardLogic(playedCard, turnCount, iterator, potentialDraw, players, nextPlayer, canRespond, deck, discardPile, currentPlayer);
			}
			if (playedCard.isWild) {
				playedCard.color = preferredColor;
			}
		}
	}
	players[currentPlayer].hand.erase(players[currentPlayer].hand.begin() + (cardToPlay - 1));
	playedCard.status = played;
	cout << players[currentPlayer].Name << " played : ";
	displayCard(playedCard);
	this_thread::sleep_for(DELAY);
	discardPile.push_back(playedCard);
	lastPlayedCard = playedCard;

}

void sortHand(vector<Card>& hand, int type = 1) {
	//I am lazy and am using bubble sort, the efficiencies don't matter too much with as small of a scale. 
	int n = hand.size();
	if (type == 0)
	{
		for (int i = 0; i < n - 1; i++) 
		{

				bool flag = false;
				for (int j = 0; j < n - i - 1; j++) 
				{
					if (hand[j].rank > hand[j + 1].rank) 
					{

						swap(hand[j], hand[j + 1]);

						flag = true;
					}
				}

				if (!flag)
					break;
			}
	}
	else 
		//Added the sorting for the color first, then sorts by the number. Unsure if wanting to remove just number sorting or allow swapping but leaving in the swapping as an option.
	{
		for (int i = 0; i < n - 1; i++)
		{

			bool flag = false;
			for (int j = 0; j < n - i - 1; j++)
			{
				if (hand[j].color < hand[j + 1].color)
				{

					swap(hand[j], hand[j + 1]);

					flag = true;
				}
			}

			for (int j = 0; j < n - i - 1; j++)
			{
				if (hand[j].color == hand[j + 1].color)
				{

					if (hand[j].rank > hand[j + 1].rank)
					{

						swap(hand[j], hand[j + 1]);

						flag = true;
					}
				}
			}

			if (!flag)
				break;
		}
	}
	
}
//Kind of reworking the way the card is selected, some UI tweaks are definitely needed but personally I like where it is at. 
int selectCard(vector<Card>& hand)
{
	int selectedCard = 0;
	char input;

	while (true)
	{

		for (int i = 0; i < hand.size(); i++)
		{
			if (i == selectedCard)
			{
				cout << ">"; 
			}
			else {
				cout << " ";
			}
			displayCard(hand[i]);
		}
		cout << endl << "Use [A] left, [D] right, [P] play the selected card, [O] for drawing a card." << endl << "> ";

		while (!(cin >> input))
		{
			cout << "Invalid input. Try again with a valid name";
			cin.clear();
			cin.ignore(10000, '\n');
		}
		input = tolower(input);
		if (input == 'a' && selectedCard > 0)
		{
			selectedCard--;
		}
		else if (input == 'd' && selectedCard < hand.size() - 1)
		{
			selectedCard++;
		}
		else if (input == 'p') 
		{
			return selectedCard;
		}
		else if (input == 'o')//New way to determine wanting to draw.
		{
			return -1000;
		}
		else 
		{
			cout << endl << "Improper input11! " << endl;
		}

	}
}

void botModeSpectate(vector<Player> players) {
	Player leadingPlayer = players[0];
	int min = leadingPlayer.hand.size();
	cout << "| CURRENT HAND SIZES |" << endl;
	for (auto player : players) {
		cout << player.Name << ": " << player.hand.size() << endl;
	}
	cout << "\n\n-----------------------";
	for (auto player : players) {
		if (player.hand.size() < min) {
			leadingPlayer = player;
		}
		cout << "\nHand of " << player.Name << endl;
		for (auto cards : player.hand) {
			displayCard(cards);
		}
		cout << endl;
	}

	cout << "Current leading player: " << leadingPlayer.Name << "!\n";

	cout << "\n\n-----------------------\n";

}

Card initRound(string& playerName, int& playerCount, vector<Player>& players, vector<Card>& deck, vector<Card>& discardPile, int& potentialDraw) {
	cout << "Enter your name: ";
	while (!(cin >> playerName))
	{
		cout << "Invalid input. Try again with a valid name";
		cin.clear();
		cin.ignore(10000, '\n');
	}
	if (playerName == "CPU0") {
		botMode = true;
	}

	cout << "Enter the total amount of opponents you want (maximum of 10 for now): ";
	while (!(cin >> playerCount))
	{
		cout << "Invalid input. Try again with ";
		cin.clear();
		cin.ignore(10000, '\n');
	}
	clearScreen();
	cout << "Bot mode active. Enjoy the show!\n";
	cout << "A total of " << playerCount + 1 << " players are competing. Good luck!\n" << endl;

	initPlayers(players, playerName, playerCount);
	dealCards(deck, players[0], players, playerCount, discardPile);
	Card firstCard = draw(deck, discardPile, discardPile);
	if (firstCard.isSpecial) {
		while (firstCard.isSpecial) {
			firstCard = draw(deck, discardPile, discardPile);
		}
	}
	firstCard.status = played;
	return firstCard;
}

void playUno() {
	//Commented out some of the variables that are no longer useful
	welcomeScreen();
	int potentialDraw = 0;
	int cardToPlay;
	bool gamePower = powerOn;
	//bool validCardPlayed;
	string wildColor;
	vector<Card> deck = createDeck();
	vector<Card> discardPile;
	//vector<Card> playerHand;
	//vector<Card> opponentHand;
	int iterator = 1;
	bool canRespond = false;
	string playerName;
	int playerCount = 0;
	vector<Player> players;
	int turnCount = players.size() * 50;
	int nextPlayer = 0;
	int currentPlayer = 0;
	Card lastPlayedCard = initRound(playerName, playerCount, players, deck, discardPile, potentialDraw);
	int round = 1;
	while (gamePower) {
		if (round == players.size())
		{
			botModeSpectate(players);
			round = 1;
		}
		currentPlayer = turnCount % players.size();
		nextPlayer = (turnCount + iterator) % players.size();
		if (turnCount % players.size() == 0 && !botMode) {
			cout << "| CURRENT HAND SIZES |" << endl;
			for (auto player : players) {
				cout << player.Name << ": " << player.hand.size() << endl;
			}
			topDiscardInfo(lastPlayedCard, potentialDraw, canRespond);

			sortHand(players[0].hand);

			cout << endl << "Your hand: " << endl;
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
			if (!canRespond) {
				cardToPlay = selectCard(players[0].hand) + 1; 
			}
			else {
				cout << "\nYou must play a card with the same draw effect as the last one or input 0 to draw " << potentialDraw << " cards!\n";
				cin >> cardToPlay; 
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
					clearScreen();
					playerCardLogic(players, colors, playedCard, cardToPlay, turnCount, iterator, potentialDraw, canRespond, deck, discardPile, wildColor, lastPlayedCard, nextPlayer); // 
				}
				else {
					cout << "Invalid play! Select another card or draw to pass your turn." << endl;
					continue;
				}
			}
			else if (cardToPlay == -999) { // slightly adjusted the execution to fit the new method. Also added a draw message since there wasn't one.
				cout << players[0].Name << " drew a card." << endl;
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
		}
		else
		{
			cout << players[currentPlayer].Name << "'s turn!\n";
			cout << "Last played card: ";
			displayCard(lastPlayedCard);
			if (lastPlayedCard.isWild) {
				cout << " (Chosen color: " << lastPlayedCard.GetColor(lastPlayedCard.color) << ") \n";
			}
			cout << endl;
			this_thread::sleep_for(DELAY);
			cpuTurn(currentPlayer, lastPlayedCard, canRespond, potentialDraw, deck, discardPile, players, nextPlayer, iterator, turnCount);
			cout << endl;
			if (botMode) round++;
		}
		turnCount += iterator;
		unoCheck(players[currentPlayer], gamePower);
	}
}

void mainMenu() {
	string userInput;
	bool programPower = powerOn;
	cout << "Input \"S\" to start playing!" << endl << "At any time, input \"X\" to terminate the program.\nInput \"D\" to launch the game in dev mode (hand is all wilds by default, can be changed in deal function.)" << endl;
	while (programPower) {
		cout << ">> ";
		cin >> userInput;
		switch (toupper(userInput[0])) {
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
