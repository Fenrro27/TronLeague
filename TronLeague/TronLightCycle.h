#pragma once

#include <GL/glew.h>
#include "CGObject.h"
#include "CGPiece.h"

class TronLightCycle : public CGObject {
private:
	CGMaterial* mtl[4];
	CGPiece* pieces[1];

public:
	TronLightCycle();
	~TronLightCycle();
	virtual int GetNumPieces();
	virtual CGPiece* GetPiece(int i);

};

