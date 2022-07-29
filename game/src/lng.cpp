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

  player->max_x_velocity = 9;
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
  player_initialise(&game_state->player, surface->w, surface->h);
  
  Environment *env = &game_state->env;
  env->friction_ground = 1.8f;
  env->friction_sky = 0.25f; 
  env->gravity_const = 9.81f;
  
  Platform *platform = &game_state->platform;
  platform->x = 210;
  platform->y = surface->h - 120;
  platform->w = 120;
  platform->h = 50; 
} 

internal B32 
collision_1d(S32 l1, S32 r1, S32 l2, S32 r2) {
  S32 result = (l1 <= l2) ? -ClampBot(0, r1 - l2) : 
               (l2 <= l1) ? ClampBot(0, r2 - l1) : 0; 
  return(result);
}

internal S32V2 
collision_2d(S32 x1, S32 y1, S32 w1, S32 h1, 
             S32 x2, S32 y2, S32 w2, S32 h2) {
  S32V2 result;
  result.x = collision_1d(x1, x1 + w1, x2, x2 + w2);
  result.y = collision_1d(y1, y1 + h1, y2, y2 + h2);
  return(result);
}

// NOTE(Elias): Update functions

internal void 
player_update(Player *player, Environment *env, Platform *platform,
              S32 screen_width, S32 screen_height)
{
  // NOTE(Elias): Player Physiscs
 
  // NOTE(Elias) x-velocity
  F32 x_friction = (player->is_grounded) ? ((player->x_velocity < 0) ? -env->friction_ground : env->friction_ground) : 
                                           ((player->x_velocity < 0) ? -env->friction_sky : env->friction_sky); 
  player->x_velocity = (player->x_velocity >= 0) ? ClampBot(0, player->x_velocity - x_friction) : 
                                                   ClampTop(player->x_velocity - x_friction, 0);
  player->x_velocity = Clamp(-player->max_x_velocity, player->x_velocity, player->max_x_velocity);

  // NOTE(Elias) y-velocity
  F32 gravity = player->m * env->gravity_const * (screen_height - player->y);
  player->y_velocity += gravity;

  // NOTE(Elias): adjust player position according to velocity
  S32 new_x = player->x + player->x_velocity;
  S32 new_y = player->y + player->y_velocity;
  
  // NOTE(Elias): collision checking 
  S32V2 col_old = collision_2d(
      player->x, player->y, player->w, player->h, 
      platform->x, platform->y, platform->w, platform->h); 
  S32V2 col_new = collision_2d(
      new_x, new_y, player->w, player->h, 
      platform->x, platform->y, platform->w, platform->h); 
  
  // printf("col_old: %3d x %3d, col_new: %3d x %3d\n", 
  //       col_old.x, col_old.y, col_new.x, col_new.y);
 
  if ((!col_old.x && col_new.x) && col_new.y)
  {
    // NOTE(Elias): x-collision detected
    new_x = new_x + col_new.x;
    player->x_velocity = 0;
  }
  if ((!col_old.y && col_new.y) && col_new.x)
  {
    new_y = new_y + col_new.y;
    if (player->y_velocity >= 0)
    {
      // printf("%d -> %d\n", player->y, new_y);
      player->is_grounded = true;
    }
    player->y_velocity = 0;
  } 
  
  // NOTE(Elias) clamp to screen border
  player->x = Clamp(0, new_x, screen_width - player->w);
  player->y = ClampTop(new_y, screen_height - player->h);
 
  if (player->y == (screen_height - player->h))
  {
    player->is_grounded = true;
    player->y_velocity = 0;
  }

  if (player->is_grounded)
  {
    player->jumpcount = 0; 
  }
  
}

internal void
game_update_and_render(GameState *game_state, GameInput *game_input, 
                       SDL_Surface *surface, S64 counter)
{
  Player *player = &game_state->player;
  Environment *env = &game_state->env;
  Platform *platform = &game_state->platform;

  // NOTE(Elias): Handle input
  if (game_input->move_up.ended_down)
  {
    if (player->is_grounded)
    {
      player->y_velocity -= player->s_y;
      player->is_grounded = false;
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
  player_update(player, env, platform, surface->w, surface->h);


  // NOTE(Elias): Log some stuff
#if 0
  S32 frames_between_log = 3;
  if ((counter % frames_between_log) == 0)
  {
    // NOTE(Elias): Usefull website for ANSII Escape Codes
    // https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
    printf("Player:   x = %6d   y = %6d   xv = %6.2f   yv = %6.2f   j_l = %6d\n"
           "Env:      g = %6.2f \n"
           "\033[2A",
           player->x, player->y, player->x_velocity, player->y_velocity, player->jumpcount, 
           env->gravity_const);
  }
#endif
  
  // NOTE(Elias): Render the scene to the buffer
  render_background(surface);
  S32 player_disp_h = ClampBot(0, (player->y < 0) ? (player->h - (0-player->y)) : player->h);
  S32 player_disp_y = ClampBot(0, player->y);
  draw_box(surface, player->x, player_disp_y, player->w, player_disp_h, 0x0);
  draw_box(surface, platform->x, platform->y, platform->w, platform->h, 0x555555);

}
