#include "soundChunk.h"
struct SoundSt* initSoundFile(char *name){
    struct SoundSt *ptr=malloc(sizeof(struct SoundSt));
    ptr->channel=255;
    ptr->playing=1;
    ptr->sound=Mix_LoadWAV(name);
    return ptr;
}
void freeSoundSt(struct SoundSt *sd){
    Mix_FreeChunk(sd->sound);
    free(sd);
}
void audioReset(struct SoundSt *sd){
    sd->channel=255;
    sd->playing=1;
}
void setAudio(struct SoundSt* sd){
    sd->channel=Mix_PlayChannel(-1,sd->sound,0);
    sd->playing=0;
}
