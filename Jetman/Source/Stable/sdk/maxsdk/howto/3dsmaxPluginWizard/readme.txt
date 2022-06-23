
Archive Contents
----------------
3ds Max Plugin Wizard Functionality and Template files.


Outline
-------
The 3ds Max Plugin Wizard allows you to create plugin projects for 3ds Max
through the App Wizard interface in the Visual Studio IDE.


Installing
----------

1. Open the 3dsmaxPluginWizard.vsz file (in the 3dsmaxPluginWizard directory root) 
   in a text editor and edit the ABSOLUTE PATH parameter to reflect the new location of the 
   3dsmaxPluginWizard root directory. Do not add a backslash after the directory name.

	Param="ABSOLUTE_PATH = [Absolute Path Location of 3dsmaxPluginWizard Root Directory]"
	
2. The version of Visual Studio you are using will determine what value to use for the 'Wizard' variable 
	in the file 3dsmaxPluginWizard.vsz.
	For Visual Studio 2010 use: Wizard=VsWizard.VsWizardEngine.10.0
	For Visual Studio 2012 use: Wizard=VsWizard.VsWizardEngine.11.0
	For Visual Studio 2015 use: Wizard=VsWizard.VsWizardEngine.14.0
	

3. Copy the following files from the 3dsmaxPluginWizard root to the 'VC\VCProjects'
	directory under your Visual Studio installation (e.g. C:\Program Files(x86)\Microsoft Visual Studio 14.0\VC\VCProjects):
	
	3dsmaxPluginWizard.ico
	3dsmaxPluginWizard.vsdir
	3dsmaxPluginWizard.vsz

	If you are using Visual Studio Express Edition, you need to copy the files listed above to 
	the 'VC\Express\VCProjects' folder.

4.  Make all files in wizard file folder to be writable and not read only. If this is not done, than visual studio may generate an
	error after creating the plugin project.

5.  At this point the 3ds Max Plugin Wizard project should appear under File menu:New:Projects:Visual C++ Projects
	in Visual Studio.
	

Usage
-----
The wizard will guide you through the steps involved in setting up your 3ds Max plugin project.


Note
----
1.	The name of the project you specify should not start with a number. 
	This is because the wizard takes that name, and creates variables based off it. And if an
	identifier starts with a number it won't compile.