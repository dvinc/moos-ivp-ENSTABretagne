/**
 * \file ValidationGate_Tests.cpp
 * \brief Tests de l'application pValidationGate
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Tests unitaires à valider pour vérifier le bon fonctionnement 
 * de l'application MOOS
 *
 */

#include <cstdlib>
#include <iostream>
#include "ValidationGate_Tests.h"

/**
 * \fn
 * \brief Procedure: launchTestsAndExitIfOk
 */

void launchTestsAndExitIfOk()
{
	Tests sessionDeTests("pValidationGate");
	sessionDeTests.afficherConclusionTests();
}