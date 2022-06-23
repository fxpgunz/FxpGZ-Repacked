#pragma once
#include <string>

class MCompanionDef
{
	std::string m_companionName;
	std::string m_companionModel;
	int			m_maxHP;
	int			m_maxAP;
	int			m_collRadius;
	int			m_collHeight;
	bool		m_drawShadow;
	bool		m_meshPicking;
public:
	std::string GetCompanionName() { return m_companionName; }
	void SetCompanionName(const char* name) { m_companionName = name; }

	std::string GetCompanionModel() { return m_companionModel; }
	void SetCompanionModel(const char* model) { m_companionModel = model; }

	int GetMaxHP() { return m_maxHP; }
	void SetMaxHP(int maxHP) { m_maxHP = maxHP; }

	int GetMaxAP() { return m_maxAP; }
	void SetMaxAP(int maxAP) { m_maxAP = maxAP; }

	int GetCollRadius() { return m_collRadius; }
	void SetCollRadius(int n) { m_collRadius = n; }

	int GetCollHeight() { return m_collHeight; }
	void SetCollHeight(int n) { m_collHeight = n; }

	bool IsDrawShadow() { return m_drawShadow; }
	void SetDrawShadow(bool b) { m_drawShadow = b; }

	bool IsMeshPicking() { return m_meshPicking; }
	void SetMeshPicking(bool b) { m_meshPicking = b; }
};

class MCompanionDefMgr
{
	std::map<string, MCompanionDef*> m_actorDefMgr;
public:
	~MCompanionDefMgr();

	void Clear();

	bool ReadXml(const char* fileName, MZFileSystem* pFileSystem = nullptr);
	bool AddCompanionDef(MCompanionDef* companionDef);


	int GetCount() { return (int)m_actorDefMgr.size(); }

	MCompanionDef* GetCompanion(const char* companionName);

	inline bool atob(const char* value) {
		if (_stricmp(value, "true") == 0)
			return true;

		return false;
	}
};