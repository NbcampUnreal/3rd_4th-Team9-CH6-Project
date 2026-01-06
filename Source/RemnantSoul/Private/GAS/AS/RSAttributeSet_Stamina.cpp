// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AS/RSAttributeSet_Stamina.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "RSGameplayTags.h"

URSAttributeSet_Stamina::URSAttributeSet_Stamina()
{
	// 기본값 (프로젝트 밸런스에 맞게 조정)
	InitMaxStamina(100.f);
	InitStamina(GetMaxStamina());

	InitStaminaRegenRate(20.f);     // 초당 회복량(예시)
	InitStaminaRegenDelay(0.35f);   // 소비 후 리젠 재개 딜레이(예시)
}

void URSAttributeSet_Stamina::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);

		// MaxStamina가 줄어들면 현재 Stamina도 같이 Clamp되어야 하므로
		// 여기서는 Max만 보정, 실제 Clamp는 PostChange/PostExecute에서 안전하게 처리
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetStaminaRegenRateAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetStaminaRegenDelayAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void URSAttributeSet_Stamina::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxStaminaAttribute())
	{
		ClampStamina();
		OnMaxStaminaChanged.Broadcast(OldValue, NewValue);
	}
	else if (Attribute == GetStaminaAttribute())
	{
		OnStaminaChanged.Broadcast(OldValue, NewValue);
	}
}

void URSAttributeSet_Stamina::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		ClampMaxStamina();
		ClampStamina();
		return;
	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		// 소비/회복 모두 이 경로로 들어옴
		ClampStamina();

		// 소비(감소)였다면 리젠 딜레이 처리(선택)
		HandleRegenDelayOnSpend(Data);
		return;
	}
}

void URSAttributeSet_Stamina::ClampStamina()
{
	const float NewValue = FMath::Clamp(GetStamina(), 0.f, GetMaxStamina());
	SetStamina(NewValue);
}

void URSAttributeSet_Stamina::ClampMaxStamina()
{
	const float NewValue = FMath::Max(GetMaxStamina(), 0.f);
	SetMaxStamina(NewValue);
}

void URSAttributeSet_Stamina::HandleRegenDelayOnSpend(const FGameplayEffectModCallbackData& Data)
{
	// "소비"인지 판단: 적용된 Magnitude가 음수면 감소일 가능성이 높다.
	// 단, 프로젝트에 따라 Additive/Override 구성에 따라 달라질 수 있으니,
	// 가장 안전한 방법은 "스태미나 소비 GE에 태그를 달고 그 태그로 판정"하는 것이다.
	// 여기서는 최소 구현으로 magnitude 기반 + (선택) 태그 훅을 같이 둔다.

	const float Magnitude = Data.EvaluatedData.Magnitude;

	if (Magnitude >= 0.f)
	{
		return; // 회복/증가면 딜레이 불필요
	}

	UAbilitySystemComponent* ASC = Data.Target.AbilityActorInfo.IsValid()
		? Data.Target.AbilityActorInfo->AbilitySystemComponent.Get()
		: nullptr;

	if (!ASC)
	{
		return;
	}

	// 리젠 딜레이를 "태그로 차단"하는 방식(추천)
	// - Tick/GA에서 "리젠 허용 여부"를 태그로 확인 가능
	// - 실제 리젠(회복)은 별도 주기 GE나, Attribute 기반 계산에서 처리
	TryAddRegenBlockTag(ASC);

	const float Delay = GetStaminaRegenDelay();
	if (Delay <= 0.f)
	{
		TryRemoveRegenBlockTag(ASC);
		return;
	}

	// 타이머는 ASC의 World를 사용
	UWorld* World = ASC->GetWorld();
	if (!World)
	{
		return;
	}

	FTimerHandle LocalHandle;
	World->GetTimerManager().SetTimer(
		LocalHandle,
		FTimerDelegate::CreateUObject(
			const_cast<URSAttributeSet_Stamina*>(this),
			&URSAttributeSet_Stamina::TryRemoveRegenBlockTag,
			ASC),
		Delay,
		false);
}

void URSAttributeSet_Stamina::TryAddRegenBlockTag(UAbilitySystemComponent* ASC) const
{
	if (!ASC)
	{
		return;
	}

	// 프로젝트 태그 네이밍에 맞춰 추가 권장:
	// 예: Status.Stamina.RegenBlocked 같은 태그를 RSGameplayTags에 등록해두고 사용
	// 지금은 "없을 수" 있으니 안전하게 조건 처리
	// 아래 라인은 네가 태그를 추가한 뒤에 활성화해라.

	// const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	// ASC->AddLooseGameplayTag(Tags.Status_Stamina_RegenBlocked);

	bRegenBlocked = true;
}

void URSAttributeSet_Stamina::TryRemoveRegenBlockTag(UAbilitySystemComponent* ASC) const
{
	if (!ASC)
	{
		return;
	}

	// const FRSGameplayTags& Tags = FRSGameplayTags::Get();
	// ASC->RemoveLooseGameplayTag(Tags.Status_Stamina_RegenBlocked);

	bRegenBlocked = false;
}

void URSAttributeSet_Stamina::OnRep_Stamina(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URSAttributeSet_Stamina, Stamina, OldValue);
	OnStaminaChanged.Broadcast(OldValue.GetCurrentValue(), Stamina.GetCurrentValue());
}

void URSAttributeSet_Stamina::OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URSAttributeSet_Stamina, MaxStamina, OldValue);
	OnMaxStaminaChanged.Broadcast(OldValue.GetCurrentValue(), MaxStamina.GetCurrentValue());
}

void URSAttributeSet_Stamina::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URSAttributeSet_Stamina, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URSAttributeSet_Stamina, MaxStamina, COND_None, REPNOTIFY_Always);
}