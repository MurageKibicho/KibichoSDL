#define STB_DS_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "Dependencies/KibichoSDL.h"

//Run: emcc SDL.c -s Dependencies/KibichoDraw.c -s Dependencies/KibichoFont.c -s Dependencies/KibichoPolynomial.c -s Dependencies/KibichoLayout.c -s Dependencies/KibichoScene.c -s Dependencies/SDL2_framerate.c -s Dependencies/SDL2_gfxPrimitives.c -s USE_SDL=2 -s FULL_ES2=1 -s WASM=1 --preload-file Assets/Fonts/wew.ttf --preload-file Assets/Fonts/GeistMono/GeistMono-ExtraBold.ttf --preload-file Assets/Fonts/GeistMono/GeistMono-Regular.ttf -o SDL.html
// Preview: emrun SDL.html

void TestPolynomialDraw(KibichoScene scene)
{
	//Set Background color
	SDL_SetRenderDrawColor(scene->renderer, 225, 225, 225, 255);
	SDL_RenderClear(scene->renderer);
	SDL_SetRenderDrawColor(scene->renderer, 0, 0, 0, 255);
	
	int visualSpacing = 30;
	uint32_t lineColour = 0x60aac6;
	uint32_t centerLineWidth  = 3;
	int dashLength = 2;
	int gapLength  = 2;
	int regularLineWidth = 2;
	bool dottedGrid   = true;
	
	int fontIndex = 1;
	int pointX = 256;
	int pointY = 256;
	int pointRadius = 4;
	uint32_t pointColor  = 0xbe161dFF;
	uint32_t pointOutlineColor = 0x000000FF;
	uint32_t pointTextColor = 0x000000FF;
	int textX = 258;
	int textY = 280;
	char *text = "(9,0)";

	KibichoDraw_Grid(scene,visualSpacing, lineColour, centerLineWidth, dashLength, gapLength, regularLineWidth, dottedGrid);
	KibichoDraw_OutlinePoint(scene, pointX,pointY,pointRadius,pointColor,pointOutlineColor);
	KibichoDraw_TitlePoint(scene, fontIndex, pointX+5,pointY+5,pointRadius,pointColor,pointOutlineColor, textX, textY, pointTextColor, text);
	
	KibichoPolynomial p0 = KibichoPolynomial_CreatePolynomial();
	int pX = 20;double p0X[pX];for(int i = 0; i <pX; i++){p0X[i] = i - (pX/2);}
	int xCoeffLength = 1;
	double xCoeff[xCoeffLength];
	double xDeg[xCoeffLength];
	xCoeff[0] = 1;
	xDeg[0] = 3;
	float scale = 5;
	KibichoPolynomial_SetPolynomialX(p0, pX, p0X,xCoeffLength,xCoeff,xDeg);
	KibichoPolynomial_EvaluatePolynomialX(p0, scale);
	KibichoDraw_Polynomial(scene, p0);
	KibichoPolynomial_DestroyPolynomial(p0);
}

void MainLoop(void *sceneArg) 
{
	static float angle = 0.0f;  
	static float progress = 0.0f;  // Animation progress (0.0 to 1.0)
	static bool animating = true;
	KibichoScene scene = *(KibichoScene*)sceneArg;
	
	//Print framerate
	snprintf(scene->fpsText, sizeof(scene->fpsText), "Current FPS: %d", SDL_getFramerate(&scene->fpsManager));
	KibichoFont_Text(scene->renderer, scene->font[0], scene->windowWidth-90, 10, scene->fpsText, 0x000000FF);
	
	//Set layout0 texture as render target
	SDL_SetRenderTarget(scene->renderer, scene->layout[0]->texture);
	
	TestPolynomialDraw(scene);
	
	//Reset render target
	SDL_SetRenderTarget(scene->renderer, NULL);
	SDL_RenderClear(scene->renderer);
	
	// Calculate animated rectangle dimensions
	int startWidth = scene->windowWidth / 4;
	int startHeight = scene->windowHeight / 4;
	int targetWidth = scene->windowWidth;
	int targetHeight = scene->windowHeight;
	scene->layout[0]->screenLocation.w = startWidth + (int)((targetWidth - startWidth) * progress);
	scene->layout[0]->screenLocation.h = startHeight + (int)((targetHeight - startHeight) * progress);
	
        SDL_RenderCopy(scene->renderer, scene->layout[0]->texture, NULL, &(scene->layout[0]->screenLocation));

	SDL_RenderPresent(scene->renderer);
	//Use framerate
	SDL_framerateDelay(&scene->fpsManager); 
	
	if(animating)
	{
		progress += 0.01f;  // Adjust speed as needed
		if(progress >= 1.0f)
		{
			progress = 1.0f;
		animating = false;  // Stop when full size
		}
	}
}

int main()
{
	int windowWidth = 512;
	int windowHeight= windowWidth;
	char *windowName = "KibichoSDL";
	
	SDL_Init(SDL_INIT_VIDEO);
	KibichoScene scene = KibichoScene_CreateScene(windowName, windowWidth, windowHeight);
	
	void *mainLoopArgument = scene;
	int fps = 0;
	bool simulateInfiniteLoop = true;
	#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop_arg(MainLoop, &mainLoopArgument, fps, simulateInfiniteLoop);
	#else
		while(true) 
		{
			MainLoop(&mainLoopArgument);
		}
	#endif
	
	KibichoScene_DestroyScene(scene);
	SDL_Quit();
	return 0;
}

