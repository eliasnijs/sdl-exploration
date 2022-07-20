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
      *(U32 *)p = 0x0095C8FF;
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
   
    Player *player = &game_state->player;
    player->w = 50;
    player->h = 50;
    player->x = ((surface->w - player->w) / 2);
    player->y = (surface->h - player->h);
    player->x_velocity = 0;
    player->y_velocity = 0;
    player->max_x_velocity = 12;
    player->max_y_velocity = 12;
    player->m = 12;

    game_state->is_initialised = true;
  }

  Player *player = &game_state->player;
  S32 x_speed = 2;
  S32 jump_speed = 12;


  if (game_input->move_up.ended_down)
  {
    if (player->y == (surface->h - player->h))
    {
      player->y_velocity -= jump_speed;
    }
  }
  if (game_input->move_right.ended_down)
  {
    player->x_velocity += x_speed;
  }
  if (game_input->move_left.ended_down)
  {
    player->x_velocity -= x_speed;
  }

  S32 x_friction = (player->x_velocity == 0) ? 0 : 
                   (player->x_velocity < 0) ? -1 : 1;

  player->x_velocity = Clamp(-player->max_x_velocity, 
                             player->x_velocity - x_friction, 
                             player->max_x_velocity);

  S32 gravity = player->m * (9.8 / 100);
  player->y_velocity -= -gravity;

  player->x = Clamp(0, player->x + player->x_velocity, surface->w - player->w);
  player->y = Clamp(0, player->y + player->y_velocity, surface->h - player->h);
  if (player->y == (surface->h - player->h)) 
  {
    player->y_velocity = 0; 
  }

  render_background(surface);
  draw_box(surface, player->x, player->y, player->w, player->h, 0x00000000);
}

