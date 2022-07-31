///////////////////////////////////////////////////////////
//// NOTE(Elias): Keyboard

inline internal B32 
key_down(GameButtonState button)
{
  return(button.ended_down);
}

inline internal B32 
key_down_single(GameButtonState button)
{
  return(button.ended_down && !button.was_down);
}

inline internal B32 
key_up_single(GameButtonState button)
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
draw_box(SDL_Surface *surface, V2F32 pos, F32 w, F32 h, S32 c)
{
  S32 bytes_per_pixel = surface->format->BytesPerPixel;
  for (S32 row = ClampBot(0, pos.y);
       (row < pos.y + h) && (row < surface->h);
       ++row)
  {
    for (S32 column = ClampBot(0, pos.x);
         (column < pos.x + w) && (column < surface->w);
         ++column)
    {
      U8 *p = (U8 *)surface->pixels + (row * surface->pitch) + (column * bytes_per_pixel);
      *(U32 *)p = c;
    }
  }
}

///////////////////////////////////////////////////////////
//// NOTE(Elias): Physics Functions 

internal F32   
collision_1d(F32 l1, F32 r1, F32 l2, F32 r2)
{
  F32 result = (l1 <= l2) ? -ClampBot(0, r1 - l2) : 
               (l2 <= l1) ? ClampBot(0, r2 - l1) : 0; 
  return(result);
}

internal V2F32 
collision_2d(V2F32 p1, F32 w1, F32 h1, V2F32 p2, F32 w2, F32 h2)
{
  V2F32 result;
  result.x = collision_1d(p1.x, p1.x + w1, p2.x, p2.x + w2);
  result.y = collision_1d(p1.y, p1.y + h1, p2.y, p2.y + h2);
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
  // NOTE(Elias): Physics
  player->w = 50.0f; 
  player->h = 50.0f; 
  player->v_max = v2f32(12.0f, 100.0f);
  player->p_mov = v2f32(3.0f, 30.0f);
  player->m = 0.002f; 
  player->pos = v2f32((window_w - player->w) / 2, window_h - player->h);
  player->v = v2f32(0,0); 

  // NOTE(Elias): Abilities 
  player->jumps_max = 2;
  player->jumps_left = player->jumps_max;
  
  // NOTE(Elias): Graphics 
  for (S32 i = 0;
       i < (S32)ArrayCount(player->tailpos);
       ++i)
  {
    player->tailpos[i] = player->pos + v2f32(0, (F32)i * 5.0f);
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
  // NOTE(Elias) Update Physics 
 
  // NOTE(Elias) Adjust x-velocity with friction
  F32 x_friction = (player->is_grounded) ? ((player->v.x < 0) ? -env->friction_ground : env->friction_ground) : 
                                           ((player->v.x < 0) ? -env->friction_sky : env->friction_sky); 
  player->v.x = (player->v.x >= 0) ? ClampBot(0, player->v.x - x_friction) : 
                                     ClampTop(player->v.x - x_friction, 0);
  player->v.x = Clamp(-player->v_max.x, player->v.x, player->v_max.x);

  // NOTE(Elias) Adjust y-velocity with gravity 
  player->v.y += player->m * env->gravity_const * (screen_height - player->pos.y);

  // NOTE(Elias): Adjust player position according to velocity
  V2F32 pos_new = player->pos + player->v;
  
  // NOTE(Elias): collision checking 
  V2F32 col_old = collision_2d(
      player->pos, player->w, player->h, 
      platform->pos, platform->w, platform->h); 
  V2F32 col_new = collision_2d(
      pos_new, player->w, player->h, 
      platform->pos, platform->w, platform->h); 
  
  if ((!col_old.x && col_new.x) && col_new.y)
  {
    pos_new.x += col_new.x;
    player->v.x = 0;
  }
  if ((!col_old.y && col_new.y) && col_new.x)
  {
    pos_new.y += col_new.y;
    if (player->v.y >= 0)
    {
      player->is_grounded = true;
    }
    player->v.y = 0;
  } 
  // NOTE(Elias): clamp to screen border down
  // TODO(Elias): should be removed when platforms are done
  pos_new.y = ClampTop(pos_new.y, screen_height - player->h);
  
  player->pos = pos_new;
 
  // TODO(Elias): should be removed when platforms are done
  if (player->pos.y == (screen_height - player->h))
  {
    player->is_grounded = true;
    player->v.y = 0;
  }

  // NOTE(Elias) Update Abilities 
  if (player->is_grounded)
  {
    player->jumps_left = player->jumps_max; 
  }
  
  // NOTE(Elias) Update Graphics
  for (S32 i = (S32)ArrayCount(player->tailpos) - 1;
       i > 0;
       --i)
  {
    player->tailpos[i] = player->tailpos[i-1] + v2f32(0.0f, 5.0f);
  }
  player->tailpos[0] = player->pos;
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
      draw_box(surface, player->tailpos[i] + v2f32((F32)j, 0.0f), 1, player->h - i*5,
          ((255  - ((i * 50 + j) / 5)) << 16) +
          ((128 + ((i * 50 + j) / 5)) << 8) +
          ((i * 50 + j) / 3));
    }
  } 
  draw_box(surface, player->pos, player->w, player->h, 0x0);
  
  // NOTE(Elias): Render player
  SDL_Rect dst_rect = {(S32)player->pos.x, (S32)player->pos.y, (S32)player->w, (S32)player->h};
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
  // TODO(Elias): Conversion to screen coordinates should happen 
  platform->pos = v2f32(210.0f, surface->h - 120.0f);
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
  if (key_down(game_input->move_up))
  {
    if (player->is_grounded)
    {
      player->v.y -= player->p_mov.y;
      player->is_grounded = false;
      --player->jumps_left;
    } 
    else if (!player->is_grounded && 
             (player->jumps_left > 0) && 
             (player->v.y > 0))
    {
      player->v.y = -player->p_mov.y;
      --player->jumps_left;
    } 
  }
  if (key_down(game_input->move_right))
  {
    if (player->is_grounded)
    {
      player->v.x += player->p_mov.x;
    } 
  }
  if (key_down(game_input->move_left))
  {
    if (player->is_grounded)
    {
      player->v.x -= player->p_mov.x;
    }
  }
  F32 gravity_const_change = 0.1f;
  if (key_down(game_input->mouse_left)) {
    env->gravity_const += gravity_const_change;
  }
  if (key_down(game_input->mouse_right)) {
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
           player->x, player->y, player->x_velocity, player->y_velocity, player->jumps_left, 
           env->gravity_const);
  }
#endif

}

internal void
game_render(SDL_Surface *surface, GameState *game_state)
{
  render_background(surface); 

  Platform *platform = &game_state->platform;
  draw_box(surface, platform->pos, platform->w, platform->h, 0x555555); 
  
  Player *player = &game_state->player;
  player_render(surface, player);
}

internal void
game_die(GameState *game_state)
{
  player_die(&game_state->player);
} 
