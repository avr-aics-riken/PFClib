/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCoeffTree.cpp
 * @brief  CPfcCoeffTree Class
 * @author aics
 */

#include <iostream>
#include <math.h>
#include "PfcCoeffTree.h"
#include "PfcFunction.h"

using namespace std;

//=======================================================================//
//        class CPfcCoeffTreeNode   ツリーノードクラス                   //
//=======================================================================//

// #################################################################
// コンストラクタ
CPfcCoeffTreeNode::CPfcCoeffTreeNode(int v0, int v1){
  value = new int[2];
  value[0] = v0;
  value[1] = v1;
  
  is_leaf= true;
  left_child = NULL;
  right_child = NULL;
}

// #################################################################
// デストラクタ
CPfcCoeffTreeNode::~CPfcCoeffTreeNode(){
  delete []value;
  if(!is_leaf){
    delete left_child;
    delete right_child;
  }    
}


//=======================================================================//
//         class CPfcCoeffTree     ツリークラス                          //
//=======================================================================//

// #################################################################
// コンストラクタ
CPfcCoeffTree::CPfcCoeffTree(int numTimeStep, int numLayer)
{
  root = new CPfcCoeffTreeNode(-1,-1);
  //layer = l;
  m_numTimeStep = numTimeStep;
  m_numLayer    = numLayer;
}

// #################################################################
// デストラクタ
CPfcCoeffTree::~CPfcCoeffTree(){
  delete root;
}

// #################################################################
// ツリー作成
void CPfcCoeffTree::construct_tree(int timestep){
  int curr_layer = 0;
  CPfcCoeffTreeNode *p = root;
  p->value[0] = 0;
  p->value[1] = timestep; 

#if 1
  //  int timestep_node = (int)(timestep / 2);
  int timestep_node = standard_timestep_node(timestep);
#else
  int timestep_node = (int)(timestep / 2);
  int timestep_node_2 = standard_timestep_node(timestep);
#endif

  construct_child(p, curr_layer, timestep_node);
  construct_array();
}

// #################################################################
// 子供ツリー作成
void CPfcCoeffTree::construct_child(CPfcCoeffTreeNode *p, int curr_layer, int timestep_node){
  //if(curr_layer < layer - 1){
  if(curr_layer < m_numLayer - 1){
    int child_interval = int_pow(2, curr_layer);
    int child_child_interval = int_pow(2, curr_layer+1);
    int timestep_change_node;
    int l_value, r_value;

    if(timestep_node % child_child_interval < (int)(child_child_interval/2)){
      timestep_change_node = timestep_node + child_interval;
      l_value = 2*timestep_node;
      r_value = 2*timestep_change_node;
    }
    else{
      timestep_change_node = timestep_node - child_interval;
      l_value = 2*timestep_change_node+1;
      r_value = 2*timestep_node+1;
    }

    CPfcCoeffTreeNode *lc = new CPfcCoeffTreeNode(curr_layer+1, l_value);
    CPfcCoeffTreeNode *rc = new CPfcCoeffTreeNode(curr_layer+1, r_value);

    p->is_leaf = false;
    p->left_child = lc;
    p->right_child = rc;

    construct_child(p->left_child, curr_layer+1, (int)(l_value/2));
    construct_child(p->right_child, curr_layer+1, (int)(r_value/2));
  }
}

// #################################################################
// 必要なのか？
int CPfcCoeffTree::standard_timestep_node(int timestep){
  int result = -1;
  //int n_2 = (int)((log(FILE_NUMBERS))/(log(SUB_FILE_NUMBERS)));
  int n_2 = (int)((log(m_numTimeStep))/(log(2)));
  int pow_2_n = (int)pow(2, n_2);
  int pow_2_n_1 = (int)pow(2, n_2-1);
  int group_n = 2;
  //if(FILE_NUMBERS > (pow_2_n+pow_2_n_1))
  if(m_numTimeStep > (pow_2_n+pow_2_n_1)) {
    group_n = 3;
  }
  //int number_group_n = (group_n+1)*pow_2_n_1-FILE_NUMBERS;
  int number_group_n = (group_n+1)*pow_2_n_1-m_numTimeStep;
  
  if(timestep < (number_group_n*group_n))
    result = (int)(timestep/group_n);
  else
    result = number_group_n + (int)((timestep-number_group_n*group_n)/(group_n+1));
  return result;
}

// #################################################################
// 
void CPfcCoeffTree::construct_array(){
  stand_space = 5;
  CPfcCoeffTreeNode *p = root;
  //data_size = int_pow(2, layer) - 1;
  data_size = int_pow(2, m_numLayer) - 1;
  data = new int*[data_size];
  position = new int[data_size];
  position_interval = 2 * stand_space;;

  for(int i = 0; i < data_size; i++)
    data[i] = new int[2];

  read_data(p, 1);

  get_position(root, 1);
}

// #################################################################
// 
void CPfcCoeffTree::print_tree(){
  //for(int i = 0; i < layer; i++)
  for(int i = 0; i < m_numLayer; i++)  {
    print_node(i);
  }
}

// #################################################################
// 
void CPfcCoeffTree::print_node(int c_layer){
  int start_pos = int_pow(2, c_layer)-1;
  int num_layer =  int_pow(2, c_layer);

  for(int i = 0 ; i < num_layer; i++){
    if(i == 0){
      for(int j = 0; j < position[start_pos]; j++)
	cout<<" ";
    }
    else{
      for(int j = 0; j < position[start_pos+i] - position[start_pos+i-1] - stand_space ; j++)
	cout<<" ";
    }
    if(data[start_pos+i][0]<10)
      cout<<"0";
    cout<<data[start_pos+i][0]<<"_";
    if(data[start_pos+i][0]<10)
      cout<<"0";
    cout<<data[start_pos+i][1];

  }

  cout<<endl;
  cout<<endl;
}

// #################################################################
// 
void CPfcCoeffTree::read_data(CPfcCoeffTreeNode *p, int pos){
  data[pos-1][0] = p->value[0];
  data[pos-1][1] = p->value[1];

  //if(p->value[0] < layer - 1){
  if(p->value[0] < m_numLayer - 1){
    read_data(p->left_child, pos*2);
    read_data(p->right_child, pos*2+1);
  }
}

// #################################################################
// 
int CPfcCoeffTree::get_position(CPfcCoeffTreeNode *p, int pos){
  //if(pos >= int_pow(2, layer-1)){
  //  position[pos-1] = 1 + (pos-int_pow(2, layer-1))*position_interval;
  if(pos >= int_pow(2, m_numLayer-1)){
    position[pos-1] = 1 + (pos-int_pow(2, m_numLayer-1))*position_interval;
    return position[pos-1];
  }
  else{
    position[pos-1] = (int)(( get_position(p->left_child, 2*pos) + get_position(p->right_child, 2*pos+1) )/2);
    return position[pos-1];
  }
}

// #################################################################
// 
void CPfcCoeffTree::deep_transerval(int*result, int index){
  //int start_point = int_pow(2, layer-1) + index;
  //for(int i = 0; i < layer; i++){
  //  result[layer-1-i] = data[(int)(start_point/(int_pow(2,i)))-1][1];
  //}

  int start_point = int_pow(2, m_numLayer-1) + index;
  for(int i = 0; i < m_numLayer; i++){
    result[m_numLayer-1-i] = data[(int)(start_point/(int_pow(2,i)))-1][1];
  }

}
