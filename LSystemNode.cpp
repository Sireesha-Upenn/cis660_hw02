#include "LSystemNode.h"
#include "LSystem.h"
#include "LSystemCmd.h"
#include <maya\MPoint.h>
#include <maya\MPointArray.h>
#include <maya\MFnMesh.h>
#include <maya\MTypeId.h>
#include <maya\MFnStringData.h>
typedef std::pair<vec3, vec3> Branch;

//Static variables 
MTypeId LSystemNode::nodeID (0x80000);
MObject LSystemNode::time;
MObject LSystemNode::outputMesh;
MObject LSystemNode::angle;
MObject LSystemNode::stepSize;
MObject LSystemNode::iterations;
MObject LSystemNode::grammar;



LSystemNode::LSystemNode() 
{}

LSystemNode::~LSystemNode()
{}

MStatus LSystemNode::doIt(const MArgList& args)
{
	MGlobal::displayInfo("NODEEEEE!");
	return MStatus::kSuccess;
}


MStatus LSystemNode::initialize()
{
	MStatus returnStatus;
	
	MFnUnitAttribute UAtime;
	MFnTypedAttribute mesh;
	
	MFnNumericAttribute m_angle;
	MFnNumericAttribute m_stepSize;
	MFnNumericAttribute m_iterations;
	MFnTypedAttribute m_grammar;
	// Create and add an input MFnTypedAttribute for the grammar file. You can specify files relative to the plug-in .mll location using MFnPlugin::loadPath()


	LSystemNode::time = UAtime.create("time", "tm", MFnUnitAttribute::kTime, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode time attribute\n");

	LSystemNode::outputMesh = mesh.create("outputgeometry", "out", MFnData::kMesh, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode outGeom attribute\n");

	//stepsize
	LSystemNode::stepSize = m_stepSize.create("stepSize", "ss", MFnNumericData::kFloat , 1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode stepsize attribute\n");
	//angle 
	LSystemNode::angle = m_angle.create("angle", "a", MFnNumericData::kFloat , 45.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode angle attribute\n");
	//iterations 
	iterations = m_iterations.create("iterations", "i", MFnNumericData::kInt, 1.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode iterations attribute\n");

	//grammar
	MString defaultGrammar = "F \n" "F->F[+F]F[-F]F";
	LSystemNode::grammar = m_grammar.create("grammar", "g", MFnData::kString, MFnStringData().create(defaultGrammar) ,&returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode grammar attribute\n");
	

	mesh.setStorable(false);

	//Attribute AfterAffects() functions assigns dependencies between attributes - establishes which are input and which are output

	returnStatus = addAttribute(LSystemNode::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	returnStatus = addAttribute(LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");
	
	returnStatus = addAttribute(LSystemNode::grammar);
	McheckErr(returnStatus, "ERROR adding grammar attribute\n");

	returnStatus = addAttribute(LSystemNode::stepSize);
	McheckErr(returnStatus, "ERROR adding stepSize attribute\n");

	returnStatus = addAttribute(LSystemNode::angle);
	McheckErr(returnStatus, "ERROR adding angle attribute\n");

	returnStatus = attributeAffects(LSystemNode::time, LSystemNode::outputMesh);
    McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(LSystemNode::grammar, LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(LSystemNode::stepSize, LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(LSystemNode::angle, LSystemNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	return MS::kSuccess;

}
MStatus LSystemNode::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;
	MFnMesh meshFS;

	if (plug == outputMesh) {
		/* Get time */
		MDataHandle timeData = data.inputValue(time, &returnStatus);
		McheckErr(returnStatus, "Error getting time data handle\n");
		MTime ip_time = timeData.asTime();

		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting Geom data handle\n");
		
		MDataHandle grammarData = data.inputValue(grammar, &returnStatus);
		McheckErr(returnStatus, "ERROR getting grammar data handle\n");
		MString ip_grammar = grammarData.asString();

		MDataHandle iterData = data.inputValue(iterations, &returnStatus);
		McheckErr(returnStatus, "ERROR getting iterations data handle\n");
		int ip_iter = iterData.asInt();

		MDataHandle angleData = data.inputValue(angle, &returnStatus);
		McheckErr(returnStatus, "ERROR getting angle data handle\n");
		double ip_angle = iterData.asDouble();

		MDataHandle stepData = data.inputValue(angle, &returnStatus);
		McheckErr(returnStatus, "ERROR getting stepsize data handle\n");
		double ip_stepSize = stepData.asDouble();

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");

		createMesh(ip_time, ip_stepSize, ip_angle, ip_iter, ip_grammar.asChar(), newOutputData, returnStatus);
		McheckErr(returnStatus, "ERROR");

		outputHandle.set(newOutputData);
		data.setClean(plug);
	}
	else
	{
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

MObject LSystemNode::createMesh(const MTime& timeInput,
	const double& stepInput,
	const double& angleInput,
	const int& iterInput,
	const std::string& grammarInput,
	MObject& outData,
	MStatus& stat)
{
	MPointArray points;
	MIntArray faceCounts;
	MIntArray faceConnects;
	MPoint p1;
	MPoint p2;
	MFnMesh meshFS;
	LSystem l;

	int frame = (int)timeInput.as(MTime::kFilm);
	if (frame == 0)
	{
		frame = 1;
	}

	l.loadProgramFromString(grammarInput);
	l.setDefaultAngle(angleInput);
	l.setDefaultStep(stepInput);

	std::vector<Branch> b = std::vector<Branch>();
	l.process(frame, b);

	for (int i = 0; i < b.size(); i++)
	{
		p1 = MPoint(b[i].first[0], b[i].first[1], b[i].first[2], 1.0);
		p2 = MPoint(b[i].second[0], b[i].second[1], b[i].second[2], 1.0);
		CylinderMesh c(p1, p2, 0.1);
		c.appendToMesh(points, faceCounts, faceConnects);
	}

	int numFaces = faceCounts.length();
	int numVertices = points.length();
	int numFaceConnects = faceConnects.length();

	MObject newMesh = meshFS.create(points.length(), faceCounts.length(), points, faceCounts, faceConnects, outData);
	return newMesh;
}