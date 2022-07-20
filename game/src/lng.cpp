internal void
render_pattern(SDL_Surface *surface, S32 offset1, S32 offset2, S32 offset3)
{
  S32 bytes_per_pixel = surface->format->BytesPerPixel;
  S32 pattern_length = 2000;
  for (S32 row = 0;
       row < surface->h;
       ++row)
  {
    for (S32 column = 0;
         column < surface->w;
         ++column)
    {
      U8 *p = (U8 *)surface->pixels + (row * surface->pitch) + (column * bytes_per_pixel);
      U8 red_channel = (U8) offset1;
      U8 green_channel = (U8) ((row + offset2) % pattern_length);
      U8 blue_channel =  (U8) ((column + offset3) % pattern_length);
      *(U32 *)p = (red_channel << 16) | (green_channel << 8) | (blue_channel);
    }
  }
}

internal void
render_background(SDL_Surface *surface)
{
  S32 bytes_per_pixel = surface->format->BytesPerPixel;
  for (S32 row = 0;
       row < surface->h;
       ++row)
  {
    for (S32 column = 0;
         column < surface->w;
         ++column)
    {
      U8 *p = (U8 *)surface->pixels + (row * surface->pitch) + (column * bytes_per_pixel);
      *(U32 *)p = 0x00FFFFFF;
    }
  }
}

internal void
draw_box(SDL_Surface *surface, S32 x, S32 y, S32 width, S32 height, S32 color)
{
  Assert(x > 0);
  Assert(y > 0);
  Assert(x + width < surface->w);
  Assert(y + height < surface->h);
  S32 bytes_per_pixel = surface->format->BytesPerPixel;
  for (S32 row = y;
       row < (y + height);
       ++row)
  {
    for (S32 column = x;
         column < (x + width);
         ++column)
    {
      U8 *p = (U8 *)surface->pixels + (row * surface->pitch) + (column * bytes_per_pixel);
      *(U32 *)p = color;
    }
  }
}

void
game_update_and_render(GameState *game_state, GameInput *game_input, SDL_Surface *surface)
{
  if (!game_state->is_initialised)
  {
    // TODO(Elias): initialisation might be better in the sdl layer or as a seperate function, 
    // take a look at it
    // TODO(Elias): ... init game
    game_state->is_initialised = true;
  }

  S32 box_width = 50;
  S32 box_height = 50;
  
  S32 box_x_speed = 5;
  S32 box_y_speed = 5;

  S32 x_velocity = 0;
  S32 y_velocity = 0;

  if (game_input->move_up.ended_down)
  {
    y_velocity -= box_y_speed;
  }
  if (game_input->move_down.ended_down)
  {
    y_velocity += box_y_speed;
  }
  if (game_input->move_right.ended_down)
  {
    x_velocity += box_x_speed;
  }
  if (game_input->move_left.ended_down)
  {
    x_velocity -= box_x_speed;
  }

  game_state->player_x = Clamp(0, game_state->player_x + x_velocity, surface->w - box_width);
  game_state->player_y = Clamp(0, game_state->player_y + y_velocity, surface->h - box_height);

  render_background(surface);
  draw_box(surface, game_state->player_x, game_state->player_y, box_width, box_height, 0x001dbcd1);
  
  ++game_state->offset1;
}

