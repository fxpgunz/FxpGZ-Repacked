#include "stdafx.h"
#include "../../sdk/UnitTest++/src/UnitTest++.h"
#include "MMatchObjectCommandHistory.h"

TEST(MatchObjectCommandHistrory)
{
	MMatchObjectCommandHistory aCommandHistory;

	// 3��� command 100ms �������� 40���� �о�־�� 
	for(int i = 0 ; i < 60 ; ++i )
	{
		aCommandHistory.PushCommand( MUID(0,1) , i, i*100 );
		aCommandHistory.PushCommand( MUID(0,2) , i, i*100 );
		aCommandHistory.PushCommand( MUID(0,3) , i, i*100 );
	}

	// 3�� ������ �ΰ� �Ѹ� �� �����ش� 
	for(int i = 0 ; i < 50 ; ++i )
	{
		aCommandHistory.PushCommand( MUID(0,4) , i, i*100 + 3 * 60 * 1000 );
	}

	// 4���� �����Ͱ� �����ϳ� �׽�Ʈ
	CHECK_EQUAL( 4, aCommandHistory.GetObjectCount() );

	// ������ �ϳ��� �޾ƿ� ����
	MOBJECTCOMMANDHISTORY* pOutput = aCommandHistory.GetCommandHistory( MUID(0,2) );

	// MAX_COUNT �Ѿ�� �ʴ��� �׽�Ʈ
	CHECK_EQUAL( MAX_COMMAND_HISTORY_COUNT, (unsigned int)pOutput->m_commands.size() );

	// ���� 10���� ���������Ѵ�
	CHECK_EQUAL ( 10, (int)pOutput->m_commands.front().first );

	// 11���� ���������
	aCommandHistory.Update( 11 * 60 * 1000 );

	// ���� 3���� ��������Ѵ�
	CHECK_EQUAL( 1 , aCommandHistory.GetObjectCount());

	// �����͵� ������Ѵ�
	MOBJECTCOMMANDHISTORY* pOutput2 = aCommandHistory.GetCommandHistory( MUID(0,2) );
	CHECK( NULL==pOutput2);

	// 4��° ����� �����Ͱ� �ִ�
	MOBJECTCOMMANDHISTORY* pOutput3 = aCommandHistory.GetCommandHistory( MUID(0,4) );
	CHECK( NULL!=pOutput3 );

	// �ð��� �� ������ �� �������� �Ѵ�
	aCommandHistory.Update( 40 * 60 * 1000 );

	CHECK_EQUAL( 0, aCommandHistory.GetObjectCount() );

	// 50�� �� �Ѹ� �� �����ش� 
	for(int i = 0 ; i < 30 ; ++i )
	{
		aCommandHistory.PushCommand( MUID(0,5) , i, i*100 + 50 * 60 * 1000 );
	}
	CHECK_EQUAL( 1, aCommandHistory.GetObjectCount() );

	// time get time �� �ѹ��� ���� 0�̵Ǹ� ��ü�� �� ��������
	aCommandHistory.Update( 0 );
	CHECK_EQUAL( 0, aCommandHistory.GetObjectCount() );


	// flooding test .. ����� �ð��� ���� �о������ �÷������� ���
	bool bFloodingSuspect;
	for(int i=0;i<100;i++)
	{
		aCommandHistory.PushCommand( MUID(0,6) , 1, i, &bFloodingSuspect );
	}
	CHECK( bFloodingSuspect);

}


