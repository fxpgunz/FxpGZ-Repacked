// �����п� ���õ� �Լ���
//
// ������
// �Ÿ� : ����
// �ð� : ��
//
#ifndef PHYSICS_H
#define PHYSICS_H

#include "RTypes.h"

/// �߷� ���ӵ�
constexpr auto  GRAVITY_ACCELERATION = 9.8f;

/// ������ �
rvector ParabolicMotion(rvector& InitialVelocity, float fSec);

#endif