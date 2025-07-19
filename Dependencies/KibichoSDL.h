#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL.h>
#include <SDL_opengles2.h>
#include <SDL_thread.h>
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
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "stb_ds.h"
#include "stb_rect_pack.h"
#include "stb_truetype.h"
#include "SDL2_gfxPrimitives.h"
#include "SDL2_framerate.h"
#define Kibicho_INDEX(x, y, cols) ((x) * (cols) + (y))

typedef struct kibicho_scene_struct *KibichoScene;
typedef struct kibicho_font_struct *KibichoFont;
typedef struct kibicho_polynomial_struct *KibichoPolynomial;
/*KibichoScene*/
struct kibicho_scene_struct
{
	/*Renderer and window*/
	SDL_Renderer *renderer;
	SDL_Window *window;
	Uint32 windowID;
	int windowWidth;
	int windowHeight;
	FPSmanager fpsManager;
	char fpsText[64];
	
	/*Fonts*/
	KibichoFont *font;
	
};

KibichoScene KibichoScene_CreateScene(char *windowName, int windowWidth, int windowHeight);
void KibichoScene_DestroyScene(KibichoScene scene);

/*KibichoFont*/
struct kibicho_font_struct
{
	bool isFontLoaded;
	stbtt_packedchar packedCharacters[96];
	stbtt_fontinfo *info; //idk how to free this lol
	SDL_Texture *sdlTexture;
	int ascent;
	int baseline;
	float scale;
	GLuint fontTexture;
	GLuint fontVBO;
	GLuint fontUVBO;
	GLfloat fontSize;
};
KibichoFont KibichoFont_LoadFont(SDL_Renderer *renderer, char *fontName, float fontSize);
void KibichoFont_DestroyFont(KibichoFont font);
void KibichoFont_Text(SDL_Renderer* renderer, KibichoFont font, float x, float y, char *text, uint32_t color);

/*KibichoPolynomial*/
struct kibicho_polynomial_struct
{
	double *coefficient;
	double *exponent;
	double *xPointsReal;
	double *yPointsReal;
	int *xPointsDraw;
	int *yPointsDraw;
};
KibichoPolynomial KibichoPolynomial_CreatePolynomial();
void KibichoPolynomial_DestroyPolynomial(KibichoPolynomial polynomial);
void KibichoPolynomial_SetPolynomialX(KibichoPolynomial polynomial, int length, double *xValues, int xCoefficientLength, double *xCoefficient, double *xDegree);
void KibichoPolynomial_EvaluatePolynomialX(KibichoPolynomial polynomial, float scale);

/*KibichoDraw*/
void KibichoDraw_Grid(KibichoScene scene, int visualSpacing, 	uint32_t lineColour,uint32_t centerLineWidth,int dashLength,int gapLength,int regularLineWidth,bool dottedGrid);
void KibichoDraw_ThickAALineRGBA(SDL_Renderer *renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,Uint8 thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void KibichoDraw_Polynomial(KibichoScene scene, KibichoPolynomial polynomial);
void KibichoDraw_TitlePoint(KibichoScene scene, int fontIndex, int x, int y, int radius, uint32_t pointColor,uint32_t pointOutlineColor, int textX, int textY, uint32_t textColor, char *text);
void KibichoDraw_OutlinePoint(KibichoScene scene, int x, int y, int radius, uint32_t pointColor,uint32_t pointOutlineColor);
