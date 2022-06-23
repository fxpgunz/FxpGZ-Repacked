//-----------------------------------------------------------------------------
// listjobs.cpp
//
//  MaxNet API Sample Program
//
//  listjobs [managername [port]]
//

//-- OS Includes
#include <windows.h>
//-- MaxNet API Includes
#include <max.h>
#include <maxnet.h>
#include <client.h>

int _tmain (int argc, TCHAR* argv[]) {
	//-- Creates new manager access module instance
	MaxNetManager* manager = CreateManager();
	if (manager) {
		try {
			//-- Setup default manager port
			short port = DF_MGRPORT;
			//-- Placeholder for manager name
			TCHAR managername[MAX_PATH];
			//-- If we have a port defined, use it
			if (argc > 2)
				port = _ttoi(argv[2]);
			//-- If we have a host name (or ip address) for the manager, use it
			if (argc > 1)
				_tcscpy(managername,argv[1]);
			else {
				//-- Otherwise, find out who is the manager (if any)
				//   The API will broadcast a message looking for a
				//   manager using the given port. If a manager is
				//   found, its name/ip address will be placed in 
				//   "managername" below.
				if (!manager->FindManager(port,managername)) {
					_tprintf(_T("Could not find a Manager\n"));
					DestroyManager(manager);
					return 1;
				}
			}
			//-- Connect to the manager
			manager->Connect(port,managername);
			//-- Get a job count
			int count = manager->GetJobCount();
			if (count) {
				//-- Allocate space for the list. We could get one job at a time as
				//   as well but collecting all at once is more efficient.
				JobList *jobList = (JobList *)LocalAlloc(LPTR,count * sizeof(JobList));
				if (jobList) {
					//-- Get job list from manager
					manager->ListJobs(0,count,jobList);
					//-- Loop printing a list of jobs
					for (int i = 0; i < count; i++) {
						_tprintf(_T("%-30s"),jobList[i].job.name);
						TCHAR state[64];
						switch (jobList[i].state) {
							case JOB_STATE_COMPLETE:
								_tcscpy(state,_T("Complete"));
								break;
							case JOB_STATE_WAITING:
								_tcscpy(state,_T("Waiting"));
								break;
							case JOB_STATE_BUSY:
								_tcscpy(state,_T("Started"));
								break;
							case JOB_STATE_ERROR:
								_tcscpy(state,_T("Error"));
								break;
							case JOB_STATE_SUSPENDED:
								_tcscpy(state,_T("Suspended"));
								break;
							default:
								_tcscpy(state,_T("Unknown"));
						}
						_tprintf(_T("%s\n"),state);
					}
					LocalFree(jobList);
				}
			} else
				_tprintf(_T("No Jobs Found\n"));
			//-- Disconnect from Manager
			manager->Disconnect();
		//-- Exception Handler
		} catch (MaxNet* maxerr) {
			TCHAR errmsg[256];
			_stprintf(errmsg,_T("\n%s (0x%08X)\n"),maxerr->GetErrorText(),maxerr->GetError());
		}
		//-- Clean up manager instance
		DestroyManager(manager);
	}
	return 0;
}