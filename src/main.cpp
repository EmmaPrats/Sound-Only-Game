#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

#define FPS 60
int lastTime = 0, currentTime, deltaTime;
float msFrame = 1 / (FPS / 1000.0f);

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
		"Dancing Torus",
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
}

void initMusic()
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024); //or 4096?
	Mix_Init(MIX_INIT_OGG);

	//TODO load audio clips.

	//mySong = Mix_LoadMUS("resources/Blastculture-Gravitation.ogg");
	//if (!mySong)
	//{
	//	std::cout << "Error loading Music: " << Mix_GetError() << std::endl;
	//	close();
	//	exit(1);
	//}
	//Mix_PlayMusic(mySong, 0);
	//MusicCurrentTime = 0;
	//MusicCurrentTimeBeat = 0;
	//MusicCurrentBeat = 0;
	//MusicPreviousBeat = -1;
	//
	//bulk = BASE_BULK_MODIFIER;
	//uniformScale = BASE_SCALE;
	
	////Load Audios
	//Mix_Chunk* loadSound;
	//loadSound = Mix_LoadWAV("Assets/track01.ogg");
	//audios.push_back(loadSound);
	//loadSound = Mix_LoadWAV("Assets/track02.ogg");
	//audios.push_back(loadSound);
	//loadSound = Mix_LoadWAV("Assets/track03.ogg");
	//audios.push_back(loadSound);
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
	//free(zbuffer);
	//free(org.vertices);
	//free(org.normals);
	//free(cur.vertices);
	//free(cur.normals);
	//free(polies);

	Mix_CloseAudio();
	
	SDL_FreeSurface(screenSurface);
	screenSurface = NULL;

	SDL_DestroyWindow(window);
	window = NULL;
	
	SDL_Quit();
}


//SDL_Renderer* gMyRenderer = NULL;

//	SDL_DestroyRenderer(gMyRenderer);
