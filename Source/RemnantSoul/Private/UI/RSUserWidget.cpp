#include "UI/RSUserWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "RemnantSoul.h"
#include "AbilitySystemComponent.h"

void URSUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
    ASC = nullptr;
    if (!IsValid(InOwner))
    {
        return;
    }

    // 1) 우선 InOwner 자체에서 ASC 시도
    ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
    if (ASC)
    {
        return;
    }

    // 2) InOwner가 PC면 Pawn(Avatar)에서 ASC 재시도
    if (APlayerController* PC = Cast<APlayerController>(InOwner))
    {
        APawn* Pawn = PC->GetPawn();
        if (Pawn)
        {
            ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
        }
        return;
    }

    // 3) InOwner가 Pawn이 아니고, Owner가 Pawn인 케이스(예: Component/WidgetOwner가 다른 액터)
    if (AActor* Owner = InOwner->GetOwner())
    {
        ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
    }
}

UAbilitySystemComponent* URSUserWidget::GetAbilitySystemComponent() const
{
	return ASC;
}