#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
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

		for (int i=0; i<K; i++){
			LiftButtons[i] = (unsigned char) (((1 << N) - 1) & hash); 
			hash >>= N;
		}

		for (int i=0; i<K; i++){
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
			if ((DownButtons & LiftPositions[i]) != 0) 
				actionsPossible[i] |= 1;

			// Open with Up possible?
			actionsPossible[i] <<= 1;
			if ((UpButtons & LiftPositions[i]) != 0) 
				actionsPossible[i] |= 1;

			// Go Down possible?
			actionsPossible[i] <<= 1;
			if (((LiftPositions[i] - 1) & LiftButtons[i]) != 0) 
				actionsPossible[i] |= 1;

			// Go Up possible?
			actionsPossible[i] <<= 1;
			if ((~(LiftPositions[i] - 1) & LiftButtons[i]) != 0) 
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
		neighbours.push_back(temp);

		for (int i=0; i<K; i++){
			vector<State> temp_list;
			for (int n = 0; n<neighbours.size(); n++){
				State temp(neighbours[n]);
				if (AddUpFloors[i]){
					for (unsigned char j=((LiftButtons[i] + (LiftPositions[i] << 1))|LiftButtons[i]); j<=((~(LiftPositions[i] - 1) & ~LiftPositions[i])&((1 << N)-1)); j = (j + (LiftPositions[i] << 1))|LiftButtons[i]){
						temp.LiftButtons[i] = j;
						temp_list.push_back(temp);
					}
				}
				if (AddDownFloors[i]){
					for (int j=((LiftButtons[i] + 1)|LiftButtons[i]); j<=((LiftPositions[i] - 1) | LiftButtons[i]); j = (j + 1)|LiftButtons[i]){
						temp.LiftButtons[i] = j;
						temp_list.push_back(temp);
					}
				}
			}
			if (temp_list.size() > 0)
				neighbours = temp_list;
		}

		vector<State> temp_list;
		for (int n = 0; n<neighbours.size(); n++){
			neighbours[n].proba = (float)(1.0/neighbours.size());
			State temp(neighbours[n]);

			temp.proba *= (1 - p);
			temp_list.push_back(temp);

			temp = neighbours[n];
			temp.proba *= p;
			temp.proba *= 1.0/(2*N - 2);

			temp.DownButtons = DownButtons;
			for (int i=0; i<N-1; i++){
				temp.UpButtons = UpButtons | (1 << i);
				temp_list.push_back(temp);
			}
			temp.UpButtons = UpButtons;
			for (int i=0; i<N-1; i++){
				temp.DownButtons = DownButtons | (1 << (i+1));
				temp_list.push_back(temp);
			}
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

	unsigned long long getSymmetricHash(){
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

};

class PolicyEvaluation{

public:

	map<unsigned long long, int> hashToIdx;
	int numStates = 0;
	vector<float> minCosts;
	vector<int> minCostActions;
	float error = 0;

	void computeMinCostForState(unsigned long long stateHash){

		State state(stateHash);
		int actions[(const int)pow(5, K)], numActions;
		state.getActions(actions, numActions);

		float minCost = 999999999;
		int minCostAction;

		for (int i=0; i<numActions; i++){
			vector<State> neighbours = state.getNeighboursForAction(actions[i]);
			initialiseStates(neighbours);
			float cost = 0;
			for (int j=0; j<neighbours.size(); j++)
				cost += neighbours[j].proba * (neighbours[j].getImmediateCost(actions[i]) + minCostOfState(neighbours[j]));
			if (cost < minCost){
				minCost = cost;
				minCostAction = actions[i];
			}
		}

		if (fabs(minCosts[hashToIdx[stateHash]] - minCost) > error)
			error = fabs(minCosts[hashToIdx[stateHash]] - minCost);

		minCosts[hashToIdx[stateHash]] = minCost;
		minCostActions[hashToIdx[stateHash]] = minCostAction;
	}

	void initialiseStates(vector<State> states){
		if (numStates < 8126464)
			for (int i=0; i<states.size(); i++)
				if (hashToIdx.find(states[i].getSymmetricHash()) == hashToIdx.end())
					if (hashToIdx.find(states[i].getHash()) == hashToIdx.end()){
						hashToIdx[states[i].getHash()] = numStates++;
						minCosts.push_back(0);
						minCostActions.push_back(0);
					}
			
	}

	float minCostOfState(State s){
		if (hashToIdx.find(s.getSymmetricHash()) != hashToIdx.end())
			return minCosts[hashToIdx[s.getSymmetricHash()]];
		return minCosts[hashToIdx[s.getHash()]];
	}

	void LearnMinCosts(){

		hashToIdx[0] = numStates++;		
		minCosts.push_back(0);
		minCostActions.push_back(0);

		int count = 0;
		int iter = 0;
		while (true){
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

};

int main(){

	PolicyEvaluation evaluation;
	evaluation.LearnMinCosts();

//	State state(0);
//	vector<int> actions = state.getActions();

//	cout << "Size : " << actions.size() << endl;

	return 0;
}