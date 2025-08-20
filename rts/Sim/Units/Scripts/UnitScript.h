/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

/* heavily based on CobInstance.h */

#pragma once

#include <string>
#include <vector>

#include "AnimTypes.hpp"
#include "Rendering/Models/LocalModelPiece.hpp"
#include "System/creg/creg_cond.h"


class CUnit;
class CPlasmaRepulser;

class CUnitScript
{
	CR_DECLARE(CUnitScript)
protected:
	size_t myId; // position in the unitscript vector
	CUnit* unit;
	bool busy;

	bool hasSetSFXOccupy;
	bool hasRockUnit;
	bool hasStartBuilding;

	void RemoveAnim(AnimType type, int piece, int axis);
	void AddAnim(AnimType type, int piece, int axis, float speed, float dest, float accel);

	virtual void ShowScriptError(const std::string& msg) const = 0;
public:
	void ShowUnitScriptError(const std::string& msg) const;
public:
	// subclass is responsible for populating this with script pieces
	std::vector<LocalModelPiece*> pieces;

	bool PieceExists(uint32_t scriptPieceNum) const {
		// NOTE: there can be NULL pieces present from the remapping in CobInstance
		return ((scriptPieceNum < pieces.size()) && (pieces[scriptPieceNum] != nullptr));
	}

	LocalModelPiece* GetScriptLocalModelPieceSafe(uint32_t scriptPieceNum) const {
		if (scriptPieceNum >= pieces.size())
			return nullptr;

		// may still return nullptr
		return pieces[scriptPieceNum];
	}

	LocalModelPiece* GetScriptLocalModelPiece(uint32_t scriptPieceNum) const {
		assert(PieceExists(scriptPieceNum));
		return pieces[scriptPieceNum];
	}

	int ScriptToModel(int scriptPieceNum) const;
	int ModelToScript(int lmodelPieceNum) const;

#define SCRIPT_TO_LOCALPIECE_FUNC(RetType, ScriptFunc, PieceFunc)       \
	RetType ScriptFunc(int scriptPieceNum) const {                      \
		if (!PieceExists(scriptPieceNum))                               \
			return RetType{};                                           \
		LocalModelPiece* p = GetScriptLocalModelPiece(scriptPieceNum);  \
		return (p->PieceFunc());                                        \
	}

	SCRIPT_TO_LOCALPIECE_FUNC(    float3, GetPiecePos      ,    GetAbsolutePos     )
	SCRIPT_TO_LOCALPIECE_FUNC( Transform, GetPieceTransform, GetModelSpaceTransform)
	SCRIPT_TO_LOCALPIECE_FUNC(CMatrix44f, GetPieceMatrix   , GetModelSpaceMatrix   )

	bool GetEmitDirPos(int scriptPieceNum, float3& pos, float3& dir) const {
		if (!PieceExists(scriptPieceNum))
			return true;

		LocalModelPiece* p = GetScriptLocalModelPiece(scriptPieceNum);
		return (p->GetEmitDirPos(pos, dir));
	}

public:
	CUnitScript(CUnit* unit);
	virtual ~CUnitScript();

	auto GetId() const { return myId; }
	bool IsBusy() const { return busy; }

	      CUnit* GetUnit()       { return unit; }
	const CUnit* GetUnit() const { return unit; }

	void Spin(int piece, int axis, float speed, float accel);
	void StopSpin(int piece, int axis, float decel);
	void Turn(int piece, int axis, float speed, float destination);
	void Move(int piece, int axis, float speed, float destination);
	void MoveNow(int piece, int axis, float destination);
	void TurnNow(int piece, int axis, float destination);

	bool NeedsWait(AnimType type, int piece, int axis) const;

	// misc, used by CCobThread and callouts for Lua unitscripts
	void SetVisibility(int piece, bool visible);

	bool EmitSfx(int sfxType, int sfxPiece);
	bool EmitRelSFX(int sfxType, const float3& relPos, const float3& relDir);
	bool EmitAbsSFX(int sfxType, const float3& absPos, const float3& absDir, const float3& relDir = FwdVector);

	void AttachUnit(int piece, int unit);
	void DropUnit(int unit);
	void Explode(int piece, int flags);
	void Shatter(int piece, const float3& pos, const float3& speed);
	void ShowFlare(int piece);
	int GetUnitVal(int val, int p1, int p2, int p3, int p4);
	void SetUnitVal(int val, int param);

	bool IsInAnimation(AnimType type, int piece, int axis) const;

	// checks for callin existence
	bool HasSetSFXOccupy () const { return hasSetSFXOccupy; }
	bool HasRockUnit     () const { return hasRockUnit; }
	bool HasStartBuilding() const { return hasStartBuilding; }

	virtual bool HasBlockShot   (int weaponNum) const { return false; }
	virtual bool HasTargetWeight(int weaponNum) const { return false; }

	// callins, called throughout sim
	virtual void RawCall(int functionId) = 0;
	virtual void Create() = 0;
	// Killed must cause unit->deathScriptFinished and unit->delayedWreckLevel to be set!
	virtual void Killed() = 0;
	virtual void WindChanged(float heading, float speed) = 0;
	virtual void ExtractionRateChanged(float speed) = 0;
	virtual void WorldRockUnit(const float3& rockDir) = 0;
	virtual void RockUnit(const float3& rockDir) = 0;
	virtual void WorldHitByWeapon(const float3& hitDir, int weaponDefId, float& inoutDamage) = 0;
	virtual void HitByWeapon(const float3& hitDir, int weaponDefId, float& inoutDamage) = 0;
	virtual void SetSFXOccupy(int curTerrainType) = 0;
	// doubles as QueryLandingPadCount and QueryLandingPad
	// in COB, the first one determines the number of arguments to the second one
	// in Lua, we can just count the number of return values
	virtual void QueryLandingPads(std::vector<int>& out_pieces) = 0;
	virtual void BeginTransport(const CUnit* unit) = 0;
	virtual int  QueryTransport(const CUnit* unit) = 0; // returns piece
	virtual void TransportPickup(const CUnit* unit) = 0;
	virtual void TransportDrop(const CUnit* unit, const float3& pos) = 0;
	virtual void StartBuilding(float heading, float pitch) = 0;
	virtual int  QueryNanoPiece() = 0; // returns piece
	virtual int  QueryBuildInfo() = 0; // returns piece

	virtual void Destroy() = 0;
	virtual void StartMoving(bool reversing) = 0;
	virtual void StopMoving() = 0;
	virtual void StartSkidding(const float3&) = 0;
	virtual void StopSkidding() = 0;
	virtual void ChangeHeading(short deltaHeading) = 0;
	virtual void StartUnload() = 0;
	virtual void EndTransport() = 0;
	virtual void StartBuilding() = 0;
	virtual void StopBuilding() = 0;
	virtual void Falling() = 0;
	virtual void Landed() = 0;
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual void MoveRate(int curRate) = 0;
	virtual void FireWeapon(int weaponNum) = 0;
	virtual void EndBurst(int weaponNum) = 0;

	// weapon callins
	virtual int   QueryWeapon(int weaponNum) = 0; // returns piece, former QueryPrimary
	virtual void  AimWeapon(int weaponNum, float heading, float pitch) = 0;
	virtual void  AimShieldWeapon(CPlasmaRepulser* weapon) = 0;
	virtual int   AimFromWeapon(int weaponNum) = 0; // returns piece, former AimFromPrimary
	virtual void  Shot(int weaponNum) = 0;
	virtual bool  BlockShot(int weaponNum, const CUnit* targetUnit, bool userTarget) = 0; // returns whether shot should be blocked
	virtual float TargetWeight(int weaponNum, const CUnit* targetUnit) = 0; // returns target weight
	virtual void AnimFinished(AnimType type, int piece, int axis) = 0;
};