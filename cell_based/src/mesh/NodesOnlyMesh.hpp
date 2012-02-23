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

#ifndef NODESONLYMESH_HPP_
#define NODESONLYMESH_HPP_

#include "ChasteSerialization.hpp"
#include <boost/serialization/base_object.hpp>
#include "BoxCollection.hpp"
#include "MutableMesh.hpp"

/**
 * Mesh class for storing lists of nodes (no elements). This inherits from MutableMesh
 * because we want to be able to add and delete nodes.
 */
template<unsigned SPACE_DIM>
class NodesOnlyMesh: public MutableMesh<SPACE_DIM, SPACE_DIM>
{
private:

    /**
     * Vector of radii of cells corresponding to nodes.
     * Each radius is set to 0.5 by default in the method
     * ConstructNodesWithoutMesh()
     */
    std::vector<double> mCellRadii;

    /**
     * A pointer to a box collection. Used to calculate neighbourhood information
     * for nodes in the mesh
     */
    BoxCollection<SPACE_DIM>* mpBoxCollection;

    /**
     * The maximum interaction distance for two nodes. Defines the maximum
     * distance between two `neighbouring` nodes.
     */
    double mMaximumInteractionDistance;


    friend class TestNodesOnlyMesh;

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Archives the member variables of the object which have to be preserved
     * during its lifetime.
     *
     * Note that we must archive any member variables FIRST so that this
     * method can call a ReMesh (to convert from TrianglesMeshReader input
     * format into our native format).
     *
     * @param archive the archive
     * @param version the current version of this class
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<MutableMesh<SPACE_DIM, SPACE_DIM> >(*this);
        /*
         * Note that the MutableMesh archiver does a remesh. If there are deleted nodes
         * then we want to wait for them to be re-numbered before archiving the radii.
         */
        archive & mCellRadii;
    }

public:

    /**
     * Default constructor to initialise BoxCollection to NULL.
     */
    NodesOnlyMesh();

    /**
     * Over-written destructor to delete pointer to BoxCollection
     */
    ~NodesOnlyMesh();

    /**
     * Construct the mesh using only nodes. No mesh is created, but the nodes are stored.
     * The original vector of nodes is deep-copied: new node objects are made with are
     * independent of the pointers in the input so that they can be safely deleted.
     *
     * If this is the only way of constructing a mesh of this type, then we can be certain that
     * elements and boundary elements are always unused.
     *
     * @param rNodes a vector of pointers to nodes
     */
    void ConstructNodesWithoutMesh(const std::vector<Node<SPACE_DIM>*>& rNodes);

    /**
     * A Helper method to enable you to construct a nodes-only mesh by stripping the nodes
     * TetrahedralMesh, this calls the ConstructNodesWithoutMesh method with the nodes
     *
     * If this is the only way of constructing a mesh of this type, then we can be certain that
     * elements and boundary elements are always unused.
     *
     * @param rGeneratingMesh any mesh with nodes, used to generate the NodesOnlyMesh
     */
    void ConstructNodesWithoutMesh(const AbstractMesh<SPACE_DIM,SPACE_DIM>& rGeneratingMesh);

    /**
     * Overridden Clear() method for NodesOnlyMesh.
     * Clears mCellRadii in addition to calling Clear() on the parent class.
     */
    void Clear();

    /**
     * Get the cell radius associated with a given node index.
     *
     * @param index the index of a node
     */
    double GetCellRadius(unsigned index);

    /**
     * Set the cell radius associated with a given node index.
     *
     * @param index the index of a node
     * @param radius the cell radius
     */
    void SetCellRadius(unsigned index, double radius);

    /**
     * Get mpBoxCollection
     *
     * @return mpBoxCollection
     */
    BoxCollection<SPACE_DIM>* GetBoxCollection();

    /**
     * Clear the BoxCollection
     */
    void ClearBoxCollection();

    /**
     * Set up the box collection
     *
     * @param cutOffLength the cut off length for node neighbours
     * @param domainSize the size of the domain containing the nodes.
     */
    void SetUpBoxCollection(double cutOffLength, c_vector<double, 2*SPACE_DIM> domainSize);

    /**
     * Set the maximum interaction distance between nodes
     *
     * @param maximumInteractionDistance the maximum distance between two neighbouring nodes
     */
    void SetMaximumInteractionDistance(double maximumInteractionDistance);

    /**
     * Calculate pairs of nodes using the BoxCollection
     *
     * @param rNodePairs reference to the set of node pairs to populate.
     */
    void CalculateNodePairs(std::set<std::pair<Node<SPACE_DIM>*, Node<SPACE_DIM>*> >& rNodePairs);

    /**
     * Overridden ReMesh() method.
     *
     * @param rMap a reference to a nodemap which should be created with the required number of nodes.
     */
    void ReMesh(NodeMap& rMap);

    /**
     * Overridden AddNode() method.
     *
     * @param pNewNode  pointer to the new node
     */
    unsigned AddNode(Node<SPACE_DIM>* pNewNode);

    /**
     * Overridden DeleteNode() method.
     *
     * @param index is the index of the node to be deleted
     */
    void DeleteNode(unsigned index);
};

#include "SerializationExportWrapper.hpp"
EXPORT_TEMPLATE_CLASS_SAME_DIMS(NodesOnlyMesh)

#endif /*NODESONLYMESH_HPP_*/
