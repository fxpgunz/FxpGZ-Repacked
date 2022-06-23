#include "stdafx.h"
#include "../../sdk/UnitTest++/src/UnitTest++.h"
#include "MConnectHistory.h"

TEST(ConnectHistory)
{
	MConnectHistory history;

	// �߰�
	history.Add("testClient", 10);

	history.Update( 20 );

	// �ִ��� Ȯ��
	CHECK ( history.IsExist("testClient") );

	// timegettime �ѹ��� �������� ������� �Ѵ�
	history.Update( 0 );

	CHECK ( !history.IsExist("testClient") );


	// �ٸ��� �߰�
	history.Add("testClient2", 10);
	CHECK ( history.IsExist("testClient2") );

	// 11�� �����ڿ� ������� �Ѵ�
	history.Update( 11000 );
	CHECK ( !history.IsExist("testClient2") );
}