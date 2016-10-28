#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <ctime>
#include <climits>
#include <map>
using namespace std;

#define N 5
#define K 2
#define p 0.9
#define q 0.9
#define r 0.9

static const unsigned char BitReverseTable256[] =
{
  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

unsigned int reverse32BitNumber (unsigned int num){
	unsigned int c = (BitReverseTable256[num & 0xff] << 24) |
      (BitReverseTable256[(num >> 8) & 0xff] << 16) |
      (BitReverseTable256[(num >> 16) & 0xff] << 8) |
      (BitReverseTable256[(num >> 24) & 0xff]);
    return c;
}

class State{

public:

	unsigned char UpButtons;
	unsigned char DownButtons;
	unsigned char LiftPositions[K];
	unsigned char LiftButtons[K];

	float proba;

	State(){
		UpButtons = 0;
		DownButtons = 0;
		proba = 0;
		for (int i=0; i<K; i++){
			LiftPositions[i] = 1;
			LiftButtons[i] = 0;
		}
	}

	State (const State &g){
		UpButtons = g.UpButtons;
		DownButtons = g.DownButtons;
		proba = g.proba;
		for (int i=0; i<K; i++){
			LiftPositions[i] = g.LiftPositions[i];
			LiftButtons[i] = g.LiftButtons[i];
		}
	}

	State (unsigned long long hash){

		DownButtons = (unsigned char) (((1 << (N-1)) - 1) & hash);
		DownButtons <<= 1;
		hash >>= N-1;
		UpButtons = (unsigned char) (((1 << (N-1)) - 1) & hash);
		hash >>= N-1;

		for (int i=K-1; i>=0; i--){
			LiftButtons[i] = (unsigned char) (((1 << N) - 1) & hash); 
			hash >>= N;
		}

		for (int i=K-1; i>=0; i--){
			LiftPositions[i] = (1 << (hash % N));
			hash /= N;
		}
	}

	void getActions (int actions[], int &n){
		// 0 - Stay
		// 1 - Go Up
		// 2 - Go Down
		// 3 - Open with Up
		// 4 - Open with Down
		// Represented in Base 5

		unsigned char actionsPossible[K];

		for (int i=0; i<K; i++){
			// Open with Down possible?
			actionsPossible[i] = 0;
			if (((DownButtons | LiftButtons[i]) & LiftPositions[i]) != 0) 
				actionsPossible[i] |= 1;

			// Open with Up possible?
			actionsPossible[i] <<= 1;
			if (((UpButtons | LiftButtons[i]) & LiftPositions[i]) != 0) 
				actionsPossible[i] |= 1;

			// Go Down possible?
			actionsPossible[i] <<= 1;
			if (((LiftPositions[i] - 1) & (LiftButtons[i] | UpButtons | DownButtons)) != 0) 
				actionsPossible[i] |= 1;

			// Go Up possible?
			actionsPossible[i] <<= 1;
			if ((~(LiftPositions[i] - 1) & (LiftButtons[i] | UpButtons | DownButtons)) != 0) 
				actionsPossible[i] |= 1;

			// Stay possible?
			actionsPossible[i] <<= 1;
			if (LiftButtons[i] == 0)
				actionsPossible[i] |= 1;

//			actionsPossible[i] = (1 << 5) - 1;
		}

		n = 0;
		for (int i=0; i<pow(5, K); i++)
			if (isPossible(i, actionsPossible))
				actions[n++] = i;

	}

	bool isPossible(int action, unsigned char actionsPossible[]){

		for (int i=0; i < K; i++){
			if ((actionsPossible[i] & (1 << (action % 5))) == 0)
				return false;
			action /= 5;
		}
		return true;
	}

	vector<State> getNeighboursForAction (int action){

		vector<State> neighbours;
		bool AddUpFloors[K] = {}, AddDownFloors[K] = {};

		// Adding consequent of action taken
		State temp(*this);
		for (int i=0; i<K; i++){
			switch(action % 5){
				case 0:
						break;
				case 1: temp.LiftPositions[i] <<= 1;
						break;
				case 2: temp.LiftPositions[i] >>= 1;
						break;
				case 3: if ((UpButtons & LiftPositions[i])!= 0){
							AddUpFloors[i] = true;
							temp.UpButtons &= (~LiftPositions[i]);
						}
						temp.LiftButtons[i] &= (~LiftPositions[i]);
						break;
				case 4:	if ((DownButtons & LiftPositions[i])!= 0){
							AddDownFloors[i] = true;
							temp.DownButtons &= (~LiftPositions[i]);
						}
						temp.LiftButtons[i] &= (~LiftPositions[i]);
						break;
			}
			action /= 5;
		}
		temp.proba = 1.0;
		neighbours.push_back(temp);

		// Adding all possibilities of buttons pressed
		for (int i=0; i<K; i++){
			vector<State> temp_list;
			for (int n = 0; n<neighbours.size(); n++){
				State temp(neighbours[n]);
				if (AddUpFloors[i]){
					for (unsigned char j=LiftButtons[i]; j<=(((~(LiftPositions[i] - 1) & ~LiftPositions[i])&((1 << N)-1))|LiftButtons[i]); j = (j + (LiftPositions[i] << 1))|LiftButtons[i]){
						temp.LiftButtons[i] = j;
						temp_list.push_back(temp);
					}
				}
				if (AddDownFloors[i]){
					for (int j = LiftButtons[i]; j<=((LiftPositions[i] - 1) | LiftButtons[i]); j = (j + 1)|LiftButtons[i]){
						temp.LiftButtons[i] = j;
						temp_list.push_back(temp);
					}
				}
			}
			if (temp_list.size() > 0)
				neighbours = temp_list;
		}

		// Adding all possible cases of someone arriving somewhere
		vector<State> temp_list;
		for (int n = 0; n<neighbours.size(); n++){

			State temp(neighbours[n]);
			float prob = (float)1.0/(float)neighbours.size();

			// No one arrives
			temp.proba = prob * (1 - p);
			temp_list.push_back(temp);

			// Someone arrives
			prob *= p;

			// At 1st floor
			temp.proba = prob * q;
			temp.DownButtons = DownButtons;
			temp.UpButtons = (UpButtons | 1);
			temp_list.push_back(temp);

			// At floors 2 to N-1
			if (N > 2)
				for (int i=1; i<N-1; i++){

					// Wants to go Down
					temp.proba = prob * ((1-q)/(N-1)) * (r + (1-r)*(i-1)/(N-2));
					temp.DownButtons = DownButtons | (1 << i);
					temp.UpButtons = UpButtons;
					temp_list.push_back(temp);

					// Wants to go Up
					temp.proba = prob * ((1-q)/(N-1)) * ((1-r)*(N-1-i)/(N-2));
					temp.DownButtons = DownButtons;
					temp.UpButtons = UpButtons | (1 << i);
					temp_list.push_back(temp);
				}

			// At floor N
			temp.proba = prob * ((1-q)/(N-1));
			temp.DownButtons = DownButtons | (1 << (N-1));
			temp.UpButtons = UpButtons;
			temp_list.push_back(temp);
		}
		neighbours = temp_list;

		return neighbours;
	}

	void applyMyAction (int action){

		for (int i=0; i<K; i++){
			switch(action % 5){
				case 0:
						break;
				case 1: LiftPositions[i] <<= 1;
						break;
				case 2: LiftPositions[i] >>= 1;
						break;
				case 3: UpButtons &= (~LiftPositions[i]);
						LiftButtons[i] &= (~LiftPositions[i]);
						break;
				case 4:	DownButtons &= (~LiftPositions[i]);
						LiftButtons[i] &= (~LiftPositions[i]);
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
			DownButtons |= (1 << stoi(action.substr(3)));
		} else if (action[1] == 'U'){
			UpButtons |= (1 << stoi(action.substr(3)));
		} else {
			string buttonString="", liftNumberString="";
			int j = 2;
			while (action[j] != '_')
				buttonString += action[j++];
			j++;
			while (j < action.size())
				liftNumberString += action[j++];
			LiftButtons[stoi(liftNumberString)-1] |= (1 << stoi(buttonString));
		}
	}

	unsigned long long getHash(){
		// KN + 2N-2 + log2(N^K) <= 64

		unsigned long long hash = 0;

		for (int i=0; i<K; i++){
			hash *= N;
			hash += log2(LiftPositions[i]);
		}

		for (int i=0; i<K; i++){
			hash <<= N;
			hash |= (unsigned long long)LiftButtons[i];
		}

		hash <<= N-1;
		hash |= (unsigned long long)UpButtons;
		hash <<= N-1;
		hash |= (unsigned long long)(DownButtons >> 1);

		return hash;
	}

	unsigned long long getSymmetricHash1(){
		// KN + 2N-2 + log2(N^K) <= 64

		unsigned long long hash = 0;

		for (int i=0; i<K; i++){
			hash *= N;
			hash += N - 1 - log2(LiftPositions[i]);
		}

		for (int i=0; i<K; i++){
			hash <<= N;
			hash |= (unsigned long long)(reverse32BitNumber(LiftButtons[i]) >> (32-N));
		}

		hash <<= N-1;
		hash |= (unsigned long long)(reverse32BitNumber(DownButtons) >> (32-N));
		hash <<= N-1;
		hash |= (unsigned long long)(reverse32BitNumber(UpButtons) >> (32-N+1));

		return hash;
	}

	unsigned long long getSymmetricHash2(){
		// KN + 2N-2 + log2(N^K) <= 64

		unsigned long long hash = 0;

		for (int i=K-1; i>=0; i--){
			hash *= N;
			hash += log2(LiftPositions[i]);
		}

		for (int i=K-1; i>=0; i--){
			hash <<= N;
			hash |= (unsigned long long)LiftButtons[i];
		}

		hash <<= N-1;
		hash |= (unsigned long long)UpButtons;
		hash <<= N-1;
		hash |= (unsigned long long)(DownButtons >> 1);

		return hash;
	}

	unsigned long long getSymmetricHash3(){
		// KN + 2N-2 + log2(N^K) <= 64

		unsigned long long hash = 0;

		for (int i=K-1; i>=0; i--){
			hash *= N;
			hash += N - 1 - log2(LiftPositions[i]);
		}

		for (int i=K-1; i>=0; i--){
			hash <<= N;
			hash |= (unsigned long long)(reverse32BitNumber(LiftButtons[i]) >> (32-N));
		}

		hash <<= N-1;
		hash |= (unsigned long long)(reverse32BitNumber(DownButtons) >> (32-N));
		hash <<= N-1;
		hash |= (unsigned long long)(reverse32BitNumber(UpButtons) >> (32-N+1));

		return hash;
	}

	float getImmediateCost(int action){

		int numPeopleWaiting = __builtin_popcount(UpButtons) + __builtin_popcount(DownButtons);
		for (int i=0; i<K; i++)
			numPeopleWaiting += __builtin_popcount(LiftButtons[i]);

		int cost = 0;
		for (int i=0; i<K; i++){
			switch(action % 5){
				case 0:
						break;
				case 1:
				case 2: cost += 1;
						break;
				case 3:
				case 4:
						break;
			}
			action /= 5;
		}

		return cost + 2*numPeopleWaiting;
	}

	void print(){
		for (int i=0; i<K; i++){
			cout << "Lift " << i+1 << "  : ";
			unsigned char pos = LiftPositions[i];
			for (int j=0; j<N; j++){
				cout << pos%2;
				pos /= 2;
			}
			cout << endl;
			cout << "Buttons : ";
			pos = LiftButtons[i];
			for (int j=0; j<N; j++){
				cout << pos%2;
				pos /= 2;
			}
			cout << endl;
			cout << endl;
		}
		cout << "Up     : ";
		unsigned char pos = UpButtons;
		for (int j=0; j<N; j++){
			cout << pos%2;
			pos /= 2;
		}
		cout << endl;
		cout << "Down   : ";
		pos = DownButtons;
		for (int j=0; j<N; j++){
			cout << pos%2;
			pos /= 2;
		}
		cout << endl;
	}

};

class LiftOperator{

public:

	map<unsigned long long, int> hashToIdx;
	int numStates = 0;
	vector<float> minCosts;
	vector<int> minCostActions;
	float error = 0;

//////////////////////////////////////////////////////////////////
// Value Iteration

	void computeMinCostForState(unsigned long long stateHash){

		State state(stateHash);
		int actions[(const int)pow(5, K)], numActions;
		state.getActions(actions, numActions);

		float minCost = 999999999;
		int minCostAction;

		for (int i=0; i<numActions; i++){
			vector<State> neighbours = state.getNeighboursForAction(actions[i]);
			float cost = 0;
			for (int j=0; j<neighbours.size(); j++)
				cost += neighbours[j].proba * (neighbours[j].getImmediateCost(actions[i]) + minCostOfState(neighbours[j]));
			if (cost < minCost){
				minCost = cost;
				minCostAction = actions[i];
			}
		}

		error = max(error, (float)fabs(minCosts[hashToIdx[stateHash]] - minCost));

		minCosts[hashToIdx[stateHash]] = minCost;
		minCostActions[hashToIdx[stateHash]] = minCostAction;
	}

	float minCostOfState(State s){
		if (hashToIdx.find(s.getSymmetricHash1()) != hashToIdx.end())
			return minCosts[hashToIdx[s.getSymmetricHash1()]];
		if (hashToIdx.find(s.getSymmetricHash2()) != hashToIdx.end())
			return minCosts[hashToIdx[s.getSymmetricHash2()]];
		if (hashToIdx.find(s.getSymmetricHash3()) != hashToIdx.end())
			return minCosts[hashToIdx[s.getSymmetricHash3()]];
		if (hashToIdx.find(s.getHash()) != hashToIdx.end())
			return minCosts[hashToIdx[s.getHash()]];

		hashToIdx[s.getHash()] = numStates++;
		minCosts.push_back(100);
		minCostActions.push_back(-1);
		return minCosts[hashToIdx[s.getHash()]];
	}

	void LearnMinCosts(){

		hashToIdx[0] = numStates++;
		minCosts.push_back(0);
		minCostActions.push_back(0);

		int count = 0;
		int iter = 0;
		while (iter < 300){
			for ( const auto &myPair : hashToIdx ){ 
				cout << "Iteration : " << iter << ", Looping : " << count++ << ", NumStates: " << numStates << ", Error : " << error << "\n";
				computeMinCostForState(myPair.first);
			}
			if (error < 1)
				break;
			error = 0;
			count = 0;
			iter++;
		}
	}

//////////////////////////////////////////////////////////////////
// Running operation on learnt policy

	State gameState;

	void printHash(unsigned long long hash){
		for (int i=0; i<N*K + 2*N - 2 + log2(pow(N,K)); i++){
			cout << hash%2;
			hash /= 2;
		}
		cout << endl;
	}

	void operate(){

		string instructionIn;
		int bestAction = 0;

		while (true){

			cout << "Game State : " << endl;
			gameState.print();
			cout << endl;
			cout << "Hash : ";
			printHash(gameState.getHash());
			printHash(gameState.getSymmetricHash1());
			printHash(gameState.getSymmetricHash2());
			printHash(gameState.getSymmetricHash3());

			int actions[25], n=0;
			gameState.getActions(actions, n);
			cout << "Actions Possible : ";
			for (int i=0; i<n; i++)
				cout << actions[i] << " ";
			cout << endl;

			getline(cin, instructionIn);
			applyInputInstruction(instructionIn);
			bestAction = getBestActionForCurrentState();
			cout << "BestAction found : " << bestAction << endl;
			gameState.applyMyAction(bestAction);
			gameState.printMyAction(bestAction);
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
		if (temp != "0")
			gameState.applyTheirAction(temp);
	}

	int getBestActionForCurrentState(){
		if (hashToIdx.find(gameState.getSymmetricHash1()) != hashToIdx.end())
			return Transform1(minCostActions[hashToIdx[gameState.getSymmetricHash1()]]);
		if (hashToIdx.find(gameState.getSymmetricHash2()) != hashToIdx.end())
			return Transform2(minCostActions[hashToIdx[gameState.getSymmetricHash2()]]);
		if (hashToIdx.find(gameState.getSymmetricHash3()) != hashToIdx.end())
			return Transform1(Transform2(minCostActions[hashToIdx[gameState.getSymmetricHash3()]]));

		return minCostActions[hashToIdx[gameState.getHash()]];
	}

	int Transform1 (int action){

		int temp = 0;
		for (int i=0; i<K; i++){
			temp *= 5;
			temp += action % 5;
			action /= 5;
		}		

		int temp2 = 0;
		for (int i=0; i<K; i++){
			temp2 *= 5;
			switch(temp % 5){
				case 0: temp2 += 0;
						break;					
				case 1: temp2 += 2;
						break;					
				case 2: temp2 += 1;
						break;					
				case 3: temp2 += 4;
						break;					
				case 4: temp2 += 3;
						break;					
			}
			temp /= 5;
		}		

		return temp2;
	}

	int Transform2 (int action){

		int temp = 0;
		for (int i=0; i<K; i++){
			temp *= 5;
			temp += action % 5;
			action /= 5;
		}		

		return temp;
	}

	void printPolicy(){

		for ( const auto &myPair : hashToIdx ){ 
			State state(myPair.first);
			state.print();
			cout << "BestAction : " << minCostActions[myPair.second];
			cout << "\nMin Cost : " << minCostOfState(state);
			cout << "\n-------------------------------------------------------\n";
		}

	}

//////////////////////////////////////////////////////////////////
// Policy Iteration

	map<unsigned long long, tuple<float,int,int>> hashtoValue;// every state has a value, an index and the action

	void initialiseValueStates(vector<State> states){

    if (numStates < 8126464)
      for (int i=0; i<states.size(); i++)
				if (hashtoValue.find(states[i].getSymmetricHash1()) == hashtoValue.end())
					if (hashtoValue.find(states[i].getHash()) == hashtoValue.end()){
            State state(states[i].getHash());
            int actions[(const int)pow(5, K)], numActions;
            state.getActions(actions, numActions);
            hashtoValue[states[i].getHash()] = make_tuple(0,numStates++,actions[0]);
					}
	}



  void generate_states(){
    int max_size = 3300000;
    State state(0);
    int actions[(const int)pow(5, K)], numActions;
		state.getActions(actions, numActions);
    hashtoValue[0] = make_tuple(0,numStates++,actions[0]);// zero state
    while(true){
      for ( const auto &myTuple : hashtoValue ){
          State state(get<0>(myTuple));
          int actions[(const int)pow(5, K)], numActions;
          state.getActions(actions, numActions);
          for (int i=0; i<numActions; i++){
            vector<State> neighbours = state.getNeighboursForAction(actions[i]);
            initialiseValueStates(neighbours);
          }
          if(hashtoValue.size() > max_size)
            return;
      }
    }
  }


  bool in_map(unsigned long long hash1){
      if(hashtoValue.find(hash1)!=hashtoValue.end())
        return true;
      return false;
  }

  void modified_policy_iteration(){
    const clock_t begin_time = clock();
    // Generate all states and assign a random possible action to every state, fix the max number of states to 3M
    generate_states();
    cout << float(clock()-begin_time)/CLOCKS_PER_SEC<<endl;

    int iterations=0;
    int max_iterations = 25;
    // Create the policy graph and calculate the values for every state assuning 0 value for starting state(0 state)
    float new_value;
    tuple<float,int,int> new_tuple;
    State temp_state;
    int states_changed;
    while(iterations < max_iterations){
      // update the value of every state in the map, according to the current action
      // if((float(clock()-begin_time)/CLOCKS_PER_SEC) >=1700){
      //     break;
      // }
      states_changed=0;
      unsigned long long hash_value,symmetric_hash_value;
      for ( const auto &myTuple : hashtoValue ){
            State state(myTuple.first);
            new_value = state.getImmediateCost(get<2>(myTuple.second));
            vector<State> neighbours = state.getNeighboursForAction(get<2>(myTuple.second));

            for (int i=0;i<neighbours.size();i++){
              hash_value = neighbours[i].getHash();
              symmetric_hash_value = neighbours[i].getSymmetricHash1();
              if(in_map(hash_value)){
                new_tuple = hashtoValue[hash_value];
                new_value += neighbours[i].proba*(get<0>(new_tuple));
              }
              else if(in_map(symmetric_hash_value)){
                new_tuple = hashtoValue[symmetric_hash_value];
                new_value += neighbours[i].proba*(get<0>(new_tuple));
              }

            }

            if(new_value!=get<0>(myTuple.second))
              states_changed++;
            hashtoValue[myTuple.first] = make_tuple(new_value,get<1>(myTuple.second),get<2>(myTuple.second));
      }
      cout << "Percent Value Changes " << 100*(states_changed/((double)hashtoValue.size())) << endl;

      // update the policy for each state
      // consider all possible actions in the state and take the min over all reachable values
      int best_action,best_value,next_action_value;
      states_changed = 0;
      for ( const auto &myTuple : hashtoValue ){
            State state(myTuple.first);
            int actions[(const int)pow(5, K)], numActions;
            state.getActions(actions, numActions);
            best_value = INT_MAX;

            for (int i=0; i<numActions; i++){
              vector<State> neighbours = state.getNeighboursForAction(actions[i]);
              next_action_value = state.getImmediateCost(actions[i]);

              for (int j=0;j<neighbours.size();j++){
                  hash_value = neighbours[j].getHash();
                  symmetric_hash_value = neighbours[j].getSymmetricHash1();
                  if(in_map(hash_value)){
                      new_tuple = hashtoValue[hash_value];
                      next_action_value += neighbours[j].proba*(get<0>(new_tuple));
                  }
                  else if(in_map(symmetric_hash_value)){
                    new_tuple = hashtoValue[symmetric_hash_value];
                    next_action_value += neighbours[j].proba*(get<0>(new_tuple));
                  }
              }

              if (next_action_value < best_value){
                  best_action = actions[i];
                  best_value = next_action_value;
              }
            }
        if(best_action!=get<2>(myTuple.second))
          states_changed++;
        hashtoValue[myTuple.first] = make_tuple(get<0>(myTuple.second),get<1>(myTuple.second),best_action);

      }
      cout << "Percent Policy Changes " << 100*(states_changed/((double)hashtoValue.size())) << endl;
      cout << float(clock()-begin_time)/CLOCKS_PER_SEC << endl;
      iterations++;
    }

  }

//////////////////////////////////////////////////////////////////

};

int main(){

	LiftOperator liftOperator;
	liftOperator.LearnMinCosts();
//	liftOperator.modified_policy_iteration();

	cout << "0" << endl;

	liftOperator.printPolicy();
	liftOperator.operate();

	return 0;
}
