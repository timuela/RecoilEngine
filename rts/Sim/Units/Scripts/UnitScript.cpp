/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

/* heavily based on CobInstance.cpp */
#include "UnitScript.h"

#include <utility>

#include "CobDefines.h"
#include "CobFile.h"
#include "CobInstance.h"
#include "UnitScriptEngine.h"
#include "AnimComponents.hpp"

#ifndef _CONSOLE

#include "Game/GameHelper.h"
#include "Game/GlobalUnsynced.h"
#include "Map/Ground.h"
#include "Sim/Misc/GlobalSynced.h"
#include "Sim/Misc/GroundBlockingObjectMap.h"
#include "Sim/Misc/LosHandler.h"
#include "Sim/Misc/TeamHandler.h"
#include "Sim/MoveTypes/AAirMoveType.h"
#include "Sim/MoveTypes/GroundMoveType.h"
#include "Sim/MoveTypes/MoveDefHandler.h"
#include "Sim/MoveTypes/MoveType.h"
#include "Sim/Projectiles/ExplosionGenerator.h"
#include "Sim/Projectiles/PieceProjectile.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Projectiles/ProjectileMemPool.h"
#include "Rendering/Env/Particles/Classes/BubbleProjectile.h"
#include "Rendering/Env/Particles/Classes/HeatCloudProjectile.h"
#include "Rendering/Env/Particles/Classes/MuzzleFlame.h"
#include "Rendering/Env/Particles/Classes/SmokeProjectile.h"
#include "Rendering/Env/Particles/Classes/WakeProjectile.h"
#include "Rendering/Env/Particles/Classes/WreckProjectile.h"
#include "Rendering/Models/3DModel.hpp"
#include "Rendering/Models/3DModelPiece.hpp"
#include "Sim/Units/CommandAI/CommandAI.h"
#include "Sim/Units/CommandAI/Command.h"
#include "Sim/Units/UnitTypes/Factory.h"
#include "Sim/Units/UnitDef.h"
#include "Sim/Units/Unit.h"
#include "Sim/Units/UnitHandler.h"
#include "Sim/Weapons/PlasmaRepulser.h"
#include "Sim/Weapons/Weapon.h"
#include "Sim/Weapons/WeaponDef.h"
#include "System/FastMath.h"
#include "System/SpringMath.h"
#include "System/Log/ILog.h"
#include "System/StringUtil.h"
#include "System/Sound/ISoundChannels.h"
#include "System/TemplateUtils.hpp"

#include "System/Misc/TracyDefs.h"

#endif

CR_BIND_INTERFACE(CUnitScript)

CR_REG_METADATA(CUnitScript, (
	CR_MEMBER(unit),
	CR_MEMBER(busy),
	CR_MEMBER(anims),
	CR_MEMBER(doneAnims),

	//Populated by children
	CR_IGNORED(pieces),
	CR_IGNORED(hasSetSFXOccupy),
	CR_IGNORED(hasRockUnit),
	CR_IGNORED(hasStartBuilding)
))

CR_BIND(CUnitScript::AnimInfo,)

CR_REG_METADATA_SUB(CUnitScript, AnimInfo,(
	CR_MEMBER(axis),
	CR_MEMBER(piece),
	CR_MEMBER(speed),
	CR_MEMBER(dest),
	CR_MEMBER(accel),
	CR_MEMBER(done),
	CR_MEMBER(hasWaiting)
))

/******************************************************************************/

namespace Impl {
	// Helper: executes a function on a AnimComponentTuple element at a specific index
	template<typename Func>
	void exec_anim_type_axis(int animType, int animAxis, Func&& f)
	{
		static constexpr AnimComponentTuple tup;

		const size_t idx = animType * AnimAxisCount + animAxis;
#ifdef _DEBUG
		spring::tuple_exec_at(idx, tup, [&f, at = animType, ax = animAxis](auto&& t) {
			using AnimInfoType = std::decay_t<decltype(t)>;

			static constexpr auto animType = AnimInfoType::animType;
			static constexpr auto animAxis = AnimInfoType::animAxis;

			assert(at == animType);
			assert(ax == animAxis);

			f(std::forward<decltype(t)>(t));
		});
#else
		spring::tuple_exec_at(idx, tup, f);
#endif
	}

	template<typename AnimInfoType>
	void RemoveTypedAnim(CUnitScript& self, LocalModelPieceEntity& lmpe, AnimInfoType* ai, int piece) {
		assert(ai);

		static constexpr auto animType = AnimInfoType::animType;
		static constexpr auto animAxis = AnimInfoType::animAxis;

		static_assert(0 <= animType && animType < AnimTypeCount, "Unknown animation type");
		static_assert(0 <= animAxis && animAxis < AnimAxisCount, "Unknown animation axis");

		// We need to unblock threads waiting on this animation, otherwise they will be lost in the void
		// NOTE: AnimFinished might result in new anims being added
		if (ai->hasWaiting) {
			self.AnimFinished(static_cast<AnimType>(animType), piece, animAxis);
		}

		lmpe.Remove<AnimInfoType>();
	}

	template<typename AnimInfoType>
	void AddTypedAnim(CUnitScript& self, LocalModelPiece* lmp, LocalModelPieceEntity& lmpe, int piece, float speed, float dest, float accel) {
		static constexpr auto animType = AnimInfoType::animType;
		static constexpr auto animAxis = AnimInfoType::animAxis;

		static_assert(0 <= animType && animType < AnimTypeCount, "Unknown animation type");
		static_assert(0 <= animAxis && animAxis < AnimAxisCount, "Unknown animation axis");

		float destf = 0.0f;

		if constexpr (animType == AMove) {
			destf = lmp->original->offset[animAxis] + dest;
		}
		else if constexpr (animType == ATurn) {
			// clamp destination (angle) for turn-anims
			destf = ClampRad(dest);

			using AnimSpinComponentType = AnimInfoBase<ASpin, animAxis>;
			// search for existing spin animation to remove it
			if (auto* ai = lmpe.TryGet<AnimSpinComponentType>(); ai != nullptr) {
				Impl::RemoveTypedAnim(self, lmpe, ai, piece);
			}
		}
		else if constexpr (animType == ASpin) {
			destf = dest;

			using AnimTurnComponentType = AnimInfoBase<ATurn, animAxis>;
			// search for existing turn animation to remove it
			if (auto* ai = lmpe.TryGet<AnimTurnComponentType>(); ai != nullptr) {
				Impl::RemoveTypedAnim(self, lmpe, ai, piece);
			}
		}
		else {
			static_assert(Recoil::always_false_v<AnimInfoType>, "Unknown animation type");
		}

		// search or emplace the requested animation type
		auto& ai = lmpe.GetOrAdd<AnimInfoType>();
		ai.dest = destf;
		ai.speed = speed;
		ai.accel = accel;
		ai.done = false;
	}

	template<typename AnimInfoType>
	void AddTypedAnim(CUnitScript& self, int piece, float speed, float dest, float accel)
	{
		auto* lmp = self.GetScriptLocalModelPieceSafe(piece);

		if (!lmp) {
			self.ShowUnitScriptError("[US::AddTypedAnim] invalid script piece index");
			return;
		}

		auto& lmpe = lmp->GetLocalModelPieceEntity();
		AddTypedAnim<AnimInfoType>(self, lmp, lmpe, piece, speed, dest, accel);
	}
}


CUnitScript::CUnitScript(CUnit* unit)
	: unit(unit)
	, busy(false)
	, hasSetSFXOccupy(false)
	, hasRockUnit(false)
	, hasStartBuilding(false)
{}

void CUnitScript::RemoveAnim(AnimType type, int piece, int axis)
{
	RECOIL_DETAILED_TRACY_ZONE;

	auto* lmp = GetScriptLocalModelPieceSafe(piece);
	if (!lmp) {
		ShowUnitScriptError("[US::RemoveAnim] invalid script piece index");
		return;
	}

	Impl::exec_anim_type_axis(type, axis, [&](auto&& t) {
		using AnimInfoType = std::decay_t<decltype(t)>;

		static constexpr auto animType = AnimInfoType::animType;
		static constexpr auto animAxis = AnimInfoType::animAxis;

		static_assert(0 <= animType && animType < AnimTypeCount, "Unknown animation type");
		static_assert(0 <= animAxis && animAxis < AnimAxisCount, "Unknown animation axis");

		auto& lmpe = lmp->GetLocalModelPieceEntity();

		if (auto* ai = lmpe.TryGet<AnimInfoType>(); ai != nullptr) {
			Impl::RemoveTypedAnim(*this, lmpe, ai, piece);
		}
	});
}


//Overwrites old information. This means that threads blocking on turn completion
//will now wait for this new turn instead. Not sure if this is the expected behaviour
//Other option would be to kill them. Or perhaps unblock them.
void CUnitScript::AddAnim(AnimType type, int piece, int axis, float speed, float dest, float accel)
{
	RECOIL_DETAILED_TRACY_ZONE;
	Impl::exec_anim_type_axis(type, axis, [&](auto&& t) {
		using AnimInfoType = std::decay_t<decltype(t)>;
		Impl::AddTypedAnim<AnimInfoType>(*this, piece, speed, dest, accel);
	});
}

bool CUnitScript::IsInAnimation(AnimType type, int piece, int axis) const
{
	bool isInAnim = false;

	Impl::exec_anim_type_axis(type, axis, [&](auto&& t) {
		using AnimInfoType = std::decay_t<decltype(t)>;

		auto* lmp = GetScriptLocalModelPieceSafe(piece);

		if (!lmp) {
			ShowUnitScriptError("[US::IsInAnimation] invalid script piece index");
			return;
		}

		auto& lmpe = lmp->GetLocalModelPieceEntity();

		isInAnim = lmpe.Has<AnimInfoType>();
	});

	return isInAnim;
}

//Returns true if there was an animation to listen to
bool CUnitScript::NeedsWait(AnimType type, int piece, int axis) const
{
	RECOIL_DETAILED_TRACY_ZONE;

	bool needsWait = false;

	Impl::exec_anim_type_axis(type, axis, [&](auto&& t) {
		using AnimInfoType = std::decay_t<decltype(t)>;

		auto* lmp = GetScriptLocalModelPieceSafe(piece);

		if (!lmp) {
			ShowUnitScriptError("[US::NeedsWait] invalid script piece index");
			return;
		}

		auto& lmpe = lmp->GetLocalModelPieceEntity();

		auto* ai = lmpe.TryGet<AnimInfoType>();
		if (!ai)
			return;

		// if the animation is already finished, listening for
		// it just adds some overhead since either the current
		// or the next Tick will remove it and call UnblockAll
		// (which calls AnimFinished for each listener)
		//
		// we could notify the listener here, but a cleaner way
		// is to treat the animation as if it did not exist and
		// simply disregard the WaitFor* (no side-effects)
		//
		// if (ai.hasWaiting)
		// 		AnimFinished(ai.type, ai.piece, ai.axis);
		if (!ai->done) {
			ai->hasWaiting = true;
			needsWait = true;
		}
	});

	return needsWait;
}

void CUnitScript::Spin(int piece, int axis, float speed, float accel)
{
	RECOIL_DETAILED_TRACY_ZONE;

	Impl::exec_anim_type_axis(ASpin, axis, [&](auto&& t) {
		using AnimInfoType = std::decay_t<decltype(t)>;

		auto* lmp = GetScriptLocalModelPieceSafe(piece);
		if (!lmp) {
			ShowUnitScriptError("[US::Spin] invalid script piece index");
			return;
		}

		auto& lmpe = lmp->GetLocalModelPieceEntity();

		// if we are already spinning, we may have to decelerate to the new speed
		if (auto* ai = lmpe.TryGet<AnimInfoType>(); ai != nullptr) {
			ai->dest = speed;

			if (accel > 0.0f) {
				ai->accel = accel;
			}
			else {
				// Go there instantly. Or have a default accel?
				ai->speed = speed;
				ai->accel = 0.0f;
			}

			return;
		}

		// no acceleration means we start at desired speed instantly
		if (accel <= 0.0f) {
			Impl::AddTypedAnim<AnimInfoType>(*this, lmp, lmpe, piece, speed, speed, 0.0f);
		}
		else {
			Impl::AddTypedAnim<AnimInfoType>(*this, lmp, lmpe, piece, 0.0f, speed, accel);
		}
	});
}


void CUnitScript::StopSpin(int piece, int axis, float decel)
{
	RECOIL_DETAILED_TRACY_ZONE;

	if (decel <= 0.0f) {
		RemoveAnim(ASpin, piece, axis);
	} else {
		Impl::exec_anim_type_axis(ASpin, axis, [&](auto&& t) {
			using AnimInfoType = std::decay_t<decltype(t)>;

			static constexpr auto animType = AnimInfoType::animType;
			static constexpr auto animAxis = AnimInfoType::animAxis;

			static_assert(0 <= animType && animType < AnimTypeCount, "Unknown animation type");
			static_assert(0 <= animAxis && animAxis < AnimAxisCount, "Unknown animation axis");

			auto* lmp = GetScriptLocalModelPieceSafe(piece);
			if (!lmp) {
				ShowUnitScriptError("[US::StopSpin] invalid script piece index");
				return;
			}

			auto& lmpe = lmp->GetLocalModelPieceEntity();
			auto& ai = lmpe.GetOrAdd<AnimInfoType>();

			ai.dest = 0.0f;
			ai.accel = decel;
		});
	}
}


void CUnitScript::Turn(int piece, int axis, float speed, float destination)
{
	RECOIL_DETAILED_TRACY_ZONE;
	AddAnim(ATurn, piece, axis, math::fabs(speed), ClampRad(destination), 0);
}


void CUnitScript::Move(int piece, int axis, float speed, float destination)
{
	RECOIL_DETAILED_TRACY_ZONE;
	AddAnim(AMove, piece, axis, math::fabs(speed), destination, 0);
}


void CUnitScript::MoveNow(int piece, int axis, float destination)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!PieceExists(piece)) {
		ShowUnitScriptError("[US::MoveNow] invalid script piece index");
		return;
	}

	LocalModelPiece* p = pieces[piece];

	float3 pos = p->GetPosition();
	float3 ofs = p->original->offset;

	pos[axis] = ofs[axis] + destination;

	p->SetPosition(pos);
	p->SetPositionNoInterpolation(true);
}


void CUnitScript::TurnNow(int piece, int axis, float destination)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!PieceExists(piece)) {
		ShowUnitScriptError("[US::TurnNow] invalid script piece index");
		return;
	}
	destination = ClampRad(destination);

	LocalModelPiece* p = pieces[piece];

	float3 rot = p->GetRotation();
	rot[axis] = destination;

	p->SetRotation(rot);
	p->SetRotationNoInterpolation(true);
}


void CUnitScript::SetVisibility(int piece, bool visible)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!PieceExists(piece)) {
		ShowUnitScriptError("[US::SetVisibility] invalid script piece index");
		return;
	}

	pieces[piece]->SetScriptVisible(visible);
}


bool CUnitScript::EmitSfx(int sfxType, int sfxPiece)
{
	RECOIL_DETAILED_TRACY_ZONE;
#ifndef _CONSOLE
	if (!PieceExists(sfxPiece)) {
		ShowUnitScriptError("[US::EmitSFX] invalid script piece index");
		return false;
	}

	float3 relPos = ZeroVector;
	float3 relDir = UpVector;

	if (!GetEmitDirPos(sfxPiece, relPos, relDir)) {
		ShowUnitScriptError("[US::EmitSFX] invalid model piece index");
		return false;
	}

	return (EmitRelSFX(sfxType, relPos, relDir.SafeNormalize()));
#endif
}

bool CUnitScript::EmitRelSFX(int sfxType, const float3& relPos, const float3& relDir)
{
	RECOIL_DETAILED_TRACY_ZONE;
	// convert piece-space {pos,dir} to unit-space
	return (EmitAbsSFX(sfxType, unit->GetObjectSpacePos(relPos), unit->GetObjectSpaceVec(relDir), relDir));
}

bool CUnitScript::EmitAbsSFX(int sfxType, const float3& absPos, const float3& absDir, const float3& relDir)
{
	RECOIL_DETAILED_TRACY_ZONE;
	// skip adding (non-CEG) particles when we have too many
	if (sfxType < SFX_CEG && projectileHandler.GetParticleSaturation() > 1.0f)
		return false;

	// make sure wakes are only emitted on water
	if (sfxType >= SFX_WAKE && sfxType <= SFX_REVERSE_WAKE_2 && !unit->IsInWater())
		return false;

	float wakeAlphaStart = 0.3f + guRNG.NextFloat() * 0.2f;
	float wakeAlphaDecay = 0.004f;
	float wakeFadeupTime = 4.0f;

	const UnitDef* ud = unit->unitDef;
	const MoveDef* md = unit->moveDef;

	// hovercraft need special care (?)
	if (md != nullptr && md->speedModClass == MoveDef::Hover) {
		wakeAlphaStart = 0.15f + guRNG.NextFloat() * 0.2f;
		wakeAlphaDecay = 0.008f;
		wakeFadeupTime = 8.0f;
	}

	switch (sfxType) {
		// reverse ship wake
		case SFX_REVERSE_WAKE:
		case SFX_REVERSE_WAKE_2: {
			projMemPool.alloc<CWakeProjectile>(
				unit,
				absPos + guRNG.NextVector() * 2.0f,
				absDir * -0.4f,
				6.0f + guRNG.NextFloat() * 4.0f,
				0.15f + guRNG.NextFloat() * 0.3f,
				wakeAlphaStart, wakeAlphaDecay, wakeFadeupTime
			);
		} break;

		// regular ship wake
		case SFX_WAKE_2:
		case SFX_WAKE: {
			projMemPool.alloc<CWakeProjectile>(
				unit,
				absPos + guRNG.NextVector() * 2.0f,
				absDir * 0.4f,
				6.0f + guRNG.NextFloat() * 4.0f,
				0.15f + guRNG.NextFloat() * 0.3f,
				wakeAlphaStart, wakeAlphaDecay, wakeFadeupTime
			);
		} break;

		// submarine bubble; does not provide direction through piece vertices
		case SFX_BUBBLE: {
			const float3 pspeed = guRNG.NextVector() * 0.1f;

			projMemPool.alloc<CBubbleProjectile>(
				unit,
				absPos + guRNG.NextVector() * 2.0f,
				pspeed + UpVector * 0.2f,
				40.0f + guRNG.NextFloat() * GAME_SPEED,
				1.0f + guRNG.NextFloat() * 2.0f,
				0.01f,
				0.3f + guRNG.NextFloat() * 0.3f
			);
		} break;

		// damaged unit smoke
		case SFX_WHITE_SMOKE: {
			projMemPool.alloc<CSmokeProjectile>(unit, absPos, guRNG.NextVector() * 0.5f + UpVector * 1.1f, 60, 4, 0.5f, 0.5f);
		} break;
		case SFX_BLACK_SMOKE: {
			projMemPool.alloc<CSmokeProjectile>(unit, absPos, guRNG.NextVector() * 0.5f + UpVector * 1.1f, 60, 4, 0.5f, 0.6f);
		} break;

		case SFX_VTOL: {
			const float4 scale = {0.5f, 0.5f, 0.5f, 0.7f};
			const float3 speed =
				unit->speed    * scale.w +
				unit->frontdir * scale.z *             relDir.z  +
				unit->updir    * scale.y * -math::fabs(relDir.y) +
				unit->rightdir * scale.x *             relDir.x;

			CHeatCloudProjectile* hc = projMemPool.alloc<CHeatCloudProjectile>(
				unit,
				absPos,
				speed,
				10.0f + guRNG.NextFloat() * 5.0f,
				3.0f + guRNG.NextFloat() * 2.0f
			);

			hc->size = 3;
		} break;

		default: {
			if ((sfxType & SFX_GLOBAL) != 0) {
				// emit defined explosion-generator (can only be custom, not standard)
				// index is made valid by callee, an ID of -1 means CEG failed to load
				explGenHandler.GenExplosion(
					sfxType - SFX_GLOBAL,
					absPos,
					absDir,
					unit->cegDamage,
					1.0f,
					0.0f,
					unit,
					ExplosionHitObject()
				);
				return true;
			}
			if ((sfxType & SFX_CEG) != 0) {
				// emit defined explosion-generator (can only be custom, not standard)
				// index is made valid by callee, an ID of -1 means CEG failed to load
				explGenHandler.GenExplosion(
					ud->GetModelExpGenID(sfxType - SFX_CEG),
					absPos,
					absDir,
					unit->cegDamage,
					1.0f,
					0.0f,
					unit,
					ExplosionHitObject()
				);
				return true;
			}

			if ((sfxType & SFX_FIRE_WEAPON) != 0) {
				// make a weapon fire from the piece
				const unsigned int index = sfxType - SFX_FIRE_WEAPON;

				if (index >= unit->weapons.size()) {
					ShowUnitScriptError("[US::EmitSFX::FIRE_WEAPON] invalid weapon index");
					break;
				}

				CWeapon* w = unit->weapons[index];

				const SWeaponTarget origTarget = w->GetCurrentTarget();

				// Ideally the below should be absPos + absDir * w->range,
				// but since the weapon could be a ballistic one, it's likely not safe.
				// So keep the target one elmo in front of the emit position
				const SWeaponTarget emitTarget = { absPos + absDir };

				const float3 origWeaponMuzzlePos = w->weaponMuzzlePos;

				w->SetAttackTarget(emitTarget);
				w->weaponMuzzlePos = absPos;
				w->Fire(true);
				w->weaponMuzzlePos = origWeaponMuzzlePos;

				// the w->Fire(true); call above might have killed the same original target
				// Drop the original target in such case, otherwise the weapon will
				// continue to shoot at `emitTarget` and given how close it's to the emit point
				// it will look like a random "ghosts" shooting
				// See https://github.com/beyond-all-reason/spring/issues/1269
				if (!w->Attack(origTarget))
					w->DropCurrentTarget();

				return true;
			}

			if ((sfxType & SFX_DETONATE_WEAPON) != 0) {
				const unsigned index = sfxType - SFX_DETONATE_WEAPON;

				if (index >= unit->weapons.size()) {
					ShowUnitScriptError("[US::EmitSFX::DETONATE_WEAPON] invalid weapon index");
					break;
				}

				// detonate weapon from piece
				const CWeapon* weapon = unit->weapons[index];
				const WeaponDef* weaponDef = weapon->weaponDef;

				const CExplosionParams params = {
					.pos                  = absPos,
					.dir                  = ZeroVector,
					.damages              = *weapon->damages,
					.weaponDef            = weaponDef,
					.owner                = unit,
					.hitObject            = ExplosionHitObject(),
					.craterAreaOfEffect   = weapon->damages->craterAreaOfEffect,
					.damageAreaOfEffect   = weapon->damages->damageAreaOfEffect,
					.edgeEffectiveness    = weapon->damages->edgeEffectiveness,
					.explosionSpeed       = weapon->damages->explosionSpeed,
					.gfxMod               = 1.0f,
					.maxGroundDeformation = 0.0f,
					.impactOnly           = weaponDef->impactOnly,
					.ignoreOwner          = weaponDef->noSelfDamage,
					.damageGround         = true,
					.projectileID         = static_cast<uint32_t>(-1u)
				};

				helper->Explosion(params);
			}
		} break;
	}

	return true;
}


void CUnitScript::AttachUnit(int piece, int u)
{
	RECOIL_DETAILED_TRACY_ZONE;
	// -1 is valid, indicates that the unit should be hidden
	if ((piece >= 0) && (!PieceExists(piece))) {
		ShowUnitScriptError("[US::AttachUnit] invalid script piece index");
		return;
	}

#ifndef _CONSOLE
	CUnit* tgtUnit = unitHandler.GetUnit(u);

	if (tgtUnit == nullptr || !unit->unitDef->IsTransportUnit())
		return;

	unit->AttachUnit(tgtUnit, piece);
#endif
}


void CUnitScript::DropUnit(int u)
{
	RECOIL_DETAILED_TRACY_ZONE;
#ifndef _CONSOLE
	CUnit* tgtUnit = unitHandler.GetUnit(u);

	if (tgtUnit == nullptr || !unit->unitDef->IsTransportUnit())
		return;

	unit->DetachUnit(tgtUnit);
#endif
}

//Flags as defined by the cob standard
void CUnitScript::Explode(int piece, int flags)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!PieceExists(piece)) {
		ShowUnitScriptError("[US::Explode] invalid script piece index");
		return;
	}

#ifndef _CONSOLE
	const float3 relPos = GetPiecePos(piece);
	const float3 absPos = unit->GetObjectSpacePos(relPos);

	// do an explosion at the location first
	if (!(flags & PF_NoHeatCloud))
		projMemPool.alloc<CHeatCloudProjectile>(nullptr, absPos, ZeroVector, 30, 30);

	// If this is true, no stuff should fly off
	if (flags & PF_NONE)
		return;

	if (pieces[piece]->original == nullptr)
		return;

	if (flags & PF_Shatter) {
		Shatter(piece, absPos, unit->speed);
		return;
	}

	// This means that we are going to do a full fledged piece explosion!
	float3 baseSpeed = unit->speed;
	float3 explSpeed;
	explSpeed.x = (0.5f -  gsRNG.NextFloat()) * 6.0f;
	explSpeed.y =  1.2f + (gsRNG.NextFloat()  * 5.0f);
	explSpeed.z = (0.5f -  gsRNG.NextFloat()) * 6.0f;

	if (unit->pos.y - CGround::GetApproximateHeight(unit->pos.x, unit->pos.z) > 15)
		explSpeed.y = (0.5f - gsRNG.NextFloat()) * 6.0f;

	if (baseSpeed.SqLength() > 9.0f) {
		const float l  = baseSpeed.Length();
		const float l2 = 3.0f + math::sqrt(l - 3.0f);
		baseSpeed *= (l2 / l);
	}

	const float partSat = projectileHandler.GetParticleSaturation();

	unsigned int newFlags = 0;
	newFlags |= (PF_Explode    *  ((flags & PF_Explode   ) != 0)                    );
	newFlags |= (PF_Smoke      * (((flags & PF_Smoke     ) != 0) && partSat < 0.95f));
	newFlags |= (PF_Fire       * (((flags & PF_Fire      ) != 0) && partSat < 0.95f));
	newFlags |= (PF_NoCEGTrail *  ((flags & PF_NoCEGTrail) != 0)                    );
	newFlags |= (PF_Recursive  *  ((flags & PF_Recursive ) != 0)                    );

	projMemPool.alloc<CPieceProjectile>(unit, pieces[piece], absPos, explSpeed + baseSpeed, newFlags, 0.5f);
#endif
}


void CUnitScript::Shatter(int piece, const float3& pos, const float3& speed)
{
	RECOIL_DETAILED_TRACY_ZONE;
	const LocalModelPiece* lmp = pieces[piece];
	const S3DModelPiece* omp = lmp->original;

	if (!omp->HasGeometryData())
		return;

	const float pieceChance = 1.0f - (projectileHandler.GetCurrentParticles() - (projectileHandler.maxParticles - 2000)) / 2000.0f;
	if (pieceChance > 0.0f) {
		const CMatrix44f m = unit->GetTransformMatrix() * lmp->GetModelSpaceMatrix();
		omp->Shatter(pieceChance, unit->model->type, unit->model->textureType, unit->team, pos, speed, m);
	}
}


void CUnitScript::ShowFlare(int piece)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!PieceExists(piece)) {
		ShowUnitScriptError("[US::ShowFlare] invalid script piece index");
		return;
	}
#ifndef _CONSOLE
	const float3 relPos = GetPiecePos(piece);
	const float3 absPos = unit->GetObjectSpacePos(relPos);

	projMemPool.alloc<CMuzzleFlame>(absPos, unit->speed, unit->lastMuzzleFlameDir, unit->lastMuzzleFlameSize);
#endif
}


/******************************************************************************/
int CUnitScript::GetUnitVal(int val, int p1, int p2, int p3, int p4)
{
	RECOIL_DETAILED_TRACY_ZONE;
	// may happen in case one uses Spring.GetUnitCOBValue (Lua) on a unit with CNullUnitScript
	if (unit == nullptr) {
		ShowUnitScriptError("[US::GetUnitVal] invoked for null-scripted unit");
		return 0;
	}

#ifndef _CONSOLE
	switch (val) {
	case ACTIVATION:
		return (unit->activated);
	case STANDINGMOVEORDERS:
		return unit->moveState;
		break;
	case STANDINGFIREORDERS:
		return unit->fireState;
		break;

	case HEALTH: {
		if (p1 <= 0)
			return int((unit->health / unit->maxHealth) * 100.0f);

		const CUnit* u = unitHandler.GetUnit(p1);

		if (u == nullptr)
			return 0;

		return int((u->health / u->maxHealth) * 100.0f);
	} break;

	case INBUILDSTANCE: {
		return (unit->inBuildStance);
	} break;
	case BUSY: {
		return (busy);
	} break;

	case PIECE_XZ: {
		if (!PieceExists(p1)) {
			ShowUnitScriptError("[US::GetUnitVal::PIECE_XZ] invalid script piece index");
			break;
		}
		const float3 relPos = GetPiecePos(p1);
		const float3 absPos = unit->GetObjectSpacePos(relPos);
		return PACKXZ(absPos.x, absPos.z);
	} break;
	case PIECE_Y: {
		if (!PieceExists(p1)) {
			ShowUnitScriptError("[US::GetUnitVal::PIECE_Y] invalid script piece index");
			break;
		}
		const float3 relPos = GetPiecePos(p1);
		const float3 absPos = unit->GetObjectSpacePos(relPos);
		return int(absPos.y * COBSCALE);
	} break;

	case UNIT_XZ: {
		if (p1 <= 0)
			return PACKXZ(unit->pos.x, unit->pos.z);

		const CUnit* u = unitHandler.GetUnit(p1);

		if (u == nullptr)
			return PACKXZ(0, 0);

		return PACKXZ(u->pos.x, u->pos.z);
	} break;
	case UNIT_Y: {
		if (p1 <= 0)
			return int(unit->pos.y * COBSCALE);

		const CUnit* u = unitHandler.GetUnit(p1);

		if (u == nullptr)
			return 0;

		return int(u->pos.y * COBSCALE);
	} break;
	case UNIT_HEIGHT: {
		if (p1 <= 0)
			return int(unit->radius * COBSCALE);

		const CUnit* u = unitHandler.GetUnit(p1);

		if (u == nullptr)
			return 0;

		return int(u->radius * COBSCALE);
	} break;

	case XZ_ATAN:
		return int(RAD2TAANG*math::atan2((float)UNPACKX(p1), (float)UNPACKZ(p1)) + 32768 - unit->heading);
	case XZ_HYPOT:
		return int(math::hypot((float)UNPACKX(p1), (float)UNPACKZ(p1)) * COBSCALE);
	case ATAN:
		return int(RAD2TAANG*math::atan2((float)p1, (float)p2));
	case HYPOT:
		return int(math::hypot((float)p1, (float)p2));
	case GROUND_HEIGHT:
		return int(CGround::GetHeightAboveWater(UNPACKX(p1), UNPACKZ(p1)) * COBSCALE);
	case GROUND_WATER_HEIGHT:
		return int(CGround::GetHeightReal(UNPACKX(p1), UNPACKZ(p1)) * COBSCALE);
	case BUILD_PERCENT_LEFT:
		return int((1.0f - unit->buildProgress) * 100);

	case YARD_OPEN:
		return (unit->yardOpen) ? 1 : 0;
	case BUGGER_OFF:
		break;
	case ARMORED:
		return (unit->armoredState) ? 1 : 0;
	case VETERAN_LEVEL:
		return int(100 * unit->experience);
	case CURRENT_SPEED:
		return int(unit->speed.w * COBSCALE);
	case ON_ROAD:
		return 0;
	case IN_WATER:
		return (unit->IsInWater());
	case MAX_ID:
		return unitHandler.MaxUnits()-1;
	case MY_ID:
		return unit->id;

	case UNIT_TEAM: {
		const CUnit* u = unitHandler.GetUnit(p1);
		return (u != nullptr)? unit->team : 0;
	} break;
	case UNIT_ALLIED: {
		const CUnit* u = unitHandler.GetUnit(p1);

		if (u != nullptr)
			return teamHandler.Ally(unit->allyteam, u->allyteam) ? 1 : 0;

		return 0;
	} break;

	case UNIT_BUILD_PERCENT_LEFT: {
		const CUnit* u = unitHandler.GetUnit(p1);
		if (u != nullptr)
			return int((1.0f - u->buildProgress) * 100);

		return 0;
	} break;
	case MAX_SPEED: {
		return int(unit->moveType->GetMaxSpeed() * COBSCALE);
	} break;
	case REVERSING: {
		CGroundMoveType* gmt = dynamic_cast<CGroundMoveType*>(unit->moveType);
		return ((gmt != nullptr)? int(gmt->IsReversing()): 0);
	} break;
	case CLOAKED:
		return !!unit->isCloaked;
	case WANT_CLOAK:
		return !!unit->wantCloak;
	case UPRIGHT:
		return !!unit->upright;
	case POW:
		return int(math::pow((p1 * 1.0f) / COBSCALE, (p2 * 1.0f) / COBSCALE) * COBSCALE);
	case PRINT: {
		const char*   unitName = unit->unitDef->name.c_str();
		const char* scriptName = unit->unitDef->scriptName.c_str();

		LOG("[UnitScript::PRINT][unit=%s script=%s] p1=%d p2=%d p3=%d p4=%d", unitName, scriptName, p1, p2, p3, p4);
	} break;
	case HEADING: {
		if (p1 <= 0)
			return unit->heading;

		const CUnit* u = unitHandler.GetUnit(p1);

		if (u != nullptr)
			return u->heading;

		return -1;
	} break;
	case TARGET_ID: {
		if (size_t(p1 - 1) < unit->weapons.size()) {
			const CWeapon* w = unit->weapons[p1 - 1];
			const auto curTarget = w->GetCurrentTarget();
			switch (curTarget.type) {
				case Target_Unit:      return curTarget.unit->id;
				case Target_None:      return -1;
				case Target_Pos:       return -2;
				case Target_Intercept: return -3;
			}
		}
		return -4; // weapon does not exist
	} break;

	case LAST_ATTACKER_ID:
		return ((unit->lastAttacker != nullptr)? unit->lastAttacker->id: -1);
	case LOS_RADIUS:
		return unit->realLosRadius;
	case AIR_LOS_RADIUS:
		return unit->realAirLosRadius;
	case RADAR_RADIUS:
		return unit->radarRadius;
	case JAMMER_RADIUS:
		return unit->jammerRadius;
	case SONAR_RADIUS:
		return unit->sonarRadius;
	case SONAR_JAM_RADIUS:
		return unit->sonarJamRadius;
	case SEISMIC_RADIUS:
		return unit->seismicRadius;

	case DO_SEISMIC_PING: {
		unit->DoSeismicPing((p1 == 0)? unit->seismicSignature: p1);
	} break;

	case CURRENT_FUEL: //deprecated
		return 0;
	case TRANSPORT_ID:
		return ((unit->transporter != nullptr)? unit->transporter->id: -1);

	case SHIELD_POWER: {
		const CWeapon* shield = unit->shieldWeapon;

		if (shield == nullptr)
			return -1;

		return int(static_cast<const CPlasmaRepulser*>(shield)->GetCurPower() * float(COBSCALE));
	} break;

	case STEALTH: {
		return unit->stealth;
	} break;
	case SONAR_STEALTH: {
		return unit->sonarStealth;
	} break;

	case CRASHING:
		return (unit->IsCrashing());

	case COB_ID: {
		if (p1 <= 0)
			return unit->unitDef->cobID;

		const CUnit* u = unitHandler.GetUnit(p1);
		return ((u == nullptr)? -1 : u->unitDef->cobID);
	} break;

 	case PLAY_SOUND: {
 		// FIXME: this currently only works for CCobInstance, because Lua can not get sound IDs
 		// (however, for Lua scripts there is already LuaUnsyncedCtrl::PlaySoundFile)
 		CCobInstance* cobInst = dynamic_cast<CCobInstance*>(this);

 		if (cobInst == nullptr)
 			return 1;

 		const CCobFile* cobFile = cobInst->GetFile();

 		if (cobFile == nullptr)
 			return 1;

		if ((p1 < 0) || (static_cast<size_t>(p1) >= cobFile->sounds.size()))
			return 1;

		// who hears the sound
		switch (p3) {
			case 0: {
				// ALOS
				if (!losHandler->InAirLos(unit, gu->myAllyTeam))
					return 0;
			} break;
			case 1: {
				// LOS
				if (!(unit->losStatus[gu->myAllyTeam] & LOS_INLOS))
					return 0;
			} break;
			case 2: {
				// ALOS or radar
				if (!(losHandler->InAirLos(unit, gu->myAllyTeam) || unit->losStatus[gu->myAllyTeam] & (LOS_INRADAR)))
					return 0;
			} break;
			case 3: {
				// LOS or radar
				if (!(unit->losStatus[gu->myAllyTeam] & (LOS_INLOS | LOS_INRADAR)))
					return 0;
			} break;
			case 4: {
				// everyone
			} break;
			case 5: {
				// allies
				if (unit->allyteam != gu->myAllyTeam)
					return 0;
			} break;
			case 6: {
				// team
				if (unit->team != gu->myTeam)
					return 0;
			} break;
			case 7: {
				// enemies
				if (unit->allyteam == gu->myAllyTeam)
					return 0;
			} break;
			default: {
			} break;
		}

		if (p4 == 0) {
			Channels::General->PlaySample(cobFile->sounds[p1], unit->pos, unit->speed, float(p2) / COBSCALE);
		} else {
			Channels::General->PlaySample(cobFile->sounds[p1], float(p2) / COBSCALE);
		}

		return 0;
	} break;

	case SET_WEAPON_UNIT_TARGET: {
		const unsigned int weaponID = p1 - 1;
		const unsigned int targetID = p2;
		const bool userTarget = !!p3;

		if (weaponID >= unit->weapons.size())
			return 0;

		CWeapon* weapon = unit->weapons[weaponID];

		if (weapon == nullptr)
			return 0;

		// if targetID is 0, just sets weapon->haveUserTarget
		// to false (and targetType to None) without attacking
		CUnit* target = (targetID > 0)? unitHandler.GetUnit(targetID): nullptr;
		return (weapon->Attack(SWeaponTarget(target, userTarget)));
	} break;

	case SET_WEAPON_GROUND_TARGET: {
		const unsigned int weaponID = p1 - 1;

		const float3 pos = float3(float(UNPACKX(p2)),
		                          float(p3) / float(COBSCALE),
		                          float(UNPACKZ(p2)));
		const bool userTarget = !!p4;

		if (weaponID >= unit->weapons.size())
			return 0;

		CWeapon* weapon = unit->weapons[weaponID];

		if (weapon == nullptr)
			return 0;

		return weapon->Attack(SWeaponTarget(pos, userTarget)) ? 1 : 0;
	} break;

	case COB_MIN:
		return std::min(p1, p2);
	case COB_MAX:
		return std::max(p1, p2);
	case ABS:
		return std::abs(p1);
	case KSIN:
		return int(1024*math::sinf(TAANG2RAD*(float)p1));
	case KCOS:
		return int(1024*math::cosf(TAANG2RAD*(float)p1));
	case KTAN:
		return int(1024*math::tanf(TAANG2RAD*(float)p1));
	case SQRT:
		return int(math::sqrt((float)p1));

	case FLANK_B_MODE:
		return unit->flankingBonusMode;
	case FLANK_B_DIR:
		switch (p1) {
			case 1: return int(unit->flankingBonusDir.x * COBSCALE);
			case 2: return int(unit->flankingBonusDir.y * COBSCALE);
			case 3: return int(unit->flankingBonusDir.z * COBSCALE);
			case 4: unit->flankingBonusDir.x = (p2 / (float)COBSCALE); return 0;
			case 5: unit->flankingBonusDir.y = (p2 / (float)COBSCALE); return 0;
			case 6: unit->flankingBonusDir.z = (p2 / (float)COBSCALE); return 0;
			case 7: unit->flankingBonusDir = float3(p2/(float)COBSCALE, p3/(float)COBSCALE, p4/(float)COBSCALE).Normalize(); return 0;
			default: return(-1);
		}
	case FLANK_B_MOBILITY_ADD:
		return int(unit->flankingBonusMobilityAdd * COBSCALE);
	case FLANK_B_MAX_DAMAGE:
		return int((unit->flankingBonusAvgDamage + unit->flankingBonusDifDamage) * COBSCALE);
	case FLANK_B_MIN_DAMAGE:
		return int((unit->flankingBonusAvgDamage - unit->flankingBonusDifDamage) * COBSCALE);

	case KILL_UNIT: {
		// ID 0 is reserved for the script's owner
		CUnit* u = (p1 > 0)? unitHandler.GetUnit(p1): this->unit;

		if (u == nullptr)
			return 0;

		if (u->beingBuilt) {
			// no explosions and no corpse for units under construction
			u->KillUnit(nullptr, false, true, -CSolidObject::DAMAGE_UNIT_SCRIPT);
		} else {
			u->KillUnit(nullptr, p2 != 0, p3 != 0, -CSolidObject::DAMAGE_UNIT_SCRIPT);
		}

		return 1;
	} break;


	case WEAPON_RELOADSTATE: {
		const int np1 = -p1;

		if (p1 > 0 && static_cast<size_t>(p1) <= unit->weapons.size())
			return unit->weapons[p1 - 1]->reloadStatus;

		if (np1 > 0 && static_cast<size_t>(np1) <= unit->weapons.size()) {
			CWeapon* w = unit->weapons[np1 - 1];
			const int old = w->reloadStatus;
			w->reloadStatus = p2;
			return old;
		}

		return -1;
	} break;

	case WEAPON_RELOADTIME: {
		const int np1 = -p1;

		if (p1 > 0 && static_cast<size_t>(p1) <= unit->weapons.size())
			return unit->weapons[p1 - 1]->reloadTime;

		if (np1 > 0 && static_cast<size_t>(np1) <= unit->weapons.size()) {
			CWeapon* w = unit->weapons[np1 - 1];
			const int old = w->reloadTime;
			w->reloadTime = p2;
			return old;
		}

		return -1;
	} break;

	case WEAPON_ACCURACY: {
		const int np1 = -p1;

		if (p1 > 0 && static_cast<size_t>(p1) <= unit->weapons.size())
			return int(unit->weapons[p1 - 1]->accuracyError * COBSCALE);

		if (np1 > 0 && static_cast<size_t>(np1) <= unit->weapons.size()) {
			CWeapon* w = unit->weapons[np1 - 1];
			const int old = w->accuracyError * COBSCALE;
			w->accuracyError = float(p2) / COBSCALE;
			return old;
		}

		return -1;
	} break;

	case WEAPON_SPRAY: {
		const int np1 = -p1;

		if (p1 > 0 && static_cast<size_t>(p1) <= unit->weapons.size())
			return int(unit->weapons[p1 - 1]->sprayAngle * COBSCALE);

		if (np1 > 0 && static_cast<size_t>(np1) <= unit->weapons.size()) {
			CWeapon* w = unit->weapons[np1 - 1];
			const int old = w->sprayAngle * COBSCALE;
			w->sprayAngle = float(p2) / COBSCALE;
			return old;
		}

		return -1;
	} break;

	case WEAPON_RANGE: {
		// get (+)
		if (p1 > 0 && static_cast<size_t>(p1) <= unit->weapons.size())
			return int(unit->weapons[p1 - 1]->range * COBSCALE);

		// set (-)
		if (-p1 > 0 && static_cast<size_t>(-p1) <= unit->weapons.size()) {
			CWeapon* w = unit->weapons[-p1 - 1];

			const float  wr = w->range * COBSCALE;
			const float nwr = float(p2) / COBSCALE;

			w->UpdateRange(nwr);
			return int(wr);
		}

		return -1;
	} break;

	case WEAPON_PROJECTILE_SPEED: {
		// get (+)
		if (p1 > 0 && static_cast<size_t>(p1) <= unit->weapons.size())
			return int(unit->weapons[p1 - 1]->projectileSpeed * COBSCALE);

		// set (-)
		if (-p1 > 0 && static_cast<size_t>(-p1) <= unit->weapons.size()) {
			CWeapon* w = unit->weapons[-p1 - 1];

			const float  wps = w->projectileSpeed * COBSCALE;
			const float nwps = float(p2) / COBSCALE;

			w->UpdateProjectileSpeed(nwps);
			return int(wps);
		}

		return -1;
	} break;


	case GAME_FRAME: {
		return gs->frameNum;
	} break;

	default:
		ShowUnitScriptError("[US::GetUnitVal] unknown get-constant " + IntToString(val) + " (params = " + IntToString(p1) + " " + IntToString(p2) + " " + IntToString(p3) + " " + IntToString(p4) + ")");
	}
#endif

	return 0;
}


void CUnitScript::SetUnitVal(int val, int param)
{
	RECOIL_DETAILED_TRACY_ZONE;
	// may happen in case one uses Spring.SetUnitCOBValue (Lua) on a unit with CNullUnitScript
	if (unit == nullptr) {
		ShowUnitScriptError("[US::SetUnitVal] invoked for null-scripted unit");
		return;
	}

#ifndef _CONSOLE
	switch (val) {
		case ACTIVATION: {
			if (unit->unitDef->onoffable) {
				Command c(CMD_ONOFF, 0, (param == 0) ? 0 : 1);
				unit->commandAI->GiveCommand(c);
			} else {
				if (param == 0) {
					unit->Deactivate();
				} else {
					unit->Activate();
				}
			}
		} break;

		case STANDINGMOVEORDERS: {
			if (param >= 0 && param <= MOVESTATE_ROAM) {
				Command c(CMD_MOVE_STATE, 0, param);
				unit->commandAI->GiveCommand(c);
			}
		} break;

		case STANDINGFIREORDERS: {
			if (param >= 0 && param <= FIRESTATE_FIREATNEUTRAL) {
				Command c(CMD_FIRE_STATE, 0, param);
				unit->commandAI->GiveCommand(c);
			}
		} break;

		case HEALTH: {
		} break;
		case INBUILDSTANCE: {
			unit->inBuildStance = (param != 0);
		} break;

		case BUSY: {
			busy = (param != 0);
		} break;

		case PIECE_XZ: {
		} break;

		case PIECE_Y: {
		} break;

		case UNIT_XZ: {
		} break;

		case UNIT_Y: {
		} break;

		case UNIT_HEIGHT: {
		} break;

		case XZ_ATAN: {
		} break;

		case XZ_HYPOT: {
		} break;

		case ATAN: {
		} break;

		case HYPOT: {
		} break;

		case GROUND_HEIGHT: {
		} break;

		case GROUND_WATER_HEIGHT: {
		} break;

		case BUILD_PERCENT_LEFT: {
		} break;

		case YARD_OPEN: {
			if (unit->GetBlockMap() != nullptr) {
				// note: if this unit is a factory, engine-controlled
				// OpenYard() and CloseYard() calls can interfere with
				// the yardOpen state (they probably should be removed
				// at some point)
				if (param == 0) {
					if (groundBlockingObjectMap.CanCloseYard(unit))
						groundBlockingObjectMap.CloseBlockingYard(unit);

				} else {
					if (groundBlockingObjectMap.CanOpenYard(unit))
						groundBlockingObjectMap.OpenBlockingYard(unit);

				}
			}
		} break;

		case BUGGER_OFF: {
			if (param != 0) {
				if (const auto* f = dynamic_cast<CFactory*>(unit)) {
					float3 boDir = (f->boRelHeading == 0) ? static_cast<float3>(f->frontdir) : GetVectorFromHeading((f->heading + f->boRelHeading) % SPRING_MAX_HEADING);
					CGameHelper::BuggerOff(f->pos + boDir * f->boOffset, f->boRadius, f->boSherical, f->boForced, f->team, nullptr);
				} else {
					assert(false); //should not be happening with non-factory units?
					CGameHelper::BuggerOff(unit->pos + unit->frontdir * unit->radius, unit->radius * 1.5f, true, false, unit->team, nullptr);
				}
			}
		} break;

		case ARMORED: {
			unit->curArmorMultiple = mix(1.0f, unit->armoredMultiple, param != 0);
			unit->armoredState = (param != 0);
		} break;

		case VETERAN_LEVEL: {
			unit->experience = param * 0.01f;
		} break;

		case MAX_SPEED: {
			if (param >= 0) {
				unit->moveType->SetMaxSpeed(param / float(COBSCALE));
			} else {
				// temporarily (until the next command is issued) change unit's speed
				unit->moveType->SetWantedMaxSpeed(-param / float(COBSCALE));
			}
		} break;

		case CLOAKED: {
			unit->wantCloak = !!param;
		} break;

		case WANT_CLOAK: {
			unit->wantCloak = !!param;
		} break;

		case UPRIGHT: {
			unit->upright = !!param;
		} break;

		case HEADING: {
			unit->SetHeading(param % COBSCALE, !unit->upright && unit->IsOnGround(), false, unit->unitDef->upDirSmoothing);
		} break;
		case LOS_RADIUS: {
			unit->ChangeLos(unit->realLosRadius = param, unit->realAirLosRadius);
		} break;

		case AIR_LOS_RADIUS: {
			unit->ChangeLos(unit->realLosRadius, unit->realAirLosRadius = param);
		} break;

		case RADAR_RADIUS: {
			unit->radarRadius = param;
		} break;

		case JAMMER_RADIUS: {
			unit->jammerRadius = param;
		} break;

		case SONAR_RADIUS: {
			unit->sonarRadius = param;
		} break;

		case SONAR_JAM_RADIUS: {
			unit->sonarJamRadius = param;
		} break;

		case SEISMIC_RADIUS: {
			unit->seismicRadius = param;
		} break;

		case CURRENT_FUEL: { // deprecated
		} break;

		case SHIELD_POWER: {
			if (unit->shieldWeapon != nullptr) {
				CPlasmaRepulser* shield = static_cast<CPlasmaRepulser*>(unit->shieldWeapon);
				shield->SetCurPower(std::max(0.0f, float(param) / float(COBSCALE)));
			}
		} break;

		case STEALTH: {
			unit->stealth = !!param;
		} break;

		case SONAR_STEALTH: {
			unit->sonarStealth = !!param;
		} break;

		case CRASHING: {
			AAirMoveType* amt = dynamic_cast<AAirMoveType*>(unit->moveType);
			if (amt != nullptr) {
				if (!!param) {
					amt->SetState(AAirMoveType::AIRCRAFT_CRASHING);
				} else {
					amt->SetState(AAirMoveType::AIRCRAFT_FLYING);
				}
			}
		} break;

		case CHANGE_TARGET: {
			if (param <                     0) { return; }
			if (param >= unit->weapons.size()) { return; }

			unit->weapons[param]->avoidTarget = true;
		} break;

		case CEG_DAMAGE: {
			unit->cegDamage = param;
		} break;

		case FLANK_B_MODE: {
			unit->flankingBonusMode = param;
		} break;
		case FLANK_B_MOBILITY_ADD: {
			unit->flankingBonusMobilityAdd = (param / (float)COBSCALE);
		} break;
		case FLANK_B_MAX_DAMAGE: {
			const float mindamage = unit->flankingBonusAvgDamage - unit->flankingBonusDifDamage;
			unit->flankingBonusAvgDamage = (param / (float)COBSCALE + mindamage) * 0.5f;
			unit->flankingBonusDifDamage = (param / (float)COBSCALE - mindamage) * 0.5f;
		} break;

		case FLANK_B_MIN_DAMAGE: {
			const float maxdamage = unit->flankingBonusAvgDamage + unit->flankingBonusDifDamage;
			unit->flankingBonusAvgDamage = (maxdamage + param / (float)COBSCALE) * 0.5f;
			unit->flankingBonusDifDamage = (maxdamage - param / (float)COBSCALE) * 0.5f;
		} break;

		default: {
			ShowUnitScriptError("[US::SetUnitVal] unknown set-constant " + IntToString(val));
		}
	}
#endif
}

/******************************************************************************/
/******************************************************************************/

int CUnitScript::ScriptToModel(int scriptPieceNum) const {
	RECOIL_DETAILED_TRACY_ZONE;
	if (!PieceExists(scriptPieceNum))
		return -1;

	const LocalModelPiece* smp = GetScriptLocalModelPiece(scriptPieceNum);

	return (smp->GetLModelPieceIndex());
}

int CUnitScript::ModelToScript(int lmodelPieceNum) const {
	RECOIL_DETAILED_TRACY_ZONE;
	LocalModel& lm = unit->localModel;

	if (!lm.HasPiece(lmodelPieceNum))
		return -1;

	const LocalModelPiece* lmp = lm.GetPiece(lmodelPieceNum);

	return (lmp->GetScriptPieceIndex());
}

void CUnitScript::ShowUnitScriptError(const std::string& error) const
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (unit == nullptr) {
		ShowScriptError("unitID=null error=\"" + error + "\"");
	} else {
		ShowScriptError("unitID=" + IntToString(unit->id) + " defName=" + unit->unitDef->name + " error=\"" + error + "\"");
	}
}

