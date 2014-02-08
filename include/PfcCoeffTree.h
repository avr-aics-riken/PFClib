#ifndef _PFC_COEFF_TREE_H_
#define _PFC_COEFF_TREE_H_


/*
 * PFClib - Parallel File Compression library
 *
 * Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   PfcCoeffTree.h 
 * @brief  CPfcCoeffTree Class Header
 * @author aics
 */
 
#include <iostream>
//#include "define.h"  // fj comentted

using namespace std;

class CPfcCoeffTreeNode{
 private:

 public:
  bool is_leaf;
  int* value;
  CPfcCoeffTreeNode *left_child;
  CPfcCoeffTreeNode *right_child;

 public:
  CPfcCoeffTreeNode(){};
  CPfcCoeffTreeNode(int v0, int v1);
  ~CPfcCoeffTreeNode();
};

class CPfcCoeffTree{
 private:
 public:
  CPfcCoeffTreeNode *root;

  int m_numTimeStep;	// 全体のタイムステップ数
  //int layer;
  int m_numLayer;		// レイヤー数

  int stand_space;
  int** data;
  int data_size;
  int *position;
  int position_interval;
 public:
  CPfcCoeffTree(int numTimeStep, int numLayer);
  ~CPfcCoeffTree();

  void construct_tree(int timestep);
  void construct_child(CPfcCoeffTreeNode *p, int curr_layer, int timestep_node);
  void print_tree();
  void print_node(int c_layer);
  void read_data(CPfcCoeffTreeNode *p, int pos);
  int get_position(CPfcCoeffTreeNode *p, int pos);
  void construct_array();
  
  void deep_transerval(
  				int*result, // [out] step位置(0～）
  				int index	// [in]  行No（0～,探索ルートNo）
  			);
  // 必要なのか？
  int standard_timestep_node(int timestep);
};

#endif  // _PFC_COEFF_TREE_H_
