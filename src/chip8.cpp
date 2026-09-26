#include <cstdint>
#include <cstring>
#include <fstream>
#include "chip8.hpp"
#include <chrono>
#include <random>
const unsigned int START_ADDRESS= 0X200;
const unsigned int FONTSET_SIZE=80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
uint8_t fontset[FONTSET_SIZE]=

{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
void Chip8::loadROM(char const* filename)
{
std::ifstream file(filename,std::ios::binary|std::ios::ate);

if (file.is_open())
{
    std::streampos size = file.tellg();  //get file size and allocate buffer
    char* buffer = new char[size];

    file.seekg(0,std::ios::beg);
    file.read(buffer,size);
    file.close();//go beginning and reads the file and inputs it in bugffer

    for(long i=0;i<size;++i)
    {
        memory[START_ADDRESS + i]= buffer[i];

    }
delete[]buffer;
}
};
void Chip8::OP_00E0()
{
	memset(video,0, sizeof(video));
}
void Chip8::OP_00EE()
{
	--sp;
	pc=stack[sp];
}
void Chip8::OP_1nnn()
{
	uint16_t address = opcode & 0x0FFFu;
	pc= address;
}
void Chip8::OP_2nnn()
{
	uint16_t address = opcode & 0x0FFFu;
	stack[sp]=pc;
	++sp;
	pc = address;
}
void Chip8::OP_3xkk()
{
	uint8_t Vx= (opcode & 0x0F00u)>>8u;
	uint8_t byte = opcode & 0x00FFu;
	if (registers[Vx]==byte)
	{
		pc+=2;
	}
}
void Chip8::OP_4xkk()
{
	uint8_t Vx=(opcode & 0x0F00u)>>8u;
	uint8_t byte = opcode & 0x00FFu;
	if(registers[Vx]!=byte)
	{
		pc+=2;
	}
}
void Chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >>8u;
	uint8_t Vy = (opcode & 0x00F0u)>> 4u;
	if(registers[Vx]==registers[Vy])
	{
		pc+=2;
	}
}
void Chip8::OP_6xkk()
{
	uint8_t Vx= (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;
	registers[Vx]=byte;
}
void Chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u)>> 8u;
	uint8_t byte = opcode & 0x00FFu;
      registers[Vx]+=byte;

}
void Chip8::OP_8xy0() // equal to
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1() //OR
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}
void Chip8::OP_8xy2() //and
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}
void Chip8::OP_8xy3() //xor
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}
void Chip8::OP_8xy4()
{
	uint8_t Vx= (opcode & 0x0F00u)>>8u;
	uint8_t Vy= (opcode & 0x00F0u)>>4u;

	uint8_t sum = registers[Vx]+registers[Vy];
	if(sum>255U)
	{
		registers[0xF]= 1;

	}
    else 
	{
     registers[0xF]=0;
    }
    registers[Vx]= sum & 0xFFu;
}
void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}
void Chip8::OP_8xy6()
{
	uint8_t Vx=(opcode & 0x0F00u)>>8u;
	registers[0xF]=(registers[Vx]&0x1u);//lsb save in VF
	registers[Vx]>>=1;
}

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}
void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}
void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if( registers[Vx]!= registers[Vy])
	{
		pc+=2;
	}
}
Chip8::Chip8(): randGen(std::chrono::system_clock::now().time_since_epoch().count())//generate random numbrs based on time or shi
{
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
    pc=START_ADDRESS;
    for(unsigned int i= 0; i< FONTSET_SIZE;++i)
    {
        memory[FONTSET_START_ADDRESS+i]= fontset[i];
    }
    
}	

