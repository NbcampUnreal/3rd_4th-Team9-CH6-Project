#include "Item/Fragments/RSItemFragment_WeaponCosmetic.h"

#include "Item/RSItemTemplate.h"
#include "Item/RSItemInstance.h"
#include "Item/RSWeaponActor.h"
#include "Input/RSInputConfig.h"
// 무기 액터가 따로 있다면 여기에 include
// #include "Gimmick/RSWeaponActor.h" 또는 "Item/RSWeaponActor.h" 등 실제 경로로 교체

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

// YKJ Annotation : 
#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemFragment_WeaponCosmetic)

URSItemFragment_WeaponCosmetic::URSItemFragment_WeaponCosmetic(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool URSItemFragment_WeaponCosmetic::IsValidForEquip() const
{
	// 최소한 WeaponActorClass 또는 WeaponMesh 둘 중 하나는 있어야
	// 코스메틱 장착이 의미가 있다. (무기라는 객체가 있어야함. BP로 따로 팀장 정영기가 만들예정.)
	return (WeaponActorClass != nullptr) || (WeaponMesh != nullptr);
}

#if WITH_EDITOR
EDataValidationResult URSItemFragment_WeaponCosmetic::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!WeaponActorClass && !WeaponMesh)
	{
		Context.AddWarning(FText::FromString(
			TEXT("WeaponCosmetic Fragment: WeaponActorClass도 WeaponMesh도 지정되지 않았습니다. 장착 시 아무것도 보이지 않을 수 있습니다.")
		));

		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	if (AttachSocketName.IsNone())
	{
		Context.AddWarning(FText::FromString(
			TEXT("WeaponCosmetic Fragment: AttachSocketName이 비어 있습니다. 기본값 hand_rSocket 사용을 권장합니다.")
		));

		if (Result == EDataValidationResult::NotValidated)
		{
			Result = EDataValidationResult::Valid;
		}
	}

	return Result;

	// if (!WeaponInputConfig)
	// {
	//     Context.AddWarning(FText::FromString(TEXT("WeaponInputConfig is null. Weapon-specific input won't change.")));
	// }
}
#endif
