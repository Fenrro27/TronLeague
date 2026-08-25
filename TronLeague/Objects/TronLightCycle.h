#pragma once

#include <GL/glew.h>
#include "CGObject.h"
#include "CGPiece.h"

//
// CLASE: TronLightCycle
//
// DESCRIPCIÓN: Objeto compuesto que ensambla la moto Tron y sus materiales
//
class TronLightCycle : public CGObject {
private:
	CGMaterial* mtl[4] = {nullptr, nullptr, nullptr, nullptr};
	CGPiece* pieces[1] = {nullptr};

public:
	TronLightCycle();
	virtual ~TronLightCycle();
	virtual int GetNumPieces() override;
	virtual CGPiece* GetPiece(int i) override;
};
