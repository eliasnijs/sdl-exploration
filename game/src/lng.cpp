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
      *(U32 *)p = 0xDDDDDD;
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

internal void
player_initialise(Player *player, S32 window_w, S32 window_h)
{
  player->w              = 50;
  player->h              = 50;

  //NOTE(Elias): Set the start position
  player->x              = (window_w - player->w) / 2;
  player->y              = window_h - player->h;

  //NOTE(Elias): Set the start velocity 
  player->x_velocity     = 0;
  player->y_velocity     = 0;

  player->max_x_velocity = 12;
  player->max_y_velocity = 100;
  player->s_x            = 6;
  player->s_y            = 64;
  player->m              = 2;
}

void
game_update_and_render(GameState *game_state, GameInput *game_input, SDL_Surface *surface)
{
  if (!game_state->is_initialised)
  {
    // TODO(Elias): initialisation might be better in the sdl layer or as a seperate function, 
    // take a look at it
    // TODO(Elias): ... init game
    player_initialise(&game_state->player, surface->w, surface->h);
    game_state->is_initialised = true;
  } Player *player = &game_state->player;

  // NOTE(Elias): Handle movement if the player is grounded 
  if (player->is_grounded)
  {
    if (game_input->move_up.ended_down)
    {
      player->y_velocity -= player->s_y;
    }
    if (game_input->move_right.ended_down)
    {
      player->x_velocity += player->s_x;
    }
    if (game_input->move_left.ended_down)
    {
      player->x_velocity -= player->s_x;
    }
  } 

  // NOTE(Elias): Update velocities
  // NOTE(Elias): Add friction for more natural movement.
  // There are 2 cases: ground friction and sky friction 
  F32 ground_friction = 0.1f, sky_friction = 0.25f;

  F32 x_friction = (player->is_grounded) ? ((player->x_velocity < 0) ? -ground_friction : ground_friction) :
                                           ((player->x_velocity < 0) ? -sky_friction : sky_friction);

  player->x_velocity = (player->x_velocity >= 0) ? ClampBot(0, player->x_velocity - x_friction) : 
                                                   ClampTop(player->x_velocity - x_friction, 0);
  player->x_velocity = Clamp(-player->max_x_velocity, player->x_velocity, player->max_x_velocity);

  // NOTE(Elias): Apply gravity
  F32 gravity_weight = 0.1f, height_weight = 0.01f;
  player->y_velocity += player->m * (9.8f * gravity_weight) * ((surface->h - player->y) * height_weight);

  // NOTE(Elias): Adjust player position according to the player's velocities
  // and clamp the position to the screen border.
  player->x = Clamp(0, player->x + player->x_velocity, surface->w - player->w);
  player->y = Clamp(0, player->y + player->y_velocity, surface->h - player->h);
  
  // NOTE(Elias): Check if the player is grounded after move. 
  // Cancel y velocity if it is.
  if (player->y == (surface->h - player->h)) 
  {
    player->y_velocity = 0;
    player->is_grounded = true; 
  } 
  else
  {
    player->is_grounded = false;
  }

  // NOTE(Elias): Render the scene to the buffer
  render_background(surface);
  draw_box(surface, player->x, player->y, player->w, player->h, 0x0);
}
