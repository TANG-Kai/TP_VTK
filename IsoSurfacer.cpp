/*
  IsoSurfacer.cpp: Isosurface computation class.
  Copyright (C) 2013  Julien Tierny <tierny@telecom-paristech.fr>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include  "IsoSurfacer.h"

vtkCxxRevisionMacro(IsoSurfacer, "$Revision$");
vtkStandardNewMacro(IsoSurfacer);

IsoSurfacer::IsoSurfacer(){
  Input = NULL;
  Output = NULL;
  pointSet_ = NULL;
  cellArray_ = NULL;
  fakeScalars_ = NULL;
  Type = SIMPLE;
}

IsoSurfacer::~IsoSurfacer(){

  if(Output)
    Output->Delete();
  if(pointSet_)
    pointSet_->Delete();
  if(cellArray_)
    cellArray_->Delete();
  if(fakeScalars_)
    fakeScalars_->Delete();
}

int IsoSurfacer::ComputePartialIntersection(const int &tetId){

  return 0;
}

int IsoSurfacer::ComputeSimpleIntersection(vtkCell *tet){

  int num_intersection= 0;
  vtkIdList *intersectPointList = vtkIdList::New();
  if(IsCellOnLevelSet(tet))
  {
    int n = tet->GetNumberOfEdges();
    for(int i=0;i<n;i++){
      vtkCell *e = tet->GetEdge(i);
      if(IsCellOnLevelSet(e)){
        pair<vtkIdType, vtkIdType> edge;
        edge.first = e->GetPointId(0);
        edge.second = e->GetPointId(1);
        vector<double> v = ComputeEdgeIntersection(edge);
        float v_in_float[3];
        v_in_float[0] = v[0];
        v_in_float[1] = v[1];
        v_in_float[2] = v[2];
        
        vtkIdType id = 
        this->Output->GetPoints()
        ->InsertNextPoint(v_in_float);
        intersectPointList->InsertNextId(id);
        num_intersection++;
      }
    }
  }
  return num_intersection;
}

int IsoSurfacer::FastExtraction(){

  return 0;
}

int IsoSurfacer::ReOrderTetEdges(
  vector<pair<vtkIdType, vtkIdType> > &edgeList) const{

  return 0;
}

int IsoSurfacer::SimpleExtraction(){
  
  return 0;
}

int IsoSurfacer::StandardExtraction(){
  
  return 0;
}



void IsoSurfacer::Update(){

  if(!Input){
    cerr << "[IsoSurfacer] No input defined..." << endl;
    return;
  }


  if(pointSet_)
    pointSet_->Delete();
  pointSet_ = vtkPoints::New();
  
  
  if(cellArray_)
    cellArray_->Delete();
  cellArray_ = vtkCellArray::New();
  
  if(Output)
    Output->Delete();
 
  if(!fakeScalars_)
    fakeScalars_ = vtkDoubleArray::New();
  
  Output = vtkPolyData::New();
  Output->SetPoints(pointSet_);
  Output->SetPolys(cellArray_);
  Output->GetPointData()->SetScalars(fakeScalars_);

  scalarField_ = Input->GetPointData()->GetScalars();
 
  DebugMemory memory;
  DebugTimer  timer;
  
  switch(Type){
    
    case SIMPLE:
      cout << "[IsoSurfacer] Using simple implementation..." << endl;
      SimpleExtraction();
      break;
      
    case STANDARD:
      cout << "[IsoSurfacer] Using standard implementation..." << endl;
      StandardExtraction();
      break;
      
    case FAST:
      cout << "[IsoSurfacer] Using fast implementation..." << endl;
      FastExtraction();
      break;
  }
  
  cout << setprecision(4);
 
  cout << "[IsoSurfacer] IsoSurface extracted ("
    << Output->GetNumberOfPoints() << " vertices, "
    << Output->GetNumberOfCells() << " faces)." << endl;
  cout << "[IsoSurfacer] Extraction performed in "
    << timer.getElapsedTime() << " s. (memory usage: "
    << memory.getInstantUsage() << " MB)." << endl;
    
  // add a scalar value to the isosurface to make sure it gets colored by the
  // rest of the pipeline
  fakeScalars_->SetNumberOfTuples(Output->GetNumberOfPoints());
  for(int i = 0; i < Output->GetNumberOfPoints(); i++)
    fakeScalars_->SetComponent(i, 0, Value);
}

