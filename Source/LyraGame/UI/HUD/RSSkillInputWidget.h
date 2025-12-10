#pragma once

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "RSSkillInputWidget.generated.h"

USTRUCT(BlueprintType)
struct FSkillInputInitializeMessage
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	bool bShouldShow = false;
};

UCLASS()
class URSSkillInputWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URSSkillInputWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void ConstructUI(FGameplayTag Channel, const FSkillInputInitializeMessage& Message);

protected:
	UPROPERTY(EditAnywhere, meta=(Categories="Message"))
	FGameplayTag MessageChannelTag;
	
private:
	FGameplayMessageListenerHandle ListenerHandle;
};
