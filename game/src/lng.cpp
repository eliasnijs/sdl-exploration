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
      *(U32 *)p = 0xFFFFFF;
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
draw_line(SDL_Surface *surface, S32 x1, S32 y1, S32 x2, S32 y2, S32 color)
{
  S32 bytes_per_pixel = surface->format->BytesPerPixel;
  S32 x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
  
  dx = x2 - x1;
  dy = y2 - y1;
  dx1 = Abs(dx);
  dy1 = Abs(dy);
  px = (2 * dy1) - dx1;
  py = (2 * dx1) - dy1;
  
  if (dy1 <= dx1) 
  {
    if (dx >= 0) 
    {
      x = x1; 
      y = y1; 
      xe = x2;
    } 
    else 
    {
      x = x2; 
      y = y2; 
      xe = x1;
    }
    U8 *p = (U8 *)surface->pixels + (y * surface->pitch) + (x * bytes_per_pixel);
    *(U32 *)p = color;
    for (i = 0; x < xe; i++) 
    {
      x = x + 1;
      if (px < 0) 
      {
        px = px + 2 * dy1;
      } 
      else 
      {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) 
        {
          y = y + 1;
        } 
        else 
        {
          y = y - 1;
        }
        px = px + 2 * (dy1 - dx1);
      }
      U8 *p = (U8 *)surface->pixels + (y * surface->pitch) + (x * bytes_per_pixel);
      *(U32 *)p = color;
    }
  } 
  else 
  { 
    if (dy >= 0) 
    {
      x = x1; 
      y = y1; 
      ye = y2;
    } 
    else 
    {
      x = x2; 
      y = y2; 
      ye = y1;
    }
    U8 *p = (U8 *)surface->pixels + (y * surface->pitch) + (x * bytes_per_pixel);
    *(U32 *)p = color;
    for (i = 0; y < ye; i++) {
      y = y + 1;
      if (py <= 0) {
        py = py + 2 * dx1;
      } 
      else 
      {
        if ((dx < 0 && dy<0) || (dx > 0 && dy > 0)) 
        {
          x = x + 1;
        } 
        else 
        {
          x = x - 1;
        }
        py = py + 2 * (dx1 - dy1);
      }
      U8 *p = (U8 *)surface->pixels + (y * surface->pitch) + (x * bytes_per_pixel);
      *(U32 *)p = color;
    }
  }
}


internal void
draw_sprite(SDL_Surface *surface, S32 x, S32 y, S32 width, S32 height, Sprite *sprite)
{
  Assert(x > 0);
  Assert(y > 0);
  Assert(x + sprite->width < surface->w);
  Assert(y + sprite->height < surface->h);
  S32 bytes_per_pixel = surface->format->BytesPerPixel;
  for (S32 sprite_row = 0;
       sprite_row < sprite->height;
       ++sprite_row)
  {
    for (S32 sprite_column = 0;
         sprite_column < sprite->width;
         ++sprite_column)
    {
      S32 buffer_row = x + sprite_row;
      S32 buffer_column = y + sprite_column;
      U8 *buffer_pixel = (U8 *)surface->pixels + 
                         (buffer_row * surface->pitch) + (buffer_column * bytes_per_pixel);
      U8 *sprite_pixel = (U8 *)sprite->pixels + 
                         (sprite_row * surface->w * bytes_per_pixel) + 
                         (sprite_column * bytes_per_pixel);
      *(U32 *)buffer_pixel = *(U32 *)sprite_pixel;
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
  S32 x_speed = 4;
  S32 jump_speed = 21;
  S32 bullet_velocity = 9;

  F32 x_friction = (player->x_velocity < 0) ? -0.25 : 0.25;
  // NOTE(Elias): is_grounded check
  if (player->y == (surface->h - player->h))
  {
    if (game_input->move_up.ended_down)
    {
      player->y_velocity -= jump_speed;
    }
    if (game_input->move_right.ended_down)
    {
      player->x_velocity += x_speed;
    }
    if (game_input->move_left.ended_down)
    {
      player->x_velocity -= x_speed;
    }
    x_friction = (player->x_velocity == 0) ? 0 : 
                 (player->x_velocity < 0) ? -3 : 3;
  }

  player->x_velocity = (player->x_velocity >= 0) ? ClampBot(0, player->x_velocity - x_friction) : 
                                                   ClampTop(player->x_velocity - x_friction, 0);
  
  player->x_velocity = Clamp(-player->max_x_velocity, 
                           player->x_velocity, 
                           player->max_x_velocity);

  F32 gravity = player->m * 9.8f / 100.0f;
  player->y_velocity -= -gravity;

  player->x = Clamp(0, player->x + player->x_velocity, surface->w - player->w);
  player->y = Clamp(0, player->y + player->y_velocity, surface->h - player->h);
  // NOTE(Elias): is_grounded check
  if (player->y == (surface->h - player->h)) 
  {
    player->y_velocity = 0; 
  }
  // printf("x-vel: %4.2f\ty-vel: %4.2f\n", 
  //        player->x_velocity, player->y_velocity);

  render_background(surface);
  draw_box(surface, player->x, player->y, player->w, player->h, 0x00000000);
    
  if (game_input->mouse_left.ended_down)
  {
    B32 bullet_found = false;
    for (S32 i = 0;
         i < (S32) ArrayCount(game_state->bullets) && !bullet_found;
         ++i)
    {
      Bullet *bullet = &game_state->bullets[i];
      if (!bullet->is_active)
      {
        // sqrt(x^2 + y^2) = bullet_velocity;
        bullet->is_active = true;
        bullet->x_velocity = 1.0f;
        bullet->x = player->x + (player->w / 2);
        bullet->y = player->y + (player->h / 2);
        ++game_state->bullet_counter;
        bullet_found = true;
      }
    }
  }
  
  for (S32 i = 0;
       i < (S32) ArrayCount(game_state->bullets);
       ++i)
  {
    Bullet *bullet = &game_state->bullets[i];
    if (bullet->is_active)
    {
      bullet->x = Clamp(0, bullet->x + bullet->x_velocity, surface->w - 20);
      bullet->y = Clamp(0, bullet->y + bullet->y_velocity, surface->h - 20);
      draw_box(surface, bullet->x, bullet->y, 20, 20, 0x00FF0000);
    }
  }

  // draw_line(surface, (player->x + (player->w / 2)), (player->y + (player->h / 2)), 
  //           game_input->mouse_x, game_input->mouse_y, 0x00FF0000);
}

