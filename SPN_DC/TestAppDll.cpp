// TestAppDll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>

int Sbox[16] = {0xE, 0x3, 0x0, 0x7, 0x2, 0xC, 0xF, 0xB, 0x5, 0xA, 0x6, 0x9, 0x8, 0x1, 0x4, 0xD};
int InverseSbox[16] = {0x2, 0xD, 0x4, 0x1, 0xE, 0x8, 0xA, 0x3, 0xC, 0xB, 0x9, 0x7, 0x5, 0xF, 0x0, 0x6};

extern "C" __declspec(dllexport) void Substitution(int* p, int* c)
{
	*c = (Sbox[(*p>>12 & 0xf)]<<12) | (Sbox[(*p>>8 & 0xf)]<<8) | (Sbox[(*p>>4  & 0xf)]<<4 ) | (Sbox[(*p    & 0xf)]   ) ;
}

extern "C" __declspec(dllexport) void Permutation(int* p, int* c)
{
	*c = ((*p>>15 & 1)<<15) | ((*p>>11 & 1)<<14) | ((*p>>7  & 1)<<13) | ((*p>>3  & 1)<<12) |
		((*p>>14 & 1)<<11) | ((*p>>10 & 1)<<10) | ((*p>>6  & 1)<<9 ) | ((*p>>2  & 1)<<8 ) |
		((*p>>13 & 1)<<7 ) | ((*p>>9  & 1)<<6 ) | ((*p>>5  & 1)<<5 ) | ((*p>>1  & 1)<<4 ) |
		((*p>>12 & 1)<<3 ) | ((*p>>8  & 1)<<2 ) | ((*p>>4  & 1)<<1 ) | ((*p     & 1)    ) ;
}

extern "C" __declspec(dllexport) void Substitution_Inverse(int* p, int* c)
{
	*c = (InverseSbox[(*p>>12 & 0xf)]<<12) | (InverseSbox[(*p>>8 & 0xf)]<<8) | 
		 (InverseSbox[(*p>>4  & 0xf)]<<4 ) | (InverseSbox[(*p    & 0xf)]   ) ;
}

extern "C" __declspec(dllimport) void Substitution(int* p, int* c);
extern "C" __declspec(dllimport) void Substitution_Inverse(int* p, int* c);
extern "C" __declspec(dllimport) void Permutation(int* p, int* c);
extern "C" __declspec(dllimport) void Encryption(int P, int* C);

bool bit_xor(int bit) {
	bool res = 0;

	for (int i = 0; i < 16; i++) {
		res ^= (bit >> i) & 1;
	}
	return res;
}

int max(unsigned int table[]) {
	int max = 0;

	for (int i = 1; i < 16; i++) {
		int tmp = abs((0xffff + 1) / 2 - table[max]);
		int tmp2 = abs((0xffff + 1) / 2 - table[i]);
		if (tmp < tmp2) max = i;
	}

	return max;
}

int GetRoundKey(int mask_in, int mask_out, int block) {
	int key = 0xAA71;
	unsigned int key_table[16] = { 0, };
	int res;

	for (int Plaintext = 0; Plaintext <= 0xffff; Plaintext++) {
		int Ciphertext;

		Encryption(Plaintext, &Ciphertext);
		Ciphertext ^= key;
		Substitution_Inverse(&Ciphertext, &Ciphertext);

		// Guess Key
		// P(Cipher ^ key) == P(Cipher) ^ P(key);
		Permutation(&Ciphertext, &Ciphertext);
		for (int i = 0; i <= 0xf; i++) {
			int tmp = Ciphertext;
			tmp ^= (i << (block * 4));
			Substitution_Inverse(&tmp, &tmp);
			unsigned short cipher_tmp = tmp & mask_out;
			unsigned int plain_tmp = Plaintext & mask_in;
			bool check = bit_xor(cipher_tmp) ^ bit_xor(plain_tmp);
			if (!check) key_table[i] += 1;
		}
	}
	res = max(key_table);
	for (int i = 0; i < block; i++) res *= 16;

	return res;
}

int main(void){
	short LC[16][16] = { 0, };	
	unsigned int key_table[16] = { 0, };
	unsigned int key = 0xAA71;
	int final_key = 0;

	// LC Table
	for (int i = 0; i <= 0xf; i++) {
		for (int j = 0; j <= 0xf; j++) {
			for (int h = 0; h <= 0xf; h++) {
				int tmp;

				tmp = Sbox[h] & j;
				if (((tmp >> 3) & 1) ^ ((tmp >> 2) & 1) ^ ((tmp >> 1) & 1) ^ (tmp & 1) ^ ((h & i) >> 3) ^ (((h & i) >> 2) & 1) ^ (((h & i) >> 1) & 1) ^ ((h & i) & 1) == 0) {
					LC[i][j] += 1;
				}
			}
		}
	}

	printf("\n===== LC Table =====\n");

	printf("\t");
	for (int i = 0; i <= 0xf; i++)
		printf("%X\t", i);
	printf("\n");
	for (int i = 0; i <= 0xf; i++) {
		printf("%X\t", i);
		for (int j = 0; j <= 0xf; j++) {
			printf("%d\t", LC[i][j] - 8);
		}
		printf("\n");
	}

	printf("\n");

	// GetRoundKey(mask_in, mask_out, block)
	final_key += GetRoundKey(0x4, 0x1000, 3);
	final_key += GetRoundKey(0x50, 0x900 ,2);
	final_key += GetRoundKey(0xd000, 0x30,1);
	final_key += GetRoundKey(0x1, 0x1, 0);
	Permutation(&final_key, &final_key);
	printf("0x%X", final_key);

	return 0;
}