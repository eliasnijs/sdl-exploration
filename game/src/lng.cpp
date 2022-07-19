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

void
game_update_and_render(GameState *game_state, GameInput *game_input, SDL_Surface *surface)
{
  if (!game_state->is_initialised)
  {
    // TODO(Elias): initialisation might be better in the sdl layer, take a look at it
    // ... init game
    game_state->is_initialised = true;
  }

  if (game_input->move_up.ended_down)
  {
    ++game_state->offset2;
  }
  if (game_input->move_right.ended_down)
  {
    ++game_state->offset3;
  }
  if (game_input->move_down.ended_down)
  {
    --game_state->offset3;
  }
  if (game_input->move_left.ended_down)
  {
    --game_state->offset2;
  }

  render_pattern(surface, game_state->offset1, game_state->offset2, game_state->offset3);
  ++game_state->offset1;
}

