#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RSItemInstance.generated.h"

class URSItemTemplate;
class URSCombatStyleData;

/**
 * RSItemInstance
 *
 * - RSItemTemplate의 "한 스택"을 표현하는 런타임 객체
 * - 템플릿 + 개수 + 소유 Actor 정도만 들고 있는 얇은 래퍼
 * - 인벤토리/장비 시스템에서 공통으로 사용
 *
 * - 설계 포인트:
 *   - 템플릿은 "정적 데이터"
 *   - 인스턴스는 "Count, Owner, 향후 Durability/강화 단계" 등 가변 데이터
 */
UCLASS(BlueprintType)
class REMNANTSOUL_API URSItemInstance : public UObject
{
	GENERATED_BODY()

public:
	URSItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 템플릿, 개수, 소유 Actor로 인스턴스를 초기화 */
	UFUNCTION(BlueprintCallable, Category = "RS|Item")
	void InitializeFromTemplate(const URSItemTemplate* InTemplate, int32 InCount, AActor* InOwningActor);

	UFUNCTION(BlueprintPure, Category = "RS|Item")
	const URSItemTemplate* GetTemplate() const { return ItemTemplate; }

	/** 템플릿을 특정 타입으로 캐스팅해서 가져오는 헬퍼 (예: URSItemTemplate_Weapon) */
	template<typename TTemplateClass>
	const TTemplateClass* GetTemplateTyped() const
	{
		return Cast<TTemplateClass>(ItemTemplate.Get());
	}

	/** 현재 스택 개수 */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	int32 GetCount() const { return Count; }

	/** 개수 설정 (0 이하로 내려가면 0으로 고정) */
	UFUNCTION(BlueprintCallable, Category = "RS|Item")
	void SetCount(int32 NewCount);

	/** 개수 증감 (Delta는 음수도 허용) */
	UFUNCTION(BlueprintCallable, Category = "RS|Item")
	void AddCount(int32 Delta);

	/** 이 인스턴스의 최대 스택 개수 (템플릿 기준, 최소 1) */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	int32 GetMaxStackCount() const;

	/** 최대 스택인지 여부 */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	bool IsStackFull() const;

	/** 이 인스턴스를 소유한 Actor (보통 RSCharacter) */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	AActor* GetOwningActor() const { return OwningActor.Get(); }

	/** 인스턴스가 유효한(템플릿/카운트>0) 상태인지 간단 체크 */
	UFUNCTION(BlueprintPure, Category = "RS|Item")
	bool IsValidInstance() const;

protected:
	/** 정적 데이터(아이콘/이름/Fragment 등)를 가진 템플릿 */
	UPROPERTY()
	TObjectPtr<const URSItemTemplate> ItemTemplate;

	/** 현재 스택 개수 */
	UPROPERTY()
	int32 Count = 0;

	/** 이 아이템 인스턴스를 소유한 Actor (Pawn/Character 등) */
	UPROPERTY()
	TWeakObjectPtr<AActor> OwningActor;


#pragma region 
public:
	const URSCombatStyleData* GetItemCombatStyle() const;

protected:
	UPROPERTY()
	TObjectPtr<const URSCombatStyleData> CombatStyle;

#pragma endregion

};
