#include "ComputeDeviceData.h"
#include "../Common/CLUtil.h"

#include <iostream>

#include "Parameters.h"

using DataType = Parameters::DataType;

ComputeDeviceData::ComputeDeviceData(cl_context Context) :
    m_Program(NULL)
{
    kernelNames.emplace_back("histogram");
    kernelNames.emplace_back("scanhistograms");
    kernelNames.emplace_back("pastehistograms");
    kernelNames.emplace_back("reorder");
    kernelNames.emplace_back("transpose");

    alternatives.emplace_back("RadixSort_01");

	// allocate device resources
	cl_int clError;
	TODO("Consider using CL_MEM_HOST_whatever");
	m_dInKeys = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(DataType) * Parameters::_NUM_MAX_INPUT_ELEMS, NULL, &clError);
	V_RETURN_CL(clError, "Error allocating device array");
	m_dOutKeys = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(DataType) * Parameters::_NUM_MAX_INPUT_ELEMS, NULL, &clError);
	V_RETURN_CL(clError, "Error allocating device array");
	m_dInPermut = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(uint32_t) * Parameters::_NUM_MAX_INPUT_ELEMS, NULL, &clError);
	V_RETURN_CL(clError, "Error allocating device array");
	m_dOutPermut = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(uint32_t) * Parameters::_NUM_MAX_INPUT_ELEMS, NULL, &clError);
	V_RETURN_CL(clError, "Error allocating device array");
	// allocate the histogram on the GPU
	m_dHistograms = clCreateBuffer(Context, CL_MEM_READ_WRITE,
		sizeof(uint32_t) * Parameters::_RADIX * Parameters::_NUM_GROUPS * Parameters::_NUM_ITEMS_PER_GROUP, NULL, &clError);
	V_RETURN_CL(clError, "Error allocating device array");

	// allocate the auxiliary histogram on GPU
	m_dGlobsum = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(uint32_t) * Parameters::_NUM_HISTOSPLIT, NULL, &clError);
	V_RETURN_CL(clError, "Error allocating device array");

	// temporary vector when the sum is not needed
	m_dTemp = clCreateBuffer(Context, CL_MEM_READ_WRITE, sizeof(uint32_t) * Parameters::_NUM_HISTOSPLIT, NULL, &clError);
	V_RETURN_CL(clError, "Error allocating device array");
}

ComputeDeviceData::~ComputeDeviceData() {
    SAFE_RELEASE_MEMOBJECT(m_dInKeys);
    SAFE_RELEASE_MEMOBJECT(m_dOutKeys);
    SAFE_RELEASE_MEMOBJECT(m_dInPermut);
    SAFE_RELEASE_MEMOBJECT(m_dOutPermut);

    for (auto& kernel : m_kernelMap) {
        SAFE_RELEASE_KERNEL(kernel.second);
    }

    SAFE_RELEASE_PROGRAM(m_Program);
}