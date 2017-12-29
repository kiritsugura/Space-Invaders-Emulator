#include "GUIComp.h"
struct borderInfo* initBorder(uint32_t color,uint8_t w,uint8_t h){
    struct borderInfo *bi=malloc(sizeof(struct borderInfo));
    bi->color=color;
    bi->width=w;
    bi->height=h;
    return bi;
}
struct Button* initButton(uint16_t x,uint16_t y,char *tx,SDL_Color cl,uint32_t back1,uint32_t back2,TTF_Font *ft,struct borderInfo *bi){
    struct Button *bt=malloc(sizeof(struct Button));
    bt->rect=malloc(sizeof(SDL_Rect));
    SDL_Surface *temp=TTF_RenderText_Solid(ft,tx,cl);
    bt->rect->w=temp->w;
    bt->rect->h=temp->h;
    if(bi==NULL){
        bt->rect->x=3;
        bt->rect->y=2;
        bt->bt=SDL_CreateRGBSurface(0,bt->rect->w+6,bt->rect->h+4,32,0,0,0,0);
        bt->act=SDL_CreateRGBSurface(0,bt->rect->w+6,bt->rect->h+4,32,0,0,0,0);
        SDL_FillRect(bt->bt,NULL,0xFFDDDDDD);
        SDL_FillRect(bt->act,NULL,0xFFDDDDDD);
    }else{
        bt->rect->x=bi->width;
        bt->rect->y=bi->height;
        bt->bt=SDL_CreateRGBSurface(0,bt->rect->w+bi->width*2,bt->rect->h+bi->height*2,32,0,0,0,0);
        bt->act=SDL_CreateRGBSurface(0,bt->rect->w+bi->width*2,bt->rect->h+bi->height*2,32,0,0,0,0);
        SDL_FillRect(bt->bt,NULL,bi->color);
        SDL_FillRect(bt->act,NULL,bi->color);
    }
    SDL_FillRect(bt->bt,bt->rect,back1);
    SDL_FillRect(bt->act,bt->rect,back2);
    SDL_BlitSurface(temp,NULL,bt->bt,bt->rect);
    SDL_BlitSurface(temp,NULL,bt->act,bt->rect);
    if(bi==NULL){
        bt->rect->w+=6;
        bt->rect->h+=4;
    }else{
        bt->rect->w+=bi->width*2;
        bt->rect->h+=bi->height*2;
    }
    bt->rect->x=x;
    bt->rect->y=y;
    SDL_FreeSurface(temp);
    bt->on=0;
    return bt;
}
void drawButton(struct Button *bt,SDL_Surface *window){
    (bt->on) ? SDL_BlitSurface(bt->act,NULL,window,bt->rect):SDL_BlitSurface(bt->bt,NULL,window,bt->rect);
}
uint8_t contains(struct Button *bt,uint16_t x,uint16_t y){
    return (bt->rect->x<x) & (bt->rect->y<y) & (bt->rect->x+bt->rect->w>x) & (bt->rect->y+bt->rect->h>y);
}
void freeButton(struct Button *bt){
    SDL_FreeSurface(bt->bt);
    SDL_FreeSurface(bt->act);
    free(bt->rect);
    free(bt);
}

struct Label* initLabel(uint16_t x,uint16_t y,TTF_Font *font,char *text,SDL_Color color){
    struct Label *lbl=malloc(sizeof(struct Label));
    lbl->text=TTF_RenderText_Solid(font,text,color);
    lbl->rect=malloc(sizeof(SDL_Rect));
    lbl->rect->x=x;
    lbl->rect->y=y;
    lbl->rect->w=lbl->text->w;
    lbl->rect->h=lbl->text->h;
    return lbl;
}
void drawLabel(struct Label *lbl,SDL_Surface *surface){
    SDL_BlitSurface(lbl->text,NULL,surface,lbl->rect);
}
void freeLabel(struct Label *lbl){
    SDL_FreeSurface(lbl->text);
    free(lbl->rect);
    free(lbl);
}

struct SelectableButton* initSelectableButton(uint8_t totalNum,struct Button **buttons){
    struct SelectableButton *buttonGroup=malloc(sizeof(struct SelectableButton));
    buttonGroup->numOf=totalNum;
    buttonGroup->Buttons=buttons;
    return buttonGroup;
}
void drawSelectableButton(struct SelectableButton *buttonGroup,SDL_Surface *surface){
    uint8_t counter=0;
    while(counter<buttonGroup->numOf)
        drawButton(buttonGroup->Buttons[counter++],surface);
}
uint8_t SelectableContains(struct SelectableButton *buttonGroup,uint16_t x,uint16_t y){
    uint8_t counter=0,val=255;
    while(counter<buttonGroup->numOf){
        if(contains(buttonGroup->Buttons[counter],x,y))
            val=counter;
        counter++;
    }
    return val;
}
void freeSelectableButton(struct SelectableButton *buttonGroup){
    uint8_t counter=0;
    while(counter<buttonGroup->numOf)
        freeButton(buttonGroup->Buttons[counter++]);
    free(buttonGroup->Buttons);
    free(buttonGroup);
}

struct Textbox* initTextbox(uint16_t x,uint16_t y,const char *text,uint8_t maxLength,uint32_t backcolor,SDL_Color textColor,TTF_Font *font,struct borderInfo *binfo){
    struct Textbox *textbox=malloc(sizeof(struct Textbox));
    SDL_Surface *textsurf=TTF_RenderText_Solid(font,text,textColor);
    textbox->maxLength=maxLength;
    textbox->text=malloc(maxLength);
    uint8_t i=0,len=strlen(text);
    while(i<len)
        textbox->text[i]=text[i++];
    textbox->text[i]=0;
    textbox->currentSize=len;
    textbox->font=font;
    textbox->textColor=textColor;
    textbox->backColor=backcolor;
    textbox->rect=malloc(sizeof(SDL_Rect));
    TTF_SizeText(font,text,&textbox->rect->x,&textbox->rect->y);
    textbox->rect->w=textsurf->w;
    textbox->rect->h=textsurf->h;
    textbox->current=SDL_CreateRGBSurface(0,textbox->rect->x+binfo->width*2,textbox->rect->y+binfo->height*2,32,0,0,0,0);
    SDL_FillRect(textbox->current,NULL,binfo->color);
    textbox->borderColor=binfo->color;
    textbox->xoff=binfo->width;
    textbox->yoff=binfo->height;
    textbox->rect->x=binfo->width;
    textbox->rect->y=binfo->width;
    SDL_FillRect(textbox->current,textbox->rect,backcolor);
    SDL_BlitSurface(textsurf,NULL,textbox->current,textbox->rect);
    textbox->rect->x=x;
    textbox->rect->y=y;
    textbox->rect->w=textbox->current->w;
    textbox->rect->h=textbox->current->h;
    SDL_FreeSurface(textsurf);
    return textbox;
}
void drawTextbox(struct Textbox *textbox,SDL_Surface *surface){
    SDL_BlitSurface(textbox->current,NULL,surface,textbox->rect);
}
uint8_t containsTextbox(struct Textbox *textbox,uint16_t x,uint16_t y){
    return (textbox->rect->x<x) & (textbox->rect->y<y) & (textbox->rect->x+textbox->rect->w>x) & (textbox->rect->y+textbox->rect->h>y);
}
uint8_t canAppend(struct Textbox *textbox){
    return textbox->currentSize<textbox->maxLength-1;
}
void appendChar(struct Textbox *textbox,uint8_t character){
    textbox->text[textbox->currentSize++]=character;
}
void appendString(struct Textbox *textbox,const char *str){
    uint8_t dsr=strlen(str),count=0;
    while(count<dsr)
        textbox->text[textbox->currentSize++]=str[count++];
    setTextboxText(textbox);
}
void resetTextbox(struct Textbox *textbox){
    while(textbox->currentSize)
        textbox->text[--textbox->currentSize]='\0';
    setTextboxText(textbox);
}
void setTextboxText(struct Textbox *textbox){
    SDL_Surface *temp=TTF_RenderText_Solid(textbox->font,textbox->text,textbox->textColor);
    uint16_t tp1=textbox->rect->x,tp2=textbox->rect->y;
    if(temp!=NULL)
        textbox->rect->w=temp->w+textbox->xoff*2;
    SDL_FreeSurface(textbox->current);
    textbox->current=SDL_CreateRGBSurface(0,textbox->rect->w,textbox->rect->h,32,0,0,0,0);
    SDL_FillRect(textbox->current,NULL,textbox->borderColor);
    textbox->rect->x=textbox->xoff;
    textbox->rect->y=textbox->yoff;
    textbox->rect->w-=textbox->xoff*2;
    textbox->rect->h-=textbox->yoff*2;
    SDL_FillRect(textbox->current,textbox->rect,textbox->backColor);
    SDL_BlitSurface(temp,NULL,textbox->current,textbox->rect);
    textbox->rect->x=tp1;
    textbox->rect->y=tp2;
    textbox->rect->w+=textbox->xoff*2;
    textbox->rect->h+=textbox->yoff*2;
    free(temp);
}
uint8_t checkTextboxTextBounds(struct Textbox *textbox,uint16_t low,uint16_t high){
    uint16_t num=textToInt(textbox->text);
    return num<=high && num>=low;
}
void textboxEraseLast(struct Textbox *textbox){
    textbox->text[--textbox->currentSize]='\0';
}
void freeTextbox(struct Textbox *textbox){
    free(textbox->current);
    free(textbox->rect);
    free(textbox->text);
    free(textbox);
}

struct Menu* initMenu(struct Button **buttons,uint8_t buttonNum,struct Label **labels,uint8_t labelNum,struct SelectableButton **sButtons,uint8_t sButtonsNum,struct Textbox **textboxes,uint8_t textboxNum,SDL_Rect *dim,uint32_t cl){
    struct Menu *menu=malloc(sizeof(struct Menu));
    uint8_t c=0;
    menu->active=0;
    menu->buttons=buttons;
    menu->buttonsNum=buttonNum;
    menu->labels=labels;
    menu->labelNum=labelNum;
    menu->sButtons=sButtons;
    menu->sbNum=sButtonsNum;
    menu->tbNum=textboxNum;
    menu->tboxes=textboxes;
    menu->area=dim;
    menu->MenuBack=SDL_CreateRGBSurface(0,dim->w,dim->h,32,0,0,0,0);
    SDL_FillRect(menu->MenuBack,NULL,cl);
    menu->startTb=buttonNum;
    while(c<sButtonsNum)
        menu->startTb+=sButtons[c++]->numOf;
    menu->menuColor=cl;
    return menu;
}
void drawMenu(struct Menu *menu,SDL_Surface *surface){
    uint8_t counter=0;
    while(counter<menu->buttonsNum)
        drawButton(menu->buttons[counter++],menu->MenuBack);
    counter=0;
    while(counter<menu->labelNum)
        drawLabel(menu->labels[counter++],menu->MenuBack);
    counter=0;
    while(counter<menu->sbNum)
        drawSelectableButton(menu->sButtons[counter++],menu->MenuBack);
    counter=0;
    while(counter<menu->tbNum)
        drawTextbox(menu->tboxes[counter++],menu->MenuBack);
    SDL_BlitSurface(menu->MenuBack,NULL,surface,menu->area);
}
uint8_t MenuContains(struct Menu *menu,uint16_t x,uint16_t y){
    uint8_t counter=0,snum=0,val=255;
    x-=menu->area->x;
    y-=menu->area->y;
    while(counter<menu->buttonsNum){
        if(contains(menu->buttons[counter++],x,y))
            val=counter-1;
    }
    counter=0;
    while(counter<menu->sbNum){
        snum=SelectableContains(menu->sButtons[counter++],x,y);
        if(snum^255)
            val=counter+menu->buttonsNum+snum-1;
    }
    counter=0;
    while(counter<menu->tbNum){
        if(containsTextbox(menu->tboxes[counter++],x,y))
            val=menu->startTb+counter-1;
    }
    if(val^255)
        menu->active=val;
    return val;
}
uint8_t isActiveButton(struct Menu *menu){
    return menu->startTb<menu->active;
}
struct Button* getActiveButton(struct Menu *menu){
    uint8_t count=0,tot=menu->buttonsNum;
    if(menu->active<menu->buttonsNum)
        return menu->buttons[menu->active];
    while(tot<=menu->active)
        tot+=menu->sButtons[count++]->numOf;
    tot-=menu->sButtons[count-1]->numOf;
    return menu->sButtons[count-1]->Buttons[menu->active-tot];
}
void fillTextbox(struct Menu *menu,struct Textbox *textbox){
    SDL_FillRect(menu->MenuBack,textbox->rect,menu->menuColor);
}
void freeMenu(struct Menu *menu){
    uint8_t counter=0;
    SDL_FreeSurface(menu->MenuBack);
    while(counter<menu->buttonsNum)
        freeButton(menu->buttons[counter++]);
    counter=0;
    while(counter<menu->labelNum)
        freeLabel(menu->labels[counter++]);
    counter=0;
    while(counter<menu->sbNum)
        freeSelectableButton(menu->sButtons[counter++]);
    counter=0;
    while(counter<menu->tbNum)
        freeTextbox(menu->tboxes[counter++]);
    free(menu->buttons);
    free(menu->labels);
    free(menu->sButtons);
    free(menu->tboxes);
    free(menu->area);
    free(menu);
}
