#ifndef EMULATOR_H_INCLUDED
#define EMULATOR_H_INCLUDED
#include <stdio.h>
#include <SDL.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "GUIComp.h"
#include "util.h"
#include "soundChunk.h"
#include <windows.h>

///The window width of the Space Invaders screen.
#define WINDOW_WIDTH 224
///The window height of the Space Invaders screen.
#define WINDOW_HEIGHT 255

/// <struct>inputCodes</struct>
/// <member name="left">Scancode for Player 1 pressing the left button.</member>
/// <member name="right">Scancode for Player 1 pressing the right button.</member>
/// <member name="shoot">Scancode for Player 1 pressing the shoot button.</member>
/// <member name="coin">Scancode to simulate inserting a coin into the arcade machine.</member>
/// <member name="onePlayer">Scancode for the 1 Player button being pressed.</member>
/// <member name="saveState">Scancode for creating a save-state at the current time.</member>
/// <member name="loadState">Scancode for loading a save-state.</member>
/// <member name="twoPlayer">Scancode for the 2 Player button being pressed.</member>
/// <member name="shoot2">Scancode for Player 2 pressing the shoot button.</member>
/// <member name="left2">Scancode for Player 2 pressing the left button</member>
/// <member name="right2">Scancode for Player 2 pressing the right button.</member>
/// <member name="pauseEm">Scancode for pausing the emulation of Space Invaders.</member>
/// <remarks>Input scancodes for the Space Invaders arcade machine and the emulator.</remarks>
struct inputCodes{
    uint8_t left;
    uint8_t right;
    uint8_t shoot;
    uint8_t coin;
    uint8_t onePlayer;
    uint8_t saveState;
    uint8_t loadState;
    uint8_t twoPlayer;
    uint8_t shoot2;
    uint8_t left2;
    uint8_t right2;
    uint8_t pauseEm;
};
/// <struct>conditions</struct>
/// <member name="zero">The Flag for 0 in the 8080.</member>
/// <member name="sign">The Flag for a 8 bit number having it`s seventh bit(0x80) as 1 in the 8080.</member>
/// <member name="parity">The Flag for bit parity in the 8080. A 1 represents even parity, while a zero is odd parity.</member>
/// <member name="carryBit">The Flag for a bit that represents a carry out of the most significant bit in the 8080.</member>
/// <member name="auxCarryBit">The Flag for a bit that represents a carry out of the third bit in the 8080.</member>
/// <member name="pad">Filler data to have a 1 byte total bit-field.</member>
/// <remarks>Represents the data in the Flag Register(F) in the 8080.</remarks>
struct conditions{
    uint8_t zero:1;
    uint8_t sign:1;
    uint8_t parity:1;
    uint8_t carryBit:1;
    uint8_t auxCarryBit:1;
    uint8_t pad:3;
};
/// <struct>emuStruct</struct>
/// <member name="screen_size">The size of window the game is rendered onto either 1,2,3,4(multiplied by the WINDOW_WIDTH and WINDOW_HEIGHT defines.)</member>
/// <member name="speed">The speed at which the emulation runs. Potential values used are: 4,2,1,1/2,1/4.</member>
/// <member name="currentVolume">The current volume level of the sound being emitted. A value from 0-128.</member>
/// <member name="isFull">1 if the emulator is currently running in full-screen mode. 0 if it is in windowed mode.</member>
/// <member name="gameSpace">A pointer to a SDL_Rect that represents the area that Space Invaders is rendered onto.</member>
/// <remarks>Contains data that the emulator uses for general options.</remarks>
struct emuStruct{
    uint8_t screen_size;
    double speed;
    int8_t currentVolume;
    int8_t isFull;
    SDL_Rect *gameSpace;
};
/// <struct>currentSystemStates</struct>
/// <member name="A">Represents Register A(Accumulator) in the 8080.</member>
/// <member name="B">Represents Register B in the 8008.</member>
/// <member name="C">Represents Register C in the 8080.</member>
/// <member name="D">Represents Register D in the 8080.</member>
/// <member name="E">Represents Register E in the 8080.</member>
/// <member name="H">Represents Register H in the 8080.</member>
/// <member name="L">Represents Register L in the 8080.</member>
/// <member name="StackPointer">Represents the dedicated 16 bit Stack Pointer in the 8080.</member>
/// <member name="ProgramCounter">Represents the dedicated 16 bit Program Counter in the 8080.</member>
/// <member name="memory">A pointer to a section of data that represents the RAM and ROM of the Space Invader arcade machine.</member>
/// <member name="cs">Represents the Flags Register(F) in the 8080.</member>
/// <member name="interrupt_enable">0 if CPU interrupts are disabled, 1 if they are enabled.</member>
/// <member name="halt">Simulates the halt command in the 8080, which stops CPU execution until an interrupt occurs.</member>
/// <member name="shift0">The data in the simulated shift hardware that occupies the least significant byte.</member>
/// <member name="shift1">The data in the simulated shift hardware that occupies the most significant byte.</member>
/// <member name="shiftOffset">The current shift offset in the simulated shifting hardware.</member>
/// <member name="inPort1">The value of the data that is currently in the simulated hardware port 1.</member>
/// <member name="inPort2">The value of the data that is currently in the simulated hardware port 2.</member>
/// <member name="outPort3">The value of the data that is currently in out port 3, which is part of the simulated sound-card.</member>
/// <member name="outPort5">The value of the data that is currently in out port 5, which is part of the simulated sound-card.</member>
/// <member name="lastP3">The previously listed value from outPort3 used to see when audio needs to stop.</member>
/// <member name="lastP5">The previously listed value from outPort5 used to see when audio needs to stop.</member>
/// <remarks>Contains data in the shift hardware and data from the Ports in addition to the CPU data.</remarks>
struct currentSystemStates{
    uint8_t A;
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t H;
    uint8_t L;
    uint16_t StackPointer;
    uint16_t ProgramCounter;
    uint8_t *memory;
    struct conditions cs;
    uint8_t interrupt_enable;
    uint8_t halt;
    uint8_t shift0;
    uint8_t shift1;
    uint8_t shiftOffset;
    uint8_t inPort1;
    uint8_t inPort2;
    uint8_t outPort3;
    uint8_t outPort5;
    uint8_t lastP3;
    uint8_t lastP5;
};
/// <remarks>The number of CPU cycles each instruction took to execute in the 8080.</remarks>
unsigned char cycles8080[] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00->0x0F
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40->0x4F
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80->8x4F
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xC0->0xCF
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11
};
/// <summary>Initializes the CPU emulator to default values.</summary>
void setUp();
/// <summary>Saves the current key-mappings and emulator options to emulator.ini.</summary>
/// <param name="ipc">A pointer to an inputCode struct that contains the current key mappings for the emulator.</param>
void saveInput(struct inputCodes *ipc);
/// <summary>Loads the current key-mappings and emulator options from emulator.ini into the program.</summary>
/// <param name="ipc">A pointer to an inputCode struct that contains the current key mappings for the emulator.</param>
void loadInput(struct inputCodes *ipc);
/// <summary>Saves the current state of the CPU and the memory state to saveState.ss.</summary>
void saveState();
/// <summary>Loads the last state of the CPU that was saved to saveState.ss into the CPU.</summary>
void loadState();
/// <summary>Emulates the Space Invaders arcade machine. Also loads menus and options that modify the way the game is played.</summary>
void Emulate8080();
/// <summary>Function used to swap two key-mappings.</summary>
/// <param name="textboxes">A pointer to a section of memory that contains pointers to textboxes(The key-mapping textboxes).<param>
/// <param name="oldValue">A pointer to the value that is being swapped into a Textbox.<param>
/// <param name="newValue">The value being compared to each Textbox to see if a key-mapping is already defined.<param>
void doSwap(struct Textbox **textboxes,uint8_t *oldValue,uint8_t newValue);
/// <summary>Emulates the CPU of the Space Invaders arcade machine.</summary>
/// <param name="lastTimer">The time-in nanoseconds-at which the last execution of the CPU occurred.</param>
/// <param name="nextInterrupt">The time-in nanoseconds-at which the next render interrupt will occur.</param>
/// <param name="whichInterrupt">Value of 1 or 2 based on which render interrupt was last handled.</param>
/// <param name="surf">The window surface that everything is rendered onto.</param>
void doCPU(uint64_t *lastTimer,uint64_t *nextInterrupt,uint8_t *whichInterrupt,SDL_Surface *surf);
/// <summary>Renders the Space Invaders rendering onto the screen.</summary>
/// <param name="surf">The window surface that everything is rendered onto.</param>
void RAMToRendered(SDL_Surface *surf);
/// <summary>Emulates the CPU`s inward-bound interaction with other hardware.</summary>
/// <param name="port">The number of the port with which the CPU is going to receive data from.</param>
/// <returns>Returns the data that was sent from port specified by the parameter port.</returns>
uint8_t MachineIn(uint8_t port);
/// <summary>Emulates the CPU`s outward bound interaction with other hardware.</summary>
/// <param name="port">The number of the port with which the CPU is going to send data to.</param>
void MachineOut(uint8_t port);
/// <summary>Generates an interrupt to the emulated processor in order to update the graphical view of Space Invaders.</summary>
/// <param name="num">The number of the interrupt that is being generated.</param>
void generateInterrupt(int num);
/// <summary>Writes data into the simulated memory space of the Space Invaders arcade machine.</summary>
/// <param name="item">The piece of data that is being written to memory.</param>
/// <param name="address">The address in memory to which parameter item is being written to.</param>
void writeToMemory(uint8_t item,uint16_t address);
/// <summary>Sets the 8080 Zero,Sign and Parity Flags variables based on the last operation.</summary>
/// <param name="value">The value used to set the CPU Flags.</param>
void setZSP(uint8_t value);
/// <summary>Sets the 8080 CPU Flags for any arithmetic operations.</summary>
/// <param name="value">The value being used to set the CPU Flags.</param>
void setAFlags(uint16_t value);
/// <summary>Sets the 8080 CPU Flags for any logical operations.</summary>
/// <param name="value">The value being used to set the CPU Flags.</param>
void setLFlags(uint8_t value);
/// <summary>Sets the 8080 CPU Flags for any compare operation.</summary>
/// <param name="val1">The value in a particular Register.</param>
/// <param name="val2">The value being compared to a particular Register.</param>
void setCFlags(uint8_t val1,uint8_t val2);
/// <summary>Simulates a Call to a specific address-typically interrupt related-in the 8080 CPU.</summary>
/// <param name="currentOpcode">A memory address that the Program Counter is being set to.</param>
void callLoc(uint16_t currentOpcode);
/// <summary>Simulates a Call in the 8080 CPU.</summary>
/// <param name="currentOpcode">A pointer to a memory address that the Program Counter is being set to.</param>
void call(uint8_t *currentOpcode);
/// <summary>Simulates a Return statement in the 8080.</summary>
void returnFrom();
/// <summary>Pops the last two items on the Program Stack into a Register Pair.</summary>
/// <param name="high">A pointer to the leading Register of a Register Pair.</param>
/// <param name="low">A pointer to the trailing Register of a Register Pair.</param>
void PopStack(uint8_t *high,uint8_t *low);
/// <summary>Pushes two items onto the Program Stack.</summary>
/// <param name="high">The most significant byte of the item being pushed onto the Stack.</param>
/// <param name="low">The least significant byte of the item being pushed onto the Stack.</param>
void PushStack(uint8_t high,uint8_t low);
/// <summary>Gets the current time in nanoseconds.</summary>
/// <returns>Returns the current time in nanoseconds.</returns>
uint64_t timeusec();
/// <summary>Emulates the instruction set of the 8080.</summary>
/// <returns>Returns the number of cycles that the last executed instruction took.</returns>
uint8_t EmulateInstruction();
#endif // EMULATOR_H_INCLUDED
