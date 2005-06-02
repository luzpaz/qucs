/*
 * spsolver.h - S-parameter solver class definitions
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: spsolver.h,v 1.12 2005/06/02 18:17:51 raimi Exp $
 *
 */

#ifndef __SPSOLVER_H__
#define __SPSOLVER_H__

class analysis;
class circuit;
class node;
class complex;
class vector;
class sweep;
class nodelist;

class spsolver : public analysis
{
 public:
  spsolver ();
  spsolver (char *);
  spsolver (spsolver &);
  ~spsolver ();
  void calc (nr_double_t);
  void init (void);
  void reduce (void);
  void solve (void);
  void insertConnections (void);
  void insertDifferentialPorts (void);
  void insertTee (node **, char *);
  void insertCross (node **, char *);
  void insertConnectors (node *);
  void insertOpen (node *);
  void insertGround (node *);
  circuit * interconnectJoin (node *, node *);
  circuit * connectedJoin (node *, node *);
  void noiseConnect (circuit *, node *, node *);
  void noiseInterconnect (circuit *, node *, node *);
  void saveResults (nr_double_t);
  void saveNoiseResults (complex[4], complex[4], nr_double_t, vector *);
  void saveVariable (char *, complex, vector *);
  char * createSP (int, int);
  void dropTee (circuit *);
  void dropCross (circuit *);
  void dropOpen (circuit *);
  void dropGround (circuit *);
  void dropDifferentialPort (circuit *);
  void dropConnections (void);

 private:
  int tees, crosses, grounds, opens;
  int noise;
  sweep * swp;
  nodelist * nlist;
  circuit * gnd;
};

#endif /* __SPSOLVER_H__ */
