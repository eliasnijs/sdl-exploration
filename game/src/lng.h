///////////////////////////////////////////////////////////
//// NOTE(Elias): Keyboard

struct GameButtonState 
{
  B32 was_down; 
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

inline internal B32 input_down(GameButtonState button);
inline internal B32 input_down_single(GameButtonState button);
inline internal B32 input_up_single(GameButtonState button);

///////////////////////////////////////////////////////////
//// NOTE(Elias): Drawing 

internal void render_background(SDL_Surface *surface); 
internal void draw_box(SDL_Surface *surface, S32 x, S32 y, S32 width, S32 height, S32 color);

///////////////////////////////////////////////////////////
//// NOTE(Elias): Physics Functions 
internal B32   collision_1d(S32 l1, S32 r1, S32 l2, S32 r2);
internal V2S32 collision_2d(S32 x1, S32 y1, S32 w1, S32 h1, S32 x2, S32 y2, S32 w2, S32 h2);

///////////////////////////////////////////////////////////
//// NOTE(Elias): Level 

struct Platform
{
  S32 x, y;
  S32 w, h;
};

struct Environment
{
  F32 friction_ground;
  F32 friction_sky;
  F32 gravity_const;
}; 

///////////////////////////////////////////////////////////
//// NOTE(Elias): Player 

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
  V2S32 tailpos[10];
  SDL_Surface* sprite;
};

internal void player_initialise(Player *player, S32 window_w, S32 window_h); 
internal void player_update(Player *player, Environment *env, Platform *platform, 
                            S32 screen_width, S32 screen_height);
internal void player_render(SDL_Surface *surface, Player *player);
internal void player_die(Player *player);

///////////////////////////////////////////////////////////
//// NOTE(Elias): Game 

struct GameState
{
  Environment env;
  Player player; 
  Platform platform;
};

internal void game_initialise(GameState *game_state, SDL_Surface *surface);
internal void game_update(GameState *game_state, GameInput *game_input, 
                          SDL_Surface *surface, S64 counter);
internal void game_render(SDL_Surface *surface, GameState *game_state);
internal void game_die(GameState *game_state); 

