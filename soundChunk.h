#ifndef SOUNDCHUNK_H_INCLUDED
#define SOUNDCHUNK_H_INCLUDED
#include <inttypes.h>
#include <SDL_mixer.h>
/// <struct>SoundSt</struct>
/// <member name="sound">A pointer to a Mix_Chunk piece of audio in .wav format.</member>
/// <member name="channel">The audio channel that member sound is currently playing on, 255 represents none.</member>
/// <member name="playing">0 if the variable sound is actively playing, 1 if it is paused or not active.</member>
struct SoundSt{
    Mix_Chunk *sound;
    uint8_t channel;
    uint8_t playing;
};
/// <summary>Initializes a SoundSt struct.</summary>
/// <param name="name">The relative path to the sound file being loaded.</param>
/// <returns>Returns a pointer to a SoundSt struct with audio file specified by filename name loaded into it.</returns>
struct SoundSt* initSoundFile(char *name);
/// <summary>Frees a SoundSt struct.</summary>
/// <param name="sd">A pointer to the SoundSt struct that is going to be freed.</param>
void freeSoundSt(struct SoundSt *sd);
/// <summary>Sets the SoundSt struct to an inactive state.</summary>
/// <param name="sd">A pointer to the SoundSt struct that is being made inactive.</param>
void audioReset(struct SoundSt *sd);
/// <summary>Sets a SoundSt struct to an active state.</summary>
/// <param name="sd">The SoundSt struct that is being set to an active state.</param>
void setAudio(struct SoundSt *sd);
#endif // SOUNDCHUNK_H_INCLUDED
