#pragma once

#include "RNavigationMesh.h"
#include "RNavigationNode.h"

// �� Ŭ�������� ���� Ŭ������ �׺���̼� �޽��� ��� �����͸� ���������ν�
// ���� ���� �ڵ尡 ������ �����ϴ� �κп� �����־��� �׽�Ʈ�� ���� mock�� ����� �� �ִ� ������ �ش�.


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
