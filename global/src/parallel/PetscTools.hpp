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

#ifndef PETSCTOOLS_HPP_
#define PETSCTOOLS_HPP_

/**
 * @file
 * Contains the PetscTools class.
 */

#include <string>
#include <vector>
#include <cstdlib> // For EXIT_FAILURE

#include <petsc.h>
#include <petscvec.h>
#include <petscmat.h>

/** For use in tests that do not work when run in parallel. */
#define EXIT_IF_PARALLEL if(PetscTools::IsParallel()){TS_TRACE("This test does not pass in parallel yet.");return;}
/** For use in tests that should ONLY be run in parallel. */
#define EXIT_IF_SEQUENTIAL if(PetscTools::IsSequential()){TS_TRACE("This test is not meant to be executed in sequential.");return;}

#ifndef NDEBUG
// Uncomment this to trace calls to PetscTools::Barrier
//#define DEBUG_BARRIERS
#endif

#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2) //PETSc 3.2 or later
typedef PetscBool PetscTruth;
/**
 * This macro is for converting a pre-PETSc3.2 "Destroy" function call (which involves an object, such as 
 * PetscViewerDestroy(VecView view) ) to a PETSc3.2 destroy via pointer call. 
 * Note that we only use this macro for calls which appear rarely in the code.  Please destroy Vec and Mat objects
 * via the overloaded PetscTools::Destroy methods.
 * @param x The object to destroy
 */
#define PETSC_DESTROY_PARAM(x) &x
#else
/**
 * This macro is for converting a pre-PETSc3.2 "Destroy" function call (which involves an object, such as 
 * PetscViewerDestroy(VecView view) ) to a PETSc3.2 destroy via pointer call. 
 * Note that we only use this macro for calls which appear rarely in the code.  Please destroy Vec and Mat objects
 * via the overloaded PetscTools::Destroy methods.
 * @param x The object to destroy
 */
#define PETSC_DESTROY_PARAM(x) x
#endif

/**
 * A helper class of static methods.
 */
class PetscTools
{
private:

    /** Whether PETSc has been initialised. */
    static bool mPetscIsInitialised;

    /** The total number of processors. */
    static unsigned mNumProcessors;

#ifdef DEBUG_BARRIERS
    /** Used to debug number of barriers. */
    static unsigned mNumBarriers;
#endif

    /** Which processors we are. */
    static unsigned mRank;

    /** Whether to pretend that we're just running many master processes independently. */
    static bool mIsolateProcesses;

    /** Private method makes sure that (if this is the first use within a test) then PETSc has been probed. */
    static inline void CheckCache()
    {
        if (mNumProcessors == 0)
        {
            ResetCache();
        }
    }

public:

    /**
     * As a convention, we consider processor 0 the master process.
     */
    static const unsigned MASTER_RANK=0;

    /**
     * Reset our cached values: whether PETSc is initialised,
     * how many processors there are, and which one we are.
     */
    static void ResetCache();

    /**
     * Just returns whether there is one process or not.
     */
    static bool IsSequential();

    /**
     * Just returns whether there is more than one process.
     */
    static bool IsParallel();

    /**
     * Returns total number of processors.
     */
    static unsigned GetNumProcs();

    /**
     * Return our rank.
     *
     * If PETSc has not been initialized, returns 0.
     */
    static unsigned GetMyRank();

    /**
     * Just returns whether it is the master process or not.
     *
     * If not running in parallel, always returns true.
     */
    static bool AmMaster();

    /**
     * Just returns whether it is the right-most process or not.
     *
     * If not running in parallel, always returns true.
     */
    static bool AmTopMost();

    /**
     * If MPI is set up, perform a barrier synchronisation.
     * If not, it's a noop.
     *
     * @param callerId  only used in debug mode; printed before & after the barrier call
     */
    static void Barrier(const std::string callerId="");

    /**
     * Call at the start of a block of code that should be executed by each process in turn.
     */
    static void BeginRoundRobin();

    /**
     * Call at the end of a block of code that should be executed by each process in turn.
     */
    static void EndRoundRobin();

    /**
     * Where work can be split between isolated processes, it would be nice to be able to do
     * so easily without worrying about collective calls made inside classes such as OutputFileHandler
     * leading to deadlock.
     * This method attempts to enable this behaviour.  If the flag is set then AmMaster always
     * returns true, Barrier becomes a no-op, and ReplicateBool doesn't replicate.
     *
     * @param isolate  whether to consider processes as isolated
     */
    static void IsolateProcesses(bool isolate=true);

    /**
     * Create a vector of the specified size. SetFromOptions is called.
     *
     * @param size  the size of the vector
     * @param localSize  the local number of items owned by this process
     * @param ignoreOffProcEntries whether to ignore entries destined to be stored on a separate processor when assembling (eliminates global reductions).
     */
    static Vec CreateVec(int size, int localSize=PETSC_DECIDE, bool ignoreOffProcEntries = true);

    /**
     * Create a Vec from the given data.
     *
     * @param data  some data
     */
    static Vec CreateVec(std::vector<double> data);

    /**
     * Create a vector of the specified size with all values set to be the given
     * constant. SetFromOptions is called.
     *
     * @param size  the size of the vector
     * @param value  the value to set each entry
     */
    static Vec CreateAndSetVec(int size, double value);

    /**
     * Set up a matrix - set the size using the given parameters. The number of local rows
     * and columns is by default PETSC_DECIDE. SetFromOptions is called.
     *
     * @param rMat the matrix
     * @param numRows the number of rows in the matrix
     * @param numColumns the number of columns in the matrix
     * @param rowPreallocation the max number of nonzero entries expected on a row
     *   A value of 0 is allowed: no preallocation is then done and the user must
     *   preallocate the memory for the matrix themselves.
     * @param numLocalRows the number of local rows (defaults to PETSC_DECIDE)
     * @param numLocalColumns the number of local columns (defaults to PETSC_DECIDE)
     * @param ignoreOffProcEntries tells PETSc to drop off-processor entries
     */
    static void SetupMat(Mat& rMat, int numRows, int numColumns,
                         unsigned rowPreallocation,
                         int numLocalRows=PETSC_DECIDE,
                         int numLocalColumns=PETSC_DECIDE,
                         bool ignoreOffProcEntries=true);

    /**
     * Boolean AND of a flags between processes
     *
     * @param flag is set to true on this process.
     * @return true if any process' flag is set to true.
     */
    static bool ReplicateBool(bool flag);

    /**
     * Ensure exceptions are handled cleanly in parallel code, by causing all processes to
     * throw if any one does.
     *
     * @param flag is set to true if this process has thrown.
     */
    static void ReplicateException(bool flag);

    /**
     * Dumps a given PETSc object to disk.
     *
     * @param rMat a matrix
     * @param rOutputFileFullPath where to dump the matrix to disk
     */
    static void DumpPetscObject(const Mat& rMat, const std::string& rOutputFileFullPath);

    /**
     * Dumps a given PETSc object to disk.
     *
     * @param rVec a vector
     * @param rOutputFileFullPath where to dump the vector to disk
     */
    static void DumpPetscObject(const Vec& rVec, const std::string& rOutputFileFullPath);

    /**
     * Read a previously dumped PETSc object from disk.
     *
     * @param rMat a matrix
     * @param rOutputFileFullPath where to read the matrix from
     * @param rParallelLayout If provided, rMat will have the same parallel layout. Its content is irrelevant.
     */
    static void ReadPetscObject(Mat& rMat, const std::string& rOutputFileFullPath, Vec rParallelLayout=NULL);

    /**
     * Read a previously dumped PETSc object from disk.
     *
     * @param rVec a vector
     * @param rOutputFileFullPath where to read the matrix from
     * @param rParallelLayout If provided, rMat will have the same parallel layout. Its content is irrelevant.
     */
    static void ReadPetscObject(Vec& rVec, const std::string& rOutputFileFullPath, Vec rParallelLayout=NULL);

    /**
     * Destroy method
     * Note that PETSc 3.1 and previous destroy based on a PETSc object
     * but PETSc 3.2 and later destroy based on a pointer to a PETSc object
     * 
     * @param rVec a reference to the PETSc object
     */
     static inline void Destroy(Vec& rVec)
     {
#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2) //PETSc 3.2 or later
        VecDestroy(&rVec);
#else
        VecDestroy(rVec);
#endif   
     }
     
    /**
     * Destroy method
     * Note that PETSc 3.1 and previous destroy based on a PETSc object
     * but PETSc 3.2 and later destroy based on a pointer to a PETSc object
     * 
     * @param rMat a reference to the PETSc object
     */
     static inline void Destroy(Mat& rMat)
     {
#if (PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR >= 2) //PETSc 3.2 or later
        MatDestroy(&rMat);
#else
        MatDestroy(rMat);
#endif   
     }        
};

#endif /*PETSCTOOLS_HPP_*/
