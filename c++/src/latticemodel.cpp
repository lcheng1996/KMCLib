/*
  Copyright (c)  2012  Mikael Leetmaa

  This file is part of the KMCLib project distributed under the terms of the
  GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
*/


/*! \file  latticemodel.cpp
 *  \brief File for the implementation code of the LatticeModel class.
 */


#include "latticemodel.h"
#include "configuration.h"
#include "random.h"

#include <cstdio>

// -----------------------------------------------------------------------------
//
LatticeModel::LatticeModel(Configuration & configuration,
                           const LatticeMap & lattice_map,
                           const Interactions & interactions) :
    configuration_(configuration),
    lattice_map_(lattice_map),
    interactions_(interactions)
{
    // ML: Temporarily init the random number generator here.
    seedRandom(false, 2013);

    printf(" KMCLib: Calculating initial matching.\n");
    // Setup the mapping between coordinates and processes.
    calculateInitialMatching();

    printf(" KMCLib: Update the probability table.\n");

    // Initialize the interactions table here.
    interactions_.updateProbabilityTable();

    printf(" KMCLib: Setup done.\n");
}


// -----------------------------------------------------------------------------
//
void LatticeModel::calculateInitialMatching()
{
    // Match all centeres.
    std::vector<int> indices;

    for(size_t i = 0; i < configuration_.elements().size(); ++i)
    {
        indices.push_back(i);
    }
    matcher_.calculateMatching(interactions_,
                               indices,
                               lattice_map_,
                               configuration_);
}


// -----------------------------------------------------------------------------
//
void LatticeModel::singleStep()
{

    printf(" KMCLib: Picking a process.\n");
    // Select a process.
    Process & process = interactions_.pickProcess();

    printf(" KMCLib: Picking a site.\n");

    // Select a site.
    const int site_index = process.pickSite();

    printf(" KMCLib: Performing the process.\n");

    // Perform the operation.
    configuration_.performProcess(process, site_index, lattice_map_);

    // Propagate the time.
    //timer_.propagateGlobalTime();
    // NEEDS IMPLEMENTATION

    // Write/update the trajectory.
    // NEEDS IMPLEMENTATION
    // printf("Taking a step.\n");

    printf(" KMCLib: Get the affected neighbours.\n");

    // Run the re-matching of the affected sites and their neighbours.
    const std::vector<int> & indices = \
        lattice_map_.supersetNeighbourIndices(process.affectedIndices());

    printf(" KMCLib: Re-calculate the matching.\n");

    matcher_.calculateMatching(interactions_,
                               indices,
                               lattice_map_,
                               configuration_);

    printf(" KMCLib: Update the probability table.\n");

    // Update the interactions' probability table.
    interactions_.updateProbabilityTable();

    // Ready for the next step.
    printf(" KMCLib: Ready for the next step.\n");
}

