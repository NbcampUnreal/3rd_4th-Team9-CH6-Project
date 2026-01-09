#pragma once

#include "CoreMinimal.h"
#include "UI/RSUserWidget.h"
#include "AbilitySystemComponent.h"
#include "RSPlayerVitalsWidget.generated.h"

class UProgressBar;
struct FOnAttributeChangeData; // 전방선언 OK (UFUNCTION이 아니므로 가능)

UCLASS()
class REMNANTSOUL_API URSPlayerVitalsWidget : public URSUserWidget
{
	GENERATED_BODY()

protected:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	void UpdateStaminaBar();

	// UFUNCTION 제거: UHT가 모르는 타입을 파라미터로 쓰면 안 됨
	void OnStaminaChanged(const FOnAttributeChangeData& ChangeData);
	void OnMaxStaminaChanged(const FOnAttributeChangeData& ChangeData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Stamina;

	float CurrentStamina = 0.f;
	float CurrentMaxStamina = 1.f;
};
