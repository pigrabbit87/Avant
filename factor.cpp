#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <tgmath.h>
#include <set>
#include <string>

using namespace std;

/************************************************************/
/* Caching Implementation 								 							    */
/* -------------------------------------------------------- */
/* There are two things that I thought of 								  */
/* First, do something like a LRU cache. We will create a 	*/
/* 1. A Node structure that contains the string format of   */
/*		the input vector, a map that contains the result of 	*/
/*    the factor results, pointer to previous and next node */
/* 2. A hash map to look up node in the link list						*/
/*                                                          */
/* The link list store the cached information and keep track*/
/* of which node to be in the cache. The head is the most   */
/* recent node and the tail is the least recent.						*/
/* Given a memory limitation, we will keep a global map and */
/* use that map as a cache holder. To search in the cache,  */
/* since it's a map it's O(1). Eventhough we have to move   */
/* the node to the head of the link list, we already have   */
/* the pointer to the node, the operation time is constant  */
/*                                                          */
/* If we were to reverse the functionality, caching will not*/
/* be different, only the factor function will differ 			*/
/************************************************************/

struct Node{
	map<int, vector<int> > m;
	string key;
	Node* previous;
	Node* next;
};

class LRU{
public:
	LRU(int size);
	void addNode(vector<int> input, map< int, vector<int> > result);
	void deleteNode();
	void moveFront(string s);
	map< int, vector<int> > factor(vector<int> input);
	string vtos(vector<int> input);
	int getAvailability();
	Node* getHead();
private:
	// Main hash that we use when we do search
	unordered_map<string, Node*> hash;
	// LRU that have the recent access on as the head node
	Node* head; 
	// Keep track of the size
	int availableNode;
};

LRU::LRU(int size){
	head = NULL;

	availableNode = size;
}

string LRU::vtos(vector<int> input){
	string result;
	for (auto& x: input){
		result += to_string(x) + ",";
	}
	return result;
}

/************************************************************/
/* AddNode function 										 										*/
/* -------------------------------------------------------- */
/* Input - input vector and the result of that vector from  */
/*				 Factor function 										 							*/
/* Output - None	 										 											*/
/* Implementation - 										 										*/
/*		Add a node to the head of the link list								*/
/************************************************************/

void LRU::addNode(vector<int> input, map< int, vector<int> > result){
	string s = vtos(input);

	// If our cache has hit the limit, we need to remove the least used one
	if (availableNode == 0){
		deleteNode();
	}

	// Put it inside the LRU
	Node* a = new Node;
	a->m = result;
	a->key = s;
	a->next = head;
	a->previous = NULL;
	
	if (head) {
		head->previous = a;
	}
	
	head = a;

	// Put it inside the hash
	hash.insert({s, a});

	availableNode--;
}

/************************************************************/
/* DeleteNode function 										 									*/
/* -------------------------------------------------------- */
/* Input - None										 													*/
/* Output - None	 										 											*/
/* Implementation - 										 										*/
/*		Delete the node at the end of the link list and in the*/
/*		hash map so we have space in the cache								*/
/************************************************************/

void LRU::deleteNode(){
	// When the link list ts full, we want to remove the tail
	Node* temp = head;
	while (temp && temp->next){
		temp = temp->next;
	}

	cout << "Delet node " << temp->key << endl;
	// temp is the last Node
	// Remove it from the map
	hash.erase(temp->key);

	// Remove it from the link list
	Node* prev = temp->previous;
	delete(temp);
	prev->next = NULL;
	availableNode++;
}

/************************************************************/
/* MoveFront function 										 									*/
/* -------------------------------------------------------- */
/* Input - string format of the vector input								*/
/* Output - None	 										 											*/
/* Implementation - 										 										*/
/*		When we hit the cache in the map, we want to move that*/
/*		node in the link list to the front of the list				*/
/************************************************************/

void LRU::moveFront(string s){
	Node* temp = hash.find(s)->second;
	if (temp == head) return;

	temp->previous->next = temp->next;

	if (temp->next != NULL){
		temp->next->previous = temp->previous;	
	}

	head->previous = temp;
	temp->next = head;
	temp->previous = NULL;

	head = temp;	
}

int LRU::getAvailability(){
	return availableNode;
}

Node* LRU::getHead(){
	return head;
}

/************************************************************/
/* Factor function																 				  */
/* -------------------------------------------------------- */
/* Input - a vector containing the input number						  */
/* Output - a map containing the result of number and its   */
/*				  factors																 				  */
/* Implementation 																 				  */
/* 		- First we sort the vector from least to largest. By  */
/*			doing this we can make sure that possible factors   */
/*			are before the number															  */
/*		- We store the number into a map first							  */
/*		- Loop through the entire vector. 									  */
/*			- Find the limit to the square root of the number   */
/*			- Loop through the vecotr from index 0 to a number  */
/*				less or equal to the square root								  */
/*				- Divide the current number with the other number */
/*					If the residual is 0, we insert it into the 	  */
/*					vector.																 				  */
/*			- Insert the vector to the map using the current 	  */
/*				number as the key																  */	
/************************************************************/

map< int, vector<int> > LRU::factor(vector<int> input){
	// First, sort the input - O(N log N)
	sort(input.begin(), input.end());

	// Then we want to see if it's already inside the hash
	string s = vtos(input);
	if (hash.find(s) != hash.end()) {
		cout << "Hit cache!" << endl;
		// Move this node to the head of the list
		moveFront(s);
		return hash.find(s)->second->m;
	}

	// If it's not in the hash, we need to calculate it

	map< int, vector<int> > result;
	// Store everything in the vector - O(N)
	for (int i = 0; i < input.size(); i++){
		vector<int> temp;
		result[input[i]] = temp;
	}

	// Loop through the input vector. We can see that for every number,
	// It's factor has to be 
	for (int i = 0; i < input.size(); i++){
		int limit = sqrt(input[i]); // This is the limit that we should test
		vector<int> v;
		for (int j = 0; input[j] <= limit; j++){
			if (input[i] % input[j] == 0){
				v.push_back(input[j]);
				if (input[j] != input[i]/input[j]) v.push_back(input[i]/input[j]);
			}
		}
		result[input[i]] = v;
	}

	// Store this new find in our cache
	addNode(input, result);

	return result;
}

/***********************************************************/
/* PrettyPrint function																		 */
/* ------------------------------------------------------- */
/* Print the result map into readable format							 */
/* Input - map																		 				 */
/* Output - None																		 			 */
/***********************************************************/

void PrettyPrint(map<int, vector<int> > input){
	cout << "{";
	for (auto it = input.begin(); it != input.end(); ++it){
		if (it == input.begin()) cout << it->first << ": [";
		else cout << ", " << it->first << ": [";
		for (int i = 0; i < it->second.size(); i++){
			if (i == 0) cout << it->second[i];
			else cout << "," << it->second[i];
		} 
		cout << "]";
	}
	cout << "}" << endl;
}

void test1(){
	LRU* first = new LRU(5);

	int v2[5] = { 2, 4, 5, 10, 20 };
	vector<int> input1(&v2[0], &v2[0]+5);

	int v3[5] = { 1, 3, 5, 6, 25 };
	vector<int> input2(&v3[0], &v3[0]+5);

	int v4[5] = { 2, 3, 9, 12, 18 };
	vector<int> input3(&v4[0], &v4[0]+5);

	int v5[5] = { 2, 4, 3, 15, 21 };
	vector<int> input4(&v5[0], &v5[0]+5);

	int v6[5] = { 22, 11, 5, 2, 20 };
	vector<int> input5(&v6[0], &v6[0]+5);

	int v7[5] = { 2, 4, 20, 25, 50 };
	vector<int> input6(&v7[0], &v7[0]+5);

	int v8[5] = { 1, 4, 30, 24, 50 };
	vector<int> input7(&v8[0], &v8[0]+5);

	int v9[5] = { 2, 4, 20, 26, 80 };
	vector<int> input8(&v9[0], &v9[0]+5);

	int v10[5] = { 3, 4, 20, 21, 50 };
	vector<int> input9(&v10[0], &v10[0]+5);

	map<int, vector<int> > result = first->factor(input1);

	Node* temp;

	result = first->factor(input1);
	result = first->factor(input2);
	result = first->factor(input3);
	result = first->factor(input4);
	result = first->factor(input5);
	result = first->factor(input6);
	result = first->factor(input5);	
	result = first->factor(input7);	
	result = first->factor(input8);	
	result = first->factor(input9);	


	temp = first->getHead();

	while (temp){
		cout << temp->key << endl;
		temp = temp->next;
	}
}

int main(int argc, char *argv[]){
	
}
