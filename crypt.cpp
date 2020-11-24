#include <bits/stdc++.h>
using namespace std;

void print_help(){
	printf("\n");
	printf("usage: ./crypt [path to txt file]\n");
	printf("       ./crypt [path to txt file] [decryption key]\n");
	printf("\n");
}

fstream file;

int subs[256] = {89, 33, 167, 73, 129, 148, 19, 131, 140, 30, 235, 66, 94, 112, 242, 172, 
				70, 104, 188, 93, 24, 187, 145, 63, 81, 199, 38, 113, 134, 192, 162, 51, 
				205, 22, 194, 60, 186, 96, 142, 208, 72, 123, 75, 161, 77, 7, 114, 191, 
				2, 201, 37, 88, 249, 168, 6, 79, 109, 206, 12, 14, 108, 209, 244, 5, 
				100, 76, 212, 211, 228, 8, 126, 179, 184, 65, 56, 23, 177, 122, 139, 222, 
				111, 45, 135, 225, 61, 207, 216, 178, 101, 97, 50, 226, 165, 255, 3, 83, 
				224, 32, 90, 68, 124, 117, 210, 120, 39, 220, 125, 138, 173, 52, 229, 31, 
				233, 57, 103, 163, 203, 236, 221, 28, 193, 35, 10, 147, 170, 169, 245, 44, 
				86, 218, 133, 132, 18, 153, 253, 17, 15, 48, 49, 189, 116, 237, 91, 34, 
				171, 85, 160, 20, 84, 92, 144, 1, 154, 252, 80, 164, 127, 36, 231, 42, 
				151, 251, 11, 54, 106, 215, 223, 157, 174, 175, 16, 29, 152, 239, 204, 234, 
				87, 190, 198, 64, 110, 98, 200, 195, 107, 115, 40, 197, 247, 46, 43, 214, 
				180, 181, 27, 254, 149, 240, 227, 202, 41, 82, 166, 47, 243, 136, 102, 213, 
				0, 62, 121, 26, 232, 156, 230, 53, 69, 248, 182, 250, 158, 78, 128, 238, 
				4, 219, 183, 58, 74, 21, 141, 185, 146, 196, 130, 13, 119, 176, 105, 67, 
				143, 155, 99, 241, 25, 59, 159, 71, 95, 150, 118, 55, 9, 246, 217, 137};
	
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

int hex_to_int(char c){
	if(c<='9'){
		return c-'0';
	}else{
		return c-'a'+10;
	}
}

string bin_to_hex(string bits){
	string hex = "";
	for(int i=0; i<512; i+=4){
		int r = (bits[i]-'0')*8 + (bits[i+1]-'0')*4 + (bits[i+2]-'0')*2 + (bits[i+3]-'0');
		hex += int_to_hex(r);
	}
	return hex;
}

int byte_to_int(char c){
	int r = 0;
	int m = 1;
	for(int i=0; i<8; i++){
		r += (c & 0b00000001)*m;
		c >>= 1;
		m *= 2;
	}
	return r;
}

string generate_key(){
	string key = "";
	for(int i=0; i<512; i++){
		key += (rand()%2)? "0" : "1";
	}
	return bin_to_hex(key);
}

void substitution(char* block){
	for(int i=0; i<64; i++){
		block[i] = subs_table[byte_to_int(block[i])];
	}
}

void rev_substitution(char* block){
	for(int i=0; i<64; i++){
		block[i] = rev_subs_table[byte_to_int(block[i])];
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
}

void rev_permutation(char* block){
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
}

void caesar(char* block){
	for(int i=0; i<64; i++){
		block[i] += 22;
	}
}

void rev_caesar(char* block){
	for(int i=0; i<64; i++){
		block[i] -= 22;
	}
}

string expand(string key, int round){
	string exp(128, '-');
	string bytes(8, '-');
	string copy(8, '-');
	
	for(int i=0; i<8; i++){
		copy[i] = key[round*8 + i];
	}

	for(int i=0; i<8; i++){
		bytes[i] = copy[(i+1)%8];
	}

	for(int i=0; i<8; i++){
		for(int j=0; j<16; j++){
			exp[i+8*j] = bytes[i];
		}
	}
	
	for(int i=0; i<128; i++){
		exp[i] = int_to_hex(hex_to_int(exp[i]) ^ hex_to_int(key[i]));
	}
	
	return exp;
}

void combine_key(char* block, string key, int round){
	string expanded = expand(key, round);
	for(int i=0; i<64; i++){
		block[i] ^= (hex_to_int(expanded[i*2]) << 4) | hex_to_int(expanded[i*2+1]);
	}
}

void encrypt(char* block, string key){
	for(int i=0; i<16; i++){
		combine_key(block, key, i);
		substitution(block);
		permutation(block);
	}
	caesar(block);
}

void decrypt(char* block, string key){
	rev_caesar(block);
	for(int i=0; i<16; i++){
		rev_permutation(block);
		rev_substitution(block);
		combine_key(block, key, 15-i);
	}
}

int main(int argc, char** argv){
	
	for(int i=0; i<256; i++){
		subs_table[i] = subs[i];
		rev_subs_table[subs[i]] = i;
	}

	// checks if s-box is asymmetric and irreflexive
		// for(int i=0; i<256; i++){
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