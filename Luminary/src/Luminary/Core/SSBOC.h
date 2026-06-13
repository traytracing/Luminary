#pragma once

#include "SSBO.h"

struct cudaGraphicsResource;

class SSBOC : public SSBO {
public:
	~SSBOC();
	SSBOC();

	void Reset();
	bool Resize(GLsizeiptr size) override;

	cudaGraphicsResource* cudassbo{ nullptr };
private:
	bool RegisterWCuda();
	void UnregisterWCuda();
};

