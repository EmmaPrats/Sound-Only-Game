#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <cmath>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const char* WINDOW_TITLE = "La Cueva de los Condenados";

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

#define FPS 60
int lastTime = 0, currentTime, deltaTime;
float msFrame = 1 / (FPS / 1000.0f);

#define PI 3.14159265

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

int deathSoundDurationMs;
int gameOverSoundDurationMs;
int hitWallSoundDurationMs;
int stepHumanSoundDurationMs;
int stepMonsterSoundDurationMs;
int victorySoundDurationMs;

int deathSoundChannel = -1;
int gameOverSoundChannel = -1;
int monsterSnoringSoundChannel = -1;
int hitWallSoundChannel = -1;
int stepHumanSoundChannel = -1;
int stepMonsterSoundChannel = -1;
int victorySoundChannel = -1;
int waterfallSoundChannel = -1;

////////////////////////////

////////// STATE ///////////

//Main loop flag
bool quit = false;

bool isDead = false;
bool hasWon = false;
bool hasLost = false;

////////////////////////////

///////// GEOMETRY /////////

// True if wall, false if no wall.
// walls[0][0] is bottom left corner, walls[9][0] is bottom right corner.
bool walls[10][10] =
{
	true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	true, false, false, false, false, false, false, false, false,  true,
	true, false, false, false, false, false,  true, false, false,  true,
	true,  true,  true,  true, false, false, false, false, false,  true,
	true, false, false, false, false, false,  true,  true,  true,  true,
	true, false, false, false, false, false, false, false, false,  true,
	true, false, false, false, false, false, false, false, false,  true,
	true,  true,  true,  true,  true,  true, false, false, false,  true,
	true, false, false, false, false, false, false, false, false,  true,
	true,  true,  true,  true,  true,  true,  true,  true,  true,  true
};
const float diagonal = sqrt(9 * 9 + 9 * 9);

unsigned char waterfallPosition[2] = { 8, 1 };

enum ORIENTATIONS { UP, DOWN, RIGHT, LEFT };

unsigned char playerPosition[2];
ORIENTATIONS playerOrientation;

unsigned char monsterPosition[2];

signed char waterfallPositionFromPlayerPOV[2];
float waterfallAngleFromPlayerPOV;

signed char monsterPositionFromPlayerPOV[2];
float monsterAngleFromPlayerPOV;

////////////////////////////

////////// INPUT ///////////

bool upInput = false;
signed char horizontalInput = 0;

////////////////////////////

///////// TIMINGS //////////

int ticksToWait = 0;

////////////////////////////

bool initSDL();
void init3D();
bool initMusic();
void update();
void close();

void updatePlayerOrientation(signed char horizontalInput);
void updateRelativePositions();
void updateSounds();

float changedInScale(float value, float oldMin, float oldMax, float newMin, float newMax)
{
	return (newMax - newMin) / (oldMax - oldMin) * (value - oldMin) + newMin;
}

Uint8 getDistance(float distance)
{
	return changedInScale(distance, 1, diagonal, 0, 255);
}

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
		if (!initMusic())
		{
			close();
			return 1;
		}

		quit = false;

		//Event handler
		SDL_Event e;

		while (!quit)
		{
			currentTime = SDL_GetTicks();
			deltaTime = currentTime - lastTime;

			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				SDL_Scancode key = e.key.keysym.scancode;
				if (e.type == SDL_KEYDOWN)
				{
					if (key == SDL_SCANCODE_ESCAPE)
						quit = true;
					if (key == SDL_SCANCODE_W || key == SDL_SCANCODE_UP)
						upInput = true;
					if (key == SDL_SCANCODE_A || key == SDL_SCANCODE_LEFT)
						horizontalInput = -1;
					if (key == SDL_SCANCODE_D || key == SDL_SCANCODE_RIGHT)
						horizontalInput = 1;
				}
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			update();
			
			SDL_UpdateWindowSurface(window);

			if (deltaTime < (int)msFrame)
				SDL_Delay((int)msFrame - deltaTime);

			lastTime = currentTime;
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
	playerPosition[0] = 2;
	playerPosition[1] = 2;

	printf("\tPlayer starts at (%d, %d)\n", playerPosition[0], playerPosition[1]);

	playerOrientation = UP;

	monsterPosition[0] = 5;
	monsterPosition[1] = 6;

	updateRelativePositions();
}

bool initMusic()
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
	{
		std::cout << "Error loading \"" << deathSoundFile << "\"." << std::endl;
		return false;
	}
	if (!gameOverSound)
	{
		std::cout << "Error loading \"" << gameOverSoundFile << "\"." << std::endl;
		return false;
	}
	if (!monsterSnoringSound)
	{
		std::cout << "Error loading \"" << monsterSnoringSoundFile << "\"." << std::endl;
		return false;
	}
	if (!hitWallSound)
	{
		std::cout << "Error loading \"" << hitWallSoundFile << "\"." << std::endl;
		return false;
	}
	if (!stepHumanSound)
	{
		std::cout << "Error loading \"" << stepHumanSoundFile << "\"." << std::endl;
		return false;
	}
	if (!stepMonsterSound)
	{
		std::cout << "Error loading \"" << stepMonsterSoundFile << "\"." << std::endl;
		return false;
	}
	if (!victorySound)
	{
		std::cout << "Error loading \"" << victorySoundFile << "\"." << std::endl;
		return false;
	}
	if (!waterfallSound)
	{
		std::cout << "Error loading \"" << waterfallSoundFile << "\"." << std::endl;
		return false;
	}

	// http://forums.libsdl.org/viewtopic.php?p=43437
	// For 44k 16-bit stereo audio:
	// durationMs = alen / ((44100 * 2(bytes) * 2(stereo)) / 1000)
	deathSoundDurationMs = deathSound->alen / 176.4;
	gameOverSoundDurationMs = gameOverSound->alen / 176.4;
	hitWallSoundDurationMs = hitWallSound->alen / 176.4;
	stepHumanSoundDurationMs = stepHumanSound->alen / 176.4;
	stepMonsterSoundDurationMs = stepMonsterSound->alen / 176.4;
	victorySoundDurationMs = victorySound->alen / 176.4;

	waterfallSoundChannel = Mix_PlayChannel(waterfallSoundChannel, waterfallSound, -1);
	monsterSnoringSoundChannel = Mix_PlayChannel(monsterSnoringSoundChannel, monsterSnoringSound, -1);

	Mix_Volume(waterfallSoundChannel, 64);

	updateSounds();

	return true;
}

void update()
{
	ticksToWait -= deltaTime;

	if (ticksToWait > 0)
	{
		horizontalInput = 0;
		upInput = false;
		return;
	}

	if (hasWon)
	{
		printf("Game ended. Player won.\n");
		quit = true;
		return;
	}

	if (hasLost)
	{
		printf("Game ended. Player lost.\n");
		quit = true;
		return;
	}

	if (isDead)
	{
		hasLost = true;
		gameOverSoundChannel = Mix_PlayChannel(gameOverSoundChannel, gameOverSound, 0);
		ticksToWait = gameOverSoundDurationMs;
		return;
	}

	if (playerPosition[0] == monsterPosition[0] &&
		playerPosition[1] == monsterPosition[1])
	{
		isDead = true;
		printf("Monster ate player.\n");

		deathSoundChannel = Mix_PlayChannel(deathSoundChannel, deathSound, 0);

		ticksToWait = deathSoundDurationMs;
		return;
	}

	if (playerPosition[0] == waterfallPosition[0] &&
		playerPosition[1] == waterfallPosition[1])
	{
		hasWon = true;
		printf("Player arrived to exit.\n");

		victorySoundChannel = Mix_PlayChannel(victorySoundChannel, victorySound, 0);

		ticksToWait = victorySoundDurationMs;
		return;
	}

	//Handling horizontal input first because it's less punishing.
	if (horizontalInput != 0)
	{
		printf("Horizontal input: %d\n", horizontalInput);
		
		updatePlayerOrientation(horizontalInput);

		updateRelativePositions();

		updateSounds();

		ticksToWait = 1000;

		horizontalInput = 0;
	}
	else if (upInput)
	{
		printf("Up input.\n");
		upInput = false;

		unsigned short targetPosition[2];

		switch (playerOrientation)
		{
		case UP:
			targetPosition[0] = playerPosition[0];
			targetPosition[1] = playerPosition[1] + 1;
			break;
		case DOWN:
			targetPosition[0] = playerPosition[0];
			targetPosition[1] = playerPosition[1] - 1;
			break;
		case LEFT:
			targetPosition[0] = playerPosition[0] - 1;
			targetPosition[1] = playerPosition[1];
			break;
		case RIGHT:
			targetPosition[0] = playerPosition[0] + 1;
			targetPosition[1] = playerPosition[1];
			break;
		}

		if (walls[targetPosition[0]][targetPosition[1]])
		{
			printf("\tTarget position (%d, %d) is a wall.\n", targetPosition[0], targetPosition[1]);

			hitWallSoundChannel = Mix_PlayChannel(hitWallSoundChannel, hitWallSound, 0);

			ticksToWait = hitWallSoundDurationMs;
		}
		else
		{
			playerPosition[0] = targetPosition[0];
			playerPosition[1] = targetPosition[1];
			printf("\tPlayer moved to (%d, %d)\n", playerPosition[0], playerPosition[1]);

			stepHumanSoundChannel = Mix_PlayChannel(stepHumanSoundChannel, stepHumanSound, 0);

			ticksToWait = stepHumanSoundDurationMs;
		}

		updateRelativePositions();

		updateSounds();
	}
}

void updatePlayerOrientation(signed char horizontalInput)
{
	switch (playerOrientation)
	{
	case UP:
		if (horizontalInput > 0)
		{
			playerOrientation = RIGHT;
			printf("\tPlayer looks RIGHT.\n");
		}
		else
		{
			playerOrientation = LEFT;
			printf("\tPlayer looks LEFT.\n");
		}
		break;
	case DOWN:
		if (horizontalInput > 0)
		{
			playerOrientation = LEFT;
			printf("\tPlayer looks LEFT.\n");
		}
		else
		{
			playerOrientation = RIGHT;
			printf("\tPlayer looks RIGHT.\n");
		}
		break;
	case LEFT:
		if (horizontalInput > 0)
		{
			playerOrientation = UP;
			printf("\tPlayer looks UP.\n");
		}
		else
		{
			playerOrientation = DOWN;
			printf("\tPlayer looks DOWN.\n");
		}
		break;
	case RIGHT:
		if (horizontalInput > 0)
		{
			playerOrientation = DOWN;
			printf("\tPlayer looks DOWN.\n");
		}
		else
		{
			playerOrientation = UP;
			printf("\tPlayer looks UP.\n");
		}
		break;
	}
}

void updateRelativePositions()
{
	waterfallPositionFromPlayerPOV[0] = waterfallPosition[0] - playerPosition[0];
	waterfallPositionFromPlayerPOV[1] = waterfallPosition[1] - playerPosition[1];

	monsterPositionFromPlayerPOV[0] = monsterPosition[0] - playerPosition[0];
	monsterPositionFromPlayerPOV[1] = monsterPosition[1] - playerPosition[1];

	double x = waterfallPositionFromPlayerPOV[0];
	double y = waterfallPositionFromPlayerPOV[1];
	double waterfallAngle = atan2(y, x) * 180 / PI;

	x = monsterPositionFromPlayerPOV[0];
	y = monsterPositionFromPlayerPOV[1];
	double monsterAngle = atan2(y, x) * 180 / PI;

	switch (playerOrientation)
	{
	case UP:
		waterfallAngleFromPlayerPOV = -waterfallAngle + 90;
		monsterAngleFromPlayerPOV = -monsterAngle + 90;
		break;
	case DOWN:
		waterfallAngleFromPlayerPOV = -waterfallAngle - 90;
		monsterAngleFromPlayerPOV = -monsterAngle - 90;
		break;
	case LEFT:
		waterfallAngleFromPlayerPOV = -waterfallAngle + 180;
		monsterAngleFromPlayerPOV = -monsterAngle + 180;
		break;
	case RIGHT:
		waterfallAngleFromPlayerPOV = -waterfallAngle;
		monsterAngleFromPlayerPOV = -monsterAngle;
		break;
	}
	printf("\t\twaterfallAngleFromPlayerPOV: %f\n", waterfallAngleFromPlayerPOV);
	printf("\t\tmonsterAngleFromPlayerPOV: %f\n", monsterAngleFromPlayerPOV);
}

void updateSounds()
{
	Sint16 waterfallAngle = waterfallAngleFromPlayerPOV + 360;
	float waterfallDistance = sqrt(
		waterfallPositionFromPlayerPOV[0] * waterfallPositionFromPlayerPOV[0] +
		waterfallPositionFromPlayerPOV[1] * waterfallPositionFromPlayerPOV[1]);
	Uint8 waterfallSoundDistance = getDistance(waterfallDistance);
	printf(
		"\t\t\tWaterfall is at %f degrees and %f units (%d) from player.\n",
		waterfallAngleFromPlayerPOV,
		waterfallDistance,
		waterfallSoundDistance);
	Mix_SetPosition(waterfallSoundChannel, waterfallAngle, waterfallSoundDistance);

	Sint16 monsterAngle = monsterAngleFromPlayerPOV + 360;
	float monsterDistance = sqrt(
		monsterPositionFromPlayerPOV[0] * monsterPositionFromPlayerPOV[0] +
		monsterPositionFromPlayerPOV[1] * monsterPositionFromPlayerPOV[1]);
	Uint8 monsterSoundDistance = getDistance(monsterDistance);
	printf(
		"\t\t\tMonster is at %f degrees and %f units (%d) from player.\n",
		monsterAngleFromPlayerPOV,
		monsterDistance,
		monsterSoundDistance);
	Mix_SetPosition(monsterSnoringSoundChannel, monsterAngle, monsterSoundDistance);
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
