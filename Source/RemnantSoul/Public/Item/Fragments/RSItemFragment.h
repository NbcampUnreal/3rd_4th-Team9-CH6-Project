#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RSItemFragment.generated.h"

class URSItemTemplate;
class URSItemInstance;

/**
 * RSItemFragment
 *
 * - RSItemTemplate에 "조합"해서 붙이는 기능 단위 베이스 클래스
 * - 예시 Fragment:
 *   - RSItemFragment_EquipStats        : 공격력/사거리/방어력 등 스탯 보정
 *   - RSItemFragment_AbilitySet        : 장착 시 부여할 GameplayAbilitySet
 *   - RSItemFragment_WeaponCosmetic    : 무기 액터/소켓/애님/메시 정보
 *   - RSItemFragment_EquipRequirement  : 레벨/태그/스탯 요구 조건
 *
 * - 설계 포인트:
 *   - 템플릿의 Fragments 배열에 Instanced/Inline으로 붙는다.
 *   - Template->CreateItemInstance(...) 호출 시 OnInstanceCreated(...)가 호출되어
 *     인스턴스 초기화에 관여할 수 있다.
 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced, CollapseCategories)
class REMNANTSOUL_API URSItemFragment : public UObject
{
	GENERATED_BODY()

public:
	URSItemFragment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 이 Fragment를 가진 템플릿에서 ItemInstance가 생성될 때 호출되는 훅 */
	virtual void OnInstanceCreated(URSItemInstance* Instance) const;

	/** 이 Fragment의 Outer를 URSItemTemplate로 캐스팅해서 반환 (없으면 nullptr) */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	URSItemTemplate* GetOwningTemplate() const;

	/** C++에서 편하게 캐스팅해서 쓰고 싶을 때를 위한 헬퍼 */
	template<typename TTemplateClass>
	TTemplateClass* GetOwningTemplateTyped() const
	{
		return Cast<TTemplateClass>(GetOwningTemplate());
	}

#if WITH_EDITOR
	/** Fragment 개별 데이터 검증용 (원하면 파생 클래스에서 override) */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
