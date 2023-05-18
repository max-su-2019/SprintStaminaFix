#pragma once
#include "DKUtil/Hook.hpp"

namespace SprintStaminaFix
{
	using namespace DKUtil::Alias;

	class SprintStaminaHook
	{
		static float ResetStaminaCost(float originResult, RE::Actor* a_me)
		{
			static auto IsGVTrue = [a_me](const std::string& varName) -> bool {
				bool result = false;
				return a_me->GetGraphVariableBool(varName, result) && result;
			};

			if (a_me && (IsGVTrue("MCO_InSprintAttack") || IsGVTrue("MCO_InSprintPowerAttack")))
				return 0.f;

			return originResult;
		}

		// 1-5-97-0 @ 0x14060E820
		static constexpr std::uintptr_t SE_FuncID = 36994;
		static constexpr std::ptrdiff_t SE_OffsetL = 0xC9;
		static constexpr std::ptrdiff_t SE_OffsetH = 0xCE;

	public:
		static void InstallHook()
		{
			SKSE::AllocTrampoline(1 << 6);

			auto funcAddr = REL::ID(SE_FuncID).address();

			Patch RelocatePointer{
				//mov rdx,rdi
				"\x48\x89\xFA",
				3
			};

			auto handle = DKUtil::Hook::AddCaveHook(
				REL::ID(SE_FuncID).address(),
				std::make_pair(SE_OffsetL, SE_OffsetH),
				FUNC_INFO(ResetStaminaCost),
				&RelocatePointer,
				nullptr,
				HookFlag::kRestoreBeforeProlog);

			// recalc disp
			Disp32 disp = *std::bit_cast<Disp32*>(AsPointer(handle->CaveEntry + 0x1));
			DKUtil::Hook::WriteImm(handle->TramEntry + 0x1, static_cast<Disp32>(handle->CaveEntry + disp - handle->TramEntry));

			handle->Enable();

			INFO("{} Done!", __FUNCTION__);
		}
	};
}