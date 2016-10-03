/******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2014 Fredrik Lindh
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
******************************************************************************/

#pragma once

#include "ModuleManager.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "SharedPointer.h"
#include "Internationalization.h"
#include "SlateBasics.h"
#include "MultiBoxExtender.h"

#include "RenderDocAPI/renderdoc_app.h"

DECLARE_LOG_CATEGORY_EXTERN(RenderDocPlugin, Log, All);

class FRenderDocLoaderPluginModule// : public IModuleInterface
{
public:
	void StartupModule(class FRenderDocPluginModule* Plugin);
	void ShutdownModule();

	typedef RENDERDOC_API_1_0_0 RENDERDOC_API_CONTEXT;

private:
	friend class FRenderDocPluginModule;
	friend class SRenderDocPluginSettingsEditorWindow;

	void* RenderDocDLL;

	RENDERDOC_API_CONTEXT* RenderDocAPI;
};
