#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL.h>
#include <SDL_opengles2.h>
#else
#include <SLD2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#include "Dependencies/stb_ds.h"
#include "Dependencies/KibichoFonts.h"
#include "Dependencies/SDL2_gfxPrimitives.h" 
#define INDEX(x, y, cols) ((x) * (cols) + (y))
#define TRUE 1
#define FALSE 0

// Run: emcc SDL.c -s Dependencies/SDL2_gfxPrimitives.c -s USE_SDL=2 -s FULL_ES2=1 -s WASM=1 -Icglm/include -DCGLM_HEADER_ONLY --preload-file Assets/Fonts/wew.ttf --preload-file Assets/Fonts/GeistMono/GeistMono-ExtraBold.ttf --preload-file Assets/Fonts/GeistMono/GeistMono-Regular.ttf -o SDL.html
// Preview: emrun SDL.html
typedef struct scene_struct *Scene;
struct scene_struct
{
	KibichoFont *font;
	SDL_Renderer *renderer;
	SDL_Window *window;
	Uint32 windowID;
	int windowWidth;
	int windowHeight;
};

Scene CreateScene(char *windowName, int windowWidth, int windowHeight)
{
	Scene scene = malloc(sizeof(struct scene_struct));
	scene->windowWidth  = windowWidth;
	scene->windowHeight = windowHeight;
	scene->window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	scene->windowID = SDL_GetWindowID(scene->window);
	scene->renderer = SDL_CreateRenderer(scene->window , -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(scene->renderer, windowWidth, windowHeight);
	SDL_SetRenderDrawBlendMode(scene->renderer, SDL_BLENDMODE_BLEND);
	
	//Load Fonts
	char *fontName1 = "Assets/Fonts/GeistMono/GeistMono-Regular.ttf";
	char *fontName2 = "Assets/Fonts/GeistMono/GeistMono-ExtraBold.ttf";
	
	KibichoFont font1  = LoadFont(scene->renderer, fontName1, 12);
	KibichoFont font2  = LoadFont(scene->renderer, fontName2, 12);
	arrput(scene->font, font1);
	arrput(scene->font, font2);
	return scene;
}

void DestroyScene(Scene scene)
{	
	if(scene)
	{	
		if(scene->window){SDL_DestroyWindow(scene->window);}
		if(scene->renderer){SDL_DestroyRenderer(scene->renderer);}
		if(scene->font)
		{
			for(size_t i = 0; i < arrlen(scene->font); i++)
			{
				DestroyFont(scene->font[i]);
			}
			arrfree(scene->font);
		}
		free(scene);
	}
}

void MainLoop(void *sceneArg) 
{
	Scene scene = *(Scene*)sceneArg;
	
	//Set Background color
	SDL_SetRenderDrawColor(scene->renderer, 225, 225, 225, 255);
	SDL_RenderClear(scene->renderer);
	SDL_SetRenderDrawColor(scene->renderer, 0, 0, 0, 255);
	
	filledCircleRGBA(scene->renderer, 320, 240, 50, 255, 0, 0, 255);
	
	SDL_RenderPresent(scene->renderer);
	SDL_Delay(16);
}

int main()
{
	int windowWidth = 512;int windowHeight= 512;
	char *windowName = "Kibicho";
	
	SDL_Init(SDL_INIT_VIDEO);
	Scene scene = CreateScene(windowName, windowWidth, windowHeight);
	
	
	void *mainLoopArgument = scene;
	#ifdef __EMSCRIPTEN__
		int fps = 0;
		emscripten_set_main_loop_arg(MainLoop, &mainLoopArgument, fps, true);
	#else
		while(true) 
		{
			MainLoop(&mainLoopArgument);
		}
	#endif
	
	DestroyScene(scene);
	SDL_Quit();
	return 0;
}
