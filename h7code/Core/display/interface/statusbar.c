#include "statusbar.h"
#include "interface.h"
#include <string.h>

static char g_statusbar_text[STATUSBAR_STR_LEN+1] = {0};
static char g_header_text[STATUSBAR_STR_LEN+1] = {0};

void CopyText(char data[STATUSBAR_STR_LEN+1], const char* text, int start_spaces)
{
    int i=0;
    while(i<start_spaces)
        data[i++] = ' ';
    while(i<STATUSBAR_STR_LEN && *text)
        data[i++] = *text++;
    while(i<STATUSBAR_STR_LEN)
        data[i++] = ' ';
    data[i++] = 0;
}

int StatusbarYMin()
{
    UTF_SetFont(g_default_font);
    return UTFT_getDisplayYSize() - UTF_Height();
}

void StatusbarCopyText(char* data, const char* text)
{
    //CopyText(data, text, 1);
    strncpy(data, text, STATUSBAR_STR_LEN);
}

void StatusbarSetTextAndRedraw(const char* text)
{
    StatusbarSetText(text);
    StatusbarRedraw();
}

void StatusbarSetText(const char* text)
{
    StatusbarCopyText(g_statusbar_text, text);
}


void StatusbarRedraw()
{
    char* text = g_statusbar_text;
    int y = StatusbarYMin();
    UTFT_setColor(VGA_WHITE);
    UTFT_setBackColor(STATUSBAR_BACKGROUND);
    UTF_DrawStringJustify(0, y, text, UTFT_getDisplayXSize(), UTF_CENTER);
}

int HeaderYEnd()
{
    UTF_SetFont(g_default_font);
    return UTF_Height();
}

void HeaderSetText(const char* text)
{
    strncpy(g_header_text, text, STATUSBAR_STR_LEN);
}

void HeaderSetTextAndRedraw(const char* text)
{
    HeaderSetText(text);
    HeaderRedraw();
}

void HeaderRedraw()
{
    UTF_SetFont(g_default_font);
    UTFT_setColor(VGA_WHITE);
    UTFT_setBackColor(STATUSBAR_BACKGROUND);
    UTF_DrawStringJustify(0, 0, g_header_text, UTFT_getDisplayXSize(), UTF_CENTER);
}

RectA R_DisplaySizeWithoutHeaderAndStatusbar()
{
    int ystart = HeaderYEnd();
    int yend = StatusbarYMin();

    RectA r = {
        .x = 0,
        .y = ystart,
        .width = UTFT_getDisplayXSize(),
        .height = yend-ystart,
        .ascent = 0,
        .back_color = VGA_BLACK
        };

    return r;

}
