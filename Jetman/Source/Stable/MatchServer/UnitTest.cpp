#include "StdAfx.h"
//#include "UnitTest.h"

#ifdef _DEBUG
#pragma comment ( lib, "../../sdk/UnitTest++/lib/UnitTest++.vsnet2003d.lib")
#else
//#pragma comment ( lib, "../../sdk/UnitTest++/lib/UnitTest++.vsnet2003.lib")
#endif

#include "../../sdk/UnitTest++/src/UnitTest++.h"
#include "../../sdk/UnitTest++/src/XmlTestReporter.h"
#include <fstream>

const char* TEST_XML_FILENAME = "./UnitTestResults.xml";

class XmlAndStdTestReporter : public UnitTest::XmlTestReporter
{
public:
	explicit XmlAndStdTestReporter(std::ostream& ostream) : XmlTestReporter(ostream) {}
private:
	virtual void ReportFailure(UnitTest::TestDetails const& details, char const* failure)
	{
		UnitTest::XmlTestReporter::ReportFailure(details, failure);

		char const* const errorFormat = "%s(%d): error: Failure in %s: %s\n";

		mlog(errorFormat, details.filename, details.lineNumber, details.testName, failure);
	}

	virtual void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed)
	{
		XmlTestReporter::ReportSummary(totalTestCount, failedTestCount, failureCount, secondsElapsed);

		if (failureCount > 0)
			mlog("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
		else
			mlog("Success: %d tests passed.\n", totalTestCount);
		mlog("Test time: %.2f seconds.\n\n", secondsElapsed);
	}
};

bool RunUnitTest()
{
	mlog("\n=============== UnitTest ===============\n");

	std::ofstream f(TEST_XML_FILENAME);
	XmlAndStdTestReporter reporter(f);  
	int failure_count = 1;

	if (failure_count > 0)
		return false;
	return true;
}
