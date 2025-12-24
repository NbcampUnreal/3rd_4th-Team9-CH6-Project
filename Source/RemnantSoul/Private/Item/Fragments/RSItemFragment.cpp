#include "Item/Fragments/RSItemFragment.h"

#include "Item/RSItemTemplate.h"
#include "Item/RSItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(RSItemFragment)

URSItemFragment::URSItemFragment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URSItemFragment::OnInstanceCreated(URSItemInstance* Instance) const
{
	// 기본 구현: 아무 것도 하지 않음.
	// 파생 클래스(RSItemFragment_EquipStats 등)에서
	// Instance 초기화/캐시/프리컴퓨트가 필요하면 override해서 사용.
	//
	// 예:
	// - EquipStats Fragment: Instance에 초기 강인도/내구도 값을 셋팅
	// - AbilitySet Fragment: Instance에 "이 인스턴스가 가진 AbilitySet ID"를 기록 등
}

URSItemTemplate* URSItemFragment::GetOwningTemplate() const
{
	// Fragment는 RSItemTemplate의 Instanced Subobject로 붙을 예정이므로
	// Outer를 템플릿으로 캐스팅하면 된다.
	return Cast<URSItemTemplate>(GetOuter());
}

#if WITH_EDITOR
EDataValidationResult URSItemFragment::IsDataValid(FDataValidationContext& Context) const
{
	// 기본적으로는 UObject의 검증 결과를 그대로 사용.
	// 파생 클래스에서 추가 필드 검증이 필요하면 override 후 Super 호출.
	return Super::IsDataValid(Context);
}
#endif
