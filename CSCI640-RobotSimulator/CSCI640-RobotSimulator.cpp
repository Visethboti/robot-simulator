// CSCI640-RobotSimulator.cpp : 


#include <iostream>
#include <vector>
#include <algorithm>
#include <conio.h>

using namespace std;

class RobotSim {
    public:
        RobotSim(int width, int height, int numGold, bool playerOn):mapWidth(width), mapHeight(height), numGold(numGold) { // constructor
            this->playerOn = playerOn;

            // init map
            for (int i = 0; i < mapHeight; i++) {
                vector<char> row;
                for (int j = 0; j < mapWidth; j++) {
                    row.push_back('-');
                }
                map.push_back(row);
            }

            // random permutation for random position
            vector<int> v1, v2;
            for (int i = 0; i < mapHeight; i++)
                v1.push_back(i);
            for (int i = 0; i < mapWidth; i++)
                v2.push_back(i);

            srand(unsigned(time(0)));
            random_shuffle(v1.begin(), v1.end());
            random_shuffle(v2.begin(), v2.end());

            // init player
            ry = v1[0];
            rx = v2[0];
            map[ry][rx] = 'R'; // R = robot(player) // ry height, rx width

            // init bomb
            by = v1[1];
            bx = v2[1];
            map[by][bx] = 'B'; // B = bomb

            // init gold
            for (int i = 0; i < numGold; i++) {
                map[v1[i+2]][v2[i+2]] = 'G'; // G = gold
            }
        } 
        ~RobotSim() {} // destructor

        void start() {
            numGoldLeft = numGold;
            numGoldCollected = 0;
            robotMoveCounter = 0;
            char key;
            vector<char> possibleMove;

            bool win = true;
            
            do {
                // print screen
                system("cls");
                printStatus();
                printMap();

                // get move
                if (playerOn) { // get move from player
                    // ask for move
                    do {
                        key = _getch();
                    } while (key != 'w' && key != 's' && key != 'a' && key != 'd');

                    

                    
                }
                else { // get move from AI
                    
                    // check each possible square
                    possibleMove = check_all_next_square();
                    

                    // randomly select a move
                    random_shuffle(possibleMove.begin(), possibleMove.end());
                    key = possibleMove[0];
                    possibleMove.clear();
                }

                
                // remove previous position
                map[ry][rx] = '-';

                // apply move
                if (key == 'w') {
                    if (ry > 0)
                        ry--;
                }
                else if (key == 's') {
                    if (ry + 1 < mapHeight)
                        ry++;
                }
                else if (key == 'a') {
                    if (rx > 0)
                        rx--;
                }
                else if (key == 'd') {
                    if (rx + 1 < mapWidth)
                        rx++;
                }

                // check for gold
                if (check_for_gold()) {
                    numGoldLeft--;
                    numGoldCollected++;
                }

                // check for robot step on bomb
                else if (check_for_bomb()) {
                    win = false;
                    map[ry][rx] = 'X';
                    break;
                }

                map[ry][rx] = 'R';
                robotMoveCounter++;

                // bomb move
                if (robotMoveCounter % 2 == 0) { // only move after every 2 robot moves
                    possibleMove = check_all_bomb_next_square();
                    if (possibleMove.size() > 0) { // bomb can move
                        random_shuffle(possibleMove.begin(), possibleMove.end());
                        key = possibleMove[0];

                        map[by][bx] = '-';

                        // apply move
                        if (key == 'w') {
                            if (by > 0)
                                by--;
                        }
                        else if (key == 's') {
                            if (by + 1 < mapHeight)
                                by++;
                        }
                        else if (key == 'a') {
                            if (bx > 0)
                                bx--;
                        }
                        else if (key == 'd') {
                            if (bx + 1 < mapWidth)
                                bx++;
                        }

                        map[by][bx] = 'B';

                        // check for robot step on bomb
                        if (check_for_bomb()) {
                            win = false;
                            map[ry][rx] = 'X';
                            break;
                        }
                    }
                    else { // bomb cant move
                        // nothing happen, bomb do not move
                    }
                    
                }

            } while (numGoldLeft > 0);

            // end game screen
            system("cls");
            printStatus();
            printMap();

            if (win)
                cout << "All gold collected!" << endl;
            else
                cout << "!!! Robot stepped on a bomb !!!" << endl;

            cout << "Robot Simulator Ended" << endl;
        }

        void printMap() {
            for (int i = 0; i < mapHeight; i++) {
                cout << "|";
                for (int j = 0; j < mapWidth; j++) {
                    cout << map.at(i).at(j) << "|";
                }
                cout << endl;
            }
        }

        void setMap(vector<vector<char>> newMap) {
            map = newMap;
        }

    private:
        const int mapWidth, mapHeight, numGold;
        vector<vector<char>> map;
        bool playerOn;

        int rx, ry, bx, by;
        int robotMoveCounter;
        int numGoldLeft, numGoldCollected;

        vector<char> check_all_next_square() {
            vector<char> result;
            if (check_next_square(rx + 1, ry)) {
                result.push_back('d');
            }
            if (check_next_square(rx - 1, ry)) {
                result.push_back('a');
            }
            if (check_next_square(rx, ry + 1)) {
                result.push_back('s');
            }
            if (check_next_square(rx, ry - 1)) {
                result.push_back('w');
            }
            return result;
        }

        vector<char> check_all_bomb_next_square() {
            vector<char> result;
            if (check_next_bomb_moves(bx + 1, by)) {
                result.push_back('d');
            }
            if (check_next_bomb_moves(bx - 1, by)) {
                result.push_back('a');
            }
            if (check_next_bomb_moves(bx, by + 1)) {
                result.push_back('s');
            }
            if (check_next_bomb_moves(bx, by - 1)) {
                result.push_back('w');
            }
            return result;
        }

        bool check_next_square(int x, int y) { // true if possible to move (no bomb, no edge)
            if (x < 0 || y < 0 || x == mapWidth || y == mapHeight)
                return false;
            if (map[y][x] == 'B')
                return false;
            

            return true;
        }

        bool check_next_bomb_moves(int x, int y) { // true if possible to move (no gold, no edge)
            if (x < 0 || y < 0 || x == mapWidth || y == mapHeight)
                return false;
            if (map[y][x] == 'G')
                return false;


            return true;
        }

        bool check_for_gold() {
            if (map[ry][rx] == 'G')
                return true;

            return false;
        }

        bool check_for_bomb() {
            if (map[ry][rx] == 'B')
                return true;

            return false;
        }

        void printStatus() {
            cout << "robotMoveCounter: " << robotMoveCounter << endl;
            cout << "numGoldLeft: " << numGoldLeft << endl;
        }

        friend class TestRobotSim;
};

class TestRobotSim{
    public:
        TestRobotSim() {} // constructor
        ~TestRobotSim() {} // destructor

        void testAll() {
            testOne(1);
            testOne(2);
            testOne(3);
        }

    private: 
        RobotSim robotsim_test = RobotSim(4, 4, 2, false);

        bool test01() { // test when there is gold 
            vector<vector<char>> map = { {'R','G','-','-'},
                                         {'G','-','-','-'},
                                         {'-','-','B','-'},
                                         {'-','-','-','-'}, };
            robotsim_test.setMap(map);

            bool expected = true;
            if (expected != robotsim_test.check_next_square(1, 0)) {
                return false;
            }

            return true;
        }

        bool test02() { // test when there is nothing 
            vector<vector<char>> map = { {'R','-','-','G'},
                                         {'-','-','-','-'},
                                         {'-','-','B','-'},
                                         {'-','G','-','-'}, };
            robotsim_test.setMap(map);

            bool expected = true;
            if (expected != robotsim_test.check_next_square(1, 0)) {
                return false;
            }

            return true;
        }

        bool test03() { // test when there is bomb
            vector<vector<char>> map = { {'R','B','-','-'},
                                         {'B','-','-','-'},
                                         {'-','-','B','-'},
                                         {'-','-','-','-'}, };
            robotsim_test.setMap(map);

            bool expected = false;
            if (expected != robotsim_test.check_next_square(1, 0)) {
                return false;
            }

            return true;
        }

        void testOne(int num) {
            if (num <= 0 || num > 5) {
                cout << "CPassword: Error: invalid test case number." << endl;
                return;
            }
            else {
                bool result = false;
                switch (num) {
                case 1:
                    result = test01();
                    break;
                case 2:
                    result = test02();
                    break;
                case 3:
                    result = test03();
                    break;
                }

                if (result) {
                    cout << "Test0" << num << ": PASSED." << endl;
                }
                else {
                    cout << "Test0" << num << ": FAILED." << endl;
                }
            }
        }
};


int main()
{
    RobotSim robotSim(5, 5, 2, false); // ({int:mapWidth},{int:mapHeight},{int:numGold},{bool:playerOn})
    robotSim.start();


    // unit testing
    //TestRobotSim testRobotSim;
    //testRobotSim.testAll();
}
