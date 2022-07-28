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
  S32 w, h;
  S32 x, y;
  F32 x_velocity, y_velocity;
  F32 max_x_velocity, max_y_velocity;
  S32 s_x, s_y;
  F32 m;
  B32 is_grounded;
  S32 jumpcount;
};

struct Environment
{
  F32 friction_ground;
  F32 friction_sky;
  F32 gravity_const;
};

struct GameState
{
  Environment env;
  Player player; 
};

// NOTE(Elias): Drawing functions
internal void render_background(SDL_Surface *surface); 
internal void draw_box(SDL_Surface *surface, S32 x, S32 y, S32 width, S32 height, S32 color);

// NOTE(Elias): Initialising functions
internal void game_initialise(GameState *game_state, SDL_Surface *surface);
internal void env_initiliase(Environment *env);
internal void player_initialise(Player *player, S32 window_w, S32 window_h); 

// NOTE(Elias): Update functions
internal void game_update_and_render(GameState *game_state, GameInput *game_input, 
                                     SDL_Surface *surface, S64 counter);
internal void player_update(Player *player, Environment *env, S32 screen_width, S32 screen_height);



