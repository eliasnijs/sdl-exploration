///////////////////////////////////////////////////////////
//// NOTE(Elias): Font

internal void
pxlfnt_load(char *path, Font *font)
{
  FILE *file;
  S32 file_length;
  file = fopen(path, "rb");
  if (!file) {
    LogErrStr("failed to open file for reading!\n");
    goto err1;
  }
  file_length = filelen(file);
  if (file_length != 4096) {
    LogErrStr("faile to load font, file is corrupted!");
    goto err2;
  }
  fread((U8 *)font->letters[0], 1, file_length, file);
err2:
  fclose(file);
err1:
  return;
}

internal void
pxlfnt_render_letter(SDL_Surface *surface, Font *font, U8 letter_id, V2S32 pos, S32 s, S32 c)
{
  S32 i_d, j_d, i_letter,
      i_letter_i, i_letter_j,
      i_byte, i_bit;
  B32 active;
  U8 *pxl_d, *letter;

  letter = font->letters[letter_id];
  i_d = pos.y, i_letter_i = 0;
  while (i_d < pos.y + 16 * s && i_d < surface->h)
  {
    j_d = pos.x; i_letter_j = 0;
    while (j_d < pos.x + 16 * s && j_d < surface->w)
    {
      i_letter = i_letter_i*16 + i_letter_j;
      i_byte = i_letter / 8;
      i_bit  = i_letter - (i_byte * 8);
      active = letter[i_byte] & (1 << i_bit);
      if (active && i_d > 0 && j_d > 0)
      {
        pxl_d = (U8 *)surface->pixels +
                (i_d*surface->pitch) +
                (j_d*surface->format->BytesPerPixel);
        *(U32 *)pxl_d = c;
      }
      ++j_d;
      if (j_d % s == 0)
        ++i_letter_j;
    }
    ++i_d;
    if (i_d % s == 0)
      ++i_letter_i;
  }
}

// TODO(Elias): BUG: Rendering fails at some scales
internal void
pxlfnt_render(SDL_Surface *surface, Font *font, char *txt,
              V2S32 pos, S32 s, U32 c, S32 krn, S32 ld,
              Font_Align_H a_h, Font_Align_V a_v)
{
  S32 i, l, l_c, nl_cnt, w, h;
  V2S32 pos_c, offset = {};

  // NOTE(Elias): Determine the
  // length of the text and the amount of lines
  i = 0; l = 0; nl_cnt = 0; l_c = 0;
  while(txt[i] != '\0')
  {
    if (txt[i] == '\n')
    {
      l = Max(l, l_c);
      ++nl_cnt;
    }
    ++l_c; ++i;
  }
  l = Max(l, l_c);

  // NOTE(Elias): Set Kerning and leading to more correct values
  // TODO(Elias): Seek a better way to define this offset.
  krn += -2 * s;
  ld += -32;

  // NOTE(Elias): Calculate Horizontal Alignment
  w = l*16*s + (l-1)*krn;
  switch (a_h)
  {
    case(Font_Align_Left):
    {
      offset.x = 0;
    } break;
    case(Font_Align_Center_H):
    {
      offset.x = -w/2;
    } break;
    case(Font_Align_Right):
    {
      offset.x = -w;
    } break;
  }

  // NOTE(Elias): Calculate Vertical Alignment
  h = nl_cnt*(16*s);
  switch (a_v)
  {
    case(Font_Align_Top):
    {
      offset.y = 0;
    } break;
    case(Font_Align_Center_V):
    {
      offset.y = -(s*5 - h/2);
    } break;
    case(Font_Align_Bottom):
    {
      offset.y = -(s*16 - h);
    } break;
  }

  // NOTE(Elias): Draw Text
  pos_c = pos + offset;
  i = 0;
  while (txt[i] != '\0')
  {
    pxlfnt_render_letter(surface, font, txt[i], pos_c, s, c);
    pos_c = pos_c + v2s32(krn + 16*s, 0);
    ++i;
  }
}
