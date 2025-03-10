// License
// This Plugin is created @Author: VicAgent Marz 2025
// This project is licensed under the [MIT License] (https://mit-license.org/).


#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FHealthHandlerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
