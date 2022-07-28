// NOTE(Elias): Drawing functions

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

// NOTE(Elias): Initialising functions

internal void
env_initiliase(Environment *env)
{
  env->friction_ground = 0.7f;
  env->friction_sky = 0.25f; 
  env->gravity_const = 9.81f;
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

  player->max_x_velocity = 18;
  player->max_y_velocity = 100;
  player->s_x            = 3;
  player->s_y            = 24;
  player->m              = 0.002f;

  //NOTE(lupy) counting doublejump
  player->jumpcount      = 0;
}

internal void
game_initialise(GameState *game_state, SDL_Surface *surface)
{
  env_initiliase(&game_state->env);
  player_initialise(&game_state->player, surface->w, surface->h);
} 

// NOTE(Elias): Update functions

internal void 
player_update(Player *player, Environment *env, S32 screen_width, S32 screen_height)
{
  // NOTE(Elias): Player Physiscs
  F32 x_friction = (player->is_grounded) ? 
    ((player->x_velocity < 0) ? -env->friction_ground : env->friction_ground) :
    ((player->x_velocity < 0) ? -env->friction_sky : env->friction_sky);

  player->x_velocity = (player->x_velocity >= 0) ? 
    ClampBot(0, player->x_velocity - x_friction) : 
    ClampTop(player->x_velocity - x_friction, 0);
  player->x_velocity = Clamp(-player->max_x_velocity, player->x_velocity, player->max_x_velocity);

  F32 gravity = player->m * env->gravity_const * (screen_height - player->y);
  player->y_velocity += gravity;

  // NOTE(Elias): For now, clamp the x position to the screen border.
  // TODO(Elias): Remove clamp when collision and correct rendering for offscreen 
  // entities is implemented.
  player->x = Clamp(0, player->x + player->x_velocity, screen_width - player->w);
  player->y = ClampTop(player->y + player->y_velocity, screen_height - player->h);
  
  // NOTE(Elias): Check if the player is grounded after move. 
  // Cancel y velocity if he is.
  // TODO(Elias): Make this work with a collision check instead of the bottom
  // of the screen.
  player->is_grounded = false;
  if (player->y == (screen_height - player->h)) 
  {
    player->y_velocity = 0;
    player->is_grounded = true;
    player->jumpcount = 0;
  } 
}

internal void
game_update_and_render(GameState *game_state, GameInput *game_input, 
                       SDL_Surface *surface, S64 counter)
{
  Player *player = &game_state->player;
  Environment *env = &game_state->env;

  // NOTE(Elias): Handle input
  if (game_input->move_up.ended_down)
  {
    if (player->is_grounded)
    {
      player->y_velocity -= player->s_y;
      player->jumpcount++;
    }
    else if (!player->is_grounded && player->jumpcount == 1 && player->y_velocity>0)
    {
      player->y_velocity = 0;
      player->y_velocity -= player->s_y;
      player->jumpcount++;
    }
    
    
    
  }
  if (game_input->move_right.ended_down)
  {
    if (player->is_grounded)
    {
      player->x_velocity += player->s_x;
    } 
  }
  if (game_input->move_left.ended_down)
  {
    if (player->is_grounded)
    {
      player->x_velocity -= player->s_x;
    }
  }
  F32 gravity_const_change = 0.1f;
  if (game_input->mouse_left.ended_down) {
    env->gravity_const += gravity_const_change;
  }
  if (game_input->mouse_right.ended_down) {
    env->gravity_const -= gravity_const_change;
  }

  // NOTE(Elias): Update state of the game
  player_update(player, env, surface->w, surface->h);

  // NOTE(Elias): Render the scene to the buffer
  render_background(surface);
  S32 player_disp_h = ClampBot(0, (player->y < 0) ? (player->h - (0-player->y)) : player->h);
  S32 player_disp_y = ClampBot(0, player->y);
  draw_box(surface, player->x, player_disp_y, player->w, player_disp_h, 0x0);

  
  
  // NOTE(Elias): Log some stuff
#if 1
  S32 frames_between_log = 3;
  if ((counter % frames_between_log) == 0)
  {
    // NOTE(Elias): Usefull website for ANSII Escape Codes
    // https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
    printf("Player:   x = %6d   y = %6d   xv = %6.2f   yv = %6.2f jumpcount = %6d\n"
           "Env:      g = %6.2f \n"
           "\033[2A",
           player->x, player->y, player->x_velocity, player->y_velocity, player->jumpcount, env->gravity_const);
  }
#endif

}
