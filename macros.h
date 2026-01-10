#define skip (10)
#define reverse (11)
#define drawTwo (12)
#define wildCard (13)
#define wildDrawFour (14)
#define notWild false
#define wild true
#define notSpecial false
#define special true
#define played true
#define notPlayed false
#define powerOn true
#define powerOff false
//Uno specific
#define RED 'R'
#define GREEN 'G'
#define YELLOW 'Y'
#define BLUE 'B'

#define RED_TEXT     "\033[31m"
#define GREEN_TEXT   "\033[32m"
//32
#define YELLOW_TEXT  "\033[33m"
//33
#define BLUE_TEXT    "\033[34m"
#define PURPLE_TEXT "\033[35m"
#define ORANGE_TEXT "\033[38;5;208m"

//Traditional playing card Specific
#define SPADE 'S'
#define CLUB 'C' 
#define DIAMOND 'D'
#define HEART 'H'

//already defined red text
#define GREY_TEXT   "\033[37m"
#define WHITE_TEXT   "\033[97m"
#define BLACK_TEXT   "\033[32m"

//Ranks to keep internal logic clean
#define THREE (3)
#define FOUR  (4)
#define FIVE  (5)
#define SIX   (6)
#define SEVEN (7)
#define EIGHT (8)
#define NINE  (9)
#define TEN   (10)
#define JACK  (11)
#define QUEEN (12)
#define KING  (13)
#define ACE   (14)
#define TWO   (15)

#define RESET_TEXT   "\033[0m"
#define CLEAR_SCREEN "\033[2J\033[1;1H"
#define UNO (1)
#define WILD_COLOR 'W'
#define START 'S'
#define TERMINATE 'X'
#define userPlayer false
#define botPlayer true
#define DEV_MODE 'D'
#define BOT_MODE_CODE
bool devMode = false;
bool botMode = false;
#define HUMAN (0)
#define STARTING_HAND_SIZE (7)
#define DELAY (1s)