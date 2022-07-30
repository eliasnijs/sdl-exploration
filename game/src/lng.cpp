///////////////////////////////////////////////////////////
//// NOTE(Elias): Keyboard

inline internal B32 
input_down(GameButtonState button)
{
  return(button.ended_down);
}

inline internal B32 
input_down_single(GameButtonState button)
{
  return(button.ended_down && !button.was_down);
}

inline internal B32 
input_up_single(GameButtonState button)
{
  return(!button.ended_down && button.was_down);
} 

///////////////////////////////////////////////////////////
//// NOTE(Elias): Drawing Functions

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
  S32 bytes_per_pixel = surface->format->BytesPerPixel;
  for (S32 row = ClampBot(0, y);
       (row < y + height) && (row < surface->h);
       ++row)
  {
    for (S32 column = ClampBot(0, x);
         (column < x + width) && (column < surface->w);
         ++column)
    {
      U8 *p = (U8 *)surface->pixels + (row * surface->pitch) + (column * bytes_per_pixel);
      *(U32 *)p = color;
    }
  }
}

///////////////////////////////////////////////////////////
//// NOTE(Elias): Physics Functions 

internal B32 
collision_1d(S32 l1, S32 r1, S32 l2, S32 r2) {
  S32 result = (l1 <= l2) ? -ClampBot(0, r1 - l2) : 
               (l2 <= l1) ? ClampBot(0, r2 - l1) : 0; 
  return(result);
}

internal V2S32 
collision_2d(S32 x1, S32 y1, S32 w1, S32 h1, 
             S32 x2, S32 y2, S32 w2, S32 h2) {
  V2S32 result;
  result.x = collision_1d(x1, x1 + w1, x2, x2 + w2);
  result.y = collision_1d(y1, y1 + h1, y2, y2 + h2);
  return(result);
}

///////////////////////////////////////////////////////////
//// NOTE(Elias): Level

// NOTE(Elias): No functions yet

///////////////////////////////////////////////////////////
//// NOTE(Elias): Player 

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
  player->s_x            = 3;
  player->s_y            = 30;
  player->m              = 0.002f;

  // NOTE(Lupy): counting doublejump
  player->jumpcount      = 0;
  
  for (S32 i = 0;
       i < (S32)ArrayCount(player->tailpos);
       ++i)
  {
    player->tailpos[i].x = player->x;
    player->tailpos[i].y = player->y + i*5;
  }

  player->sprite = SDL_LoadBMP("resources/images/player256x.bmp");
  if (!player->sprite)
  {
    printf("Something went wrong loading player sprite.\n");
  } 
}

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
  V2S32 col_old = collision_2d(
      player->x, player->y, player->w, player->h, 
      platform->x, platform->y, platform->w, platform->h); 
  V2S32 col_new = collision_2d(
      new_x, new_y, player->w, player->h, 
      platform->x, platform->y, platform->w, platform->h); 
  
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
      player->is_grounded = true;
    }
    player->y_velocity = 0;
  } 
  
  player->x = new_x;
  // NOTE(Elias): clamp to screen border down
  // TODO(Elias): should be removed when platforms are done
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
 
  for (S32 i = (S32)ArrayCount(player->tailpos) - 1;
       i > 0;
       --i)
  {
    player->tailpos[i] = player->tailpos[i-1];
    player->tailpos[i].y += 5;
  }
  player->tailpos[0] = {player->x, player->y};
}

internal void
player_render(SDL_Surface *surface, Player *player)
{
  // NOTE(Elias): Render tail
  for (S32 i = (S32)ArrayCount(player->tailpos)-1;
       i >= 0;
       --i)
  {
    for (S32 j = 0;
         j < 50;
         ++j)
    {
      draw_box(surface, player->tailpos[i].x + j, player->tailpos[i].y, 1, player->h-i*5,
          ((255  - ((i * 50 + j) / 5)) << 16) +
          ((128 + ((i * 50 + j) / 5)) << 8) +
          ((i * 50 + j) / 3));
    }
  } 
  draw_box(surface, player->x, player->y, player->w, player->h, 0x0);
  
  // NOTE(Elias): Render player
  SDL_Rect dst_rect = {player->x, player->y, player->w, player->h};
  SDL_BlitScaled(player->sprite, 0, surface, &dst_rect); 
}

internal void
player_die(Player *player)
{
  SDL_FreeSurface(player->sprite);
}

///////////////////////////////////////////////////////////
//// NOTE(Elias): Game

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
  // TODO(Elias): Conversion to screen coordinates should happen 
  // in the render function.
  platform->y = surface->h - 120;
  platform->w = 120;
  platform->h = 50; 
} 

internal void
game_update(GameState *game_state, GameInput *game_input, 
            SDL_Surface *surface, S64 counter)
{
  Player *player = &game_state->player;
  Environment *env = &game_state->env;
  Platform *platform = &game_state->platform;

  // NOTE(Elias): Handle input
  if (input_down(game_input->move_up))
  {
    if (player->is_grounded)
    {
      player->y_velocity -= player->s_y;
      player->is_grounded = false;
      player->jumpcount++;
    } else if (!player->is_grounded && player->jumpcount == 1 && player->y_velocity>0)
    {
      player->y_velocity = 0;
      player->y_velocity -= player->s_y;
      player->jumpcount++;
    } 
  }
  if (input_down(game_input->move_right))
  {
    if (player->is_grounded)
    {
      player->x_velocity += player->s_x;
    } 
  }
  if (input_down(game_input->move_left))
  {
    if (player->is_grounded)
    {
      player->x_velocity -= player->s_x;
    }
  }
  F32 gravity_const_change = 0.1f;
  if (input_down(game_input->mouse_left)) {
    env->gravity_const += gravity_const_change;
  }
  if (input_down(game_input->mouse_right)) {
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

}

internal void
game_render(SDL_Surface *surface, GameState *game_state)
{
  render_background(surface); 

  Platform *platform = &game_state->platform;
  draw_box(surface, platform->x, platform->y, platform->w, platform->h, 0x555555); 
  
  Player *player = &game_state->player;
  player_render(surface, player);
}

internal void
game_die(GameState *game_state)
{
  player_die(&game_state->player);
} 
