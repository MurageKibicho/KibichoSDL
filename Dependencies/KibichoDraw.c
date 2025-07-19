#include "KibichoSDL.h"

void KibichoDraw_ThickAALineRGBA(SDL_Renderer *renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,Uint8 thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	// Calculate direction vector
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = sqrtf(dx*dx + dy*dy);
	if (length == 0) return;

	// Normalize and get perpendicular vector
	dx /= length;
	dy /= length;
	float px = -dy * thickness/2;
	float py = dx * thickness/2;
    
	// Create polygon vertices
	Sint16 vx[4] = {(Sint16)(x1 + px), (Sint16)(x1 - px),(Sint16)(x2 - px), (Sint16)(x2 + px)};
	Sint16 vy[4] = {(Sint16)(y1 + py), (Sint16)(y1 - py),(Sint16)(y2 - py), (Sint16)(y2 + py)};
    
	// Draw filled anti-aliased polygon
	filledPolygonRGBA(renderer, vx, vy, 4, r, g, b, a);
	aapolygonRGBA(renderer, vx, vy, 4, r, g, b, a);
}

void KibichoDraw_Polynomial(KibichoScene scene, KibichoPolynomial polynomial)
{
	int visualCenterX = scene->windowWidth  / 2;
	int visualCenterY = scene->windowHeight / 2;
	for(int i = 0; i < arrlen(polynomial->xPointsDraw)-1; i++)
	{
		KibichoDraw_ThickAALineRGBA(scene->renderer, visualCenterX + polynomial->xPointsDraw[i], visualCenterY - polynomial->yPointsDraw[i], visualCenterX + polynomial->xPointsDraw[i+1], visualCenterY - polynomial->yPointsDraw[i+1], 4, 255, 0, 0, 255);		
	}
}

void KibichoDraw_DashedLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int dashLength, int gapLength, int lineWidth, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	float distance = sqrt(dx * dx + dy * dy);
	float stepX = dx / distance;
	float stepY = dy / distance;

	float currentX = x1;
	float currentY = y1;
	bool drawDash = true;
	float drawnLength = 0;	
	while(drawnLength < distance)
	{
		if(drawDash)
		{
			//Draw a dash
			int nextX = currentX + stepX * dashLength;
			int nextY = currentY + stepY * dashLength;
			thickLineRGBA(renderer, (int)currentX, (int)currentY, (int)nextX, (int)nextY,2,r,g,b,a);
			currentX = nextX;
			currentY = nextY;
			drawnLength += dashLength;
		}
		else
		{
			//Skip gap
			currentX += stepX * gapLength;
			currentY += stepY * gapLength;
			drawnLength += gapLength;
		}
		drawDash = !drawDash; 
	}
}

void KibichoDraw_Grid(KibichoScene scene, int visualSpacing, uint32_t lineColour,uint32_t centerLineWidth,int dashLength,int gapLength,int regularLineWidth,bool dottedGrid)
{
	int visualCenterX = scene->windowWidth  / 2;
	int visualCenterY = scene->windowHeight / 2;

	//Draw vertical midpoint
	thickLineRGBA(scene->renderer, visualCenterX, 0, visualCenterX, scene->windowHeight, centerLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
	//Draw horizontal midpoint
	thickLineRGBA(scene->renderer, 0, visualCenterY, scene->windowWidth, visualCenterY, centerLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
	
	if(dottedGrid == false)
	{
		//Draw vertical lines
		for(int i = 1; i < 10; i++)
		{
			int lineIndex = i * visualSpacing;
			thickLineRGBA(scene->renderer, visualCenterX + lineIndex, 0, visualCenterX + lineIndex, scene->windowHeight, centerLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
			thickLineRGBA(scene->renderer, visualCenterX - lineIndex, 0, visualCenterX - lineIndex, scene->windowHeight, centerLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
		}
		//Draw horizontal lines
		for(int i = 1; i < 10; i++)
		{
			int lineIndex = i * visualSpacing;
			thickLineRGBA(scene->renderer, 0, visualCenterY+lineIndex, scene->windowWidth, visualCenterY+lineIndex, centerLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
			thickLineRGBA(scene->renderer, 0, visualCenterY-lineIndex, scene->windowWidth, visualCenterY-lineIndex, centerLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
		}
	}
	else
	{
		//Draw Dashed lines
		for(int i = 1; i < 10; i++)
		{
			int lineIndex = i * visualSpacing;
			KibichoDraw_DashedLine(scene->renderer, visualCenterX + lineIndex, 0, visualCenterX + lineIndex, scene->windowHeight, dashLength, gapLength, regularLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
			KibichoDraw_DashedLine(scene->renderer, visualCenterX - lineIndex, 0, visualCenterX - lineIndex, scene->windowHeight, dashLength, gapLength, regularLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
		}
		//Draw horizontal lines
		for(int i = 1; i < 10; i++)
		{
			int lineIndex = i * visualSpacing;
			KibichoDraw_DashedLine(scene->renderer, 0, visualCenterY+lineIndex, scene->windowWidth, visualCenterY+lineIndex, dashLength, gapLength, regularLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
			KibichoDraw_DashedLine(scene->renderer, 0, visualCenterY-lineIndex, scene->windowWidth, visualCenterY-lineIndex, dashLength, gapLength, regularLineWidth, (lineColour >> 16) & 0xFF, (lineColour >> 8) & 0xFF, lineColour & 0xFF, 0xFF);
		}
	}
}

void KibichoDraw_OutlinePoint(KibichoScene scene, int x, int y, int radius, uint32_t pointColor,uint32_t pointOutlineColor)
{
	filledCircleRGBA(scene->renderer, x, y, radius, (pointColor >> 24) & 0xFF, (pointColor >> 16) & 0xFF, (pointColor >> 8) & 0xFF, pointColor & 0xFF);
	circleRGBA(scene->renderer, x, y, radius, (pointOutlineColor >> 24) & 0xFF, (pointOutlineColor >> 16) & 0xFF, (pointOutlineColor >> 8) & 0xFF, pointOutlineColor & 0xFF);
}

void KibichoDraw_TitlePoint(KibichoScene scene, int fontIndex, int x, int y, int radius, uint32_t pointColor,uint32_t pointOutlineColor, int textX, int textY, uint32_t textColor, char *text)
{
	KibichoDraw_OutlinePoint(scene, x,y,radius,pointColor,pointOutlineColor);
	KibichoFont_Text(scene->renderer, scene->font[fontIndex], (float) textX, (float) textY, text,textColor);
}
