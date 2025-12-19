#include "BlockLinkedList.hpp"

int BlockLinkedList::comp(const ListNode &a, char *index, int value) {
  int tmp = strcmp(a.index, index);
  if (tmp != 0) {
    return (tmp < 0) ? -1 : 1;
  } else if (a.value != value) {
    return (a.value < value) ? -1 : 1;
  }
  return 0;
}
int BlockLinkedList::FindBlockIndex(char *index, int value, BlockNode &block,
                                    int &pre_pos) {
  int pos = nxt_first_node[0];
  pre_pos = 0;
  while (nxt_first_node[pos]) {
    ListNode tmp = first_node[nxt_first_node[pos]];
    if (comp(tmp, index, value) <= 0) {
      pre_pos = pos;
      pos = nxt_first_node[pos];
    } else {
      break;
    }
  }
  block_data.read(block, vis_block_pos[pos]);
  return pos;
}
int BlockLinkedList::FindNodeIndex(const BlockNode &block, char *index,
                                   int value) {
  int pos = 0;
  ListNode nxt_node;
  while (pos < block.size) {
    nxt_node = block.list[pos + 1];
    if (comp(nxt_node, index, value) <= 0) {
      pos++;
    } else {
      break;
    }
  }
  return pos;
}
int BlockLinkedList::FindUnusedFirstNode() {
  int pos = 1;
  while (used_first_node[pos]) {
    ++pos;
  }
  return pos;
}
void BlockLinkedList::SplitBlock(int block_pos, BlockNode &block,
                                 int block_list_pos) {
  BlockNode new_block;
  int all_num = block.size, remain_num = all_num / 2;
  block.size = remain_num;
  for (int i = 1; i <= all_num - remain_num; i++) {
    new_block.list[i] = block.list[remain_num + i];
  }
  new_block.size = all_num - remain_num;
  new_block.nxt_block_index = block.nxt_block_index;
  block.nxt_block_index = block_data.write(new_block);
  block_data.update(block, block_pos);
  int new_pos = FindUnusedFirstNode();
  first_node[new_pos] = new_block.list[1];
  used_first_node[new_pos] = 1;
  vis_block_pos[new_pos] = block.nxt_block_index;
  nxt_first_node[new_pos] = nxt_first_node[block_list_pos];
  nxt_first_node[block_list_pos] = new_pos;
}
void BlockLinkedList::PrintBlock(const BlockNode &block) {
  puts("***********enter print block***********");
  int node_pos = 1;
  while (node_pos <= block.size) {
    ListNode tmp = block.list[node_pos];
    std::cout << tmp.index << ' ' << tmp.value << '\n';
    node_pos++;
  }
  puts("***********leave print block***********");
}

BlockLinkedList::BlockLinkedList(std::string str) : block_data(str) {
  memset(used_first_node, 0, sizeof(used_first_node));
  block_data.initialise();
  int index = 0;
  block_data.get_info(index, 1);
  if (index == 0) {
    BlockNode tmp;
    index = block_data.write(tmp);
    block_data.write_info(index, 1);
  }
  head_block_pos = index;
  BlockNode block;
  int block_num = 0;
  while (index) {
    block_num++;
    block_data.read(block, index);
    first_node[block_num] = block.list[1];
    nxt_first_node[block_num - 1] = block_num;
    used_first_node[block_num] = 1;
    vis_block_pos[block_num] = index;
    index = block.nxt_block_index;
  }
  nxt_first_node[block_num] = 0;
}
void BlockLinkedList::insert(char *index, int value) {
  BlockNode block;
  int pre_block,
      block_list_pos = FindBlockIndex(index, value, block, pre_block);
  int block_pos = vis_block_pos[block_list_pos];
  int node_pos = FindNodeIndex(block, index, value);
  ListNode now_node = block.list[node_pos];
  if (comp(now_node, index, value) == 0) {
    return;
  } else {
    for (int i = block.size; i > node_pos; i--) {
      block.list[i + 1] = block.list[i];
    }
    block.list[node_pos + 1] = ListNode(index, value);
    block.size++;
    block_data.update(block, block_pos);
    first_node[block_list_pos] = block.list[1];
  }
  if (block.size >= BLOCK_SIZE) {
    SplitBlock(block_pos, block, block_list_pos);
  }
}
void BlockLinkedList::del(char *index, int value) {
  BlockNode block;
  int pre_block,
      block_list_pos = FindBlockIndex(index, value, block, pre_block);
  int block_pos = vis_block_pos[block_list_pos];
  int node_pos = FindNodeIndex(block, index, value);
  ListNode now_node = block.list[node_pos];
  if (comp(now_node, index, value) != 0) {
    return;
  }
  for (int i = node_pos + 1; i <= block.size; i++) {
    block.list[i - 1] = block.list[i];
  }
  block.size--;
  block_data.update(block, block_pos);
  if (block.size == 0) {
    if (block_pos == head_block_pos) {
      if (block.nxt_block_index) {
        block_data.write_info(block.nxt_block_index, 1);
        block_data.Delete(block_pos);
        block_pos = block.nxt_block_index;
        head_block_pos = block_pos;
        used_first_node[nxt_first_node[0]] = 0;
        nxt_first_node[0] = nxt_first_node[nxt_first_node[0]];
      }
    } else {
      int nxt_block_index = block.nxt_block_index;
      int pre_block_pos = vis_block_pos[pre_block];
      block_data.read(block, pre_block_pos);
      block.nxt_block_index = nxt_block_index;
      block_data.Delete(block_pos);
      block_data.update(block, pre_block_pos);
      used_first_node[nxt_first_node[pre_block]] = 0;
      nxt_first_node[pre_block] = nxt_first_node[nxt_first_node[pre_block]];
    }
  }
}
std::vector<int> BlockLinkedList::find(char *index) {
  BlockNode block;
  int pre_block, block_list_pos = FindBlockIndex(index, -1, block, pre_block);
  int block_pos = vis_block_pos[block_list_pos];
  int node_pos = FindNodeIndex(block, index, -1);
  ListNode now_node = block.list[node_pos];
  if (node_pos < block.size) {
    node_pos++;
  } else {
    block_pos = block.nxt_block_index;
    node_pos = 1;
    if (block_pos) {
      block_data.read(block, block_pos);
    }
  }
  std::vector<int> ans;
  while (block_pos) {
    now_node = block.list[node_pos];
    if (strcmp(now_node.index, index) != 0) {
      break;
    }
    ans.push_back(now_node.value);
    if (node_pos < block.size) {
      node_pos++;
    } else {
      block_pos = block.nxt_block_index;
      node_pos = 1;
      if (block_pos) {
        block_data.read(block, block_pos);
      }
    }
  }
  return ans;
}
std::vector<int> BlockLinkedList::findall() {
  BlockNode block;
  int pre_block = 0, block_list_pos = nxt_first_node[0];
  int block_pos = vis_block_pos[block_list_pos];
  block_data.read(block, block_pos);
  int node_pos = 0;
  ListNode now_node = block.list[node_pos];
  if (node_pos < block.size) {
    node_pos++;
  } else {
    block_pos = block.nxt_block_index;
    node_pos = 1;
    if (block_pos) {
      block_data.read(block, block_pos);
    }
  }
  std::vector<int> ans;
  while (block_pos) {
    now_node = block.list[node_pos];
    ans.push_back(now_node.value);
    if (node_pos < block.size) {
      node_pos++;
    } else {
      block_pos = block.nxt_block_index;
      node_pos = 1;
      if (block_pos) {
        block_data.read(block, block_pos);
      }
    }
  }
  return ans;
}
