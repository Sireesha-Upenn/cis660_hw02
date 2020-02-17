#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>

#include "LSystemCmd.h"
#include "LSystemNode.h"

MStatus initializePlugin( MObject obj )
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj, "MyPlugin", "1.0", "Any");

    // Register Command
    status = plugin.registerCommand( "LSystemCmd", LSystemCmd::creator, LSystemCmd::newSyntax);
    if (!status) {
        status.perror("registerCommand");
        return status;
    }
	
	//Register Node
	MString grammarPath = plugin.loadPath() + "/plants/simple1.txt";
	status = plugin.registerNode("LSystemNode", LSystemNode::nodeID, LSystemNode::creator, LSystemNode::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	// You can run a MEL script from initializePlugin() to auto-register your MEL dialog boxes and menus
	char buffer[2048]; 
	sprintf_s(buffer, 2048, "source \"%s/MyPluginDialog\";", plugin.loadPath().asChar()); 
	MGlobal::executeCommand(buffer, true);
    return status;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus   status = MStatus::kSuccess;
    MFnPlugin plugin( obj );

    status = plugin.deregisterCommand( "LSystemCmd" );
    if (!status) {
	    status.perror("deregisterCommand");
	    return status;
    }

	//Deregister Node 
	status = plugin.deregisterCommand("LSystemNode");
	status = plugin.deregisterNode(LSystemNode::nodeID);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
    return status;
}


