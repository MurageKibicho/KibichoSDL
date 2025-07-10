#ifndef __KibichoFonts_IMPLEMENTATION_H__
#define __KibichoFonts_IMPLEMENTATION_H__

#include <SDL2/SDL.h>
#include <assert.h>

#include "stb_rect_pack.h"
#include "stb_truetype.h"
#define Kibichofonts_alphabet_INDEX(x, y, cols) ((x) * (cols) + (y))
const char *Kibichofonts_alphabet_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0";
const char *Kibichofonts_alphabet_lower = "abcdefghijklmnopqrstuvwxyz0";
const char *Kibichofonts_alphabet_mixed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

typedef struct kibicho_font_struct *KibichoFont;
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

KibichoFont LoadFont(SDL_Renderer *renderer, char *fontName, float fontSize)
{
	int fontBitmapWidth = 1024;
	int fontBitmapHeight= 1024;
	KibichoFont font = malloc(sizeof(struct kibicho_font_struct));
	font->isFontLoaded = false;
	font->fontSize = fontSize;
	font->info = malloc(sizeof(stbtt_fontinfo));
	
	//Open font file
	FILE *fontFile = fopen(fontName,"rb");
	if(fontFile == NULL){printf("Unable to open font: %s\n", fontName);exit(1);}
	
	// Find font file size
	fseek(fontFile, 0, SEEK_END);
	size_t fontFileSize = ftell(fontFile);
	fseek(fontFile, 0, SEEK_SET);
	
	// Load font to memory
	unsigned char *ttfBuffer = malloc(fontFileSize);
	fread(ttfBuffer, 1, fontFileSize, fontFile);
	
	
	// Basic validation of TrueType file header
	if(!(ttfBuffer[0] == 0x00 && ttfBuffer[1] == 0x01 && ttfBuffer[2] == 0x00 && ttfBuffer[3] == 0x00) &&!(ttfBuffer[0] == 'O' && ttfBuffer[1] == 'T' && ttfBuffer[2] == 'T' && ttfBuffer[3] == 'O')){printf("WARNING: File may not be a valid TrueType/OpenType font.\n");}
	//Get font info
	assert(stbtt_InitFont(font->info, ttfBuffer, 0) != 0);
	//Load font bitmap
	unsigned char *fontBitmap = calloc(fontBitmapWidth * fontBitmapHeight, sizeof(unsigned char));
	stbtt_pack_context pc;
	if(!stbtt_PackBegin(&pc, fontBitmap, fontBitmapWidth, fontBitmapHeight, 0, 1, NULL)){printf("ERROR: stbtt_PackBegin failed.\n");exit(1);}
	stbtt_PackSetOversampling(&pc, 2, 2);//IDK what goes wrong at 4,4
	int pack_ok = stbtt_PackFontRange(&pc, ttfBuffer, 0, fontSize, 32, 96, font->packedCharacters);
	
	if(!pack_ok){printf("ERROR: stbtt_PackFontRange failed. Possibly invalid or missing glyphs.\n");exit(1);}
	stbtt_PackEnd(&pc);
	
	//Create SDL texture
	font->sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, fontBitmapWidth, fontBitmapHeight);
	
	//Convert the 8-bit bitmap to 32-bit RGBA
	Uint32 *texturePixels = malloc(fontBitmapWidth * fontBitmapHeight * sizeof(Uint32));
	SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
	for(int i = 0; i < fontBitmapWidth * fontBitmapHeight ; i++)
	{
		texturePixels[i] = SDL_MapRGBA(format, 0xff, 0xff, 0xff, fontBitmap[i]);
	}
	SDL_UpdateTexture(font->sdlTexture, NULL, texturePixels, fontBitmapWidth * sizeof(Uint32));
	// Set texture blend mode for alpha blending
	SDL_SetTextureBlendMode(font->sdlTexture, SDL_BLENDMODE_BLEND);
	free(texturePixels);
	free(fontBitmap);
	SDL_FreeFormat(format);
	//Create OpenGL texture
	font->isFontLoaded = true;
	font->scale = stbtt_ScaleForPixelHeight(font->info, fontSize);
	stbtt_GetFontVMetrics(font->info, &font->ascent, 0, 0);
	font->baseline = (int) (font->ascent * font->scale);
	return font;
}

void DestroyFont(KibichoFont font)
{
	if(font)
	{
		if(font->info){SDL_DestroyTexture(font->sdlTexture);}
		if(font->sdlTexture){SDL_DestroyTexture(font->sdlTexture);}
		free(font);
	}
}
float KibichoFonts_MeasureTextWidth(KibichoFont font, char *text)
{
	float width = 0;
	for(int i = 0; text[i]; i++)
	{
		if(text[i] >= 32)
		{
			stbtt_packedchar* info = &font->packedCharacters[text[i] - 32];
			width += info->xadvance;
		}
	}
	return width;
}

void Kibicho_RenderText(SDL_Renderer* renderer, KibichoFont font, float x, float y, const char *text)
{
	Uint8 r, g, b, a;
	int prev_char = 0;  // For kerning
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
	SDL_SetTextureColorMod(font->sdlTexture, r, g, b);
	SDL_SetTextureAlphaMod(font->sdlTexture, a);
	for(int i = 0; text[i]; i++)
	{
		if(text[i] >= 32)
		{
			//This works:https://gist.github.com/benob/92ee64d9ffcaa5d3be95edbf4ded55f2
			stbtt_packedchar* info = &font->packedCharacters[text[i] - 32];
			SDL_Rect src_rect = {info->x0, info->y0, info->x1 - info->x0, info->y1 - info->y0};
			SDL_FRect dst_rect = {x + info->xoff, y + info->yoff, info->xoff2 - info->xoff, info->yoff2 - info->yoff};

			SDL_RenderCopyF(renderer, font->sdlTexture, &src_rect, &dst_rect);
			x += info->xadvance;
		}
	}
}

void RenderCenteredText(SDL_Renderer *renderer, KibichoFont font, int x, int y, int padding, char *text)
{
	int textWidth = (int)KibichoFonts_MeasureTextWidth(font, text);
	//You can get the height of a line by using font->baseline.
	int textHeight= font->baseline;
	int boxWidth = textWidth + 2 * padding;
	int boxHeight = textHeight + 2 * padding;
	
	// Draw the box
	
	SDL_Rect boxRect = {x, y, boxWidth, boxHeight};
	SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
	SDL_RenderDrawRect(renderer, &boxRect);
	// Calculate text position (centered)
	int textX = x + (boxWidth - textWidth) / 2;
	int textY = padding + y + (boxHeight - textHeight) / 2;
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
	Kibicho_RenderText(renderer, font, textX, textY, text);
}

void RenderWavyText(SDL_Renderer* renderer, KibichoFont font, float x, float y, const char* text, float time) {
	for(int i = 0; text[i]; i++)
	{
		float offset_y = sinf(time * 2.0f + i * 0.5f) * 5.0f;
		stbtt_packedchar* info = &font->packedCharacters[text[i] - 32];
		SDL_Rect src_rect = {info->x0, info->y0, info->x1 - info->x0, info->y1 - info->y0};
		SDL_FRect dst_rect = {x + info->xoff,y + info->yoff + offset_y,info->xoff2 - info->xoff,info->yoff2 - info->yoff};
		SDL_RenderCopyF(renderer, font->sdlTexture, &src_rect, &dst_rect);
		x += info->xadvance;
	}
}

void RenderTypewriterText(SDL_Renderer* renderer, KibichoFont font, float x, float y, const char* text, float time, float chars_per_sec)
{
	int chars_to_show = (int)(time * chars_per_sec);
	chars_to_show = chars_to_show > strlen(text) ? strlen(text) : chars_to_show;

	char buffer[256];
	strncpy(buffer, text, chars_to_show);
	buffer[chars_to_show] = '\0';

	Kibicho_RenderText(renderer, font, x, y, buffer);
}

void RenderGradientText(SDL_Renderer* renderer, KibichoFont font, float x, float y, const char* text, SDL_Color start, SDL_Color end) {
	for(int i = 0; text[i]; i++) 
	{
		float t = (float)i / strlen(text);
		Uint8 r = start.r + (end.r - start.r) * t;
		Uint8 g = start.g + (end.g - start.g) * t;
		Uint8 b = start.b + (end.b - start.b) * t;

		SDL_SetTextureColorMod(font->sdlTexture, r, g, b);

		// Render single character
		stbtt_packedchar* info = &font->packedCharacters[text[i] - 32];
		SDL_Rect src = {info->x0, info->y0, info->x1-info->x0, info->y1-info->y0};
		SDL_FRect dst = {x + info->xoff, y + info->yoff, info->xoff2-info->xoff, info->yoff2-info->yoff};
		SDL_RenderCopyF(renderer, font->sdlTexture, &src, &dst);
		x += info->xadvance;
	}
}


void RenderBounceText(SDL_Renderer* renderer, KibichoFont font, float x, float y, const char* text, float time)
{
	float bounce = fabs(sinf(time * 3.0f)) * 20.0f;
	Kibicho_RenderText(renderer, font, x, y - bounce, text);
}

#ifdef __KibichoFonts_IMPLEMENTATION_H__

#endif
#endif
