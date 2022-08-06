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

inline internal B32 key_down(GameButtonState button);
inline internal B32 key_down_single(GameButtonState button);
inline internal B32 key_up_single(GameButtonState button);

///////////////////////////////////////////////////////////
//// NOTE(Elias): Drawing 

internal void render_background(SDL_Surface *surface); 
internal void draw_box(SDL_Surface *surface, V2F32 cam_pos, V2F32 pos, F32 w, F32 h, S32 c);

///////////////////////////////////////////////////////////
//// NOTE(Elias): Physics Functions 

internal F32   collision_1d(V2F32 p1, V2F32 p2);
internal V2F32 collision_2d(V2F32 p1, F32 w1, F32 h1, V2F32 p2, F32 w2, F32 h2);

///////////////////////////////////////////////////////////
//// NOTE(Elias): Level 

struct Platform
{
  F32 w, h;
  V2F32 pos;
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
  // NOTE(Elias): Physics
  F32 w, h;
  V2F32 pos;
  V2F32 v;
  V2F32 v_max;
  V2F32 p_mov;
  F32 m;
  B32 is_grounded;

  // NOTE(Elias): Abilities 
  S32 jumps_max;
  S32 jumps_left;
  
  // NOTE(Elias): Graphics 
  SDL_Surface* sprite;
  V2F32 tailpos[10];
};

internal void player_initialise(Player *player, S32 window_w, S32 window_h); 
internal void player_update(Player *player, Environment *env, Platform *platform, 
                            S32 screen_width, S32 screen_height);
internal void player_render(SDL_Surface *surface, V2F32 camera, Player *player);
internal void player_die(Player *player);

///////////////////////////////////////////////////////////
//// NOTE(Elias): Game 

// IMPORTANT(Elias): 
// Must be initialised to zero when assigning memory!
struct GameState
{
  V2F32 camera;

  Player player; 
  
  Environment env;
  Platform platform;
  
  Font font;

};

internal void game_initialise(GameState *game_state, SDL_Surface *surface);
internal void game_update(GameState *game_state, GameInput *game_input, 
                          SDL_Surface *surface, U64 counter);
internal void game_render(SDL_Surface *surface, GameState *game_state);
internal void game_die(GameState *game_state); 




