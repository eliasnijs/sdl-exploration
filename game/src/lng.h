struct GameButtonState 
{
  B32 ended_down; 
};

struct GameInput 
{
  union  
  {
    GameButtonState game_buttons[14];
    struct 
    {
      GameButtonState move_up; 
      GameButtonState move_left; 
      GameButtonState move_right; 
      GameButtonState move_down; 
      GameButtonState mouse_left; 
      GameButtonState mouse_right; 
      GameButtonState action1;
      GameButtonState action2;
      GameButtonState action3;
      GameButtonState action4;
      GameButtonState action5;
      GameButtonState action6;
      GameButtonState action7;
      GameButtonState action8;
    };
  };
  union 
  {
    S32 mouse_coordinates[2];
    struct 
    {
      S32 mouse_x;
      S32 mouse_y;
    };
  };
};

struct Player 
{
  S32 x;
  S32 y;
  S32 w;
  S32 h;
  F32 x_velocity;
  F32 y_velocity;
  F32 max_x_velocity;
  F32 max_y_velocity;
  S32 m;
};

struct Bullet
{
  B32 is_active;
  S32 x;
  S32 y;
  F32 x_velocity;
  F32 y_velocity;
};

struct Sprite
{
  S32 width;
  S32 height;
  S32 *pixels;
};

struct GameState
{
  B32 is_initialised;
  Player player;

  S32 bullet_counter;
  Bullet bullets[120];
};

internal void
render_pattern(SDL_Surface *surface, S32 offset1, S32 offset2, S32 offset3);

void
game_update_and_render(GameState *game_state, GameInput *game_input, SDL_Surface *surface);
