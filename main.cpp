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

	vector<int> getActions(){
		// 0 - Stay
		// 1 - Go Up
		// 2 - Go Down
		// 3 - Open with Up
		// 4 - Open with Down
		// Represented in Base 5

		unsigned char actionsPossible[K];

		for (int i=0; i<K; i++){
/*			actionsPossible[i] = 0;
			if (((LiftPositions[i] - 1) & LiftButtons[i]) != 0) 
				actionsPossible[i] |= 1;
			actionsPossible[i] <<= 1;
			if ((~(LiftPositions[i] - 1) & LiftButtons[i]) != 0) 
				actionsPossible[i] |= 1;
			actionsPossible[i] <<= 1;
*/
			actionsPossible[i] = (1 << 5) - 1;
		}

		vector<int> actions;
		for (int i=0; i<pow(5, K); i++)
			if (isPossible(i, actionsPossible))
				actions.push_back(i);
		
		return actions;
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
		// KN + 2N + log2(N^K) <= 64

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

	float getImmediateCost(){
		return 1;
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
		vector<int> actions = state.getActions();

		float minCost = 999999999;
		int minCostAction;

		for (int i=0; i<actions.size(); i++){
			vector<State> neighbours = state.getNeighboursForAction(actions[i]);
			initialiseStates(neighbours);
			float cost = 0;
			for (int j=0; j<neighbours.size(); j++)
				cost += neighbours[j].proba * (neighbours[j].getImmediateCost() + minCosts[hashToIdx[neighbours[j].getHash()]]);
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
		for (int i=0; i<states.size(); i++){
			if (hashToIdx.find(states[i].getHash()) == hashToIdx.end()){
				hashToIdx[states[i].getHash()] = numStates++;
				minCosts.push_back(0);
				minCostActions.push_back(0);
			}
		}
	}

	void LearnMinCosts(){

		hashToIdx[0] = numStates++;		
		minCosts.push_back(0);
		minCostActions.push_back(0);

		int count = 0;
		while (true){
			for ( const auto &myPair : hashToIdx ){ 
				cout << "Looping : " << count++ << ", NumStates: " << numStates << ", Error : " << error << "\n";
				computeMinCostForState(myPair.first);
			}
			if (error < 1)
				break;
			error = 0;
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