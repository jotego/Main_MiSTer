#include <cstdlib>
#include <cstdint>
#include <cstdio>

using namespace std;

int unitlen=4;
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
	bool first = true;
	int gaps = 0;
	for (int i = 0; i < unitlen; i++)
	{
		if (map_reg & 0xf)
		{
			offsets[bytes_in_iter] = idx + (map_reg & 0xf) - 1 + gaps;
			bytes_in_iter++;
			first = false;
		} else if( !first )
		{
			gaps++;
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

void print_merged(const char *expected) {
	for(int i=0; i<8; i++) printf("%X",romdata[i]);
	printf(" - got\n");
	printf("%s - expected\n", expected);
}

void test_map(int map, const char *expected ) {
	uint8_t buf[8]={0,1,2,3,4,5,6,7};
	for(int i=0; i<8; i++) {
		romdata[i]=0;
		romlen[i]=0;
	}
	uint8_t *aux=buf;
	for(int i=0; i<8; i+=unitlen)
	{
		rom_data(aux,unitlen, map);
		aux+=unitlen;
	}
	for(int i=0; i<8; i++) {
		if(expected[i]==0){
			printf("Expected is too short!\n");
			exit(1);
		}
		if(romdata[i]!=(uint8_t)(expected[i]-'0')) {
			printf("map=%04X -> %X != %c at index %d\n",map,romdata[i],expected[i],i);
			print_merged(expected);
			exit(1);
		}
	}
}

int main() {
	unitlen=2;
	test_map(0x01,"00102030");
	test_map(0x10,"00010203");
	test_map(0x21,"01234567");
	test_map(0x12,"10325476");
	unitlen=4;
	test_map(0x0001,"00001000");
	test_map(0x4321,"01234567");
	test_map(0x0012,"10003200");
	test_map(0x1234,"32107654");
	test_map(0x1200,"00100032");
	test_map(0x2100,"00010023");
	test_map(0x0021,"01002300");
	test_map(0x0201,"00102030");
	test_map(0x2001,"00012003");
	unitlen=8;
	test_map(0x0000'0001,"00000000");
	test_map(0x0000'0021,"01000000");
	test_map(0x0000'4321,"01230000");
	test_map(0x8765'4321,"01234567");
	test_map(0x0043'0021,"01002300");
	test_map(0x4300'0021,"01000023");
	test_map(0x0403'0201,"00102030");
	// These will fail because the sequence cannot skip bytes
	// test_map(0x0034,"32007600");
	// test_map(0x0043,"23006700");
	// test_map(0x4300,"00230067");
	return 0;
}