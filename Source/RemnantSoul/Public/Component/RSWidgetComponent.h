#pragma once

#include "Components/WidgetComponent.h"
#include "RSWidgetComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTSOUL_API URSWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void InitWidget() override;

};
