#pragma once

#include "MemoryRiver.hpp"
#include <iostream>
#include <vector>

class BlockLinkedList {
private:
  static const int BLOCK_SIZE = 500;
  static const int MAX_BLOCK_NUMBER = 305;
  int head_block_pos;
  struct ListNode {
    static const int MAX_LEN = 65;
    char index[MAX_LEN];
    int value;
    ListNode() : value(0) { index[0] = '\0'; }
    ListNode(char *idx, int value) : value(value) {
      strncpy(index, idx, MAX_LEN);
    }
  };
  ListNode first_node[MAX_BLOCK_NUMBER];
  int nxt_first_node[MAX_BLOCK_NUMBER];
  int used_first_node[MAX_BLOCK_NUMBER];
  int vis_block_pos[MAX_BLOCK_NUMBER];
  struct BlockNode {
    int size;
    int nxt_block_index;
    ListNode list[BLOCK_SIZE + 5];
    BlockNode() : size(0), nxt_block_index(0) {}
  };
  MemoryRiver<BlockNode> block_data;
  int comp(const ListNode &a, char *index, int value);
  int FindBlockIndex(char *index, int value, BlockNode &block, int &pre_pos);
  int FindNodeIndex(const BlockNode &block, char *index, int value);
  int FindUnusedFirstNode();
  void SplitBlock(int block_pos, BlockNode &block, int block_list_pos);
  void PrintBlock(const BlockNode &block);

public:
  BlockLinkedList(std::string str);
  void insert(char *index, int value);
  void del(char *index, int value);
  std::vector<int> find(char *index);
  std::vector<int> findall();
};