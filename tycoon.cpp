
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
string colors = "SHCD";


class Card
{
public:
	int rank;
	char suit;
	bool isWild;
	bool isSpecial;
	bool status;
	Card(int r, char s, bool w, bool spec, bool stat) {
		rank = r;
		suit = s;
		isWild = w;
		isSpecial = spec;
		status = stat;

	}
	string GetColor(char color) {
		switch (color) {
		case(SPADE):
			return "Black";
			break;
		case(HEART):
			return "Grey";
			break;
		case(CLUB):
			return "Red";
			break;
		case(DIAMOND):
			return "White";
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
	char suits[] = { 'S', 'H', 'C', 'D' };
	for (char s : suits) {

		// Deal two of each rank for each color
		for (int i = THREE; i <= TWO; i++) 
		{
			if ((i != TWO) && (i != EIGHT)) 
			{
				deck.push_back(Card(i, s, notWild, notSpecial, notPlayed));


			}
		}
		// The two and 8 are here
		deck.push_back(Card(2, s, notWild, special, notPlayed));
		deck.push_back(Card(8, s, notWild, special, notPlayed));
		
	}
	// Two Joers
		deck.push_back(Card(16, 'j', wild, special, notPlayed)); 
		deck.push_back(Card(16, 'j', wild, special, notPlayed)); 
	
	// Shuffle
	random_device rd;
	mt19937 g(rd());
	shuffle(deck.begin(), deck.end(), g);

	return deck;
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
			//clearScreen();
			cout << "Game starting..." << endl;
			//playUno();
			programPower = powerOff;
			break;
		case(TERMINATE):
			cout << "Terminating program... Thanks for playing!" << endl;
			programPower = powerOff;
			break;
		case(DEV_MODE):
			//clearScreen();
			cout << "Game starting in dev mode..." << endl;
			devMode = true;
			//playUno();
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

*/