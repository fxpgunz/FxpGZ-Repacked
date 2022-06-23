#ifndef _MBMATCHATTACKERVICTIMPAIR_H
#define _MBMATCHATTACKERVICTIMPAIR_H


class MBMatchAttVicPair
{
private :
	DWORD m_dwAttackerCID;
	DWORD m_dwVictimCID;

public :
	MBMatchAttVicPair( const DWORD dwAttackerCID, const DWORD dwVictimCID )
	{
		m_dwAttackerCID = dwAttackerCID;
		m_dwVictimCID	= dwVictimCID;
	}

	const DWORD GetAttackerCID() const	{ return m_dwAttackerCID; }
	const DWORD GetVictimCID() const	{ return m_dwVictimCID; }

	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator > (const MBMatchAttVicPair& a, const MBMatchAttVicPair& b){
		if(a.GetAttackerCID() > b.GetAttackerCID()) return true;
		if(a.GetAttackerCID() == b.GetAttackerCID()){
			if(a.GetVictimCID() > b.GetVictimCID()) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator >= (const MBMatchAttVicPair& a, const MBMatchAttVicPair& b){
		if(a.GetAttackerCID() > b.GetAttackerCID()) return true;
		if(a.GetAttackerCID() == b.GetAttackerCID()){
			if(a.GetVictimCID() >= b.GetVictimCID()) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator < (const MBMatchAttVicPair& a, const MBMatchAttVicPair& b){
		if(a.GetAttackerCID() < b.GetAttackerCID()) return true;
		if(a.GetAttackerCID() == b.GetAttackerCID()){
			if(a.GetVictimCID() < b.GetVictimCID()) return true;
		}
		return false;
	}
	/// ũ�� �񱳸� ���� ���۷����� �����ε�
	inline friend bool operator <= (const MBMatchAttVicPair& a, const MBMatchAttVicPair& b){
		if(a.GetAttackerCID() < b.GetAttackerCID()) return true;
		if(a.GetAttackerCID() == b.GetAttackerCID()){
			if(a.GetVictimCID() <= b.GetVictimCID()) return true;
		}
		return false;
	}
};

#endif