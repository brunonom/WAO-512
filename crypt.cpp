#include <bits/stdc++.h>
using namespace std;

void print_help(){
	printf("\n");
	printf("usage: ./crypt [path to txt file]\n");
	printf("       ./crypt [path to txt file] [decryption key]\n");
	printf("\n");
}

fstream file;

int subs[128] = {43, 111, 92, 23, 10, 121, 40, 116, 77, 107, 38, 14, 98, 89, 3, 78,
				12, 20, 35, 16, 8, 106, 60, 117, 110, 5, 18, 13, 33, 93, 45, 62,
				112, 61, 63, 41, 85, 114, 100, 123, 9, 126, 57, 115, 25, 34, 51, 27,
				81, 104, 88, 1, 56, 95, 75, 15, 55, 66, 113, 82, 4, 76, 105, 48,
				50, 79, 90, 65, 11, 26, 54, 6, 7, 86, 68, 59, 21, 120, 49, 125,
				36, 58, 108, 119, 99, 102, 74, 28, 17, 0, 118, 101, 84, 47, 72, 37,
				94, 69, 39, 30, 53, 24, 64, 97, 122, 19, 29, 80, 124, 52, 91, 73,
				2, 42, 127, 71, 67, 44, 96, 46, 87, 32, 103, 31, 109, 22, 83, 70};

int mult[16] = {1, 2, 2, 3, 3, 1, 3, 2, 1, 3, 1, 1, 1, 1, 3, 1};
// int mult[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	
map<int, int> subs_table;
map<int, int> rev_subs_table;

int getfilesize(fstream& file){
	file.seekg(0, ios::end);
	int size = file.tellg();
	file.seekg(0);
	return size;
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
	for(int i=0; i<256; i+=4){
		int r = (bits[i]-'0')*8 + (bits[i+1]-'0')*4 + (bits[i+2]-'0')*2 + (bits[i+3]-'0');
		hex += int_to_hex(r);
	}
	return hex;
}

string generate_key(){
	string key = "";
	for(int i=0; i<256; i++){
		key += (rand()%2)? "0" : "1";
	}
	return bin_to_hex(key);
}

void substitution(char* block){
	for(int i=0; i<64; i++){
		block[i] = subs_table[block[i]];
	}
}

void rev_substitution(char* block){
	for(int i=0; i<64; i++){
		block[i] = rev_subs_table[block[i]];
	}
}

void copy_cube(char(*a)[4][4], char(*b)[4][4]){
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				a[i][j][k] = b[i][j][k];
			}
		}
	}
}

void permutation(char* block){
	// block = "abcdefghijklmnopABCDEFGHIJKLMNOP.,;:!?{}[]()@$&*qrstuvwxQRSTUVWX";
	//abcdefghijklmnopABCDEFGHIJKLMNOP.,;:!?{}[]()@$&*qrstuvwxQRSTUVWX
	//aG[WfL$tkM;upB}RA{QoF)VdK@seP,xj.wiO?TnD(UcE*rhJqgI&vlN:SmC!XbH]
	// printf("%s\n", block);
	int count = 0;
	char mat[4][4][4], copy[4][4][4];
	
	// string to cube
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[i][j][k] = block[count];
				copy[i][j][k] = block[count];
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
	copy_cube(copy, mat);

	// y shift
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[i][k][j] = copy[i][(k+j)%4][j];
			}
		}
	}
	copy_cube(copy, mat);

	// z shift
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[k][j][i] = copy[(k+i)%4][j][i];
			}
		}
	}
	copy_cube(copy, mat);

	// cube to string
	count = 0;
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				block[count++] = mat[i][j][k];
			}
		}
	}
	// printf("%s\n", block);
}

void rev_permutation(char* block){
	// block = "abcdefghijklmnopABCDEFGHIJKLMNOP.,;:!?{}[]()@$&*qrstuvwxQRSTUVWX";
	//abcdefghijklmnopABCDEFGHIJKLMNOP.,;:!?{}[]()@$&*qrstuvwxQRSTUVWX
	//aG[WfL$tkM;upB}RA{QoF)VdK@seP,xj.wiO?TnD(UcE*rhJqgI&vlN:SmC!XbH]
	// printf("%s\n", block);
	int count = 0;
	char mat[4][4][4], copy[4][4][4];
	
	// string to cube
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[i][j][k] = block[count];
				copy[i][j][k] = block[count];
				count++;
			}
		}
	}

	// z shift
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[k][j][i] = copy[(k-i+4)%4][j][i];
			}
		}
	}
	copy_cube(copy, mat);

	// y shift
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[i][k][j] = copy[i][(k-j+4)%4][j];
			}
		}
	}
	copy_cube(copy, mat);

	// x shift
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				mat[i][j][k] = copy[i][j][(k-j+4)%4];
			}
		}
	}
	copy_cube(copy, mat);

	// cube to string
	count = 0;
	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
			for(int k=0; k<4; k++){
				block[count++] = mat[i][j][k];
			}
		}
	}
	// printf("%s\n", block);
}

string expand(string key, int round){
	string e = "----------------------------------------------------------------";
	for(int i=0; i<4; i++){
		e[i*16] = key[round*4 + i];
		for(int j=1; j<16; j++){
			e[i*16+j] = int_to_hex(((key[round*4 + i] + i + j)*mult[j])%16);
		}
	}
	return e;
}

void combine_key(char* block, string key, int round){
	string temp = expand(key, round);
	string expanded = temp;
	for(int i=0; i<64; i++){
		block[i] ^= expanded[i];
	}
}

void encrypt(char* block, string key){
	for(int i=0; i<16; i++){
		combine_key(block, key, i);
		substitution(block);
		permutation(block);
	}
}

void decrypt(char* block, string key){
	for(int i=0; i<16; i++){
		rev_permutation(block);
		rev_substitution(block);
		combine_key(block, key, 15-i);
	}
}

int main(int argc, char** argv){
	
	for(int i=0; i<128; i++){
		subs_table[i] = subs[i];
		rev_subs_table[subs[i]] = i;
	}

	// checks if substitution is asymmetric and irreflexive
		// for(int i=0; i<128; i++){
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
		// exit(0);
	
	srand(time(NULL));
	char block[65];
	block[64] = '\0';
	string key;
		
	if(argc == 1){
		print_help();
		return 0;
	}else if(argc == 2){
		file.open(argv[1]);
		fstream encrypted("./encrypted.txt", ios::out);		
		
		key = generate_key();

		int padding = getfilesize(file);
		padding = 64 - padding%64;
		while(file.read(block, 64)){
			encrypt(block, key);
			encrypted.write(block, 64);
		}
		if(padding){
			block[63] = padding;
			encrypt(block, key);
			encrypted.write(block, 64);
		}
		encrypted.close();

		printf("\nyour key is %s\n\n", key.c_str());
	}else{
		file.open(argv[1]);
		fstream decrypted("./decrypted.txt", ios::out);		
		fstream temp("./temp.txt", ios::out);		
		
		key = argv[2];
		
		while(file.read(block, 64)){
			decrypt(block, key);
			temp.write(block, 64);
		}

		temp.close();
		temp.open("./temp.txt");
		
		int filesize = getfilesize(temp);
		temp.seekg(-1, ios::end);
		temp.read(block, 1);
		int padding = block[0];
		temp.seekg(0, ios::beg);

		for(int i=0; i<(filesize - padding); i++){
			temp.read(block, 1);
			decrypted.write(block, 1);
		}
		temp.close();
		decrypted.close();
		remove("./temp.txt");
	}

	return 0;
}