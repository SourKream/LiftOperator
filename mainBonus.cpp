#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <ctime>
#include <climits>
#include <string>
#include <cstdlib>
#include <map>
#include <time.h>

using namespace std;


int N, K, maxIter;
float p, q, r, discount;

class State{

public:

    vector<bool> UpButtons;
    vector<bool> DownButtons;
    vector<int> LiftPositions;

    State(){
        for (int i=0; i<N; i++){
            UpButtons.push_back(false);
            DownButtons.push_back(false);
        }
        for (int i=0; i<K; i++){
            LiftPositions.push_back(0);
        }
    }

    void applyMyAction (int action){

        for (int i=0; i<K; i++){
            switch(action % 5){
                case 0:
                    break;
                case 1: LiftPositions[i]++;
                    break;
                case 2: LiftPositions[i]--;
                    break;
                case 3: UpButtons[LiftPositions[i]] = false;
                    break;
                case 4: DownButtons[LiftPositions[i]] = false;
                    break;
            }
            action /= 5;
        }
    }

    void printMyAction (int action) {

        for (int i=0; i<K; i++){
            switch(action % 5){
                case 0: cout << "AS" << i+1 << " ";
                    break;
                case 1: cout << "AU" << i+1 << " ";
                    break;
                case 2: cout << "AD" << i+1 << " ";
                    break;
                case 3: cout << "AOU" << i+1 << " ";
                    break;
                case 4: cout << "AOD" << i+1 << " ";
                    break;
            }
            action /= 5;
        }
        cout << endl;
    }

    void applyTheirAction (string action){

        if (action[1] == 'D'){
            DownButtons[(stoi(action.substr(3))-1)] = true;
        } else if (action[1] == 'U'){
            UpButtons[(stoi(action.substr(3))-1)] = true;
        } else {

        }
    }

};


class RuleBasedOperator{

public:


    //////////////////////////////////////////////////////////////////
    // Running operation on learnt policy

    State gameState;
    vector<bool> LiftGoingUp;
    vector<bool> DoorOpen;
    vector<bool> stopLift;

    RuleBasedOperator(){
        for (int i=0; i<K; i++){
            LiftGoingUp.push_back(true);
            DoorOpen.push_back(true);
            stopLift.push_back(true);
        }
    }

    void operate(){

        string instructionIn = "";
        int bestAction = 0;

        int myCount = 0;

        while (true){
            getline(cin, instructionIn);
            applyInputInstruction(instructionIn);
            bestAction = getBestActionForCurrentState();
            gameState.applyMyAction(bestAction);
            gameState.printMyAction(bestAction);
            myCount++;
            if (myCount / ((4*(N-1))/K) < K)
                stopLift[myCount / ((4*(N-1))/K)] = false;
        }
    }

    void applyInputInstruction(string instruction){
        string temp = "";
        for (int i=0; i<instruction.size(); i++){
            if (instruction[i] != ' ')
                temp += instruction[i];
            else {
                if (temp != "0")
                    gameState.applyTheirAction(temp);
                temp = "";
            }
        }
        if ((temp != "0") && (temp != ""))
            gameState.applyTheirAction(temp);
    }

    int getBestActionForCurrentState(){
        // 0 - Stay
        // 1 - Go Up
        // 2 - Go Down
        // 3 - Open with Up
        // 4 - Open with Down
        int action = 0;

        // if ((gameState.LiftButtons[0] | gameState.LiftButtons[1] | gameState.UpButtons | gameState.DownButtons) == 0)
        //     return 0;

        for (int i=K-1; i>=0; i--){
            action *= 5;

            if (stopLift[i]){
                action += 3;
                continue;
            }

            if (LiftGoingUp[i] && (gameState.LiftPositions[i] == N-1))
                LiftGoingUp[i] = false;
            else if (!LiftGoingUp[i] && (gameState.LiftPositions[i] == 0))
                LiftGoingUp[i] = true;

            if (shouldOpenDoor(i)){
                if (LiftGoingUp[i])
                    action += 3;
                else 
                    action += 4;
            } else {
                if (LiftGoingUp[i])
                    action += 1;
                else
                    action += 2;
            }

        }

        return action;
    }

    bool shouldOpenDoor (int n){
        DoorOpen[n] = (!DoorOpen[n]);
        return DoorOpen[n];
        // if ((gameState.LiftPositions[n] & gameState.LiftButtons[n]) == 0)
        //     for (int i=0; i<n; i++)
        //         if (gameState.LiftPositions[i] == gameState.LiftPositions[n])
        //             return false;
        // if (LiftGoingUp[n] && (((gameState.LiftPositions[n] & gameState.UpButtons) != 0)||((gameState.LiftPositions[n] & gameState.LiftButtons[n]) != 0)))
        //     return true;
        // if (!LiftGoingUp[n] && (((gameState.LiftPositions[n] & gameState.DownButtons) != 0)||((gameState.LiftPositions[n] & gameState.LiftButtons[n]) != 0))) 
        //     return true;
        // return false;
    }


    //////////////////////////////////////////////////////////////////
};

void getParams(int argc, char *argv[]){
    N = atoi(argv[1]);
    K = atoi(argv[2]);
    p = atof(argv[3]);
    q = atof(argv[4]);
    r = atof(argv[5]);
    discount = atof(argv[6]);
    maxIter = atoi(argv[7]);
}

int main(int argc, char *argv[]){

    if (argc < 8){
        cout << "Run as \"./liftOperator <N> <K> <p> <q> <r> <discount> <maxIter>\"\n";
        return 0;
    }

    getParams(argc, argv);

    RuleBasedOperator liftOperator;
    //liftOperator.LearnMinCosts();
//    liftOperator.modified_policy_iteration();

    cout << "0" << endl;

    //    liftOperator.printPolicy();
    liftOperator.operate();

    return 0;
}
