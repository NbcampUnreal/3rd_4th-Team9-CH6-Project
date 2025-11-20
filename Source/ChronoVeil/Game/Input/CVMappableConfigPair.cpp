//#include "Game/Input/CVMappableConfigPair.h"
//#include "InputMappingContext.h"
//#include "PlayerMappableInputConfig.h"
//
//void FCVMappableConfigPair::ResolveMappingContexts(
//    TArray<UInputMappingContext*>& OutIMCs,
//    TArray<int32>& OutPriorities) const
//{
//    if (InputMapping.IsValid())
//    {
//        // SoftObject를 실제 객체로 로드
//        if (UInputMappingContext* IMC = InputMapping.LoadSynchronous())
//        {
//            OutIMCs.Add(IMC);
//            OutPriorities.Add(Priority);
//        }
//    }
//
//    if (PlayerMappableConfig.IsValid())
//    {
//        const UPlayerMappableInputConfig* PMIC = PlayerMappableConfig.LoadSynchronous();
//        if (PMIC)
//        {
//            const auto& Contexts = PMIC->GetMappingContexts();
//            for (const auto& Kvp : Contexts)
//            {
//                if (const UInputMappingContext* IMCConst = Kvp.Key.Get())
//                {
//                    OutIMCs.Add(const_cast<UInputMappingContext*>(IMCConst));
//                    OutPriorities.Add(Kvp.Value);
//                }
//            }
//        }
//    }
//}





