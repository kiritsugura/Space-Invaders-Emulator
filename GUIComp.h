#ifndef GUICOMP_H_INCLUDED
#define GUICOMP_H_INCLUDED
#include <inttypes.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include "util.h"

/// <struct>borderInfo</struct>
/// <member name="color">The color of the border represented as a uint32_t.</member>
/// <member name="width">The direction each way on x border. Example:1 means a 1 pixel border on the left and right.</member>
/// <member name="height">The direction each way on the y border.Example:1 means a 1 pixel border on the top and bottom.</member>
struct borderInfo{
    uint32_t color;
    uint8_t width;
    uint8_t height;
};
/// <struct>Button</struct>
/// <member name="rect">A pointer to a SDL_Rect that represents the area this Button covers.</member>
/// <member name="bt">A SDL_Surface that is rendered when the Button is not active.</member>
/// <member name="act">A SDL_Surface that is rendered when the Button is active.</member>
/// <member name="on">A value of 1 means this Button is active, while a value of 0 means it is inactive.</member>
struct Button{
    SDL_Rect *rect;
    SDL_Surface *bt;
    SDL_Surface *act;
    uint8_t on;
};
/// <struct>Label</struct>
/// <member name="rect">A pointer to a SDL_Rect that represents the area this Label covers.</member>
/// <member name="text">A SDL_Surface that contains the text of the Label.</member>
struct Label{
    SDL_Rect *rect;
    SDL_Surface *text;
};
/// <struct>SelectableButton</struct>
/// <member name="numOf">The number of Buttons within the Buttons variable.</member>
/// <member name="Buttons">A pointer that points to a section of memory containing pointers to Button structs.</member>
/// <remarks>Effectively a grouping of Buttons.</remarks>
struct SelectableButton{
    uint8_t numOf;
    struct Button **Buttons;
};
/// <struct>Textbox</struct>
/// <member name="text">A string that represents the text currently in the Textbox.</member>
/// <member name="maxLength">The maximum number of characters the textbox is allowed to be.</member>
/// <member name="xoff">The offset from the border in the x direction(borderInfo->width)</member>
/// <member name="yoff">The offset from the border in the y direction(borderInfo->height)</member>
/// <member name="currentSize">The current number of characters in the Textbox-the length of variable text.</member>
/// <member name="borderColor">The color of the Textbox border as a uint32_t.</member>
/// <member name="backColor">The color of the Textbox background as a uint32_t.</member>
/// <member name="rect">A pointer to a SDL_Rect that represents the area of the Textbox.</member>
/// <member name="current">A SDL_Surface that represents the current graphical appearance of the Textbox.</member>
/// <member name="font">A pointer to the font that variable text is being rendered using.</member>
/// <member name="textColor">A SDL_Color that represents the color of the text being rendered.</member>
/// <remarks>This Textbox is manually updated by the Programmer using the setTextboxText function. The Textbox can be self-sizing using this method.</remarks>
struct Textbox{
    uint8_t *text;
    uint8_t maxLength;
    uint8_t xoff;
    uint8_t yoff;
    uint8_t currentSize;
    uint32_t borderColor;
    uint32_t backColor;
    SDL_Rect *rect;
    SDL_Surface *current;
    TTF_Font *font;
    SDL_Color textColor;
};
/// <struct>Menu</struct>
/// <member name="area">A pointer to a SDL_Rect that represents the area of this Menu.</member>
/// <member name="MenuBack">A pointer to a SDL_Surface that represents the renderable surface of this Menu.</member>
/// <member name="menuColor">The color of the Menu background represented as a uint32_t.</member>
/// <member name="buttonsNum">The total number of Buttons in the buttons variable.</member>
/// <member name="labelNum">The total number of Labels in the labels variable.</member>
/// <member name="sbNum">The total number of SelectableButtons in the sButtons variable.</member>
/// <member name="tbNum">The total number of Textboxes in the tboxes variable.</member>
/// <member name="startTb">The index of elements at which the Textboxes start.</member>
/// <member name="active">A number representing the index at which the currently active element is located.</member>
/// <member name="buttons">A pointer to a section of memory that contains pointers to Button structs.</member>
/// <member name="labels">A pointer to a section of memory that contains pointers to Label structs.</member>
/// <member name="tboxes">A pointer to a section of memory that contains pointers to Textbox structs.</member>
/// <member name="sButtons">A pointer to a section of memory that contains pointers to SelectableButton structs.</member>
/// <remarks>Any elements given to the Menu will be deallocated by the menu.</remarks>
/// <remarks>The way indexing works is that index 0 through buttonNum(noninclusive) is a Button, buttonNum through tbNum(noninclusive) is a Button in a SelectableButton,startTb through startTb+tbNum is a Textbox.</remarks>
struct Menu{
    SDL_Rect *area;
    SDL_Surface *MenuBack;
    uint32_t menuColor;
    uint8_t buttonsNum;
    uint8_t labelNum;
    uint8_t sbNum;
    uint8_t tbNum;
    uint8_t startTb;
    uint8_t active;
    struct Button **buttons;
    struct Label **labels;
    struct Textbox **tboxes;
    struct SelectableButton **sButtons;
};
        /// <related>borderInfo</related>
/// <summary>Initialize a borderInfo struct with given values.</summary>
/// <param name="color">The color of the border.</param>
/// <param name="w">The width of the border on each side.</param>
/// <param name="h">The height of the border on each side.</param>
/// <returns>Returns a pointer to a borderInfo struct initialized with the parameter values.</returns>
struct borderInfo* initBorder(uint32_t color,uint8_t w,uint8_t h);

        /// <related>struct Button</related>
/// <summary>Initialize a Button struct with the parameter values.</summary>
/// <param name="x">The x value of the top left corner of the Button.</param>
/// <param name="y">The y value of the top left corner of the Button.</param>
/// <param name="tx">A string, the text that will be rendered onto the Button</param>
/// <param name="cl">The color of the text on the button.</param>
/// <param name="back1">The color of the button when not selected.</param>
/// <param name="back2">The color of the button when selected.</param>
/// <param name="ft">A pointer to the font that tx will be rendered with.</param>
/// <param name="bi">A pointer to a borderInfo struct for the Button.</param>
/// <returns>Returns a pointer to a Button struct initialized with the parameter values.</returns>
struct Button* initButton(uint16_t x,uint16_t y,char *tx,SDL_Color cl,uint32_t back1,uint32_t back2,TTF_Font *ft,struct borderInfo *bi);
/// <summary>Renders the button onto a surface.</summary>
/// <param name="bt">A pointer to the button struct that will be rendered.</param>
/// <param name="window">A pointer to a surface that the button will be rendered onto.</param>
/// <remarks>The button will render bt or act based on if it is on.</remarks>
void drawButton(struct Button *bt,SDL_Surface *window);
/// <summary>Checks to see if the Button contains a point.</summary>
/// <param name="bt">A pointer to the Button being checked for containment.</param>
/// <param name="x">The x value of the point being tested.</param>
/// <param name="y">The y value of the point being tested.</param>
/// <returns> Returns a 1 if the point is within the Button, else it returns 0.</returns>
uint8_t contains(struct Button *bt,uint16_t x,uint16_t y);
/// <summary>Frees a Button struct.</summary>
/// <param name="bt">A pointer to the Button being freed.</param>
void freeButton(struct Button *bt);

        /// <related>struct Label</related>
/// <summary>Initializes a Label struct with the parameter values.</summary>
/// <param name="x">The upper left corner x position of the Label.</param>
/// <param name="y">The upper left corner y position of the Label.</param>
/// <param name="font">A pointer to the font that the Label will be rendered with.</param>
/// <param name="text">A string, the text that the Label will render.</param>
/// <param name="color">The color of the label text.</param>
/// <returns>Returns a pointer to a Label struct initialized with the parameter values.</returns>
struct Label* initLabel(uint16_t x,uint16_t y,TTF_Font *font,char *text,SDL_Color color);
/// <summary>Renders a Label onto a surface.</summary>
/// <param name="lbl">A pointer to the Label that is going to be rendered.</param>
/// <param name="surface">A pointer to the surface being rendered onto.</param>
void drawLabel(struct Label *lbl,SDL_Surface *surface);
/// <summary>Frees a Label struct.</summary>
/// <param name="lbl">A pointer to the Label that being freed.</param>
void freeLabel(struct Label *lbl);

        ///<related>struct SelectableButton</related>
/// <summary>Initializes a SelectableButton struct with the parameter values.</summary>
/// <param name="totalNum">The total number of buttons in this SelectableButton struct.</param>
/// <param name="buttons">A pointer to a memory section that contains pointers that point to Button structs.</param>
/// <returns>Returns a pointer to a SelectableButton struct initialized with the parameter values.</returns>
struct SelectableButton* initSelectableButton(uint8_t totalNum,struct Button **buttons);
/// <summary>Renders the SelectableButton struct onto a surface.</summary>
/// <param name="buttonGroup">A pointer to the SelectableButton struct that will be rendered</param>
/// <param name="surface">A pointer to the surface being rendered onto.</param>
void drawSelectableButton(struct SelectableButton *buttonGroup,SDL_Surface *surface);
/// <summary>Checks to see if a point is contained with any of the Buttons in this SelectableButton struct.</summary>
/// <param name="buttonGroup">A pointer to the SelectableButton being checked against the point.</param>
/// <param name="x">The x position of the point being checked.</param>
/// <param name="y">The y position of the point being checked.</param>
/// <returns>Returns the index of the Button that contains the point within the SelectableButton, else returns 255.</returns>
uint8_t SelectableContains(struct SelectableButton *buttonGroup,uint16_t x,uint16_t y);
/// <summary>Frees a SelectableButton struct.</summary>
/// <param name="buttonGroup">A pointer to the SelectableButton that is being freed.</param>
void freeSelectableButton(struct SelectableButton *buttonGroup);

        ///<related>struct Textbox</related>
/// <summary>Initializes a Textbox struct with the parameter values.</summary>
/// <param name="x">The x value of the upper left corner of the Textbox.</param>
/// <param name="y">The y value of the upper left corner of the Textbox.</param>
/// <param name="text">The default string that will be placed into the Textbox;A deep copy of the text is made.</param>
/// <param name="maxLength">The maximum length that the Textbox is allowed to be.</param>
/// <param name="backcolor">The color of the Textbox background.</param>
/// <param name="textcolor">The color of the text in the Textbox.</param>
/// <param name="font">The font used to render the text in the Textbox.</param>
/// <param name="binfo">A pointer to a borderInfo struct for the Textbox.</param>
/// <returns>Returns a pointer to a Textbox struct initialized with the parameter values.</returns>
/// <remarks>The Textbox is not of a static size, it can be resized by the user based on the text within it.</remarks>
struct Textbox* initTextbox(uint16_t x,uint16_t y,const char *text,uint8_t maxLength,uint32_t backcolor,SDL_Color textColor,TTF_Font *font,struct borderInfo *binfo);
/// <summary>Renders a Textbox onto a surface.</summary>
/// <param="textbox">A pointer to the Textbox that is going to be rendered.</param>
/// <param="surface">The surface the Textbox is rendered onto.</param>
void drawTextbox(struct Textbox *textbox,SDL_Surface *surface);
/// <summary>Checks to see if a point is contained within a textbox.</summary>
/// <param name="textbox">A pointer to the Textbox that is being checked against the point.</param>
/// <param name="x">The x value of the point being checked.</param>
/// <param name="y">The y value of the point being checked.</param>
/// <returns>Returns 1 if the point is within the Textbox, else returns 0.</returns>
uint8_t containsTextbox(struct Textbox *textbox,uint16_t x,uint16_t y);
/// <summary>Checks to see if the Textbox can be appended to.</summary>
/// <param name="textbox">A pointer to the Textbox that is being checked for potential appendage.</param>
/// <returns>Returns 1 if the current length of the text in the Textbox is less than the maximum length allowed minus one.</returns>
uint8_t canAppend(struct Textbox *textbox);
/// <summary>Appends a character onto the end of the text in the current Textbox.</summary>
/// <param name="textbox">A pointer to the Textbox that is being appended onto.</param>
/// <param name="character">The character being appended onto the Textbox.</param>
void appendChar(struct Textbox *textbox,uint8_t character);
/// <summary>Appends a string onto the end of the text in the current Textbox.</summary>
/// <param name="textbox">A pointer to the Textbox being appended onto.</param>
/// <param name="str">The string being appended onto the Textbox.</param>
void appendString(struct Textbox *textbox,const char *str);
/// <summary>Sets the Textbox text to nothing.</summary>
/// <param name="textbox">The Textbox whose contents are being set to nothing.</param>
/// <remarks>Calls setTextboxText after the Textbox is cleared to update it graphically.</remarks>
void resetTextbox(struct Textbox *textbox);
/// <summary>Updates the Textbox so that the current text stored within is rendered.</summary>
/// <param name="textbox">A pointer to the Textbox whose graphical rendering is being updated.</param>
void setTextboxText(struct Textbox *textbox);
/// <summary>Checks to see if the uint8_t form of the text in the current Textbox is within a range.</summary>
/// <param name="textbox">A pointer to the Textbox whose text is being checked.</param>
/// <param name="low">The minimum value allow for the text.</param>
/// <param name="high">The maximum value allowed for the text.</param>
/// <returns>Returns 1 if the number is within the range, else returns 0 if it is outside of the range.</returns>
uint8_t checkTextboxTextBounds(struct Textbox *textbox,uint16_t low,uint16_t high);
/// <summary>Erases the last character in the current Textbox.</summary>
/// <param name="textbox">A pointer to the Textbox whose last character is being erased.</param>
void textboxEraseLast(struct Textbox *textbox);
/// <summary>Frees a Textbox struct.</summary>
/// <param name="textbox">A pointer to the textbox that is being freed.</param>
void freeTextbox(struct Textbox *textbox);

        ///<related>struct Menu</related>
/// <summary>Initializes a Menu struct with the parameter values.</summary>
/// <param name="buttons">A pointer that points to a memory section that contains pointers that point to Button structs.</param>
/// <param name="buttonNum">The total number of Buttons in buttons.</param>
/// <param name="labels">A pointer that points to a memory section that contains pointers that point to Label structs.</param>
/// <param name="labelNum">The total number of Labels in labels.</param>
/// <param name="sButtons">A pointer that points to a memory section that contains pointers that point to SelectableButton structs.</param>
/// <param name="sButtonsNum">The total number of SelectableButtons in sButtons.</param>
/// <param name="textboxes">A pointer that points to a memory section that contains pointers that point to Textboxe structs.</param>
/// <param name="textboxNum">The total number of Textboxes in textboxes.</param>
/// <param name="dim">The dimensions of the Menu.</param>
/// <param name="cl">The background color of the Menu.</param>
/// <returns>Returns a pointer to a Menu struct initialized with the values of the parameters.</returns>
struct Menu* initMenu(struct Button **buttons,uint8_t buttonNum,struct Label **labels,uint8_t labelNum,struct SelectableButton **sButtons,uint8_t sButtonsNum,struct Textbox **textboxes,uint8_t textboxNum,SDL_Rect *dim,uint32_t cl);
/// <summary>Renders the current Menu onto a surface.</summary>
/// <param name="menu">A pointer to the Menu that is going to be rendered.</param>
/// <param name="surface">The surface that the Menu is going to be rendered onto.</param>
void drawMenu(struct Menu *menu,SDL_Surface *surface);
/// <summary>Checks to see if a point is within any of the Menu`s elements.</summary>
/// <param name="menu">A pointer to the Menu whose elements are being checked.</param>
/// <param name="x">The x value of the point being checked.</param>
/// <param name="y">The y value of the point being checked.</param>
/// <returns>Returns the index of the element in the Menu that was clicked, if none where clicked returns 255.</returns>
uint8_t MenuContains(struct Menu *menu,uint16_t x,uint16_t y);
/// <summary>Checks to see if the current Menu`s active element is a button.</summary>
/// <param name="menu">A pointer to the menu which is being checked to see if a button is currently active.</param>
/// <returns>Returns 1 if the active element is a Button, else returns 0.</returns>
uint8_t isActiveButton(struct Menu *menu);
/// <summary>Gets the current Menu`s active button.</summary>
/// <param name="menu">A pointer to the menu which contains an active Button.</param>
/// <returns>Returns the active Button in the current Menu.</returns>
struct Button* getActiveButton(struct Menu *menu);
/// <summary>Fills in the area that a Textbox occupies with the Menu`s background color.</summary>
/// <param name="menu">A pointer to the Menu the needs to be partially filled.</param>
/// <param name="textbox">The textbox that is going to be filled in.</param>
void fillTextbox(struct Menu *menu,struct Textbox *textbox);
/// <summary>Frees a Menu struct and all of its elements.</summary>
/// <param name="menu">A pointer to the Menu that is being freed.</param>
void freeMenu(struct Menu *menu);

#endif // GUICOMP_H_INCLUDED
