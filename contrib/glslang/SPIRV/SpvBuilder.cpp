//
//Copyright (C) 2014-2015 LunarG, Inc.
//Copyright (C) 2015-2016 Google, Inc.
//
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of 3Dlabs Inc. Ltd. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//POSSIBILITY OF SUCH DAMAGE.

//
// Author: John Kessenich, LunarG
//

//
// Helper for making SPIR-V IR.  Generally, this is documented in the header
// SpvBuilder.h.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <unordered_set>

#include "SpvBuilder.h"

#ifndef _WIN32
    #include <cstdio>
#endif

namespace spv {

Builder::Builder(unsigned int magicNumber) :
    source(SourceLanguageUnknown),
    sourceVersion(0),
    addressModel(AddressingModelLogical),
    memoryModel(MemoryModelGLSL450),
    builderNumber(magicNumber),
    buildPoint(0),
    uniqueId(0),
    mainFunction(0),
    generatingOpCodeForSpecConst(false)
{
    clearAccessChain();
}

Builder::~Builder()
{
}

Id Builder::import(const char* name)
{
    Instruction* import = new Instruction(getUniqueId(), NoType, OpExtInstImport);
    import->addStringOperand(name);
    
    imports.push_back(std::unique_ptr<Instruction>(import));
    return import->getResultId();
}

// For creating new groupedTypes (will return old type if the requested one was already made).
Id Builder::makeVoidType()
{
    Instruction* type;
    if (groupedTypes[OpTypeVoid].size() == 0) {
        type = new Instruction(getUniqueId(), NoType, OpTypeVoid);
        groupedTypes[OpTypeVoid].push_back(type);
        constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
        module.mapInstruction(type);
    } else
        type = groupedTypes[OpTypeVoid].back();

    return type->getResultId();
}

Id Builder::makeBoolType()
{
    Instruction* type;
    if (groupedTypes[OpTypeBool].size() == 0) {
        type = new Instruction(getUniqueId(), NoType, OpTypeBool);
        groupedTypes[OpTypeBool].push_back(type);
        constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
        module.mapInstruction(type);
    } else
        type = groupedTypes[OpTypeBool].back();

    return type->getResultId();
}

Id Builder::makeSamplerType()
{
    Instruction* type;
    if (groupedTypes[OpTypeSampler].size() == 0) {
        type = new Instruction(getUniqueId(), NoType, OpTypeSampler);
        groupedTypes[OpTypeSampler].push_back(type);
        constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
        module.mapInstruction(type);
    } else
        type = groupedTypes[OpTypeSampler].back();

    return type->getResultId();
}

Id Builder::makePointer(StorageClass storageClass, Id pointee)
{
    // try to find it
    Instruction* type;
    for (int t = 0; t < (int)groupedTypes[OpTypePointer].size(); ++t) {
        type = groupedTypes[OpTypePointer][t];
        if (type->getImmediateOperand(0) == (unsigned)storageClass &&
            type->getIdOperand(1) == pointee)
            return type->getResultId();
    }

    // not found, make it
    type = new Instruction(getUniqueId(), NoType, OpTypePointer);
    type->addImmediateOperand(storageClass);
    type->addIdOperand(pointee);
    groupedTypes[OpTypePointer].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    return type->getResultId();
}

Id Builder::makeIntegerType(int width, bool hasSign)
{
    // try to find it
    Instruction* type;
    for (int t = 0; t < (int)groupedTypes[OpTypeInt].size(); ++t) {
        type = groupedTypes[OpTypeInt][t];
        if (type->getImmediateOperand(0) == (unsigned)width &&
            type->getImmediateOperand(1) == (hasSign ? 1u : 0u))
            return type->getResultId();
    }

    // not found, make it
    type = new Instruction(getUniqueId(), NoType, OpTypeInt);
    type->addImmediateOperand(width);
    type->addImmediateOperand(hasSign ? 1 : 0);
    groupedTypes[OpTypeInt].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    // deal with capabilities
    switch (width) {
    case 16:
        addCapability(CapabilityInt16);
        break;
    case 64:
        addCapability(CapabilityInt64);
        break;
    default:
        break;
    }

    return type->getResultId();
}

Id Builder::makeFloatType(int width)
{
    // try to find it
    Instruction* type;
    for (int t = 0; t < (int)groupedTypes[OpTypeFloat].size(); ++t) {
        type = groupedTypes[OpTypeFloat][t];
        if (type->getImmediateOperand(0) == (unsigned)width)
            return type->getResultId();
    }

    // not found, make it
    type = new Instruction(getUniqueId(), NoType, OpTypeFloat);
    type->addImmediateOperand(width);
    groupedTypes[OpTypeFloat].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    // deal with capabilities
    switch (width) {
    case 16:
        addCapability(CapabilityFloat16);
        break;
    case 64:
        addCapability(CapabilityFloat64);
        break;
    default:
        break;
    }

    return type->getResultId();
}

// Make a struct without checking for duplication.
// See makeStructResultType() for non-decorated structs
// needed as the result of some instructions, which does
// check for duplicates.
Id Builder::makeStructType(const std::vector<Id>& members, const char* name)
{
    // Don't look for previous one, because in the general case,
    // structs can be duplicated except for decorations.

    // not found, make it
    Instruction* type = new Instruction(getUniqueId(), NoType, OpTypeStruct);
    for (int op = 0; op < (int)members.size(); ++op)
        type->addIdOperand(members[op]);
    groupedTypes[OpTypeStruct].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);
    addName(type->getResultId(), name);

    return type->getResultId();
}

// Make a struct for the simple results of several instructions,
// checking for duplication.
Id Builder::makeStructResultType(Id type0, Id type1)
{
    // try to find it
    Instruction* type;
    for (int t = 0; t < (int)groupedTypes[OpTypeStruct].size(); ++t) {
        type = groupedTypes[OpTypeStruct][t];
        if (type->getNumOperands() != 2)
            continue;
        if (type->getIdOperand(0) != type0 || 
            type->getIdOperand(1) != type1)
            continue;
        return type->getResultId();
    }

    // not found, make it
    std::vector<spv::Id> members;
    members.push_back(type0);
    members.push_back(type1);

    return makeStructType(members, "ResType");
}

Id Builder::makeVectorType(Id component, int size)
{
    // try to find it
    Instruction* type;
    for (int t = 0; t < (int)groupedTypes[OpTypeVector].size(); ++t) {
        type = groupedTypes[OpTypeVector][t];
        if (type->getIdOperand(0) == component &&
            type->getImmediateOperand(1) == (unsigned)size)
            return type->getResultId();
    }

    // not found, make it
    type = new Instruction(getUniqueId(), NoType, OpTypeVector);
    type->addIdOperand(component);
    type->addImmediateOperand(size);
    groupedTypes[OpTypeVector].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    return type->getResultId();
}

Id Builder::makeMatrixType(Id component, int cols, int rows)
{
    assert(cols <= maxMatrixSize && rows <= maxMatrixSize);

    Id column = makeVectorType(component, rows);

    // try to find it
    Instruction* type;
    for (int t = 0; t < (int)groupedTypes[OpTypeMatrix].size(); ++t) {
        type = groupedTypes[OpTypeMatrix][t];
        if (type->getIdOperand(0) == column &&
            type->getImmediateOperand(1) == (unsigned)cols)
            return type->getResultId();
    }

    // not found, make it
    type = new Instruction(getUniqueId(), NoType, OpTypeMatrix);
    type->addIdOperand(column);
    type->addImmediateOperand(cols);
    groupedTypes[OpTypeMatrix].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    return type->getResultId();
}

// TODO: performance: track arrays per stride
// If a stride is supplied (non-zero) make an array.
// If no stride (0), reuse previous array types.
// 'size' is an Id of a constant or specialization constant of the array size
Id Builder::makeArrayType(Id element, Id sizeId, int stride)
{
    Instruction* type;
    if (stride == 0) {
        // try to find existing type
        for (int t = 0; t < (int)groupedTypes[OpTypeArray].size(); ++t) {
            type = groupedTypes[OpTypeArray][t];
            if (type->getIdOperand(0) == element &&
                type->getIdOperand(1) == sizeId)
                return type->getResultId();
        }
    }

    // not found, make it
    type = new Instruction(getUniqueId(), NoType, OpTypeArray);
    type->addIdOperand(element);
    type->addIdOperand(sizeId);
    groupedTypes[OpTypeArray].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    return type->getResultId();
}

Id Builder::makeRuntimeArray(Id element)
{
    Instruction* type = new Instruction(getUniqueId(), NoType, OpTypeRuntimeArray);
    type->addIdOperand(element);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    return type->getResultId();
}

Id Builder::makeFunctionType(Id returnType, const std::vector<Id>& paramTypes)
{
    // try to find it
    Instruction* type;
    for (int t = 0; t < (int)groupedTypes[OpTypeFunction].size(); ++t) {
        type = groupedTypes[OpTypeFunction][t];
        if (type->getIdOperand(0) != returnType || (int)paramTypes.size() != type->getNumOperands() - 1)
            continue;
        bool mismatch = false;
        for (int p = 0; p < (int)paramTypes.size(); ++p) {
            if (paramTypes[p] != type->getIdOperand(p + 1)) {
                mismatch = true;
                break;
            }
        }
        if (! mismatch)
            return type->getResultId();
    }

    // not found, make it
    type = new Instruction(getUniqueId(), NoType, OpTypeFunction);
    type->addIdOperand(returnType);
    for (int p = 0; p < (int)paramTypes.size(); ++p)
        type->addIdOperand(paramTypes[p]);
    groupedTypes[OpTypeFunction].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    return type->getResultId();
}

Id Builder::makeImageType(Id sampledType, Dim dim, bool depth, bool arrayed, bool ms, unsigned sampled, ImageFormat format)
{
    // try to find it
    Instruction* type;
    for (int t = 0; t < (int)groupedTypes[OpTypeImage].size(); ++t) {
        type = groupedTypes[OpTypeImage][t];
        if (type->getIdOperand(0) == sampledType &&
            type->getImmediateOperand(1) == (unsigned int)dim &&
            type->getImmediateOperand(2) == (  depth ? 1u : 0u) &&
            type->getImmediateOperand(3) == (arrayed ? 1u : 0u) &&
            type->getImmediateOperand(4) == (     ms ? 1u : 0u) &&
            type->getImmediateOperand(5) == sampled &&
            type->getImmediateOperand(6) == (unsigned int)format)
            return type->getResultId();
    }

    // not found, make it
    type = new Instruction(getUniqueId(), NoType, OpTypeImage);
    type->addIdOperand(sampledType);
    type->addImmediateOperand(   dim);
    type->addImmediateOperand(  depth ? 1 : 0);
    type->addImmediateOperand(arrayed ? 1 : 0);
    type->addImmediateOperand(     ms ? 1 : 0);
    type->addImmediateOperand(sampled);
    type->addImmediateOperand((unsigned int)format);

    groupedTypes[OpTypeImage].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    // deal with capabilities
    switch (dim) {
    case DimBuffer:
        if (sampled)
            addCapability(CapabilitySampledBuffer);
        else
            addCapability(CapabilityImageBuffer);
        break;
    case Dim1D:
        if (sampled)
            addCapability(CapabilitySampled1D);
        else
            addCapability(CapabilityImage1D);
        break;
    case DimCube:
        if (arrayed) {
            if (sampled)
                addCapability(CapabilitySampledCubeArray);
            else
                addCapability(CapabilityImageCubeArray);
        }
        break;
    case DimRect:
        if (sampled)
            addCapability(CapabilitySampledRect);
        else
            addCapability(CapabilityImageRect);
        break;
    case DimSubpassData:
        addCapability(CapabilityInputAttachment);
        break;
    default:
        break;
    }

    if (ms) {
        if (arrayed)
            addCapability(CapabilityImageMSArray);
        if (! sampled)
            addCapability(CapabilityStorageImageMultisample);
    }

    return type->getResultId();
}

Id Builder::makeSampledImageType(Id imageType)
{
    // try to find it
    Instruction* type;
    for (int t = 0; t < (int)groupedTypes[OpTypeSampledImage].size(); ++t) {
        type = groupedTypes[OpTypeSampledImage][t];
        if (type->getIdOperand(0) == imageType)
            return type->getResultId();
    }

    // not found, make it
    type = new Instruction(getUniqueId(), NoType, OpTypeSampledImage);
    type->addIdOperand(imageType);

    groupedTypes[OpTypeSampledImage].push_back(type);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(type));
    module.mapInstruction(type);

    return type->getResultId();
}

Id Builder::getDerefTypeId(Id resultId) const
{
    Id typeId = getTypeId(resultId);
    assert(isPointerType(typeId));

    return module.getInstruction(typeId)->getImmediateOperand(1);
}

Op Builder::getMostBasicTypeClass(Id typeId) const
{
    Instruction* instr = module.getInstruction(typeId);

    Op typeClass = instr->getOpCode();
    switch (typeClass)
    {
    case OpTypeVoid:
    case OpTypeBool:
    case OpTypeInt:
    case OpTypeFloat:
    case OpTypeStruct:
        return typeClass;
    case OpTypeVector:
    case OpTypeMatrix:
    case OpTypeArray:
    case OpTypeRuntimeArray:
        return getMostBasicTypeClass(instr->getIdOperand(0));
    case OpTypePointer:
        return getMostBasicTypeClass(instr->getIdOperand(1));
    default:
        assert(0);
        return OpTypeFloat;
    }
}

int Builder::getNumTypeConstituents(Id typeId) const
{
    Instruction* instr = module.getInstruction(typeId);

    switch (instr->getOpCode())
    {
    case OpTypeBool:
    case OpTypeInt:
    case OpTypeFloat:
        return 1;
    case OpTypeVector:
    case OpTypeMatrix:
        return instr->getImmediateOperand(1);
    case OpTypeArray:
    {
        Id lengthId = instr->getImmediateOperand(1);
        return module.getInstruction(lengthId)->getImmediateOperand(0);
    }
    case OpTypeStruct:
        return instr->getNumOperands();
    default:
        assert(0);
        return 1;
    }
}

// Return the lowest-level type of scalar that an homogeneous composite is made out of.
// Typically, this is just to find out if something is made out of ints or floats.
// However, it includes returning a structure, if say, it is an array of structure.
Id Builder::getScalarTypeId(Id typeId) const
{
    Instruction* instr = module.getInstruction(typeId);

    Op typeClass = instr->getOpCode();
    switch (typeClass)
    {
    case OpTypeVoid:
    case OpTypeBool:
    case OpTypeInt:
    case OpTypeFloat:
    case OpTypeStruct:
        return instr->getResultId();
    case OpTypeVector:
    case OpTypeMatrix:
    case OpTypeArray:
    case OpTypeRuntimeArray:
    case OpTypePointer:
        return getScalarTypeId(getContainedTypeId(typeId));
    default:
        assert(0);
        return NoResult;
    }
}

// Return the type of 'member' of a composite.
Id Builder::getContainedTypeId(Id typeId, int member) const
{
    Instruction* instr = module.getInstruction(typeId);

    Op typeClass = instr->getOpCode();
    switch (typeClass)
    {
    case OpTypeVector:
    case OpTypeMatrix:
    case OpTypeArray:
    case OpTypeRuntimeArray:
        return instr->getIdOperand(0);
    case OpTypePointer:
        return instr->getIdOperand(1);
    case OpTypeStruct:
        return instr->getIdOperand(member);
    default:
        assert(0);
        return NoResult;
    }
}

// Return the immediately contained type of a given composite type.
Id Builder::getContainedTypeId(Id typeId) const
{
    return getContainedTypeId(typeId, 0);
}

// See if a scalar constant of this type has already been created, so it
// can be reused rather than duplicated.  (Required by the specification).
Id Builder::findScalarConstant(Op typeClass, Op opcode, Id typeId, unsigned value) const
{
    Instruction* constant;
    for (int i = 0; i < (int)groupedConstants[typeClass].size(); ++i) {
        constant = groupedConstants[typeClass][i];
        if (constant->getOpCode() == opcode &&
            constant->getTypeId() == typeId &&
            constant->getImmediateOperand(0) == value)
            return constant->getResultId();
    }

    return 0;
}

// Version of findScalarConstant (see above) for scalars that take two operands (e.g. a 'double').
Id Builder::findScalarConstant(Op typeClass, Op opcode, Id typeId, unsigned v1, unsigned v2) const
{
    Instruction* constant;
    for (int i = 0; i < (int)groupedConstants[typeClass].size(); ++i) {
        constant = groupedConstants[typeClass][i];
        if (constant->getOpCode() == opcode &&
            constant->getTypeId() == typeId &&
            constant->getImmediateOperand(0) == v1 &&
            constant->getImmediateOperand(1) == v2)
            return constant->getResultId();
    }

    return 0;
}

// Return true if consuming 'opcode' means consuming a constant.
// "constant" here means after final transform to executable code,
// the value consumed will be a constant, so includes specialization.
bool Builder::isConstantOpCode(Op opcode) const
{
    switch (opcode) {
    case OpUndef: 
    case OpConstantTrue:
    case OpConstantFalse:
    case OpConstant:
    case OpConstantComposite:
    case OpConstantSampler:
    case OpConstantNull:
    case OpSpecConstantTrue:
    case OpSpecConstantFalse:
    case OpSpecConstant:
    case OpSpecConstantComposite:
    case OpSpecConstantOp:
        return true;
    default:
        return false;
    }
}

Id Builder::makeBoolConstant(bool b, bool specConstant)
{
    Id typeId = makeBoolType();
    Instruction* constant;
    Op opcode = specConstant ? (b ? OpSpecConstantTrue : OpSpecConstantFalse) : (b ? OpConstantTrue : OpConstantFalse);

    // See if we already made it. Applies only to regular constants, because specialization constants
    // must remain distinct for the purpose of applying a SpecId decoration.
    if (! specConstant) {
        Id existing = 0;
        for (int i = 0; i < (int)groupedConstants[OpTypeBool].size(); ++i) {
            constant = groupedConstants[OpTypeBool][i];
            if (constant->getTypeId() == typeId && constant->getOpCode() == opcode)
                existing = constant->getResultId();
        }

        if (existing)
            return existing;
    }

    // Make it
    Instruction* c = new Instruction(getUniqueId(), typeId, opcode);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(c));
    groupedConstants[OpTypeBool].push_back(c);
    module.mapInstruction(c);

    return c->getResultId();
}

Id Builder::makeIntConstant(Id typeId, unsigned value, bool specConstant)
{
    Op opcode = specConstant ? OpSpecConstant : OpConstant;

    // See if we already made it. Applies only to regular constants, because specialization constants
    // must remain distinct for the purpose of applying a SpecId decoration.
    if (! specConstant) {
        Id existing = findScalarConstant(OpTypeInt, opcode, typeId, value);
        if (existing)
            return existing;
    }

    Instruction* c = new Instruction(getUniqueId(), typeId, opcode);
    c->addImmediateOperand(value);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(c));
    groupedConstants[OpTypeInt].push_back(c);
    module.mapInstruction(c);

    return c->getResultId();
}

Id Builder::makeFloatConstant(float f, bool specConstant)
{
    Op opcode = specConstant ? OpSpecConstant : OpConstant;
    Id typeId = makeFloatType(32);
    union { float fl; unsigned int ui; } u;
    u.fl = f;
    unsigned value = u.ui;

    // See if we already made it. Applies only to regular constants, because specialization constants
    // must remain distinct for the purpose of applying a SpecId decoration.
    if (! specConstant) {
        Id existing = findScalarConstant(OpTypeFloat, opcode, typeId, value);
        if (existing)
            return existing;
    }

    Instruction* c = new Instruction(getUniqueId(), typeId, opcode);
    c->addImmediateOperand(value);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(c));
    groupedConstants[OpTypeFloat].push_back(c);
    module.mapInstruction(c);

    return c->getResultId();
}

Id Builder::makeDoubleConstant(double d, bool specConstant)
{
    Op opcode = specConstant ? OpSpecConstant : OpConstant;
    Id typeId = makeFloatType(64);
    union { double db; unsigned long long ull; } u;
    u.db = d;
    unsigned long long value = u.ull;
    unsigned op1 = value & 0xFFFFFFFF;
    unsigned op2 = value >> 32;

    // See if we already made it. Applies only to regular constants, because specialization constants
    // must remain distinct for the purpose of applying a SpecId decoration.
    if (! specConstant) {
        Id existing = findScalarConstant(OpTypeFloat, opcode, typeId, op1, op2);
        if (existing)
            return existing;
    }

    Instruction* c = new Instruction(getUniqueId(), typeId, opcode);
    c->addImmediateOperand(op1);
    c->addImmediateOperand(op2);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(c));
    groupedConstants[OpTypeFloat].push_back(c);
    module.mapInstruction(c);

    return c->getResultId();
}

Id Builder::findCompositeConstant(Op typeClass, std::vector<Id>& comps) const
{
    Instruction* constant = 0;
    bool found = false;
    for (int i = 0; i < (int)groupedConstants[typeClass].size(); ++i) {
        constant = groupedConstants[typeClass][i];

        // same shape?
        if (constant->getNumOperands() != (int)comps.size())
            continue;

        // same contents?
        bool mismatch = false;
        for (int op = 0; op < constant->getNumOperands(); ++op) {
            if (constant->getIdOperand(op) != comps[op]) {
                mismatch = true;
                break;
            }
        }
        if (! mismatch) {
            found = true;
            break;
        }
    }

    return found ? constant->getResultId() : NoResult;
}

// Comments in header
Id Builder::makeCompositeConstant(Id typeId, std::vector<Id>& members, bool specConstant)
{
    Op opcode = specConstant ? OpSpecConstantComposite : OpConstantComposite;
    assert(typeId);
    Op typeClass = getTypeClass(typeId);

    switch (typeClass) {
    case OpTypeVector:
    case OpTypeArray:
    case OpTypeStruct:
    case OpTypeMatrix:
        break;
    default:
        assert(0);
        return makeFloatConstant(0.0);
    }

    if (! specConstant) {
        Id existing = findCompositeConstant(typeClass, members);
        if (existing)
            return existing;
    }

    Instruction* c = new Instruction(getUniqueId(), typeId, opcode);
    for (int op = 0; op < (int)members.size(); ++op)
        c->addIdOperand(members[op]);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(c));
    groupedConstants[typeClass].push_back(c);
    module.mapInstruction(c);

    return c->getResultId();
}

Instruction* Builder::addEntryPoint(ExecutionModel model, Function* function, const char* name)
{
    Instruction* entryPoint = new Instruction(OpEntryPoint);
    entryPoint->addImmediateOperand(model);
    entryPoint->addIdOperand(function->getId());
    entryPoint->addStringOperand(name);

    entryPoints.push_back(std::unique_ptr<Instruction>(entryPoint));

    return entryPoint;
}

// Currently relying on the fact that all 'value' of interest are small non-negative values.
void Builder::addExecutionMode(Function* entryPoint, ExecutionMode mode, int value1, int value2, int value3)
{
    Instruction* instr = new Instruction(OpExecutionMode);
    instr->addIdOperand(entryPoint->getId());
    instr->addImmediateOperand(mode);
    if (value1 >= 0)
        instr->addImmediateOperand(value1);
    if (value2 >= 0)
        instr->addImmediateOperand(value2);
    if (value3 >= 0)
        instr->addImmediateOperand(value3);

    executionModes.push_back(std::unique_ptr<Instruction>(instr));
}

void Builder::addName(Id id, const char* string)
{
    Instruction* name = new Instruction(OpName);
    name->addIdOperand(id);
    name->addStringOperand(string);

    names.push_back(std::unique_ptr<Instruction>(name));
}

void Builder::addMemberName(Id id, int memberNumber, const char* string)
{
    Instruction* name = new Instruction(OpMemberName);
    name->addIdOperand(id);
    name->addImmediateOperand(memberNumber);
    name->addStringOperand(string);

    names.push_back(std::unique_ptr<Instruction>(name));
}

void Builder::addLine(Id target, Id fileName, int lineNum, int column)
{
    Instruction* line = new Instruction(OpLine);
    line->addIdOperand(target);
    line->addIdOperand(fileName);
    line->addImmediateOperand(lineNum);
    line->addImmediateOperand(column);

    lines.push_back(std::unique_ptr<Instruction>(line));
}

void Builder::addDecoration(Id id, Decoration decoration, int num)
{
    if (decoration == (spv::Decoration)spv::BadValue)
        return;
    Instruction* dec = new Instruction(OpDecorate);
    dec->addIdOperand(id);
    dec->addImmediateOperand(decoration);
    if (num >= 0)
        dec->addImmediateOperand(num);

    decorations.push_back(std::unique_ptr<Instruction>(dec));
}

void Builder::addMemberDecoration(Id id, unsigned int member, Decoration decoration, int num)
{
    Instruction* dec = new Instruction(OpMemberDecorate);
    dec->addIdOperand(id);
    dec->addImmediateOperand(member);
    dec->addImmediateOperand(decoration);
    if (num >= 0)
        dec->addImmediateOperand(num);

    decorations.push_back(std::unique_ptr<Instruction>(dec));
}

// Comments in header
Function* Builder::makeEntrypoint(const char* entryPoint)
{
    assert(! mainFunction);

    Block* entry;
    std::vector<Id> params;
    std::vector<Decoration> precisions;

    mainFunction = makeFunctionEntry(NoPrecision, makeVoidType(), entryPoint, params, precisions, &entry);

    return mainFunction;
}

// Comments in header
Function* Builder::makeFunctionEntry(Decoration precision, Id returnType, const char* name,
                                     const std::vector<Id>& paramTypes, const std::vector<Decoration>& precisions, Block **entry)
{
    // Make the function and initial instructions in it
    Id typeId = makeFunctionType(returnType, paramTypes);
    Id firstParamId = paramTypes.size() == 0 ? 0 : getUniqueIds((int)paramTypes.size());
    Function* function = new Function(getUniqueId(), returnType, typeId, firstParamId, module);

    // Set up the precisions
    setPrecision(function->getId(), precision);
    for (unsigned p = 0; p < (unsigned)precisions.size(); ++p)
        setPrecision(firstParamId + p, precisions[p]);

    // CFG
    if (entry) {
        *entry = new Block(getUniqueId(), *function);
        function->addBlock(*entry);
        setBuildPoint(*entry);
    }

    if (name)
        addName(function->getId(), name);

    functions.push_back(std::unique_ptr<Function>(function));

    return function;
}

// Comments in header
void Builder::makeReturn(bool implicit, Id retVal)
{
    if (retVal) {
        Instruction* inst = new Instruction(NoResult, NoType, OpReturnValue);
        inst->addIdOperand(retVal);
        buildPoint->addInstruction(std::unique_ptr<Instruction>(inst));
    } else
        buildPoint->addInstruction(std::unique_ptr<Instruction>(new Instruction(NoResult, NoType, OpReturn)));

    if (! implicit)
        createAndSetNoPredecessorBlock("post-return");
}

// Comments in header
void Builder::leaveFunction()
{
    Block* block = buildPoint;
    Function& function = buildPoint->getParent();
    assert(block);

    // If our function did not contain a return, add a return void now.
    if (! block->isTerminated()) {
        if (function.getReturnType() == makeVoidType())
            makeReturn(true);
        else {
            makeReturn(true, createUndefined(function.getReturnType()));
        }
    }
}

// Comments in header
void Builder::makeDiscard()
{
    buildPoint->addInstruction(std::unique_ptr<Instruction>(new Instruction(OpKill)));
    createAndSetNoPredecessorBlock("post-discard");
}

// Comments in header
Id Builder::createVariable(StorageClass storageClass, Id type, const char* name)
{
    Id pointerType = makePointer(storageClass, type);
    Instruction* inst = new Instruction(getUniqueId(), pointerType, OpVariable);
    inst->addImmediateOperand(storageClass);

    switch (storageClass) {
    case StorageClassFunction:
        // Validation rules require the declaration in the entry block
        buildPoint->getParent().addLocalVariable(std::unique_ptr<Instruction>(inst));
        break;

    default:
        constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(inst));
        module.mapInstruction(inst);
        break;
    }

    if (name)
        addName(inst->getResultId(), name);

    return inst->getResultId();
}

// Comments in header
Id Builder::createUndefined(Id type)
{
  Instruction* inst = new Instruction(getUniqueId(), type, OpUndef);
  buildPoint->addInstruction(std::unique_ptr<Instruction>(inst));
  return inst->getResultId();
}

// Comments in header
void Builder::createStore(Id rValue, Id lValue)
{
    Instruction* store = new Instruction(OpStore);
    store->addIdOperand(lValue);
    store->addIdOperand(rValue);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(store));
}

// Comments in header
Id Builder::createLoad(Id lValue)
{
    Instruction* load = new Instruction(getUniqueId(), getDerefTypeId(lValue), OpLoad);
    load->addIdOperand(lValue);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(load));

    return load->getResultId();
}

// Comments in header
Id Builder::createAccessChain(StorageClass storageClass, Id base, std::vector<Id>& offsets)
{
    // Figure out the final resulting type.
    spv::Id typeId = getTypeId(base);
    assert(isPointerType(typeId) && offsets.size() > 0);
    typeId = getContainedTypeId(typeId);
    for (int i = 0; i < (int)offsets.size(); ++i) {
        if (isStructType(typeId)) {
            assert(isConstantScalar(offsets[i]));
            typeId = getContainedTypeId(typeId, getConstantScalar(offsets[i]));
        } else
            typeId = getContainedTypeId(typeId, offsets[i]);
    }
    typeId = makePointer(storageClass, typeId);

    // Make the instruction
    Instruction* chain = new Instruction(getUniqueId(), typeId, OpAccessChain);
    chain->addIdOperand(base);
    for (int i = 0; i < (int)offsets.size(); ++i)
        chain->addIdOperand(offsets[i]);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(chain));

    return chain->getResultId();
}

Id Builder::createArrayLength(Id base, unsigned int member)
{
    Instruction* length = new Instruction(getUniqueId(), makeIntType(32), OpArrayLength);
    length->addIdOperand(base);
    length->addImmediateOperand(member);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(length));

    return length->getResultId();
}

Id Builder::createCompositeExtract(Id composite, Id typeId, unsigned index)
{
    // Generate code for spec constants if in spec constant operation
    // generation mode.
    if (generatingOpCodeForSpecConst) {
        return createSpecConstantOp(OpCompositeExtract, typeId, std::vector<Id>(1, composite), std::vector<Id>(1, index));
    }
    Instruction* extract = new Instruction(getUniqueId(), typeId, OpCompositeExtract);
    extract->addIdOperand(composite);
    extract->addImmediateOperand(index);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(extract));

    return extract->getResultId();
}

Id Builder::createCompositeExtract(Id composite, Id typeId, std::vector<unsigned>& indexes)
{
    // Generate code for spec constants if in spec constant operation
    // generation mode.
    if (generatingOpCodeForSpecConst) {
        return createSpecConstantOp(OpCompositeExtract, typeId, std::vector<Id>(1, composite), indexes);
    }
    Instruction* extract = new Instruction(getUniqueId(), typeId, OpCompositeExtract);
    extract->addIdOperand(composite);
    for (int i = 0; i < (int)indexes.size(); ++i)
        extract->addImmediateOperand(indexes[i]);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(extract));

    return extract->getResultId();
}

Id Builder::createCompositeInsert(Id object, Id composite, Id typeId, unsigned index)
{
    Instruction* insert = new Instruction(getUniqueId(), typeId, OpCompositeInsert);
    insert->addIdOperand(object);
    insert->addIdOperand(composite);
    insert->addImmediateOperand(index);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(insert));

    return insert->getResultId();
}

Id Builder::createCompositeInsert(Id object, Id composite, Id typeId, std::vector<unsigned>& indexes)
{
    Instruction* insert = new Instruction(getUniqueId(), typeId, OpCompositeInsert);
    insert->addIdOperand(object);
    insert->addIdOperand(composite);
    for (int i = 0; i < (int)indexes.size(); ++i)
        insert->addImmediateOperand(indexes[i]);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(insert));

    return insert->getResultId();
}

Id Builder::createVectorExtractDynamic(Id vector, Id typeId, Id componentIndex)
{
    Instruction* extract = new Instruction(getUniqueId(), typeId, OpVectorExtractDynamic);
    extract->addIdOperand(vector);
    extract->addIdOperand(componentIndex);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(extract));

    return extract->getResultId();
}

Id Builder::createVectorInsertDynamic(Id vector, Id typeId, Id component, Id componentIndex)
{
    Instruction* insert = new Instruction(getUniqueId(), typeId, OpVectorInsertDynamic);
    insert->addIdOperand(vector);
    insert->addIdOperand(component);
    insert->addIdOperand(componentIndex);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(insert));

    return insert->getResultId();
}

// An opcode that has no operands, no result id, and no type
void Builder::createNoResultOp(Op opCode)
{
    Instruction* op = new Instruction(opCode);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));
}

// An opcode that has one operand, no result id, and no type
void Builder::createNoResultOp(Op opCode, Id operand)
{
    Instruction* op = new Instruction(opCode);
    op->addIdOperand(operand);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));
}

// An opcode that has one operand, no result id, and no type
void Builder::createNoResultOp(Op opCode, const std::vector<Id>& operands)
{
    Instruction* op = new Instruction(opCode);
    for (auto it = operands.cbegin(); it != operands.cend(); ++it)
        op->addIdOperand(*it);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));
}

void Builder::createControlBarrier(Scope execution, Scope memory, MemorySemanticsMask semantics)
{
    Instruction* op = new Instruction(OpControlBarrier);
    op->addImmediateOperand(makeUintConstant(execution));
    op->addImmediateOperand(makeUintConstant(memory));
    op->addImmediateOperand(makeUintConstant(semantics));
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));
}

void Builder::createMemoryBarrier(unsigned executionScope, unsigned memorySemantics)
{
    Instruction* op = new Instruction(OpMemoryBarrier);
    op->addImmediateOperand(makeUintConstant(executionScope));
    op->addImmediateOperand(makeUintConstant(memorySemantics));
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));
}

// An opcode that has one operands, a result id, and a type
Id Builder::createUnaryOp(Op opCode, Id typeId, Id operand)
{
    // Generate code for spec constants if in spec constant operation
    // generation mode.
    if (generatingOpCodeForSpecConst) {
        return createSpecConstantOp(opCode, typeId, std::vector<Id>(1, operand), std::vector<Id>());
    }
    Instruction* op = new Instruction(getUniqueId(), typeId, opCode);
    op->addIdOperand(operand);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));

    return op->getResultId();
}

Id Builder::createBinOp(Op opCode, Id typeId, Id left, Id right)
{
    // Generate code for spec constants if in spec constant operation
    // generation mode.
    if (generatingOpCodeForSpecConst) {
        std::vector<Id> operands(2);
        operands[0] = left; operands[1] = right;
        return createSpecConstantOp(opCode, typeId, operands, std::vector<Id>());
    }
    Instruction* op = new Instruction(getUniqueId(), typeId, opCode);
    op->addIdOperand(left);
    op->addIdOperand(right);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));

    return op->getResultId();
}

Id Builder::createTriOp(Op opCode, Id typeId, Id op1, Id op2, Id op3)
{
    Instruction* op = new Instruction(getUniqueId(), typeId, opCode);
    op->addIdOperand(op1);
    op->addIdOperand(op2);
    op->addIdOperand(op3);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));

    return op->getResultId();
}

Id Builder::createOp(Op opCode, Id typeId, const std::vector<Id>& operands)
{
    Instruction* op = new Instruction(getUniqueId(), typeId, opCode);
    for (auto it = operands.cbegin(); it != operands.cend(); ++it)
        op->addIdOperand(*it);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));

    return op->getResultId();
}

Id Builder::createSpecConstantOp(Op opCode, Id typeId, const std::vector<Id>& operands, const std::vector<unsigned>& literals)
{
    Instruction* op = new Instruction(getUniqueId(), typeId, OpSpecConstantOp);
    op->addImmediateOperand((unsigned) opCode);
    for (auto it = operands.cbegin(); it != operands.cend(); ++it)
        op->addIdOperand(*it);
    for (auto it = literals.cbegin(); it != literals.cend(); ++it)
        op->addImmediateOperand(*it);
    module.mapInstruction(op);
    constantsTypesGlobals.push_back(std::unique_ptr<Instruction>(op));

    return op->getResultId();
}

Id Builder::createFunctionCall(spv::Function* function, std::vector<spv::Id>& args)
{
    Instruction* op = new Instruction(getUniqueId(), function->getReturnType(), OpFunctionCall);
    op->addIdOperand(function->getId());
    for (int a = 0; a < (int)args.size(); ++a)
        op->addIdOperand(args[a]);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));

    return op->getResultId();
}

// Comments in header
Id Builder::createRvalueSwizzle(Decoration precision, Id typeId, Id source, std::vector<unsigned>& channels)
{
    if (channels.size() == 1)
        return setPrecision(createCompositeExtract(source, typeId, channels.front()), precision);

    if (generatingOpCodeForSpecConst) {
        std::vector<Id> operands(2);
        operands[0] = operands[1] = source;
        return setPrecision(createSpecConstantOp(OpVectorShuffle, typeId, operands, channels), precision);
    }
    Instruction* swizzle = new Instruction(getUniqueId(), typeId, OpVectorShuffle);
    assert(isVector(source));
    swizzle->addIdOperand(source);
    swizzle->addIdOperand(source);
    for (int i = 0; i < (int)channels.size(); ++i)
        swizzle->addImmediateOperand(channels[i]);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(swizzle));

    return setPrecision(swizzle->getResultId(), precision);
}

// Comments in header
Id Builder::createLvalueSwizzle(Id typeId, Id target, Id source, std::vector<unsigned>& channels)
{
    assert(getNumComponents(source) == (int)channels.size());
    if (channels.size() == 1 && getNumComponents(source) == 1)
        return createCompositeInsert(source, target, typeId, channels.front());

    Instruction* swizzle = new Instruction(getUniqueId(), typeId, OpVectorShuffle);
    assert(isVector(source));
    assert(isVector(target));
    swizzle->addIdOperand(target);
    swizzle->addIdOperand(source);

    // Set up an identity shuffle from the base value to the result value
    unsigned int components[4];
    int numTargetComponents = getNumComponents(target);
    for (int i = 0; i < numTargetComponents; ++i)
        components[i] = i;

    // Punch in the l-value swizzle
    for (int i = 0; i < (int)channels.size(); ++i)
        components[channels[i]] = numTargetComponents + i;

    // finish the instruction with these components selectors
    for (int i = 0; i < numTargetComponents; ++i)
        swizzle->addImmediateOperand(components[i]);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(swizzle));

    return swizzle->getResultId();
}

// Comments in header
void Builder::promoteScalar(Decoration precision, Id& left, Id& right)
{
    int direction = getNumComponents(right) - getNumComponents(left);

    if (direction > 0)
        left = smearScalar(precision, left, makeVectorType(getTypeId(left), getNumComponents(right)));
    else if (direction < 0)
        right = smearScalar(precision, right, makeVectorType(getTypeId(right), getNumComponents(left)));

    return;
}

// Comments in header
Id Builder::smearScalar(Decoration precision, Id scalar, Id vectorType)
{
    assert(getNumComponents(scalar) == 1);
    assert(getTypeId(scalar) == getScalarTypeId(vectorType));

    int numComponents = getNumTypeComponents(vectorType);
    if (numComponents == 1)
        return scalar;

    Instruction* smear = nullptr;
    if (generatingOpCodeForSpecConst) {
        auto members = std::vector<spv::Id>(numComponents, scalar);
        // 'scalar' can not be spec constant here. All spec constant involved
        // promotion is done in createSpvConstantFromConstUnionArray().  This
        // 'if' branch is only accessed when 'scalar' is used in the def-chain
        // of other vector type spec constants. In such cases, all the
        // instructions needed to promote 'scalar' to a vector type constants
        // should be added at module level.
        auto result_id = makeCompositeConstant(vectorType, members, false);
        smear = module.getInstruction(result_id);
    } else {
        smear = new Instruction(getUniqueId(), vectorType, OpCompositeConstruct);
        for (int c = 0; c < numComponents; ++c)
            smear->addIdOperand(scalar);
        buildPoint->addInstruction(std::unique_ptr<Instruction>(smear));
    }

    return setPrecision(smear->getResultId(), precision);
}

// Comments in header
Id Builder::createBuiltinCall(Id resultType, Id builtins, int entryPoint, std::vector<Id>& args)
{
    Instruction* inst = new Instruction(getUniqueId(), resultType, OpExtInst);
    inst->addIdOperand(builtins);
    inst->addImmediateOperand(entryPoint);
    for (int arg = 0; arg < (int)args.size(); ++arg)
        inst->addIdOperand(args[arg]);

    buildPoint->addInstruction(std::unique_ptr<Instruction>(inst));

    return inst->getResultId();
}

// Accept all parameters needed to create a texture instruction.
// Create the correct instruction based on the inputs, and make the call.
Id Builder::createTextureCall(Decoration precision, Id resultType, bool sparse, bool fetch, bool proj, bool gather, bool noImplicitLod, const TextureParameters& parameters)
{
    static const int maxTextureArgs = 10;
    Id texArgs[maxTextureArgs] = {};

    //
    // Set up the fixed arguments
    //
    int numArgs = 0;
    bool explicitLod = false;
    texArgs[numArgs++] = parameters.sampler;
    texArgs[numArgs++] = parameters.coords;
    if (parameters.Dref)
        texArgs[numArgs++] = parameters.Dref;
    if (parameters.comp)
        texArgs[numArgs++] = parameters.comp;

    //
    // Set up the optional arguments
    //
    int optArgNum = numArgs;                        // track which operand, if it exists, is the mask of optional arguments
    ++numArgs;                                      // speculatively make room for the mask operand
    ImageOperandsMask mask = ImageOperandsMaskNone; // the mask operand
    if (parameters.bias) {
        mask = (ImageOperandsMask)(mask | ImageOperandsBiasMask);
        texArgs[numArgs++] = parameters.bias;
    }
    if (parameters.lod) {
        mask = (ImageOperandsMask)(mask | ImageOperandsLodMask);
        texArgs[numArgs++] = parameters.lod;
        explicitLod = true;
    } else if (parameters.gradX) {
        mask = (ImageOperandsMask)(mask | ImageOperandsGradMask);
        texArgs[numArgs++] = parameters.gradX;
        texArgs[numArgs++] = parameters.gradY;
        explicitLod = true;
    } else if (noImplicitLod && ! fetch && ! gather) {
        // have to explicitly use lod of 0 if not allowed to have them be implicit, and
        // we would otherwise be about to issue an implicit instruction
        mask = (ImageOperandsMask)(mask | ImageOperandsLodMask);
        texArgs[numArgs++] = makeFloatConstant(0.0);
        explicitLod = true;
    }
    if (parameters.offset) {
        if (isConstant(parameters.offset))
            mask = (ImageOperandsMask)(mask | ImageOperandsConstOffsetMask);
        else
            mask = (ImageOperandsMask)(mask | ImageOperandsOffsetMask);
        texArgs[numArgs++] = parameters.offset;
    }
    if (parameters.offsets) {
        mask = (ImageOperandsMask)(mask | ImageOperandsConstOffsetsMask);
        texArgs[numArgs++] = parameters.offsets;
    }
    if (parameters.sample) {
        mask = (ImageOperandsMask)(mask | ImageOperandsSampleMask);
        texArgs[numArgs++] = parameters.sample;
    }
    if (parameters.lodClamp) {
        // capability if this bit is used
        addCapability(CapabilityMinLod);

        mask = (ImageOperandsMask)(mask | ImageOperandsMinLodMask);
        texArgs[numArgs++] = parameters.lodClamp;
    }
    if (mask == ImageOperandsMaskNone)
        --numArgs;  // undo speculative reservation for the mask argument
    else
        texArgs[optArgNum] = mask;

    //
    // Set up the instruction
    //
    Op opCode = OpNop;  // All paths below need to set this
    if (fetch) {
        if (sparse)
            opCode = OpImageSparseFetch;
        else
            opCode = OpImageFetch;
    } else if (gather) {
        if (parameters.Dref)
            if (sparse)
                opCode = OpImageSparseDrefGather;
            else
                opCode = OpImageDrefGather;
        else
            if (sparse)
                opCode = OpImageSparseGather;
            else
                opCode = OpImageGather;
    } else if (explicitLod) {
        if (parameters.Dref) {
            if (proj)
                if (sparse)
                    opCode = OpImageSparseSampleProjDrefExplicitLod;
                else
                    opCode = OpImageSampleProjDrefExplicitLod;
            else
                if (sparse)
                    opCode = OpImageSparseSampleDrefExplicitLod;
                else
                    opCode = OpImageSampleDrefExplicitLod;
        } else {
            if (proj)
                if (sparse)
                    opCode = OpImageSparseSampleProjExplicitLod;
                else
                    opCode = OpImageSampleProjExplicitLod;
            else
                if (sparse)
                    opCode = OpImageSparseSampleExplicitLod;
                else
                    opCode = OpImageSampleExplicitLod;
        }
    } else {
        if (parameters.Dref) {
            if (proj)
                if (sparse)
                    opCode = OpImageSparseSampleProjDrefImplicitLod;
                else
                    opCode = OpImageSampleProjDrefImplicitLod;
            else
                if (sparse)
                    opCode = OpImageSparseSampleDrefImplicitLod;
                else
                    opCode = OpImageSampleDrefImplicitLod;
        } else {
            if (proj)
                if (sparse)
                    opCode = OpImageSparseSampleProjImplicitLod;
                else
                    opCode = OpImageSampleProjImplicitLod;
            else
                if (sparse)
                    opCode = OpImageSparseSampleImplicitLod;
                else
                    opCode = OpImageSampleImplicitLod;
        }
    }

    // See if the result type is expecting a smeared result.
    // This happens when a legacy shadow*() call is made, which
    // gets a vec4 back instead of a float.
    Id smearedType = resultType;
    if (! isScalarType(resultType)) {
        switch (opCode) {
        case OpImageSampleDrefImplicitLod:
        case OpImageSampleDrefExplicitLod:
        case OpImageSampleProjDrefImplicitLod:
        case OpImageSampleProjDrefExplicitLod:
            resultType = getScalarTypeId(resultType);
            break;
        default:
            break;
        }
    }

    Id typeId0 = 0;
    Id typeId1 = 0;

    if (sparse) {
        typeId0 = resultType;
        typeId1 = getDerefTypeId(parameters.texelOut);
        resultType = makeStructResultType(typeId0, typeId1);
    }

    // Build the SPIR-V instruction
    Instruction* textureInst = new Instruction(getUniqueId(), resultType, opCode);
    for (int op = 0; op < optArgNum; ++op)
        textureInst->addIdOperand(texArgs[op]);
    if (optArgNum < numArgs)
        textureInst->addImmediateOperand(texArgs[optArgNum]);
    for (int op = optArgNum + 1; op < numArgs; ++op)
        textureInst->addIdOperand(texArgs[op]);
    setPrecision(textureInst->getResultId(), precision);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(textureInst));

    Id resultId = textureInst->getResultId();

    if (sparse) {
        // set capability
        addCapability(CapabilitySparseResidency);

        // Decode the return type that was a special structure
        createStore(createCompositeExtract(resultId, typeId1, 1), parameters.texelOut);
        resultId = createCompositeExtract(resultId, typeId0, 0);
        setPrecision(resultId, precision);
    } else {
        // When a smear is needed, do it, as per what was computed
        // above when resultType was changed to a scalar type.
        if (resultType != smearedType)
            resultId = smearScalar(precision, resultId, smearedType);
    }

    return resultId;
}

// Comments in header
Id Builder::createTextureQueryCall(Op opCode, const TextureParameters& parameters)
{
    // All these need a capability
    addCapability(CapabilityImageQuery);

    // Figure out the result type
    Id resultType = 0;
    switch (opCode) {
    case OpImageQuerySize:
    case OpImageQuerySizeLod:
    {
        int numComponents = 0;
        switch (getTypeDimensionality(getImageType(parameters.sampler))) {
        case Dim1D:
        case DimBuffer:
            numComponents = 1;
            break;
        case Dim2D:
        case DimCube:
        case DimRect:
        case DimSubpassData:
            numComponents = 2;
            break;
        case Dim3D:
            numComponents = 3;
            break;

        default:
            assert(0);
            break;
        }
        if (isArrayedImageType(getImageType(parameters.sampler)))
            ++numComponents;
        if (numComponents == 1)
            resultType = makeIntType(32);
        else
            resultType = makeVectorType(makeIntType(32), numComponents);

        break;
    }
    case OpImageQueryLod:
        resultType = makeVectorType(makeFloatType(32), 2);
        break;
    case OpImageQueryLevels:
    case OpImageQuerySamples:
        resultType = makeIntType(32);
        break;
    default:
        assert(0);
        break;
    }

    Instruction* query = new Instruction(getUniqueId(), resultType, opCode);
    query->addIdOperand(parameters.sampler);
    if (parameters.coords)
        query->addIdOperand(parameters.coords);
    if (parameters.lod)
        query->addIdOperand(parameters.lod);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(query));

    return query->getResultId();
}

// External comments in header.
// Operates recursively to visit the composite's hierarchy.
Id Builder::createCompositeCompare(Decoration precision, Id value1, Id value2, bool equal)
{
    Id boolType = makeBoolType();
    Id valueType = getTypeId(value1);

    Id resultId = NoResult;

    int numConstituents = getNumTypeConstituents(valueType);

    // Scalars and Vectors

    if (isScalarType(valueType) || isVectorType(valueType)) {
        assert(valueType == getTypeId(value2));
        // These just need a single comparison, just have
        // to figure out what it is.
        Op op;
        switch (getMostBasicTypeClass(valueType)) {
        case OpTypeFloat:
            op = equal ? OpFOrdEqual : OpFOrdNotEqual;
            break;
        case OpTypeInt:
        default:
            op = equal ? OpIEqual : OpINotEqual;
            break;
        case OpTypeBool:
            op = equal ? OpLogicalEqual : OpLogicalNotEqual;
            precision = NoPrecision;
            break;
        }

        if (isScalarType(valueType)) {
            // scalar
            resultId = createBinOp(op, boolType, value1, value2);
        } else {
            // vector
            resultId = createBinOp(op, makeVectorType(boolType, numConstituents), value1, value2);
            setPrecision(resultId, precision);
            // reduce vector compares...
            resultId = createUnaryOp(equal ? OpAll : OpAny, boolType, resultId);
        }

        return setPrecision(resultId, precision);
    }

    // Only structs, arrays, and matrices should be left.
    // They share in common the reduction operation across their constituents.
    assert(isAggregateType(valueType) || isMatrixType(valueType));

    // Compare each pair of constituents
    for (int constituent = 0; constituent < numConstituents; ++constituent) {
        std::vector<unsigned> indexes(1, constituent);
        Id constituentType1 = getContainedTypeId(getTypeId(value1), constituent);
        Id constituentType2 = getContainedTypeId(getTypeId(value2), constituent);
        Id constituent1 = createCompositeExtract(value1, constituentType1, indexes);
        Id constituent2 = createCompositeExtract(value2, constituentType2, indexes);

        Id subResultId = createCompositeCompare(precision, constituent1, constituent2, equal);

        if (constituent == 0)
            resultId = subResultId;
        else
            resultId = setPrecision(createBinOp(equal ? OpLogicalAnd : OpLogicalOr, boolType, resultId, subResultId), precision);
    }

    return resultId;
}

// OpCompositeConstruct
Id Builder::createCompositeConstruct(Id typeId, std::vector<Id>& constituents)
{
    assert(isAggregateType(typeId) || (getNumTypeConstituents(typeId) > 1 && getNumTypeConstituents(typeId) == (int)constituents.size()));

    Instruction* op = new Instruction(getUniqueId(), typeId, OpCompositeConstruct);
    for (int c = 0; c < (int)constituents.size(); ++c)
        op->addIdOperand(constituents[c]);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(op));

    return op->getResultId();
}

// Vector or scalar constructor
Id Builder::createConstructor(Decoration precision, const std::vector<Id>& sources, Id resultTypeId)
{
    Id result = NoResult;
    unsigned int numTargetComponents = getNumTypeComponents(resultTypeId);
    unsigned int targetComponent = 0;

    // Special case: when calling a vector constructor with a single scalar
    // argument, smear the scalar
    if (sources.size() == 1 && isScalar(sources[0]) && numTargetComponents > 1)
        return smearScalar(precision, sources[0], resultTypeId);

    Id scalarTypeId = getScalarTypeId(resultTypeId);
    std::vector<Id> constituents;  // accumulate the arguments for OpCompositeConstruct
    for (unsigned int i = 0; i < sources.size(); ++i) {
        assert(! isAggregate(sources[i]));
        unsigned int sourceSize = getNumComponents(sources[i]);
        unsigned int sourcesToUse = sourceSize;
        if (sourcesToUse + targetComponent > numTargetComponents)
            sourcesToUse = numTargetComponents - targetComponent;

        for (unsigned int s = 0; s < sourcesToUse; ++s) {
            Id arg = sources[i];
            if (sourceSize > 1) {
                std::vector<unsigned> swiz;
                swiz.push_back(s);
                arg = createRvalueSwizzle(precision, scalarTypeId, arg, swiz);
            }

            if (numTargetComponents > 1)
                constituents.push_back(arg);
            else
                result = arg;
            ++targetComponent;
        }

        if (targetComponent >= numTargetComponents)
            break;
    }

    if (constituents.size() > 0)
        result = createCompositeConstruct(resultTypeId, constituents);

    return setPrecision(result, precision);
}

// Comments in header
Id Builder::createMatrixConstructor(Decoration precision, const std::vector<Id>& sources, Id resultTypeId)
{
    Id componentTypeId = getScalarTypeId(resultTypeId);
    int numCols = getTypeNumColumns(resultTypeId);
    int numRows = getTypeNumRows(resultTypeId);

    // Will use a two step process
    // 1. make a compile-time 2D array of values
    // 2. construct a matrix from that array

    // Step 1.

    // initialize the array to the identity matrix
    Id ids[maxMatrixSize][maxMatrixSize];
    Id  one = makeFloatConstant(1.0);
    Id zero = makeFloatConstant(0.0);
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            if (col == row)
                ids[col][row] = one;
            else
                ids[col][row] = zero;
        }
    }

    // modify components as dictated by the arguments
    if (sources.size() == 1 && isScalar(sources[0])) {
        // a single scalar; resets the diagonals
        for (int col = 0; col < 4; ++col)
            ids[col][col] = sources[0];
    } else if (isMatrix(sources[0])) {
        // constructing from another matrix; copy over the parts that exist in both the argument and constructee
        Id matrix = sources[0];
        int minCols = std::min(numCols, getNumColumns(matrix));
        int minRows = std::min(numRows, getNumRows(matrix));
        for (int col = 0; col < minCols; ++col) {
            std::vector<unsigned> indexes;
            indexes.push_back(col);
            for (int row = 0; row < minRows; ++row) {
                indexes.push_back(row);
                ids[col][row] = createCompositeExtract(matrix, componentTypeId, indexes);
                indexes.pop_back();
                setPrecision(ids[col][row], precision);
            }
        }
    } else {
        // fill in the matrix in column-major order with whatever argument components are available
        int row = 0;
        int col = 0;

        for (int arg = 0; arg < (int)sources.size(); ++arg) {
            Id argComp = sources[arg];
            for (int comp = 0; comp < getNumComponents(sources[arg]); ++comp) {
                if (getNumComponents(sources[arg]) > 1) {
                    argComp = createCompositeExtract(sources[arg], componentTypeId, comp);
                    setPrecision(argComp, precision);
                }
                ids[col][row++] = argComp;
                if (row == numRows) {
                    row = 0;
                    col++;
                }
            }
        }
    }


    // Step 2:  Construct a matrix from that array.
    // First make the column vectors, then make the matrix.

    // make the column vectors
    Id columnTypeId = getContainedTypeId(resultTypeId);
    std::vector<Id> matrixColumns;
    for (int col = 0; col < numCols; ++col) {
        std::vector<Id> vectorComponents;
        for (int row = 0; row < numRows; ++row)
            vectorComponents.push_back(ids[col][row]);
        Id column = createCompositeConstruct(columnTypeId, vectorComponents);
        setPrecision(column, precision);
        matrixColumns.push_back(column);
    }

    // make the matrix
    return setPrecision(createCompositeConstruct(resultTypeId, matrixColumns), precision);
}

// Comments in header
Builder::If::If(Id cond, Builder& gb) :
    builder(gb),
    condition(cond),
    elseBlock(0)
{
    function = &builder.getBuildPoint()->getParent();

    // make the blocks, but only put the then-block into the function,
    // the else-block and merge-block will be added later, in order, after
    // earlier code is emitted
    thenBlock = new Block(builder.getUniqueId(), *function);
    mergeBlock = new Block(builder.getUniqueId(), *function);

    // Save the current block, so that we can add in the flow control split when
    // makeEndIf is called.
    headerBlock = builder.getBuildPoint();

    function->addBlock(thenBlock);
    builder.setBuildPoint(thenBlock);
}

// Comments in header
void Builder::If::makeBeginElse()
{
    // Close out the "then" by having it jump to the mergeBlock
    builder.createBranch(mergeBlock);

    // Make the first else block and add it to the function
    elseBlock = new Block(builder.getUniqueId(), *function);
    function->addBlock(elseBlock);

    // Start building the else block
    builder.setBuildPoint(elseBlock);
}

// Comments in header
void Builder::If::makeEndIf()
{
    // jump to the merge block
    builder.createBranch(mergeBlock);

    // Go back to the headerBlock and make the flow control split
    builder.setBuildPoint(headerBlock);
    builder.createSelectionMerge(mergeBlock, SelectionControlMaskNone);
    if (elseBlock)
        builder.createConditionalBranch(condition, thenBlock, elseBlock);
    else
        builder.createConditionalBranch(condition, thenBlock, mergeBlock);

    // add the merge block to the function
    function->addBlock(mergeBlock);
    builder.setBuildPoint(mergeBlock);
}

// Comments in header
void Builder::makeSwitch(Id selector, int numSegments, std::vector<int>& caseValues, std::vector<int>& valueIndexToSegment, int defaultSegment,
                         std::vector<Block*>& segmentBlocks)
{
    Function& function = buildPoint->getParent();

    // make all the blocks
    for (int s = 0; s < numSegments; ++s)
        segmentBlocks.push_back(new Block(getUniqueId(), function));

    Block* mergeBlock = new Block(getUniqueId(), function);

    // make and insert the switch's selection-merge instruction
    createSelectionMerge(mergeBlock, SelectionControlMaskNone);

    // make the switch instruction
    Instruction* switchInst = new Instruction(NoResult, NoType, OpSwitch);
    switchInst->addIdOperand(selector);
    auto defaultOrMerge = (defaultSegment >= 0) ? segmentBlocks[defaultSegment] : mergeBlock;
    switchInst->addIdOperand(defaultOrMerge->getId());
    defaultOrMerge->addPredecessor(buildPoint);
    for (int i = 0; i < (int)caseValues.size(); ++i) {
        switchInst->addImmediateOperand(caseValues[i]);
        switchInst->addIdOperand(segmentBlocks[valueIndexToSegment[i]]->getId());
        segmentBlocks[valueIndexToSegment[i]]->addPredecessor(buildPoint);
    }
    buildPoint->addInstruction(std::unique_ptr<Instruction>(switchInst));

    // push the merge block
    switchMerges.push(mergeBlock);
}

// Comments in header
void Builder::addSwitchBreak()
{
    // branch to the top of the merge block stack
    createBranch(switchMerges.top());
    createAndSetNoPredecessorBlock("post-switch-break");
}

// Comments in header
void Builder::nextSwitchSegment(std::vector<Block*>& segmentBlock, int nextSegment)
{
    int lastSegment = nextSegment - 1;
    if (lastSegment >= 0) {
        // Close out previous segment by jumping, if necessary, to next segment
        if (! buildPoint->isTerminated())
            createBranch(segmentBlock[nextSegment]);
    }
    Block* block = segmentBlock[nextSegment];
    block->getParent().addBlock(block);
    setBuildPoint(block);
}

// Comments in header
void Builder::endSwitch(std::vector<Block*>& /*segmentBlock*/)
{
    // Close out previous segment by jumping, if necessary, to next segment
    if (! buildPoint->isTerminated())
        addSwitchBreak();

    switchMerges.top()->getParent().addBlock(switchMerges.top());
    setBuildPoint(switchMerges.top());

    switchMerges.pop();
}

Block& Builder::makeNewBlock()
{
    Function& function = buildPoint->getParent();
    auto block = new Block(getUniqueId(), function);
    function.addBlock(block);
    return *block;
}

Builder::LoopBlocks& Builder::makeNewLoop()
{
    // Older MSVC versions don't allow inlining of blocks below.
    LoopBlocks blocks = {makeNewBlock(), makeNewBlock(), makeNewBlock(), makeNewBlock()};
    loops.push(blocks);
    return loops.top();
}

void Builder::createLoopContinue()
{
    createBranch(&loops.top().continue_target);
    // Set up a block for dead code.
    createAndSetNoPredecessorBlock("post-loop-continue");
}

void Builder::createLoopExit()
{
    createBranch(&loops.top().merge);
    // Set up a block for dead code.
    createAndSetNoPredecessorBlock("post-loop-break");
}

void Builder::closeLoop()
{
    loops.pop();
}

void Builder::clearAccessChain()
{
    accessChain.base = NoResult;
    accessChain.indexChain.clear();
    accessChain.instr = NoResult;
    accessChain.swizzle.clear();
    accessChain.component = NoResult;
    accessChain.preSwizzleBaseType = NoType;
    accessChain.isRValue = false;
}

// Comments in header
void Builder::accessChainPushSwizzle(std::vector<unsigned>& swizzle, Id preSwizzleBaseType)
{
    // swizzles can be stacked in GLSL, but simplified to a single
    // one here; the base type doesn't change
    if (accessChain.preSwizzleBaseType == NoType)
        accessChain.preSwizzleBaseType = preSwizzleBaseType;

    // if needed, propagate the swizzle for the current access chain
    if (accessChain.swizzle.size()) {
        std::vector<unsigned> oldSwizzle = accessChain.swizzle;
        accessChain.swizzle.resize(0);
        for (unsigned int i = 0; i < swizzle.size(); ++i) {
            accessChain.swizzle.push_back(oldSwizzle[swizzle[i]]);
        }
    } else
        accessChain.swizzle = swizzle;

    // determine if we need to track this swizzle anymore
    simplifyAccessChainSwizzle();
}

// Comments in header
void Builder::accessChainStore(Id rvalue)
{
    assert(accessChain.isRValue == false);

    transferAccessChainSwizzle(true);
    Id base = collapseAccessChain();

    if (accessChain.swizzle.size() && accessChain.component != NoResult)
        MissingFunctionality("simultaneous l-value swizzle and dynamic component selection");

    // If swizzle still exists, it is out-of-order or not full, we must load the target vector,
    // extract and insert elements to perform writeMask and/or swizzle.
    Id source = NoResult;
    if (accessChain.swizzle.size()) {
        Id tempBaseId = createLoad(base);
        source = createLvalueSwizzle(getTypeId(tempBaseId), tempBaseId, rvalue, accessChain.swizzle);
    }

    // dynamic component selection
    if (accessChain.component != NoResult) {
        Id tempBaseId = (source == NoResult) ? createLoad(base) : source;
        source = createVectorInsertDynamic(tempBaseId, getTypeId(tempBaseId), rvalue, accessChain.component);
    }

    if (source == NoResult)
        source = rvalue;

    createStore(source, base);
}

// Comments in header
Id Builder::accessChainLoad(Decoration precision, Id resultType)
{
    Id id;

    if (accessChain.isRValue) {
        // transfer access chain, but keep it static, so we can stay in registers
        transferAccessChainSwizzle(false);
        if (accessChain.indexChain.size() > 0) {
            Id swizzleBase = accessChain.preSwizzleBaseType != NoType ? accessChain.preSwizzleBaseType : resultType;

            // if all the accesses are constants, we can use OpCompositeExtract
            std::vector<unsigned> indexes;
            bool constant = true;
            for (int i = 0; i < (int)accessChain.indexChain.size(); ++i) {
                if (isConstantScalar(accessChain.indexChain[i]))
                    indexes.push_back(getConstantScalar(accessChain.indexChain[i]));
                else {
                    constant = false;
                    break;
                }
            }

            if (constant)
                id = createCompositeExtract(accessChain.base, swizzleBase, indexes);
            else {
                // make a new function variable for this r-value
                Id lValue = createVariable(StorageClassFunction, getTypeId(accessChain.base), "indexable");

                // store into it
                createStore(accessChain.base, lValue);

                // move base to the new variable
                accessChain.base = lValue;
                accessChain.isRValue = false;

                // load through the access chain
                id = createLoad(collapseAccessChain());
            }
            setPrecision(id, precision);
        } else
            id = accessChain.base;  // no precision, it was set when this was defined
    } else {
        transferAccessChainSwizzle(true);
        // load through the access chain
        id = createLoad(collapseAccessChain());
        setPrecision(id, precision);
    }

    // Done, unless there are swizzles to do
    if (accessChain.swizzle.size() == 0 && accessChain.component == NoResult)
        return id;

    // Do remaining swizzling
    // First, static swizzling
    if (accessChain.swizzle.size()) {
        // static swizzle
        Id swizzledType = getScalarTypeId(getTypeId(id));
        if (accessChain.swizzle.size() > 1)
            swizzledType = makeVectorType(swizzledType, (int)accessChain.swizzle.size());
        id = createRvalueSwizzle(precision, swizzledType, id, accessChain.swizzle);
    }

    // dynamic single-component selection
    if (accessChain.component != NoResult)
        id = setPrecision(createVectorExtractDynamic(id, resultType, accessChain.component), precision);

    return id;
}

Id Builder::accessChainGetLValue()
{
    assert(accessChain.isRValue == false);

    transferAccessChainSwizzle(true);
    Id lvalue = collapseAccessChain();

    // If swizzle exists, it is out-of-order or not full, we must load the target vector,
    // extract and insert elements to perform writeMask and/or swizzle.  This does not
    // go with getting a direct l-value pointer.
    assert(accessChain.swizzle.size() == 0);
    assert(accessChain.component == NoResult);

    return lvalue;
}

// comment in header
Id Builder::accessChainGetInferredType()
{
    // anything to operate on?
    if (accessChain.base == NoResult)
        return NoType;
    Id type = getTypeId(accessChain.base);

    // do initial dereference
    if (! accessChain.isRValue)
        type = getContainedTypeId(type);

    // dereference each index
    for (auto it = accessChain.indexChain.cbegin(); it != accessChain.indexChain.cend(); ++it) {
        if (isStructType(type))
            type = getContainedTypeId(type, getConstantScalar(*it));
        else
            type = getContainedTypeId(type);
    }

    // dereference swizzle
    if (accessChain.swizzle.size() == 1)
        type = getContainedTypeId(type);
    else if (accessChain.swizzle.size() > 1)
        type = makeVectorType(getContainedTypeId(type), (int)accessChain.swizzle.size());

    // dereference component selection
    if (accessChain.component)
        type = getContainedTypeId(type);

    return type;
}

// comment in header
void Builder::eliminateDeadDecorations() {
    std::unordered_set<const Block*> reachable_blocks;
    std::unordered_set<Id> unreachable_definitions;
    // Collect IDs defined in unreachable blocks. For each function, label the
    // reachable blocks first. Then for each unreachable block, collect the
    // result IDs of the instructions in it.
    for (std::vector<Function*>::const_iterator fi = module.getFunctions().cbegin();
        fi != module.getFunctions().cend(); fi++) {
        Function* f = *fi;
        Block* entry = f->getEntryBlock();
        inReadableOrder(entry, [&reachable_blocks](const Block* b) {
            reachable_blocks.insert(b);
        });
        for (std::vector<Block*>::const_iterator bi = f->getBlocks().cbegin();
            bi != f->getBlocks().cend(); bi++) {
            Block* b = *bi;
            if (!reachable_blocks.count(b)) {
                for (std::vector<std::unique_ptr<Instruction> >::const_iterator
                         ii = b->getInstructions().cbegin();
                    ii != b->getInstructions().cend(); ii++) {
                    Instruction* i = ii->get();
                    unreachable_definitions.insert(i->getResultId());
                }
            }
        }
    }
    decorations.erase(std::remove_if(decorations.begin(), decorations.end(),
        [&unreachable_definitions](std::unique_ptr<Instruction>& I) -> bool {
            Instruction* inst = I.get();
            Id decoration_id = inst->getIdOperand(0);
            return unreachable_definitions.count(decoration_id) != 0;
        }),
        decorations.end());
}

void Builder::dump(std::vector<unsigned int>& out) const
{
    // Header, before first instructions:
    out.push_back(MagicNumber);
    out.push_back(Version);
    out.push_back(builderNumber);
    out.push_back(uniqueId + 1);
    out.push_back(0);

    // Capabilities
    for (auto it = capabilities.cbegin(); it != capabilities.cend(); ++it) {
        Instruction capInst(0, 0, OpCapability);
        capInst.addImmediateOperand(*it);
        capInst.dump(out);
    }

    // TBD: OpExtension ...

    dumpInstructions(out, imports);
    Instruction memInst(0, 0, OpMemoryModel);
    memInst.addImmediateOperand(addressModel);
    memInst.addImmediateOperand(memoryModel);
    memInst.dump(out);

    // Instructions saved up while building:
    dumpInstructions(out, entryPoints);
    dumpInstructions(out, executionModes);

    // Debug instructions
    if (source != SourceLanguageUnknown) {
        Instruction sourceInst(0, 0, OpSource);
        sourceInst.addImmediateOperand(source);
        sourceInst.addImmediateOperand(sourceVersion);
        sourceInst.dump(out);
    }
    for (int e = 0; e < (int)extensions.size(); ++e) {
        Instruction extInst(0, 0, OpSourceExtension);
        extInst.addStringOperand(extensions[e]);
        extInst.dump(out);
    }
    dumpInstructions(out, names);
    dumpInstructions(out, lines);

    // Annotation instructions
    dumpInstructions(out, decorations);

    dumpInstructions(out, constantsTypesGlobals);
    dumpInstructions(out, externals);

    // The functions
    module.dump(out);
}

//
// Protected methods.
//

// Turn the described access chain in 'accessChain' into an instruction
// computing its address.  This *cannot* include complex swizzles, which must
// be handled after this is called, but it does include swizzles that select
// an individual element, as a single address of a scalar type can be
// computed by an OpAccessChain instruction.
Id Builder::collapseAccessChain()
{
    assert(accessChain.isRValue == false);

    if (accessChain.indexChain.size() > 0) {
        if (accessChain.instr == 0) {
            StorageClass storageClass = (StorageClass)module.getStorageClass(getTypeId(accessChain.base));
            accessChain.instr = createAccessChain(storageClass, accessChain.base, accessChain.indexChain);
        }

        return accessChain.instr;
    } else
        return accessChain.base;

    // note that non-trivial swizzling is left pending...
}

// clear out swizzle if it is redundant, that is reselecting the same components
// that would be present without the swizzle.
void Builder::simplifyAccessChainSwizzle()
{
    // If the swizzle has fewer components than the vector, it is subsetting, and must stay
    // to preserve that fact.
    if (getNumTypeComponents(accessChain.preSwizzleBaseType) > (int)accessChain.swizzle.size())
        return;

    // if components are out of order, it is a swizzle
    for (unsigned int i = 0; i < accessChain.swizzle.size(); ++i) {
        if (i != accessChain.swizzle[i])
            return;
    }

    // otherwise, there is no need to track this swizzle
    accessChain.swizzle.clear();
    if (accessChain.component == NoResult)
        accessChain.preSwizzleBaseType = NoType;
}

// To the extent any swizzling can become part of the chain
// of accesses instead of a post operation, make it so.
// If 'dynamic' is true, include transferring a non-static component index,
// otherwise, only transfer static indexes.
//
// Also, Boolean vectors are likely to be special.  While
// for external storage, they should only be integer types,
// function-local bool vectors could use sub-word indexing,
// so keep that as a separate Insert/Extract on a loaded vector.
void Builder::transferAccessChainSwizzle(bool dynamic)
{
    // too complex?
    if (accessChain.swizzle.size() > 1)
        return;

    // non existent?
    if (accessChain.swizzle.size() == 0 && accessChain.component == NoResult)
        return;

    // single component...

    // skip doing it for Boolean vectors
    if (isBoolType(getContainedTypeId(accessChain.preSwizzleBaseType)))
        return;

    if (accessChain.swizzle.size() == 1) {
        // handle static component
        accessChain.indexChain.push_back(makeUintConstant(accessChain.swizzle.front()));
        accessChain.swizzle.clear();
        // note, the only valid remaining dynamic access would be to this one
        // component, so don't bother even looking at accessChain.component
        accessChain.preSwizzleBaseType = NoType;
        accessChain.component = NoResult;
    } else if (dynamic && accessChain.component != NoResult) {
        // handle dynamic component
        accessChain.indexChain.push_back(accessChain.component);
        accessChain.preSwizzleBaseType = NoType;
        accessChain.component = NoResult;
    }
}

// Utility method for creating a new block and setting the insert point to
// be in it. This is useful for flow-control operations that need a "dummy"
// block proceeding them (e.g. instructions after a discard, etc).
void Builder::createAndSetNoPredecessorBlock(const char* /*name*/)
{
    Block* block = new Block(getUniqueId(), buildPoint->getParent());
    block->setUnreachable();
    buildPoint->getParent().addBlock(block);
    setBuildPoint(block);

    //if (name)
    //    addName(block->getId(), name);
}

// Comments in header
void Builder::createBranch(Block* block)
{
    Instruction* branch = new Instruction(OpBranch);
    branch->addIdOperand(block->getId());
    buildPoint->addInstruction(std::unique_ptr<Instruction>(branch));
    block->addPredecessor(buildPoint);
}

void Builder::createSelectionMerge(Block* mergeBlock, unsigned int control)
{
    Instruction* merge = new Instruction(OpSelectionMerge);
    merge->addIdOperand(mergeBlock->getId());
    merge->addImmediateOperand(control);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(merge));
}

void Builder::createLoopMerge(Block* mergeBlock, Block* continueBlock, unsigned int control)
{
    Instruction* merge = new Instruction(OpLoopMerge);
    merge->addIdOperand(mergeBlock->getId());
    merge->addIdOperand(continueBlock->getId());
    merge->addImmediateOperand(control);
    buildPoint->addInstruction(std::unique_ptr<Instruction>(merge));
}

void Builder::createConditionalBranch(Id condition, Block* thenBlock, Block* elseBlock)
{
    Instruction* branch = new Instruction(OpBranchConditional);
    branch->addIdOperand(condition);
    branch->addIdOperand(thenBlock->getId());
    branch->addIdOperand(elseBlock->getId());
    buildPoint->addInstruction(std::unique_ptr<Instruction>(branch));
    thenBlock->addPredecessor(buildPoint);
    elseBlock->addPredecessor(buildPoint);
}

void Builder::dumpInstructions(std::vector<unsigned int>& out, const std::vector<std::unique_ptr<Instruction> >& instructions) const
{
    for (int i = 0; i < (int)instructions.size(); ++i) {
        instructions[i]->dump(out);
    }
}

void TbdFunctionality(const char* tbd)
{
    static std::unordered_set<const char*> issued;

    if (issued.find(tbd) == issued.end()) {
        printf("TBD functionality: %s\n", tbd);
        issued.insert(tbd);
    }
}

void MissingFunctionality(const char* fun)
{
    printf("Missing functionality: %s\n", fun);
}

}; // end spv namespace
