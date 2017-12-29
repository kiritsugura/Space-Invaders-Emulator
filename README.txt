This is an emulator for the Space Invaders arcade machine that I made during the summer.
This is a version which I spent a little bit of time optimizing the rendering aspect of, which I am releasing to public domain. I made this for fun, so I hope that this can be of use to someone other than myself if at all possible.
Program related information:
I used the CodeBlocks IDE for development, with GCC as the compiler.
This project is mostly ANSI standard C code, along with some SDL code that I used for the graphical and audio emulation.
On my CPU(an i7), it runs using less than 1.5% of my CPU on average when at the third resolution(672 by 768) on normal speed. 
The main factor that affects the CPU usage is the renderering of the graphics, while the secondary factor is the speed at which the emulated CPU runs.
The refresh rate is the same as the Arcade Machine(60 fps), but instead of rendering a half screen each frame, I render the full screen at 30fps instead.
I had made this with the intention of it being as close to cross-platfrom as possible, but I had to use some windows specific functions(like Sleep) to reduce the CPU usage.
I found the hardware information for the machine at: http://computerarcheology.com/Arcade/SpaceInvaders/
I got the sound samples from the MAME website: https://www.arcadeathome.com/dl.phtml?samples/invaders.zip|
If anyone has any comments or questions please feel free to contact me.
