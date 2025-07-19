#include "KibichoSDL.h"

KibichoPolynomial KibichoPolynomial_CreatePolynomial()
{
	KibichoPolynomial polynomial = malloc(sizeof(struct kibicho_polynomial_struct));
	polynomial->coefficient = NULL;
	polynomial->exponent = NULL;
	polynomial->xPointsReal = NULL;
	polynomial->yPointsReal = NULL;
	polynomial->xPointsDraw = NULL;
	polynomial->yPointsDraw = NULL;
	return polynomial;
}

void KibichoPolynomial_DestroyPolynomial(KibichoPolynomial polynomial)
{
	if(polynomial)
	{
		arrfree(polynomial->coefficient);
		arrfree(polynomial->exponent);
		arrfree(polynomial->xPointsReal);
		arrfree(polynomial->yPointsReal);
		arrfree(polynomial->xPointsDraw);
		arrfree(polynomial->yPointsDraw);
		free(polynomial);
	}
}

void KibichoPolynomial_SetPolynomialX(KibichoPolynomial polynomial, int length, double *xValues, int xCoefficientLength, double *xCoefficient, double *xDegree)
{
	for(int i = 0; i < length; i++)
	{
		arrput(polynomial->xPointsReal, xValues[i]);
		arrput(polynomial->yPointsReal, 0);
		arrput(polynomial->xPointsDraw, xValues[i]);
		arrput(polynomial->yPointsDraw, 0);
	}
	for(int i = 0; i < xCoefficientLength; i++)
	{
		arrput(polynomial->coefficient, xCoefficient[i]);
		arrput(polynomial->exponent, xDegree[i]);
	}
}

void KibichoPolynomial_EvaluatePolynomialX(KibichoPolynomial polynomial, float scale)
{
	assert(arrlen(polynomial->coefficient) == arrlen(polynomial->exponent));
	assert(arrlen(polynomial->xPointsReal) == arrlen(polynomial->yPointsReal));

	for(int i = 0; i < arrlen(polynomial->xPointsReal); i++)
	{
		double x =  polynomial->xPointsReal[i];
		double y = 0;
		for(size_t j = 0; j < arrlen(polynomial->coefficient); j++)
		{
			y += polynomial->coefficient[j] * pow(x, polynomial->exponent[j]);
		}
		polynomial->yPointsReal[i] = y;
		
		polynomial->xPointsDraw[i] = (int) (polynomial->xPointsReal[i] * scale);
		polynomial->yPointsDraw[i] = (int) (polynomial->yPointsReal[i] * scale);
		printf("%d : %.3f %.3f | %d %d\n", i, x,y,polynomial->xPointsDraw[i],polynomial->yPointsDraw[i]);
	}
}

