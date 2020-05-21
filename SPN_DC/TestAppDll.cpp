// TestAppDll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

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


int main(int argc, char* argv[])
{
	int LC[16][16] = { 0, };
	int Plaintext, Ciphertext;
	
//	Plaintext = 0x26B7;
//	Plaintext = 0x0000;

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

	return 0;
}