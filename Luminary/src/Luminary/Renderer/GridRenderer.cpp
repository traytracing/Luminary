#include "GridRenderer.h"

GridRenderer::GridRenderer(Camera& POVCamera) : POVCamera(POVCamera)
{
}

GridRenderer::~GridRenderer()
{
}

void GridRenderer::Render() {
	glEnable(GL_DEPTH_TEST);	
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	gridRenderProgram.Activate();
	POVCamera.UploadPosition(gridRenderProgram, "camPos");
	POVCamera.UploadMatrix(gridRenderProgram, "MVP");

	gridVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}