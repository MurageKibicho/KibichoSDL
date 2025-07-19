#include "KibichoSDL.h"
KibichoLayout KibichoLayout_CreateLayout(KibichoScene scene)
{
	KibichoLayout layout = malloc(sizeof(struct kibicho_layout_struct));
	layout->texture = SDL_CreateTexture(scene->renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_TARGET,scene->windowWidth, scene->windowHeight);
	if(!layout->texture)
	{
		printf("Texture creation failed: %s\n", SDL_GetError());
		exit(1);
	}
	layout->screenLocation.x = 0;
	layout->screenLocation.y = 0;
	layout->screenLocation.w = 0;
	layout->screenLocation.h = 0;
	return layout;
}

void KibichoLayout_DestroyLayout(KibichoLayout layout)
{
	if(layout != NULL)
	{
		if(layout->texture != NULL)
		{
			SDL_DestroyTexture(layout->texture);
		}
		free(layout);
	}
}
