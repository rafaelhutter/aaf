//=---------------------------------------------------------------------=
//
// $Id: LoadPhase.cpp,v 1.1 2005/08/18 16:04:20 greek_fire Exp $
//
// The contents of this file are subject to the AAF SDK Public
// Source License Agreement (the "License"); You may not use this file
// except in compliance with the License.  The License is available in
// AAFSDKPSL.TXT, or you may obtain a copy of the License from the AAF
// Association or its successor.
//
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
// the License for the specific language governing rights and limitations
// under the License.
//
// The Original Code of this file is Copyright 1998-2004, Licensor of the
// AAF Association.
//
//=---------------------------------------------------------------------=

#include "LoadPhase.h"

#include <TestGraph.h>

namespace {

using namespace aafanalyzer;

} // end of namespace


//======================================================================
//======================================================================
//======================================================================

namespace aafanalyzer 	
{

LoadPhase::LoadPhase(std::ostream& os, const std::basic_string<wchar_t> AAFFile) 
: TestPhase(os),
  _FileName(AAFFile)
{
}

LoadPhase::~LoadPhase()
{
}

boost::shared_ptr<TestGraph> LoadPhase::GetTestGraph()
{
  return _spTestGraph;
}

std::vector<TestResult> LoadPhase::Execute() 
{
  std::vector<TestResult> LoadTest;

  //load the AAF file and create the graph
  FileLoad load(GetOutStream(), _FileName);
  LoadTest.push_back(load.Execute());

  //get the TestGraph object we need for other tests
  _spTestGraph = load.GetTestGraph();

  //resolve all the references in the AAF graph
  RefResolver ref(GetOutStream(), _spTestGraph);
  LoadTest.push_back(ref.Execute());

  //ensure the AAF file graph is acyclic
  AcyclicAnalysis acy(GetOutStream(), _spTestGraph);
  LoadTest.push_back(acy.Execute());

  return LoadTest;
}


} // end of namespace diskstream
