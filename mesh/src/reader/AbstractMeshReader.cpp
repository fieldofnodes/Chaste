/*

Copyright (c) 2005-2012, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <cassert>
#include "AbstractMeshReader.hpp"
#include "Exception.hpp"

///////////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////////

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
unsigned AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetNumElementAttributes() const
{
    // By default returns 0.  If a concrete class does read attributes
    // it needs to overload this method.
    return 0;
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
unsigned AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetNumEdges() const
{
    return GetNumFaces();
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
unsigned AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetNumFaceAttributes() const
{
    // By default returns 0.  If a concrete class does read attributes
    // it needs to overload this method.
    return 0;
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
std::vector<double> AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetNodeAttributes()
{
    // By default returns an empty vector.  If a concrete class does read node attributes
    // it needs to overload this method.
    std::vector<double> empty;
    return empty;
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
ElementData AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetNextEdgeData()
{
    return GetNextFaceData();
}


template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
std::vector<double> AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetNode(unsigned index)
{
    EXCEPTION("Random access is only implemented in mesh readers for binary mesh files.");
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
ElementData AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetElementData(unsigned index)
{
    EXCEPTION("Random access is only implemented in mesh readers for binary mesh files.");
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
ElementData AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetFaceData(unsigned index)
{
    EXCEPTION("Random access is only implemented in mesh readers for binary mesh files.");
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
ElementData AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetEdgeData(unsigned index)
{
    return GetFaceData(index);
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
std::vector<unsigned> AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetContainingElementIndices(unsigned index)
{
    EXCEPTION("Ncl files are only implemented in mesh readers for binary mesh files.");
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
std::string AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetMeshFileBaseName()
{
    return "";
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
bool AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::IsFileFormatBinary()
{
    return false;
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
bool AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::HasNclFile()
{
    return false;
}


// Cable elements aren't supported in most formats

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
unsigned AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetNumCableElements() const
{
    return 0u;
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
unsigned AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetNumCableElementAttributes() const
{
    return 0u;
}

template<unsigned ELEMENT_DIM, unsigned SPACE_DIM>
ElementData AbstractMeshReader<ELEMENT_DIM, SPACE_DIM>::GetNextCableElementData()
{
    EXCEPTION("Cable elements are not supported by this mesh format.");
}


/////////////////////////////////////////////////////////////////////////////////////
// Explicit instantiation
/////////////////////////////////////////////////////////////////////////////////////

template class AbstractMeshReader<0,1>;
template class AbstractMeshReader<1,1>;
template class AbstractMeshReader<1,2>;
template class AbstractMeshReader<1,3>;
template class AbstractMeshReader<2,2>;
template class AbstractMeshReader<2,3>;
template class AbstractMeshReader<3,3>;
