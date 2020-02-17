#pragma once
#include <maya/MPxCommand.h>
#include <maya/MGlobal.h>

#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPlugin.h>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include "cylinder.h"
#include "LSystem.h"

#define McheckErr(stat,msg)             \
        if ( MS::kSuccess != stat ) {   \
                cerr << msg;            \
                return MS::kFailure;    \
        }

class LSystemNode : public MPxNode
{
public:
	//Constructor 
	LSystemNode();
	//Destructor
	virtual ~LSystemNode();
	//Member Variables
	static MTypeId nodeID;
	static MObject time;
	static MObject outputMesh;
	
	static MObject grammar;
	static MObject angle;
	static MObject stepSize;
	static MObject iterations;
	
	static MFnNumericAttribute m_angle;
	static MFnNumericAttribute m_stepSize;
	static MFnTypedAttribute m_grammar;
	static MFnTypedAttribute m_iterations;

	static MFnUnitAttribute UAtime;
	static MFnTypedAttribute mesh;

	//Member functions
	static MStatus initialize();
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static void* creator() { return new LSystemNode(); }
	virtual MStatus doIt(const MArgList& args);
	MObject createMesh(const MTime& timeInput,
		const double& stepInput,
		const double& angleInput,
		const int& iterInput,
		const std::string& grammarInput,
		MObject& outData,
		MStatus& stat);
};