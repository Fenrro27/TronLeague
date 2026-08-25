#include "TronLightCycle.h"
#include "TronLightCycle_pieces.h"
#include <GL/glew.h>
#include "CGObject.h"
#include "CGPiece.h"

TronLightCycle::TronLightCycle()
{
	model = glm::mat4(1.0f);

	mtl[0] = new CGMaterial();
	mtl[0]->SetAmbientReflect(1.0f, 1.0f, 1.0f);
	mtl[0]->SetDifusseReflect(0.400769f, 0.428827f, 0.48313f);
	mtl[0]->SetSpecularReflect(0.5f, 0.5f, 0.5f);
	mtl[0]->SetShininess(250.0f);
	mtl[0]->InitTexture("textures/Grid.jpg");

	mtl[1] = new CGMaterial();
	mtl[1]->SetAmbientReflect(1.0f, 1.0f, 1.0f);
	mtl[1]->SetDifusseReflect(0.8f, 0.426359f, 0.0f);
	mtl[1]->SetSpecularReflect(0.5f, 0.5f, 0.5f);
	mtl[1]->SetShininess(250.0f);

	mtl[2] = new CGMaterial();
	mtl[2]->SetAmbientReflect(1.0f, 1.0f, 1.0f);
	mtl[2]->SetDifusseReflect(0.0f, 0.0f, 0.0f);
	mtl[2]->SetSpecularReflect(0.5f, 0.5f, 0.5f);
	mtl[2]->SetShininess(250.0f);

	mtl[3] = new CGMaterial();
	mtl[3]->SetAmbientReflect(1.0f, 1.0f, 1.0f);
	mtl[3]->SetDifusseReflect(0.0f, 0.0f, 0.0f);
	mtl[3]->SetSpecularReflect(0.5f, 0.5f, 0.5f);
	mtl[3]->SetShininess(250.0f);

	pieces[0] = new TronLightCycle_0(mtl[0]);
}

TronLightCycle::~TronLightCycle()
{
	for (int i = 0; i < 1; i++)
	{
		delete pieces[i];
		pieces[i] = nullptr;
	}
	for (int i = 0; i < 4; i++)
	{
		delete mtl[i];
		mtl[i] = nullptr;
	}
}

int TronLightCycle::GetNumPieces()
{
	return 1;
}

CGPiece* TronLightCycle::GetPiece(int index)
{
	if (index >= 0 && index < 1)
	{
		return pieces[index];
	}
	return nullptr;
}
