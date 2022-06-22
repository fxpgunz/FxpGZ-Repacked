@echo off
path = %path%;c:\program files\sliksvn\bin;c:\program files\subversion\bin
cd SVNRevision
svn info .. --xml > revision.xml
SVNRevisionTracker revision.xml
cd ..