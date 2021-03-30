#include "main.h"
#include "scene_about.h"
#include "images/images.h"

#include "UTFT.h"
#include "interface/interface.h"
#include "interface/rect_utils.h"
#include "interface/statusbar.h"

static ReturnSceneFunction func_return;

static void SceneAboutQuant();

void SceneAboutStart(ReturnSceneFunction func)
{
    func_return = func;

    UTFT_setColor(VGA_WHITE);
    UTFT_setBackColor(VGA_BLACK);
    UTF_SetFont(g_default_font);

    HeaderSetTextAndRedraw("About");
    StatusbarSetTextAndRedraw("BalmerDX 2021");

    RectA r_all = R_DisplaySizeWithoutHeaderAndStatusbar();
    R_FillRectBack(&r_all);

    UTFT_drawBitmap4(r_all.x + (r_all.width-balmer_img.width)/2,
                     r_all.y + (r_all.height-balmer_img.height)/2,
                     &balmer_img);

    InterfaceGoto(SceneAboutQuant);
}

void SceneAboutQuant()
{
    if(EncButtonPressed())
    {
        func_return();
        return;
    }
}
