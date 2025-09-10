#include <cstdlib>
#include <cstdint>
#include <cstdio>

using namespace std;

const int unitlen=4;
uint8_t romdata[8]={0,0,0,0, 0,0,0,0};
int romlen[8]={0,0,0,0, 0,0,0,0};

int rom_data(const uint8_t *buf, int chunk, int map)
{
	uint8_t offsets[8]; // assert (unitlen <= 8)
	int bytes_in_iter = 0;

	int idx = 0;
	if (!map) map = 1;

	int map_reg = map;
	for (int i = 0; i < unitlen; i++)
	{
		if (map_reg & 0xf)
			break;
		map_reg >>= 4;
		idx++;
	}

	if (idx >= unitlen)
		return 0; // illegal map

	map_reg = map;
	for (int i = 0; i < unitlen; i++)
	{
		if (map_reg & 0xf)
		{
			offsets[bytes_in_iter] = idx + (map_reg & 0xf) - 1;
			bytes_in_iter++;
		}
		map_reg >>= 4;
	}

	while (chunk)
	{
		for (int i = 0; i < bytes_in_iter; i++)
		{
			*(romdata + romlen[idx] + offsets[i]) = *buf++;
			chunk--;
		}
		romlen[idx] += unitlen;
	}

	return 1;
}

void print_merged() {
	for(int i=0; i<8; i++) {
		printf("%X ",romdata[i]);
	}
	printf("- got\n");
	printf("0 0 1 0 2 0 3 0 - expected\n");
}

int main() {
	uint8_t buf[8]={0,1,2,3,4,5,6,7};
	int map=0x0201;
	rom_data(buf, 8, map);
	print_merged();
}