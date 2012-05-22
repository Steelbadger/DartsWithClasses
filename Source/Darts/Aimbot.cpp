//===========Game of Darts, 2 Computer Controlled Characters==========
//===========================By Ross Davies===========================
//
//=====This Program will prompt the user for the number of points=====
//======to play from, and will then simulate a game between two=======
//====players until one wins, with the requirement that, to win,======
//===============the player MUST finish with the bull=================
//====================================================================


#include <iostream>
#include <cstdlib>
#include <string>
#include <time.h>

using namespace std;


//  Array of all outer board points, in clockwise order.
const int boardpoints[20] =  {20, 1, 18, 4, 13, 6, 10, 15, 2, 17, 3, 19, 7, 16, 8, 11, 14, 9, 12, 5};

const int densitysize = 12;
int throwsarray[densitysize] = {0};



//  Class to hold all values pertinent to each participant in the game.
class Player {

public:

	Player(int accuracy = 70, int outeraccuracy = 80, int points = 301, string name = "Default"); 
	
	void TakeAShot(bool displaytext);  
	void ResetPoints(int points);
	int GetPoints() const;
	string GetName() const;

	int wins;
private:
	int m_target;  // Holds the points value of the board location at which the player is aiming
	int m_hit;  // Holds value of last throw player made
	int m_accuracy; // Holds player's accuracy when aiming for bull
	int m_outeraccuracy;
	int m_points; // Holds the player's current points

	string m_name;  // Holds the player's name

	int TargetSelect();
	int Inaccuracy();
	int OuterBoardInaccuracy();
	int BullInaccuracy(); 
	bool PointsLegality(); 
};

int games = 0;
int playernumber = 1;
int playerplaying = 0;
int basethrows = 7;

void AssignThrowsToDensity(int throws);
void SimulatePerfectGame(int points);
void DisplayResults(int playernumber, int games, int totalthrows, Player joe, Player sid);

void main() {

	//srand(time(NULL));

	int points = 0;  // To hold points to be input by player later
	int throws = 0;
	int totalthrows = 0;

	int playfirstinput = 0;
	int playfirst = 0;
	bool alternatestarter = 0;
	bool displaygametext = 0;


	// Set up Game, prompt for points to start.
	cout << "Simulate One or Two player game [1 or 2]: ";
	cin >> playernumber;
	cout << endl;

	if (playernumber == 1) {
		cout << "Which player to simulate? [1 = Joe, 2 = Sid]: ";
		cin >> playerplaying;
		cout << endl;
	}


	cout << "Enter starting points : ";
	cin >> points;
	cout << endl;


	cout << "Enter number of games to simulate: ";
	cin >> games;
	cout << endl;
	
	if (playernumber == 2) {
		cout << "Who throws first? [Joe = 0, Sid = 1, RandomStart Thereafter Loser First = 2]: " << endl;
		cin >> playfirstinput;
		cout << endl;
	}



	//  Prepare for simulation

	Player joe(70, 80, points, "Joe");
	Player sid(72, 80, points, "Sid");


	//  Find fewest possible throws to complete in, this becomes lowest value of the density holder throwsarray
	SimulatePerfectGame(points);

	if (playfirstinput == 2) {
		playfirstinput = rand()%2;

		alternatestarter = 1;
	}

	if (games == 1) {
		displaygametext = 1;
	}

	//  Play all games
	for (int c = games; c > 0; c--) {
	
		//  Reset points and throws for each new game
		joe.ResetPoints(points);
		sid.ResetPoints(points);
		throws = 0;
		
	
		if (alternatestarter == 0) {
			playfirst = playfirstinput;
		}


	//  Play a Game, Joe throws first, keep throwing until one player hits zero.
		while (joe.GetPoints() != 0 && sid.GetPoints() != 0) {
			
			
			if ((playernumber == 2 || (playernumber == 1 && playerplaying == 1)) && playfirst == 0) {
				joe.TakeAShot(displaygametext);
				throws++;
			}

			if (joe.GetPoints() == 0)
				break;
			if (playernumber == 2 || (playernumber == 1 && playerplaying == 2)) {
				sid.TakeAShot(displaygametext);
				if ((playernumber == 1 && playerplaying == 2) || (playfirst == 1 && playernumber == 2)) {
					throws++;
					playfirst = 0;
				}
			}
		}
		totalthrows += throws;

		if (playernumber == 1) {
			AssignThrowsToDensity(throws);
		}

		//  Declare winner once game is complete.

		if (joe.GetPoints() == 0) {
			if (games == 1) {
				cout << endl << joe.GetName() << " Wins!" << endl << throws << " Throws to win the game." << endl << endl;
			}
			joe.wins += 1;
			if (alternatestarter == 1) {
				playfirst = 1;
			}

		} else {
			if (games == 1) {
				cout << endl << sid.GetName() << " Wins!" << endl << throws << " Throws to win the game." << endl << endl;
			}
			sid.wins += 1;
			if (alternatestarter == 1) {
				playfirst = 0;
			}
		}	
	}

	DisplayResults(playernumber, games, totalthrows, joe, sid);

	

}


//  Constructor, assigns player's starting points, accuracy and name to display, also provides visual
//  confirmation of new player's creation
Player::Player(int bullaccuracy, int outeraccuracy, int points, string name) :
	m_accuracy(bullaccuracy),
	m_points(points),
	m_name(name),
	wins(0),
	m_outeraccuracy(outeraccuracy)
{
	if (playernumber == 2 && m_name != "Default") {
		cout << m_name << " enters the Game!" << endl;
	}
}



void Player::TakeAShot(bool displaytext) {
//  Primary function for playing game, calls other private functions in player class
//  First decides a target, then checks if that's the bull or on the outer board, then check to see if target is hit
//  and returns text at each step to keep user informed of each player's actions.
//  Also checks legality of throw, if illegal then resets points to what they were before throw.
	
	//  Select a target
	m_target = TargetSelect();

	//  Check for misses and return what gets hit
	m_hit = Inaccuracy();
	if (displaytext) {
		cout << endl << m_name << " Aimed for " << m_target << " and hit " << m_hit << endl;
	}
	
	//  Check to ensure new score is legal
	if (!PointsLegality()) {
		if (displaytext) {
			cout << m_name << " Got an Illegal Score ("<< m_points - m_hit << ") Points reset to " << m_points << endl;
		}
	} else {
	m_points -= m_hit;
	}
	
	//  Display an update of the player's points
	if (displaytext) {
		cout << m_name << "'s Points: " << m_points << endl;	
	}
}

//  Reset the points for a new game
inline void Player::ResetPoints(int points) {
	m_points = points;
}

//  Simple function for robust calling of m_points value
inline int Player::GetPoints() const {	
	return m_points;
}

//  Simple function for robust calling of m_name value
inline string Player::GetName() const {
	return m_name;
}




int Player::TargetSelect() {
//  Decides optimal points for which to aim, includes requirement that bull must be got to finish

	//  interim value for holding current target
	int calc = 0;

	
	//  when points is greater than 100, aim for 50, between 50 and 100 choose the highest value that yields
	//  a number higher than 50, at 50 then aim for 50 to finish.  Return 0 if any other situation occurs
	if (m_points >= 50+50) {
		return 50;
	} else if (m_points > 50 && m_points < 50+50) {
		calc = m_points - 50;
		if (calc > 20) {
			return 20;
		} else {
			return calc;
		}
	} else if (m_points == 50){
		return 50;
	} else {
		return 0;
	}

}


int Player::Inaccuracy () {
//  Calls relevant innaccuracy function depending on location of target (bull or outer board), returns the points value hit

	//  Interim value to hold location hit
	int output = 0;

	//  Call inaccuracy function for the area of the board being aimed at, bull for bull, outerboard for outerboard
	if (m_target > 0 && m_target <= 20) {
		output = OuterBoardInaccuracy();
	} else if (m_target == 50) {
		output = BullInaccuracy();
	}

	return output;
}


int Player::OuterBoardInaccuracy() {
//  Checks to see if player hits target, if a miss happens then decides what is hit instead, chooses between two
//  adjacent values in boardpoints array

	//  Interim values to hold location and value of target/hit location
	int output = 0;
	int index = 0;

	//  If a miss happens then find the index value of the aimed for points in the boardpoints array
	//  Half the time increment up, rest of time increment down.
	//  If no miss happens then skip all that and return the target as the item hit.
	if ((rand()*100)/RAND_MAX > m_outeraccuracy) {
		while (boardpoints[index] != m_target) {  
			index++;
		}
		if ((rand()*100)/RAND_MAX > 50) {	
			if (index < 1) {				
				index = 19;
			} else {
				index -=1;
			}
		} else {
			if (index > 18) {
				index = 0;
			} else {
				index += 1;
			}
		}

		output = boardpoints[index];		
	} else {
		output = m_target;	
	}
	return output;
}



int Player::BullInaccuracy() {
//  Checks to see if player hits target (specific case for aiming for bull)
//  if a miss occurs then choose random value from boardpoints array
//  otherwise output 50 (value of bull)

	int output = 0;
	int randm = 0;

	//  If a miss happens then return a value from 0 to 19, use this as index in boardpoints array for
	//  Value hit.
	if ((rand()*100)/RAND_MAX > m_accuracy) {

		do {
			randm = rand();
		} while (randm > 32700);
		randm = randm%20;

		output = boardpoints[randm];
	} else {
		output = 50;
	}

	return output;

}


bool Player::PointsLegality() {
//  if score would be less than zero, or between 0 and 50 (non inclusive) then return false (illegal throw)
//  Otherwise return true.
	if ((m_points - m_hit) < 0 || ((m_points - m_hit) > 0 &&(m_points - m_hit < 50))) {
		return false;
	} else {
		return true;
	}
}

void AssignThrowsToDensity(int throws) {
//  This function maintains the throw frequency density array
//  By subtracting the base from the actual we find the position
//  of that throw number in the array, then add one to that position

	throws -= basethrows;
	if (throws > densitysize - 1) {
		throws = densitysize-1;
	}

	throwsarray[throws] = throwsarray[throws] + 1;

}

void SimulatePerfectGame(int points) {
//  This function finds the best possible number of throws to finish a game
//  By playing a single game with a perfect player

	Player perfect(100, 100, points, "Default");

	basethrows = 0;
	while (perfect.GetPoints() != 0) {
		perfect.TakeAShot(0);
		basethrows++;
	}
}

void DisplayResults(int playernumber, int games, int totalthrows, Player joe, Player sid) {
//  This function displays all the behavioural data gathered over the course of the program run

	int alltotalthrows = 0;

	for (int i = 0; i < densitysize; i++) {
		alltotalthrows += throwsarray[i];
	}

	if (playernumber == 2 && games > 1) {
		cout << endl << endl << joe.GetName() << " Won " << joe.wins << " of " << games << " Games" << "[" << ((float)joe.wins/games)*100 << "%]" << endl;
		cout << endl << endl << sid.GetName() << " Won " << sid.wins << " of " << games << " Games" << "[" << ((float)sid.wins/games)*100 << "%]"  << endl;
	}
	float averagethrows =(float) totalthrows/games;
	if (playernumber == 1 && games > 1) {
		cout << endl << endl << "Average throws to win: " << averagethrows << endl;
		
		for (int i = 0; i < (densitysize-1); i++) {
			cout << "Games completed in " << basethrows + i << " throws \t = \t" << throwsarray[i] << "\t[" << ((float)throwsarray[i]/games*100) << "%]" << endl;
		}

		cout << "Games completed in more than " << basethrows + densitysize-1 << " throws \t = \t" << throwsarray[densitysize-1] << "\t[" << ((float)throwsarray[densitysize-1]/games*100) << "%]" << endl;
	}

	while(true){}
}


