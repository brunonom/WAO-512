#include <bits/stdc++.h>
using namespace std;
#define KEY_SIZE 256
#define KEY_SIZE_HEX KEY_SIZE/4

void print_help(){
	printf("\n");
	printf("usage: ./crypt [path to txt file]\n");
	printf("       ./crypt [path to txt file] [decryption key]\n");
	printf("\n");
}

fstream file;

int subs[16] = {14, 0, 5, 6, 3, 15, 9, 11, 4, 1, 7, 8, 2, 10, 13, 12};
	
map<int, int> subs_table;
map<int, int> reverse_subs_table;

int hex_to_int(char c){
	if(c<='9'){
		return c-'0';
	}else{
		return c-'a'+10;
	}
}

char int_to_hex(int i){
	if(i<10){
		return '0'+i;
	}else{
		return 'a'+i-10;
	}
}

string bin_to_hex(string bits){
	string hex = "";
	for(int i=0; i<KEY_SIZE; i+=4){
		int r = (bits[i]-'0')*8 + (bits[i+1]-'0')*4 + (bits[i+2]-'0')*2 + (bits[i+3]-'0');
		hex += int_to_hex(r);
	}
	return hex;
}

string hex_to_bin(string hex){
	string bits = "";
	for(int i=0; i<KEY_SIZE_HEX; i++){
		switch(hex_to_int(hex[i])){
			case 0: bits += "0000"; break;
			case 1: bits += "0001"; break;
			case 2: bits += "0010"; break;
			case 3: bits += "0011"; break;
			case 4: bits += "0100"; break;
			case 5: bits += "0101"; break;
			case 6: bits += "0110"; break;
			case 7: bits += "0111"; break;
			case 8: bits += "1000"; break;
			case 9: bits += "1001"; break;
			case 10: bits += "1010"; break;
			case 11: bits += "1011"; break;
			case 12: bits += "1100"; break;
			case 13: bits += "1101"; break;
			case 14: bits += "1110"; break;
			case 15: bits += "1111"; break;
		}
	}
}

string generate_key(){
	string key = "";
	for(int i=0; i<KEY_SIZE; i++){
		key += (rand()%2)? "0" : "1";
	}
	return bin_to_hex(key);
}

void substitution(string& key){
	for(int i=0; i<KEY_SIZE_HEX; i++){
		key[i] = int_to_hex(subs_table[hex_to_int(key[i])]);
	}
}

void permutation(string& key){
	// key = "abcdefghijklmnopABCDEFGHIJKLMNOP.,;:!?{}[]()@$&*qrstuvwxQRSTUVWX";
	//abcdefghijklmnopABCDEFGHIJKLMNOP.,;:!?{}[]()@$&*qrstuvwxQRSTUVWX
	//aG[WfL$tkM;upB}RA{QoF)VdK@seP,xj.wiO?TnD(UcE*rhJqgI&vlN:SmC!XbH]
	// printf("%s\n", key.c_str());
	int count = 0;
	char mat[4][4][4], copy[4][4][4];
	
	// string to cube
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[i][j][k] = key[count];
				copy[i][j][k] = key[count];
				count++;
			}
		}
	}

	// x shift
	for(int i=0; i<4; i++){	
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[i][j][k] = copy[i][j][(k+j)%4];
			}
		}
	}
	for(int i=0; i<4; i++){	
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				copy[i][j][k] = mat[i][j][k];
			}
		}
	}

	// y shift
	for(int i=0; i<4; i++){	
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[i][k][j] = copy[i][(k+j)%4][j];
			}
		}
	}
	for(int i=0; i<4; i++){	
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				copy[i][j][k] = mat[i][j][k];
			}
		}
	}

	// z shift
	for(int i=0; i<4; i++){	
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[k][j][i] = copy[(k+i)%4][j][i];
			}
		}
	}
	for(int i=0; i<4; i++){	
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				copy[i][j][k] = mat[i][j][k];
			}
		}
	}

	// cube to string
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				key[count++%64] = mat[i][j][k];
			}
		}
	}
	// printf("%s\n", key.c_str());
}

void encrypt(string message, string key){
	// printf("%s\n", message.c_str());
	for(int i=0; i<16; i++){
		substitution(key);
		permutation(key);
		for(int j=0; j<message.size(); j++){
			message[j] = message[j] ^ key[j%KEY_SIZE_HEX];
		}
	}
	fstream output("./encrypted.txt", ios::out);		
	output << message;
	// printf("%s\n", message.c_str());
}

void decrypt(string message, string key){
	// printf("%s\n", message.c_str());
	for(int i=0; i<16; i++){
		substitution(key);
		permutation(key);
		for(int j=0; j<message.size(); j++){
			message[j] = message[j] ^ key[j%KEY_SIZE_HEX];
		}
	}	
	fstream output("./decrypted.txt", ios::out);		
	output << message;
	// printf("%s\n", message.c_str());
}

int main(int argc, char** argv){
	
	for(int i=0; i<16; i++){
		subs_table[i] = subs[i];
		reverse_subs_table[subs[i]] = i;
	}

	// checks if substitution is asymmetric and irreflexive
		// for(int i=0; i<16; i++){
		// 	if(i == subs_table[i]){
		// 		printf("whack af: %d\n", i);
		// 		break;
		// 	}
		// 	int j = subs_table[i];
		// 	if(subs_table[j] == i){
		// 		printf("whacky: %d %d\n", i, j);
		// 		break;
		// 	}
		// }
	
	srand(time(NULL));
	
	if(argc == 1){
		print_help();
		return 0;
	}else if(argc == 2){
		file.open(argv[1]);
		string message = "";
		char c;
		while(file >> noskipws >> c){
			message += c;
		}

		string key = generate_key();
		encrypt(message, key);
		printf("\n");
		printf("your key is %s\n", key.c_str());
		printf("\n");
	}else{
		file.open(argv[1]);
		string key = argv[2];
		string message = "";
		char c;
		while(file >> noskipws >> c){
			message += c;
		}

		decrypt(message, key);
	}

	return 0;
}