#ifndef _GAME_H_
#define _GAME_H_

#define WINDOW_TITLE "Game Engine V0.1 - By @juliocesardev"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define LOCK_FPS 60
#define FRAME_DELAY (1000 / LOCK_FPS)

#define GAME_RUNNING 1
#define GAME_EXIT 0

#define GRAVITY_FORCE 2
#define JUMP_FORCE 20
// Keyboard
typedef struct {
	int up;						// W ou Seta para Cima
	int down;					// S ou Seta para Baixo
	int left;					// A ou Seta para Esquerda
	int right;					// D ou Seta para Direita
} keyboard_t;
// Mouse
typedef struct {
	int x;						// Posição X do Rato em relação ao Window
	int y;						// Posição Y do Rato em relação ao Window
	uint32_t button;			// Numero do Botao do Rato(Pressionado)
} mouse_t;
// Player
typedef struct {
	int isJumping;
	int isGrounded;
	float dy;
	//
	SDL_Surface *surface;		// Image do Player
	SDL_Texture	*texture;		// Textura do Player
	SDL_Rect destinyRect;		// Rect
} player_t;
// Game
typedef struct {
	int state;					// Estado do Jogo
	// Objectos
	player_t player;			// Jogador
	// Eventos
	keyboard_t *keyboard;		// Evento do Teclado
	mouse_t *mouse;				// Evento do Rato(Mouse)
} game_t;

#endif // !_GAME_H_

