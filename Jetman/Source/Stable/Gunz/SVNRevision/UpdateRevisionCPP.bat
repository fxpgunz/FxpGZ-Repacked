@echo off
cd SVNRevision
svn info .. --xml > revision.xml
SVNRevisionTracker revision.xml
cd ..