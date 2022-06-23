#pragma once

#include "RNavigationMesh.h"
#include "RNavigationNode.h"

// 이 클래스들은 엔진 클래스인 네비게이션 메쉬와 노드 포인터를 래핑함으로써
// 게임 로직 코드가 엔진에 의존하는 부분에 끼워넣어져 테스트를 위한 mock을 사용할 수 있는 여지를 준다.


class ZNavigationNode
{
	RNavigationNode* m_pNavNode;

public:
	ZNavigationNode(RNavigationNode* p) : m_pNavNode(p) {}
	ZNavigationNode(const ZNavigationNode& other) { m_pNavNode = other.m_pNavNode; }

	virtual bool IsNull() { return !m_pNavNode; }

	virtual const rvector& CenterVertex() const {
		return m_pNavNode->CenterVertex();
	}

	virtual void SetWeight(float fWeight) {
		m_pNavNode->SetWeight(fWeight);
	}
	
	virtual float GetWeight() {
		return m_pNavNode->GetWeight();
	}
};



class ZNavigationMesh
{
	RNavigationMesh* m_pNavMesh;

public:
	ZNavigationMesh(RNavigationMesh* p) : m_pNavMesh(p) {}
	ZNavigationMesh(const ZNavigationMesh& other) { m_pNavMesh = other.m_pNavMesh; }

	bool IsNull() { return !m_pNavMesh; }

	virtual ZNavigationNode FindClosestNode(const rvector& point) const { 
		return ZNavigationNode( m_pNavMesh->FindClosestNode(point));
	}

	virtual bool BuildNavigationPath(const rvector& vStartPos, const rvector& vGoalPos) {
		return m_pNavMesh->BuildNavigationPath(vStartPos, vGoalPos);
	}

	virtual void ClearAllNodeWeight() {
		m_pNavMesh->ClearAllNodeWeight();
	}

	virtual list<rvector>& GetWaypointList() {
		return m_pNavMesh->GetWaypointList(); 
	}



};
