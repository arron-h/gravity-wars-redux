#include "stdafx.h"

// Code originally by Gary Symons.
void HashMap_TEAEncipher(unsigned int *v,unsigned int *w,unsigned int *k,unsigned int n)
	{
	unsigned int y,z,delta,sum;

	y=v[0];z=v[1];delta=0x9E3779B9;sum=0;

	while(n-->0)
		{
		y += (z << 4 ^ z >> 5) + z ^ sum + k[sum&3];
		sum += delta;
		z += (y << 4 ^ y >> 5) + y ^ sum + k[sum>>11 & 3];
		}

	w[0]=y; w[1]=z;
	}

Uint32 HashMap_Hash(const char* c_pszString)
	{
	char  Key[16] = "3422FE0F793B630";
	Uint32 Hash[2];
	Sint32 i;

	Hash[0] = 0;
	Hash[1] = 0;
	if(!c_pszString) 
		return 0;

	i = 0;
	while(c_pszString[i])
		{
		Key[i & 15] = Key[i & 15] + c_pszString[i];
		i++;
		if((i & 15) == 0) HashMap_TEAEncipher(Hash, Hash, (Uint32 *)Key, 32);
		}

	if(i & 15) HashMap_TEAEncipher(Hash, Hash, (Uint32 *)Key, 32);

	return (Hash[0] + Hash[1]);
	}
