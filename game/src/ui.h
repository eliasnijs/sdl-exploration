///////////////////////////////////////////////////////////
//// NOTE(Elias): Fonts

enum Font_Align_H {
  Font_Align_Left,
  Font_Align_Center_H,
  Font_Align_Right,
};

enum Font_Align_V {
  Font_Align_Top,
  Font_Align_Center_V,
  Font_Align_Bottom,
};

struct Font 
{
  U8 letters[128][32];
};

internal void pxlfnt_load(char *path, Font *font);
internal void pxlfnt_render_letter(SDL_Surface *surface, Font *font, 
                                   U8 letter_id, V2S32 pos, S32 s, S32 c);
internal void pxlfnt_render(SDL_Surface *surface, Font *font, char *txt, 
                            V2S32 pos, S32 s, U32 c, S32 krn, S32 ld, 
                            Font_Align_H a_h, Font_Align_V a_v);
