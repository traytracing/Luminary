#include "SSBOC.h"
#include <cuda_gl_interop.h>
#include <iostream>

bool SSBOC::RegisterWCuda() {
	cudaError_t regErr = cudaGraphicsGLRegisterBuffer(&cudassbo, ID, cudaGraphicsRegisterFlagsWriteDiscard);

	if (regErr != cudaSuccess) {
		std::cerr << "CUDA Error: cudaGraphicsGLRegisterBuffer : " << cudaGetErrorString(regErr) << std::endl;
		cudassbo = nullptr;
		return false;
	}
	return true;
}
void SSBOC::UnregisterWCuda() {
	if (cudassbo) {
		cudaGraphicsUnregisterResource(cudassbo);
		cudassbo = nullptr;
	}
}

SSBOC::~SSBOC() {
	UnregisterWCuda();
}
SSBOC::SSBOC() {
}

void SSBOC::Reset() {
	UnregisterWCuda();
	SetData(nullptr, 0);
}

bool SSBOC::Resize(GLsizeiptr size) {
	if (size <= 0) {
		Reset();
		return true;
	}

	if (size > Capacity || !cudassbo) {
		UnregisterWCuda();
		SSBO::Resize(size);
		return RegisterWCuda();
	}
	
	return true;
}