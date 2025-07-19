#include "KibichoSDL.h"

KibichoScene KibichoScene_CreateScene(char *windowName, int windowWidth, int windowHeight)
{
	KibichoScene scene = malloc(sizeof(struct kibicho_scene_struct));
	//Setup window properties
	scene->windowWidth  = windowWidth;
	scene->windowHeight = windowHeight;
	scene->window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	scene->windowID = SDL_GetWindowID(scene->window);
	scene->renderer = SDL_CreateRenderer(scene->window , -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(scene->renderer, windowWidth, windowHeight);
	SDL_SetRenderDrawBlendMode(scene->renderer, SDL_BLENDMODE_BLEND);
	
	//Setup framerate
	SDL_initFramerate(&scene->fpsManager);
	SDL_setFramerate(&scene->fpsManager, 1);
	
	//Load Default Fonts
	char *fontName1 = "Assets/Fonts/GeistMono/GeistMono-Regular.ttf";
	char *fontName2 = "Assets/Fonts/GeistMono/GeistMono-ExtraBold.ttf";
	
	KibichoFont font1  = KibichoFont_LoadFont(scene->renderer, fontName1, 12);
	KibichoFont font2  = KibichoFont_LoadFont(scene->renderer, fontName2, 12);
	scene->font = NULL;
	arrput(scene->font, font1);
	arrput(scene->font, font2);
	
	scene->layout = NULL;
	KibichoLayout layout0  = KibichoLayout_CreateLayout(scene);
	arrput(scene->layout,layout0);
	return scene;
}

void KibichoScene_DestroyScene(KibichoScene scene)
{	
	if(scene)
	{
		if(scene->window){SDL_DestroyWindow(scene->window);}
		if(scene->renderer){SDL_DestroyRenderer(scene->renderer);}
		if(scene->font)
		{
			for(size_t i = 0; i < arrlen(scene->font); i++)
			{
				KibichoFont_DestroyFont(scene->font[i]);
			}
			arrfree(scene->font);
		}
		if(scene->layout)
		{
			for(size_t i = 0; i < arrlen(scene->layout); i++)
			{
				KibichoLayout_DestroyLayout(scene->layout[i]);
			}
			arrfree(scene->layout);
		}
		free(scene);
	}
}
