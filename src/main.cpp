#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const char* WINDOW_TITLE = "La Cueva de los Condenados";

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

#define FPS 60
int lastTime = 0, currentTime, deltaTime;
float msFrame = 1 / (FPS / 1000.0f);

////////// SOUNDS //////////

const char* deathSoundFile = "resources/death.wav";
const char* gameOverSoundFile = "resources/gameover.wav";
const char* monsterSnoringSoundFile = "resources/Monster-Snoring.wav";
const char* hitWallSoundFile = "resources/shock-wall.wav";
const char* stepHumanSoundFile = "resources/step-human.wav";
const char* stepMonsterSoundFile = "resources/step-monster.wav";
const char* victorySoundFile = "resources/victory.wav";
const char* waterfallSoundFile = "resources/waterfall.wav";

Mix_Chunk* deathSound;
Mix_Chunk* gameOverSound;
Mix_Chunk* monsterSnoringSound;
Mix_Chunk* hitWallSound;
Mix_Chunk* stepHumanSound;
Mix_Chunk* stepMonsterSound;
Mix_Chunk* victorySound;
Mix_Chunk* waterfallSound;

int deathSoundChannel;
int gameOverSoundChannel;
int monsterSnoringSoundChannel;
int hitWallSoundChannel;
int stepHumanSoundChannel;
int stepMonsterSoundChannel;
int victorySoundChannel;
int waterfallSoundChannel;

////////////////////////////

///////// GEOMETRY /////////

// True if wall, false if no wall.
// walls[0][0] is bottom left corner, walls[9][0] is bottom right corner.
bool walls[10][10] =
{
	true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	true, false, false,  true, false, false, false,  true, false,  true,
	true, false, false,  true, false, false, false,  true, false,  true,
	true, false, false,  true, false, false, false,  true, false,  true,
	true, false, false, false, false, false, false,  true, false,  true,
	true, false, false, false, false, false, false,  true, false,  true,
	true, false,  true, false,  true, false, false, false, false,  true,
	true, false, false, false,  true, false, false, false, false,  true,
	true, false, false, false,  true, false, false, false, false,  true,
	true,  true,  true,  true,  true,  true,  true,  true,  true,  true
};

std::vector<short> playerPosition;
std::vector<short> playerForward;

std::vector<short> monsterPosition;

std::vector<short> waterfallPosition;

////////////////////////////

bool initSDL();
void init3D();
void initMusic();
void update();
void waitForNextFrame();
void close();

int main(int argc, char* args[])
{
	if (!initSDL())
	{
		std::cout << "Failed to initialize!\n";
		return 1;
	}
	else
	{
		init3D();
		initMusic();

		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_KEYDOWN)
				{
					if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
						quit = true;
				}
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			update();
			
			SDL_UpdateWindowSurface(window);

			waitForNextFrame();
		}
	}

	close();

	return 0;
}

bool initSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	window = SDL_CreateWindow(
		WINDOW_TITLE,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	screenSurface = SDL_GetWindowSurface(window);
	return true;
}

void init3D()
{
	//TODO fill matrix, position elements.

	Mix_SetPosition(waterfallSoundChannel, 180, 10);
}

void initMusic()
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024); //or 4096?
	Mix_Init(MIX_INIT_OGG);

	deathSound = Mix_LoadWAV(deathSoundFile);
	gameOverSound = Mix_LoadWAV(gameOverSoundFile);
	monsterSnoringSound = Mix_LoadWAV(monsterSnoringSoundFile);
	hitWallSound = Mix_LoadWAV(hitWallSoundFile);
	stepHumanSound = Mix_LoadWAV(stepHumanSoundFile);
	stepMonsterSound = Mix_LoadWAV(stepMonsterSoundFile);
	victorySound = Mix_LoadWAV(victorySoundFile);
	waterfallSound = Mix_LoadWAV(waterfallSoundFile);

	if (!deathSound)
		std::cout << "Error loading \"" << deathSoundFile << "\"." << std::endl;
	if (!gameOverSound)
		std::cout << "Error loading \"" << gameOverSoundFile << "\"." << std::endl;
	if (!monsterSnoringSound)
		std::cout << "Error loading \"" << monsterSnoringSoundFile << "\"." << std::endl;
	if (!hitWallSound)
		std::cout << "Error loading \"" << hitWallSoundFile << "\"." << std::endl;
	if (!stepHumanSound)
		std::cout << "Error loading \"" << stepHumanSoundFile << "\"." << std::endl;
	if (!stepMonsterSound)
		std::cout << "Error loading \"" << stepMonsterSoundFile << "\"." << std::endl;
	if (!victorySound)
		std::cout << "Error loading \"" << victorySoundFile << "\"." << std::endl;
	if (!waterfallSound)
		std::cout << "Error loading \"" << waterfallSoundFile << "\"." << std::endl;

	waterfallSoundChannel = Mix_PlayChannel(-1, waterfallSound, -1);
}

void update()
{

}

void waitForNextFrame()
{
	currentTime = SDL_GetTicks();
	deltaTime = currentTime - lastTime;
	if (deltaTime < (int)msFrame)
		SDL_Delay((int)msFrame - deltaTime);
	lastTime = currentTime;
}

void close()
{
	Mix_CloseAudio();
	
	SDL_FreeSurface(screenSurface);
	screenSurface = NULL;

	SDL_DestroyWindow(window);
	window = NULL;
	
	SDL_Quit();
}
