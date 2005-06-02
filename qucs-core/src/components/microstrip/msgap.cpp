/*
 * msgap.cpp - microstrip gap class implementation
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2004 Michael Margraf <Michael.Margraf@alumni.TU-Berlin.DE>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id: msgap.cpp,v 1.9 2005/06/02 18:17:55 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "substrate.h"
#include "constants.h"
#include "matrix.h"
#include "msopen.h"
#include "msgap.h"

msgap::msgap () : circuit (2) {
  type = CIR_MSGAP;
}

void msgap::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));
}

matrix msgap::calcMatrixY (nr_double_t frequency) {

  /* how to get properties of this component, e.g. W */
  nr_double_t W1 = getPropertyDouble ("W1");
  nr_double_t W2 = getPropertyDouble ("W2");
  nr_double_t s  = getPropertyDouble ("s");
  char * SModel  = getPropertyString ("MSModel");
  char * DModel  = getPropertyString ("MSDispModel");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");

  nr_double_t Q1, Q2, Q3, Q4, Q5;
  bool flip = false;
  if (W2 < W1) {  // equations are valid for 1 <= W2/W1 <= 3
    Q1 = W1;
    W1 = W2;
    W2 = Q1;
    flip = true;
  }

  // calculate parallel open end capacitances
  nr_double_t C1 = msopen::calcCend (frequency, W1, h, t, er,
				     SModel, DModel, "Kirschning");
  nr_double_t C2 = msopen::calcCend (frequency, W2, h, t, er,
				     SModel, DModel, "Kirschning");

  W2 /= W1;
  W1 /= h;
  s  /= h;

  // local variables
  Q5 = 1.23 / (1.0 + 0.12 * pow (W2 - 1.0, 0.9));
  Q1 = 0.04598 * (0.03 + pow (W1, Q5)) * (0.272 + 0.07 * er);
  Q2 = 0.107 * (W1 + 9.0) * pow (s, 3.23) +
    2.09 * pow (s, 1.05) * (1.5 + 0.3 * W1) / (1.0 + 0.6 * W1);
  Q3 = exp (-0.5978 * pow (W2, +1.35)) - 0.55;
  Q4 = exp (-0.5978 * pow (W2, -1.35)) - 0.55;

  nr_double_t Cs = 5e-10 * h * exp (-1.86 * s) * Q1 *
    (1.0 + 4.19 * (1.0 - exp (-0.785 * sqrt (1.0 / W1) * W2)));
  C1 *= (Q2 + Q3) / (Q2 + 1.0);
  C2 *= (Q2 + Q4) / (Q2 + 1.0);

  if (flip) { // if neccessary flip ports back
    Q1 = C1;
    C1 = C2;
    C2 = Q1;
  }

  // build Y-parameter matrix
  complex y21 = rect (0.0, -2.0 * M_PI * frequency * Cs);
  complex y11 = rect (0.0,  2.0 * M_PI * frequency * (C1 + Cs));
  complex y22 = rect (0.0,  2.0 * M_PI * frequency * (C2 + Cs));
  matrix y (2);
  y.set (0, 0, y11);
  y.set (0, 1, y21);
  y.set (1, 0, y21);
  y.set (1, 1, y22);
  return y;
}

void msgap::initDC (void) {
  allocMatrixMNA ();
  clearY ();
}

void msgap::calcAC (nr_double_t frequency) {
  setMatrixY (calcMatrixY (frequency));
}
