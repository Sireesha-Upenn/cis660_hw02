#include "LSystemCmd.h"
#include <maya/MGlobal.h>
#include <list>
#include <string.h>
#include <iostream>

using namespace std;
LSystemCmd::LSystemCmd() : MPxCommand()
{
}

LSystemCmd::~LSystemCmd() 
{
}

MStatus LSystemCmd::doIt(const MArgList& args)
{
	MStatus stat;
	LSystem lSystem;
	double step_size;
	double angle;
	MString grammar;
	int iter;

	MArgDatabase argData(syntax(), args, &stat);
	if (stat != MS::kSuccess) {
		return stat; 
	}

	if (argData.isFlagSet("-ss")) {
		argData.getFlagArgument("-ss", 0, step_size);
	}
	if (argData.isFlagSet("-a")) {
		argData.getFlagArgument("-a", 0, angle);
	}
	if (argData.isFlagSet("-g")) {
		argData.getFlagArgument("-g", 0, grammar);
	}
	if (argData.isFlagSet("-i")) {
		argData.getFlagArgument("-i", 0, iter);
	}

	if (stat.error())
	{
		MGlobal::displayError(stat.errorString());
	}

	//LSystem from User Commands 
	lSystem.loadProgramFromString(grammar.asChar());
	lSystem.setDefaultAngle(angle);
	lSystem.setDefaultStep(step_size);

	std::vector<std::pair<vec3, vec3>> branches;
	lSystem.process(iter, branches);

	for (int i = 0; i < branches.size(); i++)
	{
		MString i_str =  to_string(i + 1).c_str();
		MString startx = to_string(branches.at(i).first[0]).c_str();
		MString starty = to_string(branches.at(i).first[1]).c_str();
		MString startz = to_string(branches.at(i).first[2]).c_str();

		MString endx = to_string(branches.at(i).second[0]).c_str();
		MString endy = to_string(branches.at(i).second[1]).c_str();
		MString endz = to_string(branches.at(i).second[2]).c_str();

		vec3 normal = (branches.at(i).second - branches.at(i).first).Normalize();
		MString nx = to_string(normal[0]).c_str();
		MString ny = to_string(normal[1]).c_str();
		MString nz = to_string(normal[2]).c_str();

		MString displacement = to_string(branches.at(i).first[1] - step_size).c_str();

		MString command = "curve -d 1 -p " + startx + " " + starty + " " + startz + "-p " + endx + " " + endy + " " + endz + " -k 0 -k 1 -name \" curve" + i_str + "\";"
			"circle -radius 0.1 -nr " + nx + " " + ny + " " + nz + "-c " + startx + " " + starty + " " + startz + " -name \"nurbsCircle" + i_str + "\";"
			"select -r nurbsCircle" + i_str + " curve" + i_str + " ;"
			"extrude -ch true -rn false -po 1 -et 2 -rotation 0 -scale 1 -rsp 1 \"nurbsCircle " + i_str + "\" \"curve" + i_str + "\"";
		MGlobal::executeCommand(command);
	}
	
	
    return MStatus::kSuccess;
}

MSyntax LSystemCmd::newSyntax()
{
	MStatus stat = MS::kSuccess;
	MSyntax syntax; 
	stat = syntax.addFlag("-ss", "-stepsize", MSyntax::kDouble);
	if (stat != MS::kSuccess)
	{
		return syntax;
	}
	stat = syntax.addFlag("-a", "-angle", MSyntax::kDouble);
	if (stat != MS::kSuccess)
	{
		return syntax;
	}
	stat = syntax.addFlag("-g", "-grammar", MSyntax::kString);
	if (stat != MS::kSuccess)
	{
		return syntax;
	}
	stat = syntax.addFlag("-i", "-iterations", MSyntax::kUnsigned);
	
	if (stat != MS::kSuccess)
	{
		return syntax;
	}

	return syntax;
}