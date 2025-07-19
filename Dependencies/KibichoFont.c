#include "KibichoSDL.h"
#define Kibichofont_alphabet_INDEX(x, y, cols) ((x) * (cols) + (y))
const char *Kibichofont_alphabet_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0";
const char *Kibichofont_alphabet_lower = "abcdefghijklmnopqrstuvwxyz0";
const char *Kibichofont_alphabet_mixed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

KibichoFont KibichoFont_LoadFont(SDL_Renderer *renderer, char *fontName, float fontSize)
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

void KibichoFont_DestroyFont(KibichoFont font)
{
	if(font)
	{
		if(font->info){SDL_DestroyTexture(font->sdlTexture);}
		if(font->sdlTexture){SDL_DestroyTexture(font->sdlTexture);}
		free(font);
	}
}

float KibichoFont_MeasureTextWidth(KibichoFont font, char *text)
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

void KibichoFont_Text(SDL_Renderer* renderer, KibichoFont font, float x, float y, char *text, uint32_t color)
{
	Uint8 r, g, b, a;
	r = (color >> 24) & 0xFF;
	g = (color >> 16) & 0xFF;
	b = (color >> 8)  & 0xFF;
	a = (color)       & 0xFF;
	int prev_char = 0;  // For kerning
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

