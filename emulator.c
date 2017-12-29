#include "emulator.h"

uint8_t reset=0;
uint16_t SCREEN_WIDTH,SCREEN_HEIGHT;
struct emuStruct *emu;
struct Button *display,*sound,*input,*misc;
struct Label *emulationRunning;
struct inputCodes *ipc;
struct Menu *inputMenu,*miscMenu,*soundMenu,*displayMenu;
struct currentSystemStates *state;
SDL_Rect *topBarBounds;
SDL_Window* window=NULL;
struct SoundSt *s0,*s1,*s2,*s3,*s4,*s5,*s6,*s7,*s8;
SDL_DisplayMode *displayMode;
int main(int argc,char *argv[]){
    FILE *file=fopen("testGame\\invaders.rom","rb");
    if(file==NULL)
        exit(0);
    fseek(file,0L,SEEK_END);
    uint64_t fileSize=ftell(file);
    fseek(file,0L,SEEK_SET);
    uint8_t *buffer=malloc(5 * 0x1000);
    if(buffer==NULL)
        exit(2);
    fread(buffer,fileSize,1,file);
    fclose(file);
    if(SDL_Init(SDL_INIT_VIDEO)<0)
        exit(3);
    if(Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096)==-1)
        exit(4);
    if(TTF_Init()<0)
        exit(5);
    state=malloc(sizeof(struct currentSystemStates));
    emu=malloc(sizeof(struct emuStruct));
    emu->gameSpace=malloc(sizeof(SDL_Rect));
    state->memory=buffer;
    setUp();
    ipc=malloc(sizeof(struct inputCodes));
    loadInput(ipc);
    displayMode=malloc(sizeof(SDL_DisplayMode));
    SDL_GetDesktopDisplayMode(0,displayMode);
    s0=initSoundFile("sound//0.wav");
    s1=initSoundFile("sound//1.wav");
    s2=initSoundFile("sound//2.wav");
    s3=initSoundFile("sound//3.wav");
    s4=initSoundFile("sound//4.wav");
    s5=initSoundFile("sound//5.wav");
    s6=initSoundFile("sound//6.wav");
    s7=initSoundFile("sound//7.wav");
    s8=initSoundFile("sound//8.wav");
    do{
        Emulate8080();
    }while(reset);
    freeLabel(emulationRunning);
    Mix_CloseAudio();
    saveInput(ipc);
    free(state->memory);
    free(state);
    free(emu->gameSpace);
    free(emu);
    free(displayMode);
    free(ipc);
    freeSoundSt(s0);
    freeSoundSt(s1);
    freeSoundSt(s2);
    freeSoundSt(s3);
    freeSoundSt(s4);
    freeSoundSt(s5);
    freeSoundSt(s6);
    freeSoundSt(s7);
    freeSoundSt(s8);
    return 1;
}
void setUp(){
    state->interrupt_enable=0;
    state->A=0;
    state->B=0;
    state->C=0;
    state->D=0;
    state->E=0;
    state->H=0;
    state->L=0;
    state->StackPointer=0;
    state->shift0=0;
    state->shift1=0;
    state->inPort1=0;
    state->halt=0;
    state->inPort2=0;
    state->shiftOffset=0;
    state->ProgramCounter=0;
    state->outPort3=0;
    state->outPort5=0;
    state->lastP3=0;
    state->lastP5=0;
}
void saveInput(struct inputCodes *ipc){
    FILE *file=fopen("emulator.ini","wb");
    uint8_t temp=0;
    if(file==NULL){
        printf("Could not open file.");
        exit(1);
    }
    fprintf(file,"[CO]=%c\r\n",ipc->coin);
    fprintf(file,"[ML]=%c\r\n",ipc->left);
    fprintf(file,"[MR]=%c\r\n",ipc->right);
    fprintf(file,"[SH]=%c\r\n",ipc->shoot);
    fprintf(file,"[OP]=%c\r\n",ipc->onePlayer);
    fprintf(file,"[SS]=%c\r\n",ipc->saveState);
    fprintf(file,"[SL]=%c\r\n",ipc->loadState);
    fprintf(file,"[TP]=%c\r\n",ipc->twoPlayer);
    fprintf(file,"[PE]=%c\r\n",ipc->pauseEm);
    fprintf(file,"[TL]=%c\r\n",ipc->left2);
    fprintf(file,"[TR]=%c\r\n",ipc->right2);
    fprintf(file,"[TS]=%c\r\n",ipc->shoot2);
    if(emu->speed==1.0)
        temp=1;
    else if(emu->speed==2.0)
        temp=2;
    else if(emu->speed==4.0)
        temp=3;
    else if(emu->speed==0.5)
        temp=4;
    else if(emu->speed==0.25)
        temp=5;
    fprintf(file,"[ES]=%c\r\n",temp);
    fprintf(file,"[CV]=%c\r\n",emu->currentVolume);
    fprintf(file,"[WS]=%c\r\n",emu->screen_size);
    fprintf(file,"[IF]=%c\r\n",emu->isFull);
    fclose(file);
}
void loadInput(struct inputCodes *ipc){
    ipc->coin=0;
    ipc->left=0;
    ipc->right=0;
    ipc->shoot=0;
    ipc->onePlayer=0;
    ipc->left2=0;
    ipc->loadState=0;
    ipc->pauseEm=0;
    ipc->right2=0;
    ipc->saveState=0;
    ipc->shoot2=0;
    ipc->twoPlayer=0;
    FILE *file=fopen("emulator.ini","rb");
    char line[9];
    char *maping;
    if(file==NULL)
        exit(11);
    while(fgets(line,8,file)){
        maping=strstr(line,"[CO]=");
        if(maping!=NULL)
            ipc->coin=maping[5];
        maping=strstr(line,"[ML]=");
        if(maping!=NULL)
            ipc->left=maping[5];
        maping=strstr(line,"[MR]=");
        if(maping!=NULL)
            ipc->right=maping[5];
        maping=strstr(line,"[SH]=");
        if(maping!=NULL)
            ipc->shoot=maping[5];
        maping=strstr(line,"[OP]=");
        if(maping!=NULL)
            ipc->onePlayer=maping[5];
        maping=strstr(line,"[SS]=");
        if(maping!=NULL)
            ipc->saveState=maping[5];
        maping=strstr(line,"[SL]=");
        if(maping!=NULL)
            ipc->loadState=maping[5];
        maping=strstr(line,"[TP]=");
        if(maping!=NULL)
            ipc->twoPlayer=maping[5];
        maping=strstr(line,"[PE]=");
        if(maping!=NULL)
            ipc->pauseEm=maping[5];
        maping=strstr(line,"[TL]=");
        if(maping!=NULL)
            ipc->left2=maping[5];
        maping=strstr(line,"[TR]=");
        if(maping!=NULL)
            ipc->right2=maping[5];
        maping=strstr(line,"[TS]=");
        if(maping!=NULL)
            ipc->shoot2=maping[5];
        maping=strstr(line,"[WS]=");
        if(maping!=NULL)
            emu->screen_size=maping[5];
        maping=strstr(line,"[CV]=");
        if(maping!=NULL)
            emu->currentVolume=maping[5];
        maping=strstr(line,"[IF]=");
        if(maping!=NULL)
            emu->isFull=maping[5];
        maping=strstr(line,"[ES]=");
        if(maping!=NULL){
             switch(maping[5]){
                case 1:
                    emu->speed=1;
                    break;
                case 2:
                    emu->speed=2;;
                    break;
                case 3:
                    emu->speed=4;
                    break;
                case 4:
                    emu->speed=0.50;
                    break;
                case 5:
                    emu->speed=0.25;
                    break;
            }
        }

    }
    if(ipc->coin==0)
        ipc->coin=6;
    if(ipc->left==0)
        ipc->left=4;
    if(ipc->right==0)
        ipc->right=7;
    if(ipc->shoot==0)
        ipc->shoot=26;
    if(ipc->onePlayer==0)
        ipc->onePlayer=30;
    if(ipc->saveState==0)
        ipc->saveState=60;
    if(ipc->loadState==0)
        ipc->loadState=62;
    if(ipc->pauseEm==0)
        ipc->pauseEm=58;
    if(ipc->twoPlayer==0)
        ipc->twoPlayer=31;
    if(ipc->left2==0)
        ipc->left2=13;
    if(ipc->right2==0)
        ipc->right2=15;
    if(ipc->shoot2==0)
        ipc->shoot2=12;
    fclose(file);
}
void saveState(){
    FILE *file=fopen("save//saveState.ss","wb");
    fprintf(file,"%c%c%c%c%c%c%c",state->A,state->B,state->C,state->D,state->E,state->H,state->L);
    fprintf(file,"%c%c%c%c%c",state->cs.auxCarryBit,state->cs.carryBit,state->cs.parity,state->cs.zero,state->cs.sign);
    fprintf(file,"%c%c%c%c%c",state->halt,state->interrupt_enable,state->shift0,state->shift1,state->shiftOffset);
    fprintf(file,"%c%c%c%c%c%c",state->inPort1,state->inPort2,state->outPort3,state->outPort5,state->lastP3,state->lastP5);
    fprintf(file,"%c%c",(state->ProgramCounter&0xff00)>>8,(state->ProgramCounter&0xff));
    fprintf(file,"%c%c",(state->StackPointer&0xff00)>>8,(state->StackPointer&0xff));
    fwrite(&state->memory[0x2000],1,0x2000,file);
    fclose(file);
}
void loadState(){
    FILE *file=fopen("save//saveState.ss","rb");
    if(file!=NULL){
        uint8_t temp[27];
        fread(temp,1,27,file);
        state->A=temp[0];
        state->B=temp[1];
        state->C=temp[2];
        state->D=temp[3];
        state->E=temp[4];
        state->H=temp[5];
        state->L=temp[6];
        state->cs.auxCarryBit=temp[7];
        state->cs.carryBit=temp[8];
        state->cs.parity=temp[9];
        state->cs.zero=temp[10];
        state->cs.sign=temp[11];
        state->halt=temp[12];
        state->interrupt_enable=temp[13];
        state->shift0=temp[14];
        state->shift1=temp[15];
        state->shiftOffset=temp[16];
        state->inPort1=temp[17];
        state->inPort2=temp[18];
        state->outPort3=temp[19];
        state->outPort5=temp[20];
        state->lastP3=temp[21];
        state->lastP5=temp[22];
        state->ProgramCounter=(temp[23]<<8)|temp[24];
        state->StackPointer=(temp[25]<<8)|temp[26];
        fread(&state->memory[0x2000],1,0x2000,file);
        fclose(file);
    }
}
void Emulate8080(){
    SDL_Event event;
    uint8_t whichInterrupt=1,paused=reset,done=0,currentMenu=0,result=255,tpr[4]="tmp";
    int type=0,x,y;
    uint64_t lastTimer=0,nextInterrupt;
    struct Button *activeButton=NULL,*exitInput;
    struct Textbox *activeTextbox;
    //Elements for input menu:
    struct Button **inputButtons=malloc(sizeof(struct Button*));
    struct Label **inputLabels=malloc(sizeof(struct Label*)*13),*p1StartInput,*p1ShootInput,*p1LeftInput,*p1RightInput,*p2StartInput,*p2ShootInput,*p2LeftInput,*p2RightInput,*coinInput,*pauseEmuInput,*saveEmuInput,*loadEmuInput,*inputLabel;
    struct Textbox **inputBoxes=malloc(sizeof(struct Textbox)*12),*p1StartBox,*p1ShootBox,*p1LeftBox,*p1RightBox,*p2StartBox,*p2ShootBox,*p2LeftBox,*p2RightBox,*coinBox,*pauseEmuBox,*saveEmuBox,*loadEmuBox;
    //Elements for display menu.
    struct Label **displayLabels=malloc(sizeof(struct Label*)),*resolution;
    struct Button **displayButtons=malloc(sizeof(struct Button*)),**displaySelectButtons=malloc(sizeof(struct Button*)*5),*tpbt;
    struct SelectableButton **sbButtons=malloc(sizeof(struct SelectableButton*)),*displaySB;
    //Elements for sound menu.
    struct Button **soundButtons=malloc(sizeof(struct Button*));
    struct Label **soundLabels=malloc(sizeof(struct Label*)*3),*volumeLevel,*currentVolume,*percent;
    struct Textbox **soundTextboxes=malloc(sizeof(struct Textbox)),*volumePercent;
    //Elements for misc menu.
    struct Label **miscLabels=malloc(sizeof(struct Label*)*2),*speedLabel,*miscLabel;
    struct Button **miscButtons=malloc(sizeof(struct Button*)*4),**speedButtons=malloc(sizeof(struct Button*)*5),*emuOneFourth,*emuOneHalf,*emuNormal,*emuTimesTwo,*emuTimesFour,*endEmu,*loadEmu,*saveEmu;;
    struct SelectableButton **miscSB=malloc(sizeof(struct SelectableButton*)*1),*emuSpeed;
    struct Label *emulationPaused;
    struct borderInfo *exitBorder=initBorder(0xFF888888,1,1),*tbBorder=initBorder(0xFF000000,2,2);
    SDL_Surface *screen=NULL;
    SDL_Color textColor={0,0,56};
    SDL_Rect *menuBounds=malloc(sizeof(SDL_Rect));
    topBarBounds=malloc(sizeof(SDL_Rect));
    if(reset==1){
        while(displayMode->w<=WINDOW_WIDTH*emu->screen_size && displayMode->h<=WINDOW_HEIGHT*emu->screen_size)
            emu->screen_size--;
        currentMenu=1;
    }
    emu->gameSpace->x=0;
    if(reset==2 || emu->isFull)
        emu->gameSpace->x=displayMode->w/2-(WINDOW_WIDTH*emu->screen_size)/2;
    emu->gameSpace->w=WINDOW_WIDTH*emu->screen_size;
    emu->gameSpace->h=WINDOW_HEIGHT*emu->screen_size;
    SCREEN_WIDTH=emu->gameSpace->w;
    SCREEN_HEIGHT=emu->gameSpace->h;
    Mix_Volume(-1,emu->currentVolume*128/100);
    TTF_Font *font=TTF_OpenFont("razer-regu.ttf",(uint16_t)(16*(emu->screen_size/2.0))),*heading=TTF_OpenFont("razer-regu.ttf",(uint16_t)(24*(emu->screen_size/2.0)));
    TTF_SizeText(font,"X",&x,&y);
    emu->gameSpace->y=y+5;
    SCREEN_HEIGHT=emu->gameSpace->y+emu->gameSpace->h;
    if(!emu->isFull)
        window=SDL_CreateWindow("Space Invaders Emulator",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if(reset==2 || emu->isFull){
        SCREEN_WIDTH=displayMode->w;
        SCREEN_HEIGHT=displayMode->h;
        emu->gameSpace->y=SCREEN_HEIGHT/2-emu->gameSpace->h/2;
        window=SDL_CreateWindow("Space Invaders Emulator",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);
        currentMenu=1;
    }
    if(window==NULL)
        exit(6);
    screen=SDL_GetWindowSurface(window);
    if(reset){
        SDL_FillRect(screen,NULL,0x00808080);
        RAMToRendered(screen);
    }
    TTF_SizeText(font,"Display",&x,&y);
    display=initButton(emu->gameSpace->x,0,"Display",textColor,0xAAAAAAFF,0x000088FF,font,NULL);
    sound=initButton(display->rect->x+display->rect->w+8,0,"Sound",textColor,0xAAAAAAFF,0x000088FF,font,NULL);
    input=initButton(sound->rect->x+sound->rect->w+8,0,"Input",textColor,0xAAAAAAFF,0x000088FF,font,NULL);
    misc=initButton(input->rect->x+input->rect->w+8,0,"Misc",textColor,0xAAAAAAFF,0x000088FF,font,NULL);
    topBarBounds->x=emu->gameSpace->x;
    topBarBounds->y=0;
    topBarBounds->w=emu->gameSpace->w;
    topBarBounds->h=y+5;
    TTF_SizeText(font,"Paused",&x,NULL);
    emulationPaused=initLabel(emu->gameSpace->x+emu->gameSpace->w-x-2,0,font,"Paused",textColor);
    TTF_SizeText(font,"Running",&x,NULL);
    emulationRunning=initLabel(emu->gameSpace->x+emu->gameSpace->w-x-2,0,font,"Running",textColor);
    menuBounds->x=10;
    menuBounds->y=topBarBounds->h;
    menuBounds->w=SCREEN_WIDTH-20;
    menuBounds->h=SCREEN_HEIGHT-topBarBounds->h-10;
    TTF_SizeText(heading,"Key-Bindings:",&x,NULL);
    inputLabel=initLabel(SCREEN_WIDTH/2-x/2,10,heading,"Key-Bindings:",textColor);
    TTF_SizeText(font,"Start 1 Player:",&x,&y);
    p1StartInput=initLabel(SCREEN_WIDTH/2-x/2,menuBounds->h/10,font,"Start 1 Player:",textColor);
    p1LeftInput=initLabel(SCREEN_WIDTH/2-x/2,p1StartInput->rect->y+y*2,font,"Player 1 Left:",textColor);
    p1RightInput=initLabel(SCREEN_WIDTH/2-x/2,p1LeftInput->rect->y+y*2,font,"Player 1 Right:",textColor);
    p1ShootInput=initLabel(SCREEN_WIDTH/2-x/2,p1RightInput->rect->y+y*2,font,"Player 1 Shoot:",textColor);
    p2StartInput=initLabel(SCREEN_WIDTH/2-x/2,p1ShootInput->rect->y+y*2,font,"Start 2 Player:",textColor);
    p2LeftInput=initLabel(SCREEN_WIDTH/2-x/2,p2StartInput->rect->y+y*2,font,"Player 2 Left:",textColor);
    p2RightInput=initLabel(SCREEN_WIDTH/2-x/2,p2LeftInput->rect->y+y*2,font,"Player 2 Right:",textColor);
    p2ShootInput=initLabel(SCREEN_WIDTH/2-x/2,p2RightInput->rect->y+y*2,font,"Player 2 Shoot:",textColor);
    saveEmuInput=initLabel(SCREEN_WIDTH/2-x/2,p2ShootInput->rect->y+y*2,font,"Save State:",textColor);
    loadEmuInput=initLabel(SCREEN_WIDTH/2-x/2,saveEmuInput->rect->y+y*2,font,"Load State:",textColor);
    coinInput=initLabel(SCREEN_WIDTH/2-x/2,loadEmuInput->rect->y+y*2,font,"Insert Coin:",textColor);
    pauseEmuInput=initLabel(SCREEN_WIDTH/2-x/2,coinInput->rect->y+y*2,font,"Pause Emulation:",textColor);
    inputLabels[0]=inputLabel;
    inputLabels[1]=p1StartInput;
    inputLabels[2]=p1LeftInput;
    inputLabels[3]=p1RightInput;
    inputLabels[4]=p1ShootInput;
    inputLabels[5]=p2StartInput;
    inputLabels[6]=p2LeftInput;
    inputLabels[7]=p2RightInput;
    inputLabels[8]=p2ShootInput;
    inputLabels[9]=saveEmuInput;
    inputLabels[10]=loadEmuInput;
    inputLabels[11]=coinInput;
    inputLabels[12]=pauseEmuInput;
    p1StartBox=initTextbox(p1StartInput->rect->x+p1StartInput->rect->w+4,p1StartInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->onePlayer)),20,0xFFFFFFFF,textColor,font,tbBorder);
    p1ShootBox=initTextbox(p1ShootInput->rect->x+p1ShootInput->rect->w+4,p1ShootInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->shoot)),20,0xFFFFFFFF,textColor,font,tbBorder);
    p1LeftBox=initTextbox(p1LeftInput->rect->x+p1LeftInput->rect->w+4,p1LeftInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->left)),20,0xFFFFFFFF,textColor,font,tbBorder);
    p1RightBox=initTextbox(p1RightInput->rect->x+p1RightInput->rect->w+4,p1RightInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->right)),20,0xFFFFFFFF,textColor,font,tbBorder);
    p2StartBox=initTextbox(p2StartInput->rect->x+p2StartInput->rect->w+4,p2StartInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->twoPlayer)),20,0xFFFFFFFF,textColor,font,tbBorder);
    p2ShootBox=initTextbox(p2ShootInput->rect->x+p2ShootInput->rect->w+4,p2ShootInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->shoot2)),20,0xFFFFFFFF,textColor,font,tbBorder);
    p2LeftBox=initTextbox(p2LeftInput->rect->x+p2LeftInput->rect->w+4,p2LeftInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->left2)),20,0xFFFFFFFF,textColor,font,tbBorder);
    p2RightBox=initTextbox(p2RightInput->rect->x+p2RightInput->rect->w+4,p2RightInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->right2)),20,0xFFFFFFFF,textColor,font,tbBorder);
    coinBox=initTextbox(coinInput->rect->x+coinInput->rect->w+4,coinInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->coin)),20,0xFFFFFFFF,textColor,font,tbBorder);
    saveEmuBox=initTextbox(saveEmuInput->rect->x+saveEmuInput->rect->w+4,saveEmuInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->saveState)),20,0xFFFFFFFF,textColor,font,tbBorder);
    loadEmuBox=initTextbox(loadEmuInput->rect->x+loadEmuInput->rect->w+4,loadEmuInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->loadState)),20,0xFFFFFFFF,textColor,font,tbBorder);
    pauseEmuBox=initTextbox(pauseEmuInput->rect->x+pauseEmuInput->rect->w+4,pauseEmuInput->rect->y,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->pauseEm)),20,0xFFFFFFFF,textColor,font,tbBorder);
    inputBoxes[0]=p1StartBox;
    inputBoxes[1]=p1ShootBox;
    inputBoxes[2]=p1LeftBox;
    inputBoxes[3]=p1RightBox;
    inputBoxes[4]=p2StartBox;
    inputBoxes[5]=p2ShootBox;
    inputBoxes[6]=p2LeftBox;
    inputBoxes[7]=p2RightBox;
    inputBoxes[8]=coinBox;
    inputBoxes[9]=saveEmuBox;
    inputBoxes[10]=loadEmuBox;
    inputBoxes[11]=pauseEmuBox;
    TTF_SizeText(font,"X",&x,NULL);
    exitInput=initButton(menuBounds->w-x-exitBorder->width*2,0,"X",textColor,0xAAAA2222,0xAACC3333,font,exitBorder);
    inputButtons[0]=exitInput;
    inputMenu=initMenu(inputButtons,1,inputLabels,13,NULL,0,inputBoxes,12,menuBounds,0x00555555);
    TTF_SizeText(heading,"Resolutions:",&x,NULL);
    resolution=initLabel(SCREEN_WIDTH/2-x/2,10,heading,"Resolutions:",textColor);
    displayLabels[0]=resolution;
    TTF_SizeText(font,"224x256",&x,NULL);
    tpbt=initButton(SCREEN_WIDTH/2-x/2,menuBounds->h/10,"224x256",textColor,0xAAAA2222,0xAAFF3333,font,NULL);
    displaySelectButtons[0]=tpbt;
    TTF_SizeText(font,"448x512",&x,NULL);
    tpbt=initButton(SCREEN_WIDTH/2-x/2,menuBounds->h/5,"448x512",textColor,0xAAAA2222,0xAAFF3333,font,NULL);
    displaySelectButtons[1]=tpbt;
    TTF_SizeText(font,"672x768",&x,NULL);
    tpbt=initButton(SCREEN_WIDTH/2-x/2,menuBounds->h/10*3,"672x768",textColor,0xAAAA2222,0xAAFF3333,font,NULL);
    displaySelectButtons[2]=tpbt;
    TTF_SizeText(font,"896x1024",&x,NULL);
    tpbt=initButton(SCREEN_WIDTH/2-x/2,menuBounds->h/5*2,"896x1024",textColor,0xAAAA2222,0xAAFF3333,font,NULL);
    displaySelectButtons[3]=tpbt;
    TTF_SizeText(font,"FullScreen",&x,NULL);
    tpbt=initButton(SCREEN_WIDTH/2-x/2,menuBounds->h/2,"Fullscreen",textColor,0xAAAA2222,0xAAFF3333,font,NULL);
    displaySelectButtons[4]=tpbt;
    displaySB=initSelectableButton(5,displaySelectButtons);
    sbButtons[0]=displaySB;
    TTF_SizeText(font,"X",&x,NULL);
    exitInput=initButton(menuBounds->w-x-exitBorder->width*2,0,"X",textColor,0xAAAA2222,0xAACC3333,font,exitBorder);
    displayButtons[0]=exitInput;
    displayMenu=initMenu(displayButtons,1,displayLabels,1,sbButtons,1,NULL,0,menuBounds,0x00555555);
    TTF_SizeText(heading,"Sound Options:",&x,NULL);
    volumeLevel=initLabel(SCREEN_WIDTH/2-x/2,10,heading,"Sound Options:",textColor);
    TTF_SizeText(font,"Current Volume:",&x,NULL);
    currentVolume=initLabel(SCREEN_WIDTH/4-x/2,menuBounds->h/10,font,"Current Volume:",textColor);
    intToText(tpr,emu->currentVolume);
    volumePercent=initTextbox(SCREEN_WIDTH/4+x/2+2,menuBounds->h/10,tpr,4,0xFFFFFFFF,textColor,font,tbBorder);
    soundTextboxes[0]=volumePercent;
    percent=initLabel(volumePercent->rect->x+volumePercent->rect->w+5,volumePercent->rect->y,font,"%",textColor);
    soundLabels[0]=volumeLevel;
    soundLabels[1]=currentVolume;
    soundLabels[2]=percent;
    TTF_SizeText(font,"X",&x,NULL);
    exitInput=initButton(menuBounds->w-x-exitBorder->width*2,0,"X",textColor,0xAAAA2222,0xAACC3333,font,exitBorder);
    soundButtons[0]=exitInput;
    soundMenu=initMenu(soundButtons,1,soundLabels,3,NULL,0,soundTextboxes,1,menuBounds,0x00555555);
    TTF_SizeText(heading,"Emulation Speed:",&x,NULL);
    speedLabel=initLabel(SCREEN_WIDTH/2-x/2,10,heading,"Emulation Speeds:",textColor);
    TTF_SizeText(heading,"Misc Options:",&x,NULL);
    miscLabel=initLabel(SCREEN_WIDTH/2-x/2,menuBounds->h/10*3,heading,"Misc Options:",textColor);
    miscLabels[0]=speedLabel;
    miscLabels[1]=miscLabel;
    TTF_SizeText(font,"1/4",&x,NULL);
    emuOneFourth=initButton(SCREEN_WIDTH/4-x/2,menuBounds->h/10,"1/4",textColor,0xAAAA2222,0xAACC3333,font,NULL);
    TTF_SizeText(font,"1/2",&x,NULL);
    emuOneHalf=initButton(SCREEN_WIDTH/4*3-x/2,menuBounds->h/10,"1/2",textColor,0xAAAA2222,0xAACC3333,font,NULL);
    TTF_SizeText(font,"Normal",&x,NULL);
    emuNormal=initButton(SCREEN_WIDTH/2-x/2,menuBounds->h/10,"Normal",textColor,0xAAAA2222,0xAACC3333,font,NULL);
    TTF_SizeText(font,"x2",&x,NULL);
    emuTimesTwo=initButton(SCREEN_WIDTH/4-x/2,menuBounds->h/5,"x2",textColor,0xAAAA2222,0xAACC3333,font,NULL);
    TTF_SizeText(font,"x4",&x,NULL);
    emuTimesFour=initButton(SCREEN_WIDTH/4*3-x/2,menuBounds->h/5,"x4",textColor,0xAAAA2222,0xAACC3333,font,NULL);
    speedButtons[0]=emuOneFourth;
    speedButtons[1]=emuOneHalf;
    speedButtons[2]=emuNormal;
    speedButtons[3]=emuTimesTwo;
    speedButtons[4]=emuTimesFour;
    if(emu->speed==1.0)
        reset=2;
    else if(emu->speed==2.0)
        reset=1;
    else if(emu->speed==4.0)
        reset=0;
    else if(emu->speed==0.50)
        reset=3;
    else if(emu->speed==0.25)
        reset=4;
    emuSpeed=initSelectableButton(5,speedButtons);
    miscSB[0]=emuSpeed;
    TTF_SizeText(font,"Exit Emulator",&x,NULL);
    endEmu=initButton(SCREEN_WIDTH/2-x/2,menuBounds->h/5*3,"Exit Emulator",textColor,0xAAAA2222,0xAACC3333,font,NULL);
    TTF_SizeText(font,"Save State",&x,NULL);
    saveEmu=initButton(SCREEN_WIDTH/2-x/2,menuBounds->h/5*2,"Save State",textColor,0xAAAA2222,0xAACC3333,font,NULL);
    TTF_SizeText(font,"Load State",&x,NULL);
    loadEmu=initButton(SCREEN_WIDTH/2-x/2,menuBounds->h/2,"Load State",textColor,0xAAAA2222,0xAACC3333,font,NULL);
    TTF_SizeText(font,"X",&x,NULL);
    exitInput=initButton(menuBounds->w-x-exitBorder->width*2,0,"X",textColor,0xAAAA2222,0xAACC3333,font,exitBorder);
    miscButtons[0]=exitInput;
    miscButtons[1]=saveEmu;
    miscButtons[2]=loadEmu;
    miscButtons[3]=endEmu;
    miscMenu=initMenu(miscButtons,4,miscLabels,2,miscSB,1,NULL,0,menuBounds,0x00555555);
    reset=0;
    result=0;
    free(exitBorder);
    free(tbBorder);
    SDL_FillRect(screen,NULL,0x20202020);
    SDL_UpdateWindowSurface(window);
    for(;!done;){
        while(SDL_PollEvent(&event)){
            SDL_GetMouseState(&x,&y);
            result=255;
            if(event.type==SDL_MOUSEMOTION){
                if(activeButton && (activeButton==display || activeButton==sound || activeButton==input || activeButton==misc) && contains(activeButton,x,y))
                    activeButton->on=1;
                else if(activeButton && (activeButton!=display && activeButton!=sound && activeButton!=input && activeButton!=misc) && contains(activeButton,x-menuBounds->x,y-menuBounds->y))
                    activeButton->on=1;
                else if(activeButton)
                    activeButton->on=0;
                result=0;
            }
            if(event.type==SDL_MOUSEBUTTONDOWN){
                result=0;
                activeTextbox=NULL;
                if(contains(display,x,y))
                    activeButton=display;
                else if(contains(sound,x,y))
                    activeButton=sound;
                else if(contains(input,x,y))
                    activeButton=input;
                else if(contains(misc,x,y))
                    activeButton=misc;
                if(currentMenu==1){
                    result=MenuContains(displayMenu,x,y);
                    if(result^255)
                        activeButton=getActiveButton(displayMenu);
                }
                else if(currentMenu==2){
                    result=MenuContains(soundMenu,x,y);
                    if(result==0)
                        activeButton=getActiveButton(soundMenu);
                    else if(result^255)
                        activeTextbox=soundMenu->tboxes[0];
                }
                else if(currentMenu==3){
                    result=MenuContains(inputMenu,x,y);
                    if(result==0)
                        activeButton=getActiveButton(inputMenu);
                    else if(result^255)
                        activeTextbox=inputMenu->tboxes[result-inputMenu->startTb];
                }
                else if(currentMenu==4){
                    result=MenuContains(miscMenu,x,y);
                    if(result^255)
                        activeButton=getActiveButton(miscMenu);
                }
                if(activeButton)
                    activeButton->on=1;
            }
            if(event.type==SDL_MOUSEBUTTONUP){
                if(activeButton && (activeButton==display || activeButton==sound || activeButton==input || activeButton==misc) && contains(activeButton,x,y)){
                    if(activeButton==display)
                        currentMenu=1;
                    else if(activeButton==sound)
                        currentMenu=2;
                    else if(activeButton==input)
                        currentMenu=3;
                    else if(activeButton==misc)
                        currentMenu=4;
                    result=1;
                    activeTextbox=NULL;
                }else if(activeButton && (activeButton!=display && activeButton!=sound && activeButton!=input && activeButton!=misc) && contains(activeButton,x-menuBounds->x,y-menuBounds->y)){
                    result=255;
                    if(currentMenu==1){
                        result=MenuContains(displayMenu,x,y);
                        if(result==1 && (emu->screen_size!=1||emu->isFull))
                            emu->screen_size=1;
                        else if(result==2 && (emu->screen_size!=2||emu->isFull))
                            emu->screen_size=2;
                        else if(result==3 && (emu->screen_size!=3||emu->isFull))
                            emu->screen_size=3;
                        else if(result==4 && (emu->screen_size!=4||emu->isFull))
                            emu->screen_size=4;
                        else if(result==5 && !emu->isFull){
                            reset=2;
                            done=1;
                            emu->isFull=1;
                        }else if(result!=0)
                            result=6;
                        if(result>0 && result<5){
                            reset=1;
                            done=1;
                            emu->isFull=0;
                        }
                    }
                    else if(currentMenu==2)
                        result=MenuContains(soundMenu,x,y);
                    else if(currentMenu==3)
                        result=MenuContains(inputMenu,x,y);
                    else if(currentMenu==4){
                        result=MenuContains(miscMenu,x,y);
                        if(result==1)
                            saveState(state);
                        else if(result==2)
                            loadState(state);
                        else if(result==3)
                            done=1;
                        else if(result==4)
                            emu->speed=4.0;
                        else if(result==5)
                            emu->speed=2.0;
                        else if(result==6)
                            emu->speed=1.0;
                        else if(result==7)
                            emu->speed=0.50;
                        else if(result==8)
                            emu->speed=0.25;
                    }
                }else if(activeTextbox && containsTextbox(activeTextbox,x-menuBounds->x,y-menuBounds->y)){
                    if(currentMenu==2)
                        resetTextbox(volumePercent);
                    if(currentMenu==3){
                        result=MenuContains(inputMenu,x,y);
                        if(result^255)
                            resetTextbox(inputBoxes[result-inputMenu->startTb]);
                    }
                }else if(activeTextbox==NULL){
                    if(p1StartBox->text[0]==0)
                        appendString(p1StartBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->onePlayer)));
                    else if(p2StartBox->text[0]==0)
                        appendString(p2StartBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->twoPlayer)));
                    else if(p1ShootBox->text[0]==0)
                        appendString(p1ShootBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->shoot)));
                    else if(p2ShootBox->text[0]==0)
                        appendString(p2ShootBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->shoot2)));
                    else if(p1LeftBox->text[0]==0)
                        appendString(p1LeftBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->left)));
                    else if(p2LeftBox->text[0]==0)
                        appendString(p2LeftBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->left2)));
                    else if(p1RightBox->text[0]==0)
                        appendString(p1RightBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->right)));
                    else if(p2RightBox->text[0]==0)
                        appendString(p2RightBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->right2)));
                    else if(coinBox->text[0]==0)
                        appendString(coinBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->coin)));
                    else if(saveEmuBox->text[0]==0)
                        appendString(saveEmuBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->saveState)));
                    else if(loadEmuBox->text[0]==0)
                        appendString(loadEmuBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->loadState)));
                    else if(pauseEmuBox->text[0]==0)
                        appendString(pauseEmuBox,SDL_GetKeyName(SDL_GetKeyFromScancode(ipc->pauseEm)));
                }
                if(result==0){
                    currentMenu=0;
                    SDL_FillRect(screen,topBarBounds,0x00808080);
                    RAMToRendered(screen);
                }
                if(activeButton)
                    activeButton->on=0;
                activeButton=NULL;
            }
            if(event.type==SDL_QUIT)
                done=1;
            if(event.type==SDL_KEYDOWN){
                type=event.key.keysym.scancode;
                if(currentMenu==3){
                    if(p1StartBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->onePlayer,type);
                    else if(p2StartBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->twoPlayer,type);
                    else if(p1ShootBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->shoot,type);
                    else if(p2ShootBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->shoot2,type);
                    else if(p1LeftBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->left,type);
                    else if(p2LeftBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->left2,type);
                    else if(p1RightBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->right,type);
                    else if(p2RightBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->right2,type);
                    else if(coinBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->coin,type);
                    else if(saveEmuBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->saveState,type);
                    else if(loadEmuBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->loadState,type);
                    else if(pauseEmuBox==activeTextbox)
                        doSwap(inputBoxes,&ipc->pauseEm,type);
                    if(activeTextbox && activeTextbox->currentSize==0){
                        fillTextbox(inputMenu,activeTextbox);
                        appendString(activeTextbox,SDL_GetKeyName(SDL_GetKeyFromScancode(type)));
                        SDL_FillRect(screen,topBarBounds,0x00808080);
                        RAMToRendered(screen);
                        activeTextbox=NULL;
                    }
                }
                if(currentMenu==2 && volumePercent==activeTextbox){
                    if(((type>29 && type<40) || (type>88 && type<99))){
                        if(type>29 && type<40)
                            type=(type!=39) ? type+19:48;
                        else
                            type=(type!=98) ? type-40:48;
                        appendChar(volumePercent,type);
                        if(!checkTextboxTextBounds(volumePercent,0,100))
                            volumePercent->text[--volumePercent->currentSize]='\0';
                        fillTextbox(soundMenu,volumePercent);
                        setTextboxText(volumePercent);
                        SDL_FillRect(soundMenu->MenuBack,percent->rect,soundMenu->menuColor);
                        percent->rect->x=volumePercent->rect->x+volumePercent->rect->w+5;
                        if(!canAppend(volumePercent))
                            emu->currentVolume=textToInt(volumePercent->text);
                        continue;
                    }else if(type==40){
                        if(!volumePercent->text[0])
                            volumePercent->text[volumePercent->currentSize++]=48;
                        fillTextbox(soundMenu,volumePercent);
                        setTextboxText(volumePercent);
                        emu->currentVolume=textToInt(volumePercent->text);
                        Mix_Volume(-1,emu->currentVolume*128/100);
                        activeTextbox=NULL;
                    }else if(type==42 && volumePercent->currentSize){
                        textboxEraseLast(volumePercent);
                        fillTextbox(soundMenu,volumePercent);
                        setTextboxText(volumePercent);
                    }
                }
                if(type==ipc->pauseEm && currentMenu==0){
                    paused=0;
                    lastTimer=timeusec();
                    break;
                }
                if(type==ipc->saveState)
                    saveState(state);
                if(type==ipc->loadState){
                    loadState(state);
                    SDL_FillRect(screen,topBarBounds,0x00808080);
                    RAMToRendered(screen);
                    SDL_UpdateWindowSurface(window);
                }
                result=0;
            }
        }
        Sleep(1);
        if(result^255){
            if(emu->isFull){
                SDL_FillRect(screen,NULL,0x00202020);
                RAMToRendered(screen);
            }
            SDL_FillRect(screen,topBarBounds,0x00808080);
            drawButton(display,screen);
            drawButton(sound,screen);
            drawButton(input,screen);
            drawButton(misc,screen);
            drawLabel(emulationPaused,screen);
            if(currentMenu==1)
                drawMenu(displayMenu,screen);
            if(currentMenu==2)
                drawMenu(soundMenu,screen);
            if(currentMenu==3)
                drawMenu(inputMenu,screen);
            if(currentMenu==4)
                drawMenu(miscMenu,screen);
            SDL_UpdateWindowSurface(window);
        }
        if(!paused){
            if(!lastTimer)
                nextInterrupt=(nextInterrupt-lastTimer)+timeusec();
            else
                nextInterrupt=timeusec();
            Mix_Resume(-1);
            drawLabel(emulationRunning,screen);
            for(;;){
                if(event.type==SDL_QUIT){
                    paused=1;
                    done=1;
                    break;
                }
                while(SDL_PollEvent(&event)){
                    SDL_GetMouseState(&x,&y);
                    if(event.type==SDL_MOUSEBUTTONDOWN){
                        if(contains(display,x,y))
                            activeButton=display;
                        else if(contains(sound,x,y))
                            activeButton=sound;
                        else if(contains(input,x,y))
                            activeButton=input;
                        else if(contains(misc,x,y))
                            activeButton=misc;
                        if(activeButton)
                            activeButton->on=1;
                    }
                    if(event.type==SDL_MOUSEBUTTONUP){
                        if(activeButton!=NULL && contains(activeButton,x,y)){
                            if(activeButton==display)
                                currentMenu=1;
                            else if(activeButton==sound)
                                currentMenu=2;
                            else if(activeButton==input)
                                currentMenu=3;
                            else if(activeButton==misc)
                                currentMenu=4;
                            activeButton->on=0;
                            paused=1;
                            activeButton=NULL;
                        }
                    }
                    if(event.type==SDL_MOUSEMOTION){
                        if(activeButton && contains(activeButton,x,y))
                            activeButton->on=1;
                        else if(activeButton)
                            activeButton->on=0;
                    }
                    if(event.type==SDL_KEYDOWN){
                        type=event.key.keysym.scancode;
                        if(type==ipc->pauseEm)
                            paused=1;
                        else if(type==ipc->shoot)
                            state->inPort1|=0x10;
                        else if(type==ipc->left)
                            state->inPort1|=0x20;
                        else if(type==ipc->right)
                            state->inPort1|=0x40;
                        else if(type==ipc->left2)
                            state->inPort2|=0x20;
                        else if(type==ipc->right2)
                            state->inPort2|=0x40;
                        else if(type==ipc->shoot2)
                            state->inPort2|=0x10;
                        else if(type==ipc->coin)
                            state->inPort1|=0x1;
                        else if(type==ipc->onePlayer)
                            state->inPort1|=0x4;
                        else if(type==ipc->twoPlayer)
                            state->inPort1|=0x2;
                        else if(type==ipc->saveState)
                            saveState(state);
                        else if(type==ipc->loadState)
                            loadState(state);
                    }else if(event.type==SDL_KEYUP){
                        type=event.key.keysym.scancode;
                        if(type==ipc->shoot)
                            state->inPort1&=~0x10;
                        else if(type==ipc->left)
                            state->inPort1&=~0x20;
                        else if(type==ipc->right)
                            state->inPort1&=~0x40;
                        else if(type==ipc->left2)
                            state->inPort2&=~0x20;
                        else if(type==ipc->right2)
                            state->inPort2&=~0x40;
                        else if(type==ipc->shoot2)
                            state->inPort2&=~0x10;
                        else if(type==ipc->coin)
                            state->inPort1&=~0x1;
                        else if(type==ipc->onePlayer)
                            state->inPort1&=~0x4;
                        else if(type==ipc->twoPlayer)
                            state->inPort1&=~0x2;
                    }
                }
                if(state->outPort3^state->lastP3 && state->outPort3|0x00){
                    if((Mix_Playing(s1->channel)^0x01)&&(s1->playing^0x01))
                        audioReset(s1);
                    if((Mix_Playing(s2->channel)^0x01)&&(s2->playing^0x01))
                        audioReset(s2);
                    if((Mix_Playing(s3->channel)^0x01)&&(s3->playing^0x01))
                        audioReset(s3);
                    if((state->outPort3&0x01)==0 && (s0->playing^0x01)){
                        s0->playing=1;
                        Mix_HaltChannel(s0->channel);
                        s0->channel=-1;
                    }
                    if((s0->playing)&&(state->outPort3&0x01)){
                        s0->channel=Mix_PlayChannel(-1,s0->sound,-1);
                        s0->playing=0;
                    }
                    if((s1->playing)&&(state->outPort3&0x02))
                        setAudio(s1);
                    if((s2->playing)&&(state->outPort3&0x04))
                        setAudio(s2);
                    if((s3->playing)&&(state->outPort3&0x08))
                        setAudio(s3);
                    state->lastP3=state->outPort3;
                }
                if(state->outPort5^state->lastP5 && state->outPort5|0x00){
                    if((Mix_Playing(s4->channel)^0x01)&&(s5->playing^0x01))
                        audioReset(s4);
                    if((Mix_Playing(s5->channel)^0x01)&&(s4->playing^0x01))
                        audioReset(s5);
                    if((Mix_Playing(s6->channel)^0x01)&&(s6->playing^0x01))
                        audioReset(s6);
                    if((Mix_Playing(s7->channel)^0x01)&&(s7->playing^0x01))
                        audioReset(s7);
                    if((Mix_Playing(s8->channel)^0x01)&&(s8->playing^0x01))
                        audioReset(s8);
                    if((s4->playing)&&(state->outPort5&0x01))
                        setAudio(s4);
                    if((s5->playing)&&(state->outPort5&0x02))
                        setAudio(s5);
                    if((s6->playing)&&(state->outPort5&0x04))
                        setAudio(s6);
                    if((s7->playing)&&(state->outPort5&0x08))
                        setAudio(s7);
                    if((s8->playing)&&(state->outPort5&0x10))
                        setAudio(s8);
                    state->lastP5=state->outPort5;
                }
                if(paused){
                    Mix_Pause(-1);
                    SDL_FillRect(screen,topBarBounds,0x00808080);
                    drawButton(display,screen);
                    drawButton(sound,screen);
                    drawButton(input,screen);
                    drawButton(misc,screen);
                    RAMToRendered(screen);
                    break;
                }
                doCPU(&lastTimer,&nextInterrupt,&whichInterrupt,screen);
            }
        }
        result=0;
    }
    free(topBarBounds);
    freeLabel(emulationPaused);
    freeMenu(displayMenu);
    freeMenu(miscMenu);
    freeMenu(soundMenu);
    freeMenu(inputMenu);
    freeButton(input);
    freeButton(display);
    freeButton(misc);
    freeButton(sound);
    free(menuBounds);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_CloseFont(heading);
}
void doSwap(struct Textbox **textboxes,uint8_t *oldValue,uint8_t newValue){
    uint8_t index=255;
    if(newValue==ipc->onePlayer){
        ipc->onePlayer=*oldValue;
        index=0;
    }
    else if(newValue==ipc->shoot){
        ipc->shoot=*oldValue;
        index=1;
    }
    else if(newValue==ipc->left){
        ipc->left=*oldValue;
        index=2;
    }
    else if(newValue==ipc->right){
        ipc->right=*oldValue;
        index=3;
    }
    else if(newValue==ipc->twoPlayer){
        ipc->twoPlayer=*oldValue;
        index=4;
    }
    else if(newValue==ipc->shoot2){
        ipc->shoot2=*oldValue;
        index=5;
    }
    else if(newValue==ipc->left2){
        ipc->left2=*oldValue;
        index=6;
    }
    else if(newValue==ipc->right2){
        ipc->right2=*oldValue;
        index=7;
    }
    else if(newValue==ipc->coin){
        ipc->coin=*oldValue;
        index=8;
    }
    else if(newValue==ipc->saveState){
        ipc->saveState=*oldValue;
        index=9;
    }
    else if(newValue==ipc->loadState){
        ipc->loadState=*oldValue;
        index=10;
    }else if(newValue==ipc->pauseEm){
        ipc->pauseEm=*oldValue;
        index=11;
    }
    if(index^255){
        resetTextbox(textboxes[index]);
        fillTextbox(inputMenu,textboxes[index]);
        appendString(textboxes[index],SDL_GetKeyName(SDL_GetKeyFromScancode(*oldValue)));
    }
    *oldValue=newValue;
}
void doCPU(uint64_t *lastTimer,uint64_t *nextInterrupt,uint8_t *whichInterrupt,SDL_Surface *surf){
    uint64_t current=timeusec(),sinceLast;
    uint32_t cyclesToCatchUp,currentCycles=0;
    if(*lastTimer==0){
        *lastTimer=timeusec();
        *nextInterrupt=*lastTimer+16000.0*emu->speed;
        *whichInterrupt=1;
    }
    if(*nextInterrupt<current && state->interrupt_enable){
        if(*whichInterrupt==1){
            generateInterrupt(1);
            *whichInterrupt=2;
        }else{
            generateInterrupt(2);
            if(emu->isFull){
                *whichInterrupt=emu->gameSpace->y;
                emu->gameSpace->y=0;
                SDL_FillRect(surf,topBarBounds,0x00808080);
                emu->gameSpace->y=*whichInterrupt;
            }else
                SDL_FillRect(surf,topBarBounds,0x00808080);
            *whichInterrupt=1;
            drawLabel(emulationRunning,surf);
            drawButton(display,surf);
            drawButton(sound,surf);
            drawButton(input,surf);
            drawButton(misc,surf);
            RAMToRendered(surf);
            SDL_UpdateWindowSurface(window);
        }
        state->halt=0;
        *nextInterrupt=current+8000.0*((emu->speed==0.25) ? 0.75:1);
    }
    sinceLast=current-*lastTimer;
    cyclesToCatchUp=2*sinceLast/emu->speed;
    while(cyclesToCatchUp>currentCycles && !state->halt)
        currentCycles+=EmulateInstruction();
    *lastTimer=timeusec();
    switch((int)(4*emu->speed)){
        case 1:
            Sleep(1);
            break;
        case 2:
            Sleep(1);
            break;
        case 4:
            Sleep(2);
            break;
        case 8:
            Sleep(4);
            break;
        case 16:
            Sleep(6);
            break;
    }
}
void RAMToRendered(SDL_Surface *surf){
    uint16_t x,y,ss=255*emu->screen_size,xs,x32;
    register uint16_t yp;
    SDL_Surface *temp=SDL_CreateRGBSurface(0,224*emu->screen_size,256*emu->screen_size,32,0,0,0,0);
    uint32_t *pixels=(uint32_t*)temp->pixels;
    uint8_t p,pix,*invBuffer=&state->memory[0x2400],pshift;
    uint32_t color;
    for(x=0;x++<224;){
        xs=x*emu->screen_size;
        x32=x<<5;
        for(y=0;y<256;y+=8){
            pix=*(invBuffer+x32+(y>>3));
            if(pix|0x0){
                for(p=0;p<8;p++){
                    if(!((pix>>p)|0))
                        break;
                    yp=y+p;
                    color=0xffffffff;
                    pshift=pix&(1<<p);
                    if(pshift && 255-(yp)<64 && 255-(yp)>32)
                        color=0x00ff0000;
                    else if(pshift && (255-(yp)>183 && 255-(yp)<239)||(255-(yp)>239 && 255-(yp)<255 && x>16 && x<122))
                        color=0x0000ff00;
                    switch(emu->screen_size){
                        case 1:
                            pixels[xs+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            break;
                        case 2:
                            pixels[xs+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+1)+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            pixels[xs+(ss-(yp*emu->screen_size-1))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+1)+(ss-(yp*emu->screen_size-1))*temp->w]= (pshift) ? color:0;
                            break;
                        case 3:
                            pixels[xs+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+1)+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+2)+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            pixels[xs+(ss-(yp*emu->screen_size-1))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+1)+(ss-(yp*emu->screen_size-1))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+2)+(ss-(yp*emu->screen_size-1))*temp->w]=(pshift) ? color:0;
                            pixels[xs+(ss-(yp*emu->screen_size-2))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+1)+(ss-(yp*emu->screen_size-2))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+2)+(ss-(yp*emu->screen_size-2))*temp->w]=(pshift) ? color:0;
                            break;
                        case 4:
                            pixels[xs+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+1)+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+2)+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+3)+(ss-(yp*emu->screen_size))*temp->w]=(pshift) ? color:0;
                            pixels[xs+(ss-(yp*emu->screen_size-1))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+1)+(ss-(yp*emu->screen_size-1))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+2)+(ss-(yp*emu->screen_size-1))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+3)+(ss-(yp*emu->screen_size-1))*temp->w]=(pshift) ? color:0;
                            pixels[xs+(ss-(yp*emu->screen_size-2))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+1)+(ss-(yp*emu->screen_size-2))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+2)+(ss-(yp*emu->screen_size-2))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+3)+(ss-(yp*emu->screen_size-2))*temp->w]=(pshift) ? color:0;
                            pixels[xs+(ss-(yp*emu->screen_size-3))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+1)+(ss-(yp*emu->screen_size-3))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+2)+(ss-(yp*emu->screen_size-3))*temp->w]=(pshift) ? color:0;
                            pixels[(xs+3)+(ss-(yp*emu->screen_size-3))*temp->w]=(pshift) ? color:0;
                            break;

                    }
                }
            }
        }
    }
    SDL_BlitSurface(temp,NULL,surf,emu->gameSpace);
    SDL_FreeSurface(temp);
}
uint8_t MachineIn(uint8_t port){
    switch(port){
        case 0:
            return 1;
        case 1:
            return state->inPort1;
        case 2:
            return state->inPort2;
        case 3:{
            uint16_t v=(state->shift1<<8)|state->shift0;
            return ((v>>(8-state->shiftOffset))& 0xff);
        }
    }
    return 0;
}
void MachineOut(uint8_t port){
    switch(port){
        case 2:
            state->shiftOffset=state->A & 0x7;
            break;
        case 3:
            state->outPort3=state->A;
            break;
        case 4:
            state->shift0=state->shift1;
            state->shift1=state->A;
            break;
        case 5:
            state->outPort5=state->A;
            break;
    }
}
void generateInterrupt(int num){
    uint16_t returnPlace=state->ProgramCounter;
    writeToMemory((returnPlace>>8)&0xff,state->StackPointer-1);
    writeToMemory((returnPlace)&0xff,state->StackPointer-2);
    state->StackPointer-=2;
    state->ProgramCounter=8*num;
    state->interrupt_enable=0;
}
void writeToMemory(uint8_t item,uint16_t address){
    if(address<0x2000)
        exit(9);
    else if(address<0x4000)
        state->memory[address]=item;
    else if(address>=0x5000)
        exit(10);
}
void setZSP(uint8_t value){
    state->cs.zero=(value==0);
    state->cs.sign=((value & 0x80)==0x80);
    state->cs.parity=parity(value);
}
void setAFlags(uint16_t value){
    state->cs.carryBit=(value>0xff);
    state->cs.zero=((value & 0xff)==0);
    state->cs.sign=((value & 0x80)==0x80);
    state->cs.parity=parity(value&0xff);
}
void setLFlags(uint8_t value){
    state->cs.zero=(value==0);
    state->cs.sign=((value & 0x80)==0x80);
    state->cs.parity=parity(value);
    state->cs.carryBit=0;
}
void setCFlags(uint8_t val1,uint8_t val2){
    state->cs.zero=(val1==val2);
    state->cs.sign=(((val1-val2)&0x80)==0x80);
    state->cs.parity=(val1-val2);
    state->cs.carryBit=(val1<val2);
}
void callLoc(uint16_t currentOpcode){
    uint16_t returnPlace=state->ProgramCounter+2;
    state->memory[state->StackPointer-1]=(returnPlace>>8) & 0xff;
    state->memory[state->StackPointer-2]=returnPlace & 0xff;
    state->StackPointer-=2;
    state->ProgramCounter=currentOpcode;
}
void call(uint8_t *currentOpcode){
    uint16_t returnPlace=state->ProgramCounter+2;
    state->memory[state->StackPointer-1]=(returnPlace>>8) & 0xff;
    state->memory[state->StackPointer-2]=returnPlace & 0xff;
    state->StackPointer-=2;
    state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
}
void returnFrom(){
    state->ProgramCounter=(state->memory[state->StackPointer+1]<<8)|state->memory[state->StackPointer];
    state->StackPointer+=2;
}
void PopStack(uint8_t *high,uint8_t *low){
    *low=state->memory[state->StackPointer];
    *high=state->memory[state->StackPointer+1];
    state->StackPointer+=2;
}
void PushStack(uint8_t high,uint8_t low){
    writeToMemory(high,state->StackPointer-1);
    writeToMemory(low,state->StackPointer-2);
    state->StackPointer-=2;
}
uint64_t timeusec(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1E6) + time.tv_usec;
}
uint8_t EmulateInstruction(){
    unsigned char *currentOpcode=&state->memory[state->ProgramCounter];
    uint32_t regPair1,regPair2,result;
    uint16_t offset,ans;
    uint8_t rot;
    state->ProgramCounter++;
    if(*currentOpcode<0x10){
        switch(*currentOpcode){
            case 0x00:/*NOP::Does nothing.*/break;
            case 0x01:/*LXI B,16BitData::Loads 1 byte into Register B and 1 byte into Register C.");*/
                state->C=currentOpcode[1];
                state->B=currentOpcode[2];
                state->ProgramCounter+=2;
                break;
            case 0x02:/*STAX B::Stores data in Accumulator at address specified by register pair B,C.*/
                offset=(state->B<<8)|state->C;
                writeToMemory(state->A,offset);
                break;
            case 0x03:/*INX B::Increment the total value of Register Pair B,C by one.*/
                state->C++;
                if(state->C==0)
                    state->B++;
                break;
            case 0x04:/*INR B::Increment the value in Register B by one.*/
                state->B++;
                setZSP(state->B);
                break;
            case 0x05:/*DCR B::Decrement the value in Register B by one.*/
                state->B--;
                setZSP(state->B);
                break;
            case 0x06:/*MVI B,8BitData::Moves 8BitData into Register B.*/
                state->B=currentOpcode[1];
                state->ProgramCounter++;
                break;
            case 0x07:/*RLC::Rotate the value in the Accumulator left through carry.*/
                rot=state->A;
                state->A=((rot & 0x80)>>7)|(rot<<1);
                state->cs.carryBit=(rot & 0x80)==0x80;
                break;
            case 0x08:/*NOP::Does nothing.*/break;
            case 0x09:/*DAD B::Register Pair H,L += Register Pair B,C.*/
                regPair1=(state->H<<8)|state->L;
                regPair2=(state->B<<8)|state->C;
                result=regPair1+regPair2;
                state->H=(result & 0xff00)>>8;
                state->L=result & 0xff;
                state->cs.carryBit=(result & 0xffff0000)>0;
                break;
            case 0x0a:/*LDAX B::Loads the byte specified by the address in Register Pair B,C into the Accumulator.*/
                offset=(state->B<<8)|state->C;
                state->A=state->memory[offset];
                break;
            case 0x0b:/*DCX B::Decrement the total value stored in Register pair B,C by one.*/
                state->C--;
                if(state->C==0xff)
                    state->B--;
                break;
            case 0x0c:/*INR C::Increment the value stored in Register C by one.*/
                state->C++;
                setZSP(state->C);
                break;
            case 0x0d:/*DCR C::Decrement the value stored in Register C by one.*/
                state->C--;
                setZSP(state->C);
                break;
            case 0x0e:/*MVI C,8BitData::Moves 8BitData into Register C.*/
                state->C=currentOpcode[1];
                state->ProgramCounter++;
                break;
            case 0x0f:/*RRC::Rotates the value in the Accumulator right through carry.*/
                rot=state->A;
                state->A=((rot & 1)<<7)|(rot>>1);
                state->cs.carryBit=((rot & 1)==1);
                break;
        }
    }
    else if(*currentOpcode>=0x10 && *currentOpcode<0x20){
        switch(*currentOpcode){
            case 0x10:/*NOP::Does Nothing.*/break;
            case 0x11:/*LXI D,16BitData::Loads 1 byte into Register D and 1 byte into Register E.*/
                state->E=currentOpcode[1];
                state->D=currentOpcode[2];
                state->ProgramCounter+=2;
                break;
            case 0x12:/*STAX D::Stores the byte in the Accumulator at the address specified by Register Pair D,E.*/
                offset=(state->D<<8)|state->E;
                writeToMemory(state->A,offset);
                break;
            case 0x13:/*INX D::Increments the total value stored in Register pair D,E by one.*/
                state->E++;
                if(state->E==0)
                    state->D++;
                break;
            case 0x14:/*INR D::Increments the value stored in Register D by one.*/
                state->D++;
                setZSP(state->D);
                break;
            case 0x15:/*DCR D::Decrements the value stored in Register D by one.*/
                state->D--;
                setZSP(state->D);
                break;
            case 0x16:/*MVI D,8BitData::Moves 8BitData into register D.*/
                state->D=currentOpcode[1];
                state->ProgramCounter++;
                break;
            case 0x17:/*RAL::Rotate Accumulator left using carry.*/
                rot=state->A;
                state->A=(rot<<1)|state->cs.carryBit;
                state->cs.carryBit=((rot & 0x80)==0x80);
                break;
            case 0x18:/*NOP::Does Nothing.*/break;
            case 0x19:/*DAD D::Register pair H,L+= Register pair D,E.*/
                regPair1=(state->H<<8)|(state->L);
                regPair2=(state->D<<8)|(state->E);
                result=regPair1+regPair2;
                state->H=(result & 0xff00)>>8;
                state->L=result & 0xff;
                state->cs.carryBit=(result & 0xffff0000)>0;
                break;
            case 0x1a:/*LDAX D::Loads the byte specified by the address in Register Pair D,E into the Accumulator.*/
                offset=(state->D<<8)|state->E;
                state->A=state->memory[offset];
                break;
            case 0x1b:/*DCX D::Decrements the value stored in the Register pair D,E by one.*/
                state->E--;
                if(state->E==0xff)
                    state->D--;
                break;
            case 0x1c:/*INR E::Increments the value stored in Register E by one.*/
                state->E++;
                setZSP(state->E);
                break;
            case 0x1d:/*DCR E::Decrements the value stored in Register E by one.*/
                state->E--;
                setZSP(state->E);
                break;
            case 0x1e:/*MVI E,8BitData::Moves 8BitData into Register E.*/
                state->E=currentOpcode[1];
                state->ProgramCounter++;
                break;
            case 0x1f:/*RAR::Rotate Accumulator right using carry.*/
                rot=state->A;
                state->A=(state->cs.carryBit<<7)|(rot>>1);
                state->cs.carryBit=((rot & 1)==1);
                break;
        }
    }
    else if(*currentOpcode>=0x20 && *currentOpcode<0x30){
        switch(*currentOpcode){
            case 0x20:/*NOP::Does nothing.*/break;
            case 0x21:/*LXI H,16BitData::Loads 16BitData into Register pair L,H.*/
                state->L=currentOpcode[1];
                state->H=currentOpcode[2];
                state->ProgramCounter+=2;
                break;
            case 0x22:/*SHLD 16BitAddress::Stores the contents of Register L at 16BitAddress, and Register H at 16BitAddress+1.*/
                offset=(currentOpcode[2]<<8)|currentOpcode[1];
                writeToMemory(state->L,offset);
                writeToMemory(state->H,offset+1);
                state->ProgramCounter+=2;
                break;
            case 0x23:/*INX H::Increments the total value stored in Register pair H,L by one.*/
                state->L++;
                if(state->L==0)
                    state->H++;
                break;
            case 0x24:/*INR H::Increments the value stored in Register H by one.*/
                state->H++;
                setZSP(state->H);
                break;
            case 0x25:/*DCR H::Decrements the value stored in Register H by one.*/
                state->H--;
                setZSP(state->H);
                break;
            case 0x26:/*MVI H,8BitData::Moves 8BitData into Register H.*/
                state->H=currentOpcode[1];
                state->ProgramCounter++;
                break;
            case 0x27:/*DAA::Converts Accumulator contents to packed Binary Coded Decimal.*/
                if((state->A & 0xf)>9 || state->cs.auxCarryBit)
                    state->A+=6;
                else
                    state->cs.auxCarryBit=0;
                if((state->A & 0xf0)>0x90 || state->cs.carryBit){
                    ans=(uint16_t)state->A+0x60;
                    state->A=(ans & 0xff);
                    setAFlags(ans);
                }
                break;
            case 0x28:/*NOP::Does nothing.*/break;
            case 0x29:/*DAD H::Register Pair H,L+=Register Pair H,L.*/
                regPair1=(state->H<<8)|state->L;
                result=regPair1+regPair1;
                state->H=(result & 0xff00)>>8;
                state->L=result & 0xff;
                state->cs.carryBit=(result & 0xffff0000)>0;
                break;
            case 0x2a:/*LHLD 16BitAddress::Loads Register pair H,L with data at 16BitAddress and 16BitAddress+1.*/
                offset=(currentOpcode[2]<<8)|currentOpcode[1];
                state->L=state->memory[offset];
                state->H=state->memory[offset+1];
                state->ProgramCounter+=2;
                break;
            case 0x2b:/*DCX H::Decreases the value stored in Register pair H,L by one.*/
                state->L--;
                if(state->L==0xff)
                    state->H--;
                break;
            case 0x2c:/*INR L::Increments the value stored in Register L by one.*/
                state->L++;
                setZSP(state->L);
                break;
            case 0x2d:/*DCR L::Decrements the value stored in Register L by one.*/
                state->L--;
                setZSP(state->L);
                break;
            case 0x2e:/*MVI L,8BitData::Loads 8BitData into Register L.*/
                state->L=currentOpcode[1];
                state->ProgramCounter++;
                break;
            case 0x2f:/*CMA::Compliment of Register A.*/
                state->A=~state->A;
                break;
        }
    }
    else if(*currentOpcode>=0x30 && *currentOpcode<0x40){
        switch(*currentOpcode){
            case 0x30:/*NOP::Does nothing.*/break;
            case 0x31:/*LXI StackPointer,16BitData::Loads the StackPointer at address 16BitData.*/
                state->StackPointer=(currentOpcode[2]<<8)|currentOpcode[1];
                state->ProgramCounter+=2;
                break;
            case 0x32:/*STA 16BitAddress::Stores data in Accumulator at 16BitAddress.*/
                offset=(currentOpcode[2]<<8)|currentOpcode[1];
                writeToMemory(state->A,offset);
                state->ProgramCounter+=2;
                break;
            case 0x33:/*INX StackPointer::Increments the StackPointer by one.*/
                state->StackPointer++;
                break;
            case 0x34:/*INR M:Increments the byte at memory location specified by Register pair H,L by one.*/
                offset=(state->H<<8)|state->L;
                rot=state->memory[offset]+1;
                setZSP(rot);
                writeToMemory(rot,offset);
                break;
            case 0x35:/*DCR M::Decrements the value store in Register pair H,L by one.*/
                offset=(state->H<<8)|state->L;
                rot=state->memory[offset]-1;
                setZSP(rot);
                writeToMemory(rot,offset);
                break;
            case 0x36:/*MVI M,8BitData::Moves 8BitData to the memory address specified by Register pair H,L.*/
                offset=(state->H<<8)|state->L;
                writeToMemory(currentOpcode[1],offset);
                state->ProgramCounter++;
                break;
            case 0x37:/*STC::Set carryFlag to one.*/
                state->cs.carryBit=1;
                break;
            case 0x38:/*NOP::Does nothing.*/break;
            case 0x39:/*DAD StackPointer::Register pair HL+=StackPointer.*/
                regPair1=(state->H<<8)|state->L;
                regPair2=state->StackPointer;
                result=regPair1+regPair2;
                state->H=(result & 0xff00)>>8;
                state->L=result & 0xff;
                state->cs.carryBit=(result & 0xffff0000)>0;
                break;
            case 0x3a:/*LDA 16BitAddress::Puts the value at memory address 16BitAddress into the Accumulator.*/
                offset=(currentOpcode[2]<<8)|currentOpcode[1];
                state->A=state->memory[offset];
                state->ProgramCounter+=2;
                break;
            case 0x3b:/*DCX StackPointer::Decrements the StackPointer by 1.*/
                state->StackPointer--;
                break;
            case 0x3c:/*INR A::Increments the value store in the Accumulator by one.*/
                state->A++;
                setZSP(state->A);
                break;
            case 0x3d:/*DCR A::Decrements the value stored in the Accumulator by one.*/
                state->A--;
                setZSP(state->A);
                break;
            case 0x3e:/*MVI A,8BitData::Moves 8BitData into the Accumulator.*/
                state->A=currentOpcode[1];
                state->ProgramCounter++;
                break;
            case 0x3f:/*CMC::Inverts the value of the carryFlag.*/
                state->cs.carryBit=(state->cs.carryBit) ? 0:1;
                break;
        }
    }
    else if(*currentOpcode>=0x40 && *currentOpcode<0x50){
        switch(*currentOpcode){
            case 0x40:/*MOV B,B::Moves the value stored in Register B to Register B.*/
                state->B=state->B;
                break;
            case 0x41:/*MOV B,C::Moves the value stored in Register C to Register B.*/
                state->B=state->C;
                break;
            case 0x42:/*MOV B,D::Moves the value stored in Register D to Register B.*/
                state->B=state->D;
                break;
            case 0x43:/*MOV B,E::Moves the value stored in Register E to Register B.*/
                state->B=state->E;
                break;
            case 0x44:/*MOV B,H::Moves the value stored in Register H to Register B.*/
                state->B=state->H;
                break;
            case 0x45:/*MOV B,L::Moves the value stored in Register L to Register B.*/
                state->B=state->L;
                break;
            case 0x46:/*MOV B,M::Moves the value stored at memory location HL to Register B.*/
                offset=(state->H<<8)|state->L;
                state->B=state->memory[offset];
                break;
            case 0x47:/*MOV B,A::Moves the value stored in the Accumulator to Register B.*/
                state->B=state->A;
                break;
            case 0x48:/*MOV C,B::Moves the value stored in Register B to Register C.*/
                state->C=state->B;
                break;
            case 0x49:/*MOV C,C::Moves the value stored in Register C to Register C.*/
                state->C=state->C;
                break;
            case 0x4a:/*MOV C,D::Moves the value stored in Register D to Register C.*/
                state->C=state->D;
                break;
            case 0x4b:/*MOV C,E::Moves the value stored in Register E to Register C.*/
                state->C=state->E;
                break;
            case 0x4c:/*MOV C,H::Moves the value stored in Register H to Register C.*/
                state->C=state->H;
                break;
            case 0x4d:/*MOV C,L::Moves the value stored in Register L to Register C.*/
                state->C=state->L;
                break;
            case 0x4e:/*MOV C,M::Moves the value stored at memory location HL to Register C.*/
                offset=(state->H<<8)|state->L;
                state->C=state->memory[offset];
                break;
            case 0x4f:/*MOV C,A::Moves the value stored in the Accumulator to Register C.*/
                state->C=state->A;
                break;
        }
    }
    else if(*currentOpcode>=0x50 && *currentOpcode<0x60){
        switch(*currentOpcode){
            case 0x50:/*MOV D,B::Moves the value stored in Register B to Register D.*/
                state->D=state->B;
                break;
            case 0x51:/*MOV D,C::Moves the value stored in Register C to Register B.*/
                state->D=state->C;
                break;
            case 0x52:/*MOV D,D::Moves the value stored in Register D to Register D.*/
                state->D=state->D;
                break;
            case 0x53:/*MOV D,E::Moves the value stored in Register E to Register D.*/
                state->D=state->E;
                break;
            case 0x54:/*MOV D,H::Moves the value stored in Register H to Register D.*/
                state->D=state->H;
                break;
            case 0x55:/*MOV D,L::Moves the value stored in Register L to Register D.*/
                state->D=state->L;
                break;
            case 0x56:/*MOV D,M::Moves the value stored at memory address HL to Register D.*/
                offset=(state->H<<8)|state->L;
                state->D=state->memory[offset];
                break;
            case 0x57:/*MOV D,A::Moves the value stored in the Accumulator to Register D.*/
                state->D=state->A;
                break;
            case 0x58:/*MOV E,B::Moves the value stored in Register B to Register E.*/
                state->E=state->B;
                break;
            case 0x59:/*MOV E,C::Moves the value stored in Register C to Register E.*/
                state->E=state->C;
                break;
            case 0x5a:/*MOV E,D::Moves the value stored in Register D to Register E.*/
                state->E=state->D;
                break;
            case 0x5b:/*MOV E,E::Moves the value stored in Register E to Register E.*/
                state->E=state->E;
                break;
            case 0x5c:/*MOV E,H::Moves the value stored in Register H to Register E.*/
                state->E=state->H;
                break;
            case 0x5d:/*MOV E,L::Moves the value stored in Register L to Register E.*/
                state->E=state->L;
                break;
            case 0x5e:/*MOV E,M::Moves the value stored at memory address HL to Register E.*/
                offset=(state->H<<8)|state->L;
                state->E=state->memory[offset];
                break;
            case 0x5f:/*MOV E,A::Moves the value stored in the Accumulator to Register E.*/
                state->E=state->A;
                break;
        }
    }
    else if(*currentOpcode>=0x60 && *currentOpcode<0x70){
        switch(*currentOpcode){
            case 0x60:/*MOV H,B::Moves the value stored in Register B to Register H.*/
                state->H=state->B;
                break;
            case 0x61:/*MOV H,C::Moves the value stored in Register C to Register H.*/
                state->H=state->C;
                break;
            case 0x62:/*MOV H,D::Moves the value stored in Register D to Register H.*/
                state->H=state->D;
                break;
            case 0x63:/*MOV H,E::Moves the value stored in Register E to Register H.*/
                state->H=state->E;
                break;
            case 0x64:/*MOV H,H::Moves the value stored in Register H to Register H.*/
                state->H=state->H;
                break;
            case 0x65:/*MOV H,L::Moves the value stored in Register L to Register H.*/
                state->H=state->L;
                break;
            case 0x66:/*MOV H,M::Moves the value stored at memory address HL to Register H.*/
                offset=(state->H<<8)|state->L;
                state->H=state->memory[offset];
                break;
            case 0x67:/*MOV H,A::Moves the value stored in the Accumulator to Register H.*/
                state->H=state->A;
                break;
            case 0x68:/*MOV L,B::Moves the value stored in Register B to Register L.*/
                state->L=state->B;
                break;
            case 0x69:/*MOV L,C::Moves the value stored in Register C to Register L.*/
                state->L=state->C;
                break;
            case 0x6a:/*MOV L,D::Moves the value stored in Register D to Register L.*/
                state->L=state->D;
                break;
            case 0x6b:/*MOV L,E::Moves the value stored in Register E to Register L.*/
                state->L=state->E;
                break;
            case 0x6c:/*MOV L,H::Moves the value stored in Register H to Register L.*/
                state->L=state->H;
                break;
            case 0x6d:/*MOV L,L::Moves the value stored in Register L to Register L.*/
                state->L=state->L;
                break;
            case 0x6e:/*MOV L,M::Moves the value stored in memory address HL to Register L.*/
                offset=(state->H<<8)|state->L;
                state->L=state->memory[offset];
                break;
            case 0x6f:/*MOV L,A::Moves the value stored in the Accumulator to Register L.*/
                state->L=state->A;
                break;
        }
    }
    else if(*currentOpcode>=0x70 && *currentOpcode<0x80){
        switch(*currentOpcode){
            case 0x70:/*MOV M,B::Moves the value stored in Register B to memory location HL.*/
                offset=(state->H<<8)|state->L;
                writeToMemory(state->B,offset);
                break;
            case 0x71:/*MOV M,C::Moves the value stored in Register C to memory location HL.*/
                offset=(state->H<<8)|state->L;
                writeToMemory(state->C,offset);
                break;
            case 0x72:/*MOV M,D::Moves the value stored in Register D to memory location HL.*/
                offset=(state->H<<8)|state->L;
                writeToMemory(state->D,offset);
                break;
            case 0x73:/*MOV M,E::Moves the value stored in Register E to memory location HL.*/
                offset=(state->H<<8)|state->L;
                writeToMemory(state->E,offset);
                break;
            case 0x74:/*MOV M,H::Moves the value stored in Register H to memory location HL.*/
                offset=(state->H<<8)|state->L;
                writeToMemory(state->H,offset);
                break;
            case 0x75:/*MOV M,L::Moves the value stored in Register L to memory location HL.*/
                offset=(state->H<<8)|state->L;
                writeToMemory(state->L,offset);
                break;
            case 0x76:/*HLT::Stop CPU processing until an Interrupt occurs.*/state->halt=1;exit(0);break;
            case 0x77:/*MOV M,A::Moves the value stored in the Accumulator to memory location HL.*/
                offset=(state->H<<8)|state->L;
                writeToMemory(state->A,offset);
                break;
            case 0x78:/*MOV A,B::Moves the value stored in Register B to the Accumulator.*/
                state->A=state->B;
                break;
            case 0x79:/*MOV A,C::Moves the value stored in Register C to the Accumulator.*/
                state->A=state->C;
                break;
            case 0x7a:/*MOV A,D::Moves the value stored in Register D to the Accumulator.*/
                state->A=state->D;
                break;
            case 0x7b:/*MOV A,E::Moves the value stored in Register E to the Accumulator.*/
                state->A=state->E;
                break;
            case 0x7c:/*MOV A,H::Moves the value stored in Register H to the Accumulator.*/
                state->A=state->H;
                break;
            case 0x7d:/*MOV A,L::Moves the value stored in Register L to the Accumulator.*/
                state->A=state->L;
                break;
            case 0x7e:/*MOV A,M::Moves the value stored at memory location HL to the Accumulator.*/
                offset=(state->H<<8)|state->L;
                state->A=state->memory[offset];
                break;
            case 0x7f:/*MOV A,A::Moves the value stored in the Accumulator to the Accumulator.*/
                state->A=state->A;
                break;
        }
    }
    else if(*currentOpcode>=0x80 && *currentOpcode<0x90){
        switch(*currentOpcode){
            case 0x80:/*ADD B::Accumulator+=Register B.*/
                ans=(uint16_t)state->A+(uint16_t)state->B;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x81:/*ADD C::Accumulator+=Register C.*/
                ans=(uint16_t)state->A+(uint16_t)state->C;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x82:/*ADD D::Accumulator+=Register D.*/
                ans=(uint16_t)state->A+(uint16_t)state->D;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x83:/*ADD E::Accumulator+=Register E.*/
                ans=(uint16_t)state->A+(uint16_t)state->E;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x84:/*ADD H::Accumulator+=Register H.*/
                ans=(uint16_t)state->A+(uint16_t)state->H;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x85:/*ADD L::Accumulator+=Register L.*/
                ans=(uint16_t)state->A+(uint16_t)state->L;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x86:/*ADD M::Accumulator+=value at memory location HL.*/
                offset=(state->H<<8)|(state->L);
                ans=(uint16_t)state->A+(uint16_t)state->memory[offset];
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x87:/*ADD A::Accumulator+=Accumulator.*/
                ans=(uint16_t)state->A+(uint16_t)state->A;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x88:/*ADC B::Accumulator+=Register B+carryFlag.*/
                ans=(uint16_t)state->A+(uint16_t)state->B+state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x89:/*ADC C::Accumulator+=Register C+carryFlag.*/
                ans=(uint16_t)state->A+(uint16_t)state->C+state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x8a:/*ADC D::Accumulator+=Register D+carryFlag.*/
                ans=(uint16_t)state->A+(uint16_t)state->D+state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x8b:/*ADC E::Accumulator+=Register E+carryFlag.*/
                ans=(uint16_t)state->A+(uint16_t)state->E+state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x8c:/*ADC H::Accumulator+=Register H+carryFlag.*/
                ans=(uint16_t)state->A+(uint16_t)state->H+state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x8d:/*ADC L::Accumulator+=Register L+carryFlag.*/
                ans=(uint16_t)state->A+(uint16_t)state->L+state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x8e:/*ADC M::Accumulator+=value at memory location HL+carryFlag.*/
                offset=(state->H<<8)|state->L;
                ans=(uint16_t)state->A+(uint16_t)state->memory[offset]+state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x8f:/*ADC A::Accumulator+=Accumulator+carryFlag.*/
                ans=(uint16_t)state->A+(uint16_t)state->A+state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
        }
    }
    else if(*currentOpcode>=0x90 && *currentOpcode<0xa0){
        switch(*currentOpcode){
            case 0x90:/*SUB B::Accumulator-=Register B.*/
                ans=(uint16_t)state->A-(uint16_t)state->B;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x91:/*SUB C::Accumulator-=Register C.*/
                ans=(uint16_t)state->A-(uint16_t)state->C;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x92:/*SUB D::Accumulator-=Register D.*/
                ans=(uint16_t)state->A-(uint16_t)state->D;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x93:/*SUB E::Accumulator-=Register E.*/
                ans=(uint16_t)state->A-(uint16_t)state->E;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x94:/*SUB H::Accumulator-=Register H.*/
                ans=(uint16_t)state->A-(uint16_t)state->H;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x95:/*SUB L::Accumulator-=Register L.*/
                ans=(uint16_t)state->A-(uint16_t)state->L;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x96:/*SUB M::Accumulator-=value at memory location HL.*/
                offset=(state->H<<8)|state->L;
                ans=(uint16_t)state->A-(uint16_t)state->memory[offset];
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x97:/*SUB A::Accumulator-=Accumulator.*/
                ans=(uint16_t)state->A-(uint16_t)state->A;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x98:/*SBB B::Accumulator-=(Register B+carryFlag).*/
                ans=(uint16_t)state->A-(uint16_t)state->B-state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x99:/*SBB C::Accumulator-=(Register C+carryFlag).*/
                ans=(uint16_t)state->A-(uint16_t)state->C-state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x9a:/*SBB D::Accumulator-=(Register D+carryFlag).*/
                ans=(uint16_t)state->A-(uint16_t)state->D-state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x9b:/*SBB E::Accumulator-=(Register E+carryFlag).*/
                ans=(uint16_t)state->A-(uint16_t)state->E-state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x9c:/*SBB H::Accumulator-=(Register H+carryFlag).*/
                ans=(uint16_t)state->A-(uint16_t)state->H-state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x9d:/*SBB L::Accumulator-=(Register L+carryFlag).*/
                ans=(uint16_t)state->A-(uint16_t)state->L-state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x9e:/*SBB M::Accumulator-=(value stored at memory location HL+carryFlag).*/
                offset=(state->H<<8)|state->L;
                ans=(uint16_t)state->A-(uint16_t)state->memory[offset]-state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
            case 0x9f:/*SBB A::Accumulator-=(Accumulator+carryFlag).*/
                ans=(uint16_t)state->A-(uint16_t)state->A-state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                break;
        }
    }
    else if(*currentOpcode>=0xa0 && *currentOpcode<0xb0){
        switch(*currentOpcode){
            case 0xa0:/*ANA B::Accumulator &=Register B.*/
                state->A&=state->B;
                setLFlags(state->A);
                break;
            case 0xa1:/*ANA C::Accumulator &=Register C.*/
                state->A&=state->C;
                setLFlags(state->A);
                break;
            case 0xa2:/*ANA D::Accumulator &=Register D.*/
                state->A&=state->D;
                setLFlags(state->A);
                break;
            case 0xa3:/*ANA E::Accumulator &=Register E.*/
                state->A&=state->E;
                setLFlags(state->A);
                break;
            case 0xa4:/*ANA H::Accumulator &=Register H.*/
                state->A&=state->H;
                setLFlags(state->A);
                break;
            case 0xa5:/*ANA L::Accumulator &=Register L.*/
                state->A&=state->L;
                setLFlags(state->A);
                break;
            case 0xa6:/*ANA M::Accumulator &=value at memory location HL.*/
                offset=(state->H<<8)|state->L;
                state->A&=state->memory[offset];
                setLFlags(state->A);
                break;
            case 0xa7:/*ANA A::Accumulator &=Accumulator.*/
                state->A&=state->A;
                setLFlags(state->A);
                break;
            case 0xa8:/*XRA B::Accumulator ^=Register B.*/
                state->A^=state->B;
                setLFlags(state->A);
                break;
            case 0xa9:/*XRA C::Accumulator ^=Register C.*/
                state->A^=state->C;
                setLFlags(state->A);
                break;
            case 0xaa:/*XRA D::Accumulator ^=Register D.*/
                state->A^=state->D;
                setLFlags(state->A);
                break;
            case 0xab:/*XRA E::Accumulator ^=Register E.*/
                state->A^=state->E;
                setLFlags(state->A);
                break;
            case 0xac:/*XRA H::Accumulator ^=Register H.*/
                state->A^=state->H;
                setLFlags(state->A);
                break;
            case 0xad:/*XRA L::Accumulator ^=Register L.*/
                state->A^=state->L;
                setLFlags(state->A);
                break;
            case 0xae:/*XRA M::Accumulator ^=value at memory location HL*/
                offset=(state->H<<8)|state->L;
                state->A^=state->memory[offset];
                setLFlags(state->A);
                break;
            case 0xaf:/*XRA A::Accumulator ^=Accumulator.*/
                state->A^=state->A;
                setLFlags(state->A);
                break;
        }
    }
    else if(*currentOpcode>=0xb0 && *currentOpcode<0xc0){
        switch(*currentOpcode){
            case 0xb0:/*ORA B::Accumulator |=Register B*/
                state->A|=state->B;
                setLFlags(state->A);
                break;
            case 0xb1:/*ORA C::Accumulator |=Register C*/
                state->A|=state->C;
                setLFlags(state->A);
                break;
            case 0xb2:/*ORA D::Accumulator |=Register D.*/
                state->A|=state->D;
                setLFlags(state->A);
                break;
            case 0xb3:/*ORA E::Accumulator |=Register E.*/
                state->A|=state->E;
                setLFlags(state->A);
                break;
            case 0xb4:/*ORA H::Accumulator |=Register H.*/
                state->A|=state->H;
                setLFlags(state->A);
                break;
            case 0xb5:/*ORA L::Accumulator |=Register L.*/
                state->A|=state->L;
                setLFlags(state->A);
                break;
            case 0xb6:/*ORA M::Accumulator |=value at memory address HL.*/
                offset=(state->H<<8)|state->L;
                state->A|=state->memory[offset];
                setLFlags(state->A);
                break;
            case 0xb7:/*ORA A::Accumulator |=Accumulator.*/
                state->A|=state->A;
                setLFlags(state->A);
                break;
            case 0xb8:/*CMP B::Accumulator-Register B.*/
                state->cs.auxCarryBit=(state->A & 0x08)==1 && ((state->A-state->B) & 0x08)==0;
                setCFlags(state->A,state->B);
                break;
            case 0xb9:/*CMP C::Accumulator-Register C.*/
                state->cs.auxCarryBit=(state->A & 0x08)==1 && ((state->A-state->C) & 0x08)==0;
                setCFlags(state->A,state->C);
                break;
            case 0xba:/*CMP D::Accumulator-Register D.*/
                state->cs.auxCarryBit=(state->A & 0x08)==1 && ((state->A-state->D) & 0x08)==0;
                setCFlags(state->A,state->D);
                break;
            case 0xbb:/*CMP E::Accumulator-Register E.*/
                state->cs.auxCarryBit=(state->A & 0x08)==1 && ((state->A-state->E) & 0x08)==0;
                setCFlags(state->A,state->E);
                break;
            case 0xbc:/*CMP H::Accumulator-Register H.*/
                state->cs.auxCarryBit=(state->A & 0x08)==1 && ((state->A-state->H) & 0x08)==0;
                setCFlags(state->A,state->H);
                break;
            case 0xbd:/*CMP L::Accumulator-Register L.*/
                state->cs.auxCarryBit=(state->A & 0x08)==1 && ((state->A-state->L) & 0x08)==0;
                setCFlags(state->A,state->B);
                break;
            case 0xbe:/*CMP M::Accumulator-value at memory location HL.*/
                offset=(state->H<<8)|state->L;
                state->cs.auxCarryBit=(state->A & 0x08)==1 && ((state->A-state->memory[offset]) & 0x08)==0;
                setCFlags(state->A,state->memory[offset]);
                break;
            case 0xbf:/*CMP B::Accumulator-Accumulator*/
                state->cs.auxCarryBit=(state->A & 0x08)==1 && ((state->A-state->A) & 0x08)==0;
                setCFlags(state->A,state->B);
                break;
        }
    }
    else if(*currentOpcode>=0xc0 && *currentOpcode<0xd0){
        switch(*currentOpcode){
            case 0xc0:/*RNZ::Returns if nonzero.*/
                if(!state->cs.zero)
                    returnFrom();
                break;
            case 0xc1:/*POP B::Pops the top 2 values on the Stack into Register Pair B,C.*/
                PopStack(&state->B,&state->C);
                break;
            case 0xc2:/*JNZ 16BitAddress::Jumps to 16BitAddress if nonzero.*/
                if(!state->cs.zero)
                    state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
                else
                    state->ProgramCounter+=2;
                break;
            case 0xc3:/*JMP 16BitAddress::Jumps to 16BitAddress.*/
                state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
                break;
            case 0xc4:/*CNZ 16BitAddress:Calls 16BitAddress if nonzero.*/
                if(!state->cs.zero)
                    call(currentOpcode);
                else
                    state->ProgramCounter+=2;
                break;
            case 0xc5:/*PUSH B::Pushes Register Pair B,C onto the Stack.*/
                PushStack(state->B,state->C);
                break;
            case 0xc6:/*ADI 8BitData::Adds 8BitData to the value in the Accumulator.*/
                ans=(uint16_t)state->A+(uint16_t)currentOpcode[1];
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                state->ProgramCounter++;
                break;
            case 0xc7:/*RST 0::Calls address 0x00.*/
                callLoc(0x00);
                break;
            case 0xc8:/*RZ::If zero, return.*/
                if(state->cs.zero)
                    returnFrom();
                break;
            case 0xc9:/*RET::Returns.*/
                returnFrom();
                break;
            case 0xca:/*JZ 16BitAddress::If zero, jump to 16BitAddress.*/
                if(state->cs.zero)
                    state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
                else
                    state->ProgramCounter+=2;
                break;
            case 0xcb:/*NOP::Does nothing.*/break;
            case 0xcc:/*CZ 16BitAddress:If zero,call 16BitAddress.*/
                if(state->cs.zero)
                    call(currentOpcode);
                else
                    state->ProgramCounter+=2;
                break;
            case 0xcd:/*CALL 16BitAddress::Calls 16BitAddress.*/
                call(currentOpcode);
                break;
            case 0xce:/*ACI 8BitData::Accumulator+=8BitData+carryFlag.*/
                ans=(uint16_t)state->A+(uint16_t)currentOpcode[1]+state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                state->ProgramCounter++;
                break;
            case 0xcf:/*RST 1::Calls address 0x08.*/
                callLoc(0x08);
                break;
        }
    }
    else if(*currentOpcode>=0xd0 && *currentOpcode<0xe0){
        switch(*currentOpcode){
            case 0xd0:/*RNC::If not carryFlag RET.*/
                if(!state->cs.carryBit)
                    returnFrom();
                break;
            case 0xd1:/*POP D::Pops the top two Stack Values into Register Pair D,E.*/
                PopStack(&state->D,&state->E);
                break;
            case 0xd2:/*JNC 16BitAddress::If not carryFlag, jump to 16BitAddress.*/
                if(!state->cs.carryBit)
                    state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
                else
                    state->ProgramCounter+=2;
                break;
            case 0xd3:/*OUT 8BitData::Sends the contents of the Accumulator to the Port specified by 8BitData.*/
                MachineOut(currentOpcode[1]);
                state->ProgramCounter++;
                break;
            case 0xd4:/*CNC 16BitAddress(If not carryFlag,call 16BitAddress.*/
                if(!state->cs.carryBit)
                    call(currentOpcode);
                else
                    state->ProgramCounter+=2;
                break;
            case 0xd5:/*PUSH D::Pushes the values of Register Pair D,E onto the Stack..*/
                PushStack(state->D,state->E);
                break;
            case 0xd6:/*SUI 8BitData::Accumulator-=8BitData.*/
                ans=(uint16_t)state->A-(uint16_t)currentOpcode[1];
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                state->ProgramCounter++;
                break;
            case 0xd7:/*RST 2::Calls address 0x10.*/
                callLoc(0x10);
                break;
            case 0xd8:/*RC::Return if carryFlag.*/
                if(state->cs.carryBit)
                    returnFrom();
                break;
            case 0xd9:/*NOP::Does nothing.*/break;
            case 0xda:/*JC 16BitAddress::If carryFlag, jump to 16BitAddress.*/
                if(state->cs.carryBit)
                    state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
                else
                    state->ProgramCounter+=2;
                break;
            case 0xdb:/*IN 8BitData::Stores 8BitData(Received from a Port) in the Accumulator.*/
                state->A=MachineIn(currentOpcode[1]);
                state->ProgramCounter++;
                break;
            case 0xdc:/*CC 16BitAddress::If carryFlag, call 16BitAddress.*/
                if(state->cs.carryBit)
                    call(currentOpcode);
                else
                    state->ProgramCounter+=2;
                break;
            case 0xdd:/*NOP::Does nothing.*/break;
            case 0xde:/*SBI 8BitData::Accumulator-=(8BitData-carryFlag).*/
                ans=(uint16_t)state->A-(uint16_t)currentOpcode[1]-state->cs.carryBit;
                setAFlags(ans);
                state->cs.auxCarryBit=(state->A & 0x08)==1 && (ans & 0x08)==0;
                state->A=(ans & 0xff);
                state->ProgramCounter++;
                break;
            case 0xdf:/*RST 3::Calls address 0x18.*/
                callLoc(0x18);
                break;
        }
    }
    else if(*currentOpcode>=0xe0 && *currentOpcode<0xf0){
        switch(*currentOpcode){
            case 0xe0:/*RPO::If parity odd, return.*/
                if(!state->cs.parity)
                    returnFrom();
                break;
            case 0xe1:/*POP H::Pops the top 2 Stack values into Register Pair H,L.*/
                PopStack(&state->H,&state->L);
                break;
            case 0xe2:/*JPO 16BitAddress::If parity is odd, jump to 16BitAddress.*/
                if(!state->cs.parity)
                    state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
                else
                    state->ProgramCounter+=2;
                break;
            case 0xe3:/*XTHL::Swaps Register pair H,L values with top two Stack values.*/
                rot=state->L;
                state->L=state->memory[state->StackPointer];
                state->memory[state->StackPointer]=rot;
                rot=state->H;
                state->H=state->memory[state->StackPointer+1];
                state->memory[state->StackPointer+1]=rot;
                break;
            case 0xe4:/*CPO 16BitAddress::If parity odd, call 16BitAddress.*/
                if(!state->cs.parity)
                    call(currentOpcode);
                else
                    state->ProgramCounter+=2;
                break;
            case 0xe5:/*PUSH H::Pushes the values stored in Register pair H,L onto the Stack.*/
                PushStack(state->H,state->L);
                break;
            case 0xe6:/*ANI 8BitData::Accumulator &=8BitData.*/
                state->A&=currentOpcode[1];
                setLFlags(state->A);
                state->ProgramCounter++;
                break;
            case 0xe7:/*RST 4::Calls address 0x20.*/
                callLoc(0x20);
                break;
            case 0xe8:/*RPE::If parity even, return.*/
                if(state->cs.parity)
                    returnFrom();
                break;
            case 0xe9:/*PCHL::Set the program counter to the address specified by Register Pair H,L.*/
                state->ProgramCounter=(state->H<<8)|state->L;
                break;
            case 0xea:/*JPE 16BitAddress::If parity even, jump to 16BitAddress.*/
                if(state->cs.parity)
                    state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
                else
                    state->ProgramCounter+=2;
                break;
            case 0xeb:/*XCHG::Swap data in Register H with Register D and data in Register L with Register E.*/
                rot=state->H;
                state->H=state->D;
                state->D=rot;
                rot=state->L;
                state->L=state->E;
                state->E=rot;
                break;
            case 0xec:/*CPE 16BitAddress::If parity even, call 16BitAddress.*/
                if(state->cs.parity)
                    call(currentOpcode);
                else
                    state->ProgramCounter+=2;
                break;
            case 0xed:/*NOP::Does nothing.*/break;
            case 0xee:/*XRI 8BitData::Accumulator ^=8BitData.*/
                state->A^=currentOpcode[1];
                setLFlags(state->A);
                state->ProgramCounter++;
                break;
            case 0xef:/*RST 5::Calls address 0x28.*/
                callLoc(0x28);
                break;
        }
    }
    else if(*currentOpcode>=0xf0 && *currentOpcode<=0xff){
        switch(*currentOpcode){
            case 0xf0:/*RP::If positive, return.*/
                if(!state->cs.sign)
                    returnFrom();
                break;
            case 0xf1:/*POP PSW::The top 2 values on the Stack are put in the Accumulator and Flags.*/
                state->A=state->memory[state->StackPointer+1];
                rot=state->memory[state->StackPointer];
                state->cs.carryBit=((rot & 0x01)==0x01);
                state->cs.sign=((rot & 0x80)==0x80);
                state->cs.parity=((rot & 0x04)==0x04);
                state->cs.zero=((rot & 0x40)==0x40);
                state->cs.auxCarryBit=((rot & 0x10)==0x10);
                state->StackPointer+=2;
                break;
            case 0xf2:/*JP 16BitAddress::If positive,jumps to 16BitAddress.*/
                if(!state->cs.sign)
                    state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
                else
                    state->ProgramCounter+=2;
                break;
            case 0xf3:/*DI::Disables Interrupts.*/
                state->interrupt_enable=0;
                break;
            case 0xf4:/*CP 16BitAddress::If positive,calls 16BitAddress.*/
                if(!state->cs.sign)
                    call(currentOpcode);
                else
                    state->ProgramCounter+=2;
                break;
            case 0xf5:/*PUSH PSW::Pushes the Accumulator and Flags onto the Stack.*/
                rot=state->cs.carryBit|(state->cs.parity<<2)|(state->cs.auxCarryBit<<4)|(state->cs.zero<<6)|(state->cs.sign<<7);
                PushStack(state->A,rot);
                break;
            case 0xf6:/*ORI 8BitData::Accumulator |=8BitData.*/
                state->A|=currentOpcode[1];
                setLFlags(state->A);
                state->ProgramCounter++;
                break;
            case 0xf7:/*RST 6::Calls address 0x30.*/
                callLoc(0x30);
                break;
            case 0xf8:/*RM::If negative, returns.*/
                if(state->cs.sign)
                    returnFrom();
                break;
            case 0xf9:/*SPHL::Sets Stack pointer to address specified by Register Pair H,L.*/
                state->StackPointer=(state->H<<8)|state->L;
                break;
            case 0xfa:/*JM 16BitAddress::If negative,jump to 16BitAddress.*/
                if(state->cs.sign)
                    state->ProgramCounter=(currentOpcode[2]<<8)|currentOpcode[1];
                else
                    state->ProgramCounter+=2;
                break;
            case 0xfb:/*EI::Enables Interrupts.*/
                state->interrupt_enable=1;
                break;
            case 0xfc:/*CM 16BitAddress::If negative, call 16BitAddress.*/
                if(state->cs.sign)
                    call(currentOpcode);
                else
                    state->ProgramCounter+=2;
                break;
            case 0xfd:/*NOP::Does nothing.*/break;
            case 0xfe:/*CPI 8BitData::Accumulator-8BitData.*/
                state->cs.auxCarryBit=(state->A & 0x08)==1 && ((state->A-currentOpcode[1])& 0x08)==0;
                setCFlags(state->A,currentOpcode[1]);
                state->ProgramCounter++;
                break;
            case 0xff:/*RST 7::Calls address 0x38.*/
                callLoc(0x38);
                break;
        }
    }
    return cycles8080[*currentOpcode];
}
