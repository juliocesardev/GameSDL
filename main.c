#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include "./game.h"

int main(int argc, char* argv[])
{

	// Flags do SDL Init
	/*
	* SDL_INIT_TIMER		- Subsistema de timer
	* SDL_INIT_AUDIO		- Subsistema de áudio
	* SDL_INIT_VIDEO		- Subsistema de vídeo; inicializa automaticamente o subsistema de eventos
	* SDL_INIT_EVENTS		- Subsistema de eventos
	* SDL_INIT_EVERYTHING	- Todos os subsistemas acima
	*/
	const uint32_t SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
	/* 
	* Inicia o SDL e verifica se foi possivel inicial o SDL
	*
	* @param - SDL_Init(1)
	*
	* 1 - uint32_t	-> Flags do SDL Init
	*/
	if (SDL_Init(SDL_INIT_FLAGS) != 0)
	{
		printf("Não foi possivel iniciar o SDL! SDL_Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	// Flags do SDL Window
	/*
	* SDL_WINDOW_FULLSCREEN			- Fullscreen window
	* SDL_WINDOW_FULLSCREEN_DESKTOP - Fullscreen window at the current desktop resolution
	* SDL_WINDOW_OPENGL				- Window usable with OpenGL context
	* SDL_WINDOW_VULKAN				- Window usable with a Vulkan instance
	* SDL_WINDOW_SHOWN				- Window is visible
	* SDL_WINDOW_BORDERLESS			- No window decoration
	* SDL_WINDOW_INPUT_GRABBED		- Window has grabbed input focus
	* SDL_WINDOW_INPUT_FOCUS		- Window has input focus
	* SDL_WINDOW_MOUSE_FOCUS		- Window has mouse focus
	* SDL_WINDOW_ALLOW_HIGHDPI		- Window should be created in high - DPI mode if supported(>= SDL 2.0.1)
	* SDL_WINDOW_MOUSE_CAPTURE		- Window has mouse captured(unrelated to INPUT_GRABBED, >= SDL 2.0.4)
	*/
	const uint32_t SDL_WINDOW_FLAGS = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS;
	/*
	* Cria uma janela
	*
	* @param - SDL_CreateWindow(1, 2, 3, 4, 5, 6)
	*
	* 1 - string	-> Titulo da Janela
	* 2 - int		-> Posicao X da Janela quando inicia
	* 3 - int		-> Posicao Y da Janela quando inicia
	* 4 - int		-> Comprimento da Janela
	* 5 - int		-> Altura da Janela
	* 6 - uint32_t	-> Flags da Janela
	*/
	SDL_Window* window = SDL_CreateWindow(
		WINDOW_TITLE,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FLAGS);
	// Verifica se foi possivel criar uma janela
	if (!window)
	{
		printf("Não foi possivel criar uma janela! SDL Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	// Flags do SDL Renderer
	/* 
	* SDL_RENDERER_SOFTWARE the renderer is a software fallback
	* SDL_RENDERER_ACCELERATED the renderer uses hardware acceleration
	* SDL_RENDERER_PRESENTVSYNC present is synchronized with the refresh rate
	* SDL_RENDERER_TARGETTEXTURE the renderer supports rendering to texture
	*/
	const uint32_t SDL_RENDERER_FLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	/*
	* Cria um renderer para a janela
	* 
	* @param - SDL_CreateRenderer(1, 2, 3)
	* 
	* 1 - SDL_Window	-> Janela
	* 2 - int			-> Qual Grafica usar
	* 3 - uint32_t		-> Flags do SDL Renderer
	*/
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_FLAGS);
	// Verifica se foi possivel criar Renderer para a Janela
	if (!renderer)
	{
		printf("Não foi possivel criar renderer para janela! SDL Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		return EXIT_FAILURE;
	}
	// Player
	player_t player;
	player.isJumping = 0;
	player.isGrounded = 0;
	player.dy = 2;
	/*
	* Carregar uma imagem
	*
	* @param - IMG_Load(1)
	* 
	* 1 - char* -> O caminho da imagem, pode ser qualquer formato de imagem JPG, JPEG, PNG, GIF, etc
	*/
	player.surface = IMG_Load("./images/player.png");
	// Verifica se foi possivel carregar a imagem
	if (!player.surface)
	{
		printf("Não foi possivel carregar a imagem! SDL Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return EXIT_FAILURE;
	}
	/*
	* Carregar Textura (Carregar imagem para placa grafica)
	* 
	* @param - SDL_CreateTextureFromSurface(1, 2)
	*
	* 1 - SDL_Renderer	-> 
	* 2 - SDL_Surface	->
	*/
	player.texture = SDL_CreateTextureFromSurface(renderer, player.surface);
	// Liberta a imagem carregada (Surface)
	SDL_FreeSurface(player.surface);
	// Set Player Surface para NULL
	player.surface = NULL;
	// Verifica se foi possivel carregar textura
	if (!player.texture)
	{
		printf("Não foi possivel carregar a textura! SDL Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return EXIT_FAILURE;
	}
	//
	SDL_QueryTexture(player.texture, NULL, NULL, &player.destinyRect.w, &player.destinyRect.h);
	player.destinyRect.x = 0;
	player.destinyRect.y = 0;
	// Dados do Teclado
	keyboard_t keyboard = { 
		.up = 0, .down = 0, 
		.left = 0, .right = 0
	};
	// Dados do Mouse
	mouse_t mouse = {
		.x = 0,	.y = 0,
		.button = 0
	};
	// Dados do Jogo
	game_t game = {
		.state = GAME_RUNNING,
		.keyboard = &keyboard,
		.mouse = &mouse,
		.player = player
	};
	// Limitador de FPS
	uint32_t frameStart;
	uint32_t frameTime;
	// Eventos do SDL -> Teclado, Mouse, etc
	SDL_Event event;
	// Loop do jogo
	while (game.state)
	{
		// Quando comeca o Frame
		frameStart = SDL_GetTicks();
		// Eventos
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					game.state = GAME_EXIT;
					break;
				// Busca os Eventos do Teclado
				case SDL_KEYDOWN:
					switch (event.key.keysym.scancode)
					{
						case SDL_SCANCODE_W:
						case SDL_SCANCODE_UP:
							keyboard.up = 1;
							break;
						case SDL_SCANCODE_S:
						case SDL_SCANCODE_DOWN:
							keyboard.down = 1;
							break;
						case SDL_SCANCODE_A:
						case SDL_SCANCODE_LEFT:
							keyboard.left = 1;
							break;
						case SDL_SCANCODE_D:
						case SDL_SCANCODE_RIGHT:
							keyboard.right = 1;
							break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.scancode)
					{
						case SDL_SCANCODE_W:
						case SDL_SCANCODE_UP:
							keyboard.up = 0;
							break;
						case SDL_SCANCODE_S:
						case SDL_SCANCODE_DOWN:
							keyboard.down = 0;
							break;
						case SDL_SCANCODE_A:
						case SDL_SCANCODE_LEFT:
							keyboard.left = 0;
							break;
						case SDL_SCANCODE_D:
						case SDL_SCANCODE_RIGHT:
							keyboard.right = 0;
							break;
					}
					break;
			}
		}
		// Busca os Eventos do Rato(Mouse)
		mouse.button = SDL_GetMouseState(&mouse.x, &mouse.y);
		// Atualizacao do Dados do Jogo
		if (game.keyboard->up && !game.keyboard->down && game.player.isGrounded)
		{
			//game.player.destinyRect.y -= GRAVITY_FORCE * JUMP_FORCE;
			game.player.dy = -JUMP_FORCE;
			game.player.isJumping = 1;
		}
		if (game.keyboard->left && !game.keyboard->right) game.player.destinyRect.x -= 5;
		if (!game.keyboard->left && game.keyboard->right) game.player.destinyRect.x += 5;
		//
		if (game.mouse->button == 1)
		{
			game.player.destinyRect.x = game.mouse->x - (game.player.destinyRect.w / 2);
			game.player.destinyRect.y = game.mouse->y - (game.player.destinyRect.h / 2);
			game.player.dy = 2;
		}
		else
		{
			if (game.player.destinyRect.y + game.player.destinyRect.h < SCREEN_HEIGHT || game.player.isJumping)
			{
				game.player.destinyRect.y = game.player.destinyRect.y + game.player.dy;
				game.player.dy = game.player.dy + GRAVITY_FORCE;
				game.player.isJumping = 0;
				game.player.isGrounded = 0;
			}
			else
			{
				game.player.destinyRect.y = SCREEN_HEIGHT - game.player.destinyRect.h;
				game.player.isJumping = 0;
				game.player.dy = 2;
				game.player.isGrounded = 1;
			}
		}
		// Limpa o Buffer Atual
		SDL_RenderClear(renderer);
		// Desenhar o Jogo
		SDL_RenderCopy(renderer, game.player.texture, NULL, &game.player.destinyRect);
		// Trocar Buffers
		SDL_RenderPresent(renderer);
		// Tempo de Update e Render o Jogo
		frameTime = SDL_GetTicks() - frameStart;
		// Caso for muito rapido, ele da um delay
		if (FRAME_DELAY > frameTime)
		{
			SDL_Delay(FRAME_DELAY - frameTime);
		}
		// Debug 
		printf("--- Keyboard ----\nKeyboard.Up:%i\nKeyboard.Down:%i\nKeyboard.Left:%i\nKeyboard.Right:%i\n", game.keyboard->up, game.keyboard->down, game.keyboard->left, game.keyboard->right);
		printf("--- Mouse ----\nMouse.Button:%i\nMouse.X:%i\nMouse.Y:%i\n", game.mouse->button, game.mouse->x, game.mouse->y);
		printf("--- Player ----\nPlayer.Rect.X:%i\nPlayer.Rect.Y:%i\nPlayer.IsJumping:%i\nPlayer.dy:%f\nPlayer.IsGrounded:%i\n", game.player.destinyRect.x, game.player.destinyRect.y, game.player.isJumping, game.player.dy, game.player.isGrounded);
	}
	// Destroi todo do SDL que foi criado
	SDL_DestroyTexture(player.texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}