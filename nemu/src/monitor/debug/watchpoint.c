#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP* head = NULL, * free_ = NULL;

static int next_wp_NO = 0; // 全局变量，用于追踪下一个监视点的序号

//wp_pool链表初始化
void init_wp_pool() {
   int i;
   for (i = 0; i < NR_WP; i++) {
      wp_pool[i].NO = i;
      wp_pool[i].next = &wp_pool[i + 1];
   }//链接链表并对NO赋值
   wp_pool[NR_WP - 1].next = NULL;//wp_pool[31] 第32个wp 置NULL

   head = NULL;
   free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

// 返回新的监视点: wp->expr清空 wp->next清除至NULL old_value=0 new_value = 0
WP* new_wp() {
   if (free_ == NULL) {
      Assert(0, "static WP* free_ == NULL");  // 没有可用的空闲监视点
   }

   WP* wp = free_;  // 从空闲链表中获取一个监视点
   free_ = free_->next;  // 更新空闲链表的头指针
   wp->next = NULL;      // 清除新监视点的next指针

   // 初始化新监视点的其他字段...
   wp->NO = next_wp_NO; // 分配一个序号给新监视点
   next_wp_NO = (next_wp_NO + 1) % NR_WP; // 循环序号

   wp->enabled = true;    // 默认启用新监视点
   memset(wp->expr, 0, sizeof(wp->expr));  // 清空表达式字符串
   wp->old_value = 0;     // 初始化旧值
   wp->new_value = 0;     // 初始化新值

   return wp;  // 返回新的监视点
}

//重置监视点的部分字段,将监视点插入到空闲链表的头部 
void free_wp(WP* wp) {
   if (wp == NULL) {
      return;  // 如果传入的监视点为空，则直接返回
   }

   // 重置监视点的部分字段，以便于重用
   wp->enabled = false;    // 禁用监视点
   memset(wp->expr, 0, sizeof(wp->expr)); // 清空表达式字符串
   wp->old_value = 0;      // 重置旧值
   wp->new_value = 0;      // 重置新值

   // 将监视点插入到空闲链表的头部
   wp->next = free_;       // 设置当前监视点的next指向当前的空闲链表头
   free_ = wp;             // 更新空闲链表的头指针
}

bool delete_watchpoint(int no) {
   WP* wp = head;
   WP* prev = NULL;

   // 遍历监视点链表
   while (wp != NULL) {
      if (wp->NO == no) {
         // 找到了要删除的监视点
         if (prev == NULL) {
            // 要删除的监视点是链表的头部
            head = wp->next;
         }
         else {
            // 要删除的监视点在链表中间或尾部
            prev->next = wp->next;
         }
         free_wp(wp);  // 释放监视点
         return true;  // 返回删除成功的标志
      }
      prev = wp;
      wp = wp->next;
   }
   return false;  // 没有找到指定序号的监视点
}
